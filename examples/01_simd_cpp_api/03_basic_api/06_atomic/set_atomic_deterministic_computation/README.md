# 数据搬运随路原子操作做确定性计算的样例
## 概述
本样例首先介绍在数据搬运随路原子操作的场景下做确定性计算的必要性和具体实现方案，随后分别介绍在单个AIV核、多AIV核和多AIC核场景下如何运用上述方案。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | >= CANN 9.0.0 |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | >= CANN 9.0.0 |

## 目录结构介绍
```
├── set_atomic_deterministic_computation
│   ├── figures                                   // README中图片资源目录
│   ├── scripts
│   │   ├── gen_data.py                           // 输入数据和真值数据生成脚本
│   │   └── verify_result.py                      // 验证输出数据和真值数据是否一致的验证脚本
│   ├── CMakeLists.txt                            // 编译工程文件
│   ├── data_utils.h                              // 数据读入写出函数
│   ├── set_atomic_add_multi_aic.h                // 多AIC核同步实现
│   ├── set_atomic_add_multi_aiv.h                // 多AIV核同步实现
│   ├── set_atomic_add_single_aiv.h               // 单AIV核同步实现
│   ├── set_atomic_deterministic_computation.asc  // Ascend C样例实现 & 调用样例
│   └── README.md                                 // 样例说明文档
```

## 1. 确定性计算

### 1.1 确定性计算的必要性

浮点数累加不具备数学上的交换律和结合律，不同的累加顺序可能导致不同的计算结果，可以通过以下数据验证这一点：

```python
x = 1.0
y = 1e16
z = -1e16

# 不同的累加顺序会产生不同结果
left = (x + y) + z    # 先加 x 和 y，再与 z 相加
right = x + (y + z)   # 先加 y 和 z，再与 x 相加
```

由于浮点数精度限制，`(1.0 + 1e16) + (-1e16)` 和 `1.0 + (1e16 + (-1e16))` 的结果不同：
- 前者：`1.0 + 1e16` 会丢失精度，结果接近 `1e16`，再加上 `-1e16` 得到 `0.0`
- 后者：`1e16 + (-1e16)` 精确等于 `0.0`，再加上 `1.0` 得到 `1.0`

因此，在并行计算环境中，必须通过同步机制确保固定的累加顺序，以保证结果的确定性。确定性计算是指在相同输入条件下，无论执行次数或执行环境如何变化，总能产生完全一致输出结果的计算过程。确定性计算为系统稳定性和实验可验证性提供保障。

### 1.2 确定性计算概述

为引出原子操作场景下非确定性计算的问题，我们构建如下常见的确定性计算场景：首先，通过单组浮点数数据搬运完成GM的初始化；随后，启动原子累加操作；最后，经由多次数据搬运，在 GM 上对多组浮点数数据进行累加。具体伪代码如下：

```
①向GM搬运数据data0;    // 数据搬运，覆盖GM原有随机值，期望GM数据为data0 
②SetAtomicAdd();         // 开启原子累加，后续从UB/L0C/L1到GM的搬运均执行原子累加 
③向GM搬运data1;    // 带随路原子操作的数据搬运，期望GM数据为data0 + data1 
④向GM搬运data2;    // 带随路原子操作的数据搬运，期望GM数据为data0 + data1 + data2 
⑤向GM搬运data3;    // 带随路原子操作的数据搬运，期望GM数据为data0 + data1 + data2 + data3
```

如下图所示，开发者的预期结果：指令发射的顺序能够严格对应实际指令执行顺序，多次执行该段代码，无论执行多少次，最终 GM 数据均为 data0 + data1 + data2 + data3，结果完全一致，实现确定性计算。

![确定性计算场景](figures/确定性计算场景，GM上数据变化过程.png)

图1：确定性计算场景，GM上数据变化过程

但实际情况是，若开发者不做干预，程序每次运行时这些指令的执行顺序都可能发生变化，最终导致 GM 数据与预期结果不一致。下面列举两种可能的指令执行顺序及其对应的执行流程。

#### 1.2.1 非确定性计算，结果1

![非确定性计算场景1](figures/非确定性计算场景1，GM上数据变化过程.png)

图2：非确定性计算场景1，GM上数据变化过程

如图所示，该场景中指令执行流程如下：
1. 初始状态，GM 数据为：随机值；
2. 向 GM 搬运 data0，GM数据被初始化为：data0；
3. 执行SetAtomicAdd，为后续搬运指令开启原子累加，GM数据为：data0；
4. 三次带随路原子操作的搬运指令乱序，实际执行顺序为"搬出data2 →搬出 data3 →搬出 data1"，最终GM 上数据为： data0 + data2 + data3 + data1。

**非确定性计算的产生原因1**：
带随路原子操作的搬运指令乱序，由于浮点数加法不满足结合律，即(a+b)+c != a+(b+c)，使得最终 GM 数据 data0 + data2 + data3 + data1与预期的 data0 + data1 + data2 + data3 存在偏差。

带随路原子操作的搬运指令乱序，导致最终结果产生偏差的前提条件有以下三条：
- 原子操作类型为原子累加（最大值、最小值运算满足结合律）
- 原子操作数据类型为浮点数（整数加法满足结合律）
- 带随路原子操作的搬运指令达到3条及以上（浮点数加法满足交换律）

#### 1.2.2 非确定性计算，结果2

![非确定性计算场景2](figures/非确定性计算场景2，GM上数据变化过程.png)

图3：非确定性计算场景2，GM上数据变化过程

如图所示，该场景中指令执行流程如下：
1. 初始状态，GM 数据为：随机值；
2. 执行SetAtomicAdd，为后续搬运指令开启原子累加，GM数据为：随机值；
3. 先后执行两次带随路原子操作的搬运指令，执行顺序为"搬出data1 →搬出 data2"，GM 数据为：随机值 + data1 + data2；
4. 向 GM 搬运 data0，GM上累加的结果被 data0 覆盖，GM数据为：data0；
5. 最后执行 data3 的搬运，最终GM 上数据为：data0 + data3。

**非确定性计算的产生原因2**：
开启原子累加前的普通搬运指令与开启原子操作的搬运指令之间发生乱序，会导致 GM 上已完成原子操作的数据被 data0 错误覆盖，进而产生非确定性计算结果。

此类乱序导致结果偏差无需满足任何前提条件，开发者无需再区分原子操作类型、原子操作数据类型，也无需考虑带随路原子操作的搬运指令数量是否达到 3 条及以上。

### 1.3 确定性计算实现方案

根据导致非确定性计算的两个根因，下面也从解决这两个方面描述确定性计算实现的方案。核心思想是在指令之间插入适当的同步，使每次程序运行时相关指令都按照预期确定的顺序执行，最终保证每次执行程序输出的结果都相同。具体来说包含以下两个方面：

- 开启原子累加前的搬运指令与开启原子操作的指令之间插入同步
  如下伪代码所示，在指令①与②之间插入同步，能够确保开始原子操作前GM的初始值符合预期。
- 开启原子累加操作后，多条搬运指令之间的同步
  指令③与④、④与⑤之间插入同步，能够确保浮点数累加的顺序符合预期。

开启原子操作的指令与后续搬运指令之间不需要开发者插入同步。

#### 1.3.1 核内同步实现

```cpp
// 整个原子累加在同一个核内执行，控制5个指令的执行顺序为"①→②→③→④→⑤" 
①向GM搬运数据data0;    // 数据搬运，覆盖GM原有随机值，期望GM数据为data0 
核内同步 
②SetAtomicAdd();         // 开启原子累加，后续从UB/L0C/L1到GM的搬运均执行原子累加 
// 指令②与③间无需插入同步 
③向GM搬运data1;    // 开启原子累加后的数据搬运，期望GM数据为data0 + data1 
核内同步 
④向GM搬运data2;    // 开启原子累加后的数据搬运，期望GM数据为data0 + data1 + data2 
核内同步 
⑤向GM搬运data3;    // 开启原子累加后的数据搬运，期望GM数据为data0 + data1 + data2 + data3
```
搬运指令和开启原子操作的指令流水类型如下表所示，当上述指令在同一个核内执行时，开发者按需插入[单流水同步](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/900beta2/API/ascendcopapi/atlasascendc_api_07_0271.html)或者[多流水同步](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/900beta2/API/ascendcopapi/atlasascendc_api_07_0270.html)，可参考样例中SCENARIO_NUM=1的分支。
<table border="1" style="text-align: left;">
  <tr>
    <th style="padding: 8px;">指令名称</th>
    <th style="padding: 8px;">流水类型</th>
  </tr>
  <tr>
    <td style="padding: 8px;">DataCopy</td>
    <td style="padding: 8px;">PIPE_MTE3</td>
  </tr>
  <tr>
    <td style="padding: 8px;">Fixpipe</td>
    <td style="padding: 8px;">PIPE_FIX</td>
  </tr>
  <tr>
    <td style="padding: 8px;">SetAtomicAdd/SetAtomicMax/SetAtomicMin</td>
    <td style="padding: 8px;">PIPE_S</td>
  </tr>
</table>

#### 1.3.2 核间同步实现

如下伪代码所示，当上述指令都在不同核中执行时，需要将上述的核内同步替换为核间同步。

```cpp
// 整个原子累加在4个不同核中执行,控制4个核执行的顺序为"核0→核1→核2→核3" 
if (GetBlockIdx == 0) { 
   向GM搬运数据data0; 
   核间同步 
} else if (GetBlockIdx == 1) { 
   核间同步     
   SetAtomicAdd();          
   向GM搬运data1; 
   核间同步    
} else if (GetBlockIdx == 2) { 
   核间同步 
   SetAtomicAdd();          
   向GM搬运data2;   
   核间同步  
} else if (GetBlockIdx == 3) { 
   核间同步 
   SetAtomicAdd();          
   向GM搬运data3;    
}
```
由于当前未提供用于控制不同核之间执行顺序的硬件同步接口，因此确定性计算场景下的核间同步需通过软件方式实现。针对纯 Vector 样例、纯 Cube 样例及 Mix（同时包含 Vector 与 Cube 计算）样例三种场景，所采用的软件同步方案存在差异，具体如下表所示。
<table border="1" style="text-align: left;">
  <tr>
    <th style="padding: 8px;">样例类型</th>
    <th style="padding: 8px;">软件同步方案</th>
    <th style="padding: 8px;">说明</th>
  </tr>
  <tr>
    <td style="padding: 8px;" rowspan="2">纯Vector样例</td>
    <td style="padding: 8px;">方案1：通过多对IBSet和IBWait接口组合，可实现多个AIV间的同步，可参考样例中SCENARIO_NUM=2的分支。</td>
    <td style="padding: 8px;">方案1支持指定部分AIV参与同步，并可控制各AIV的执行顺序。</td>
  </tr>
  <tr>
    <td style="padding: 8px;">方案2： 通过InitDetermineComputeWorkspace、NotifyNextBlock 和 WaitPreBlock三个接口配合使用，确保所有AIV核按照blockIdx升序执行。</td>
    <td style="padding: 8px;">方案2要求所有AIV必须参与同步，且执行顺序固定为blockIdx升序。</td>
  </tr>
  <tr>
    <td style="padding: 8px;">纯Cube样例</td>
    <td style="padding: 8px;">通过GM中的信号量实现核间同步，先建立一对核之间的同步，进而扩展至多个核之间的同步，可参考样例中SCENARIO_NUM=3的分支。</td>
    <td style="padding: 8px;">通过Scalar单元访问GM时，需考虑多核间数据一致性问题。<br>此处的"核"可以是AIV或者AIC。</td>
  </tr>
</table>
下图展示了两个核之间如何通过GM中的信号量进行核间同步：

![一对核之间软件同步方案流程图](figures/一对核之间软件同步方案流程图.png)

图4：一对核之间软件同步方案流程图

- 上一个核完成数据搬运或开启原子操作后，会通过 Scalar 单元向核间共享的 GM 中的信号量写入值 1，表示自己的任务已完成。上一个核中也需要插入核内同步：
  - 当上一个核内有多条搬运指令时，它们之间需要插入核内同步1。
  - Scalar 单元向GM写数据之前必须保证前序所有搬运指令都已经执行完成，因此它们之间也需要插入核内同步2。
- 当前核在执行搬运任务前，会通过 Scalar 单元不断读取该信号量的值。如果信号量不等于 1，当前核会进入阻塞等待状态；当检测到信号量等于 1 时，当前核会解除阻塞，开始执行自己的数据搬运或原子操作。为确保信号量等于1之前，当前核不会执行搬运指令，需要在搬运指令之前插入核内同步3。

#### 1.3.3 Scalar单元访问GM上的信号量方式

Scalar单元访问GM上的信号量，存在两种访问方式：

1. **经过 DCache 访问**
   使用 GlobalTensor的成员函数GetValue 和 SetValue 进行操作。这种情况下，开发者需要手动调用[DataCacheCleanAndInvalid](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/900beta2/API/ascendcopapi/atlasascendc_api_07_0177.html)接口，以确保多核间数据的一致性。

2. **不通过DCache 访问**
   使用[WriteGmBypassDCache](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/900beta2/API/ascendcopapi/atlasascendc_api_07_00089.html) 和 ReadGmBypassDCache。这种方式无需额外操作即可保证多核间数据的一致性。

两种方案在性能上的差异：不经过DCache，性能会较差，但是如果读写GM数据较少，可以考虑使用不经过DCache的方法。
核间同步方案中也需要与核内同步配合使用，现将三处核内同步作用说明如下：

- **核内同步1（可选）**：核内存在多条数据搬运指令时，通过该同步保证各搬运操作严格按顺序执行。
- **核内同步2（必备）**：等待前一个核全部任务执行完毕后，才允许 Scalar 单元向全局内存信号量写入1。
- **核内同步3（必备）**：等待 Scalar 单元检测到信号量更新为1后，当前核再启动后续任务执行。

## 2.样例描述
### 2.1 场景配置说明
<table border="1" style="text-align: left;">
  <tr>
    <td>SCENARIO_NUM取值</td>
    <td>业务场景</td>
    <td>核函数</td>
    <td>使用的同步模式</td>
  </tr>
  <tr>
    <td>1</td>
    <td>单个AIV核内的确定性计算</td>
    <td>set_atomic_add_single_aiv_custom</td>
    <td>PipeBarrier 确保顺序</td>
  </tr>
  <tr>
    <td>2</td>
    <td>多个AIV核间的确定性计算</td>
    <td>set_atomic_add_multi_aiv_custom</td>
    <td>IBSet/IBWait AIV核间同步</td>
  </tr>
  <tr>
    <td>3</td>
    <td>多个AIC核间的确定性计算</td>
    <td>set_atomic_add_multi_aic_custom</td>
    <td>GM信号量核间同步</td>
  </tr>
</table>

### 2.2 计算公式与样例规格

三个场景使用相同的计算公式：

$$
z = src0 + src2 + src3 + src1
$$

其中：
- `src0` 为 GM 初始值向量（全为 0）
- `src1`、`src2`、`src3` 为参与原子累加的三个输入向量
- 累加顺序固定为：`src0`（初始）→ +`src2` → +`src3` → +`src1`
- `z` 为最终累加结果

#### 2.2.1 SCENARIO_NUM=1（单个AIV核内的确定性计算）
- **计算方式**：在单个 AIV 核内按固定顺序依次执行原子累加操作
- **同步机制**：使用 PipeBarrier 确保每次原子操作完成后再执行下一次

- **样例规格**：
  <table border="1">
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center">SetAtomicAddSingleAiv</td></tr>
  <tr><td rowspan="5" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">src0</td><td align="center">[8]</td><td align="center">float32</td><td align="center">ND</td></tr>
  <tr><td align="center">src1</td><td align="center">[8]</td><td align="center">float32</td><td align="center">ND</td></tr>
  <tr><td align="center">src2</td><td align="center">[8]</td><td align="center">float32</td><td align="center">ND</td></tr>
  <tr><td align="center">src3</td><td align="center">[8]</td><td align="center">float32</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">z</td><td align="center">[8]</td><td align="center">float32</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">核数(numBlocks)</td><td colspan="4" align="center">1</td></tr>
  </table>

#### 2.2.2 SCENARIO_NUM=2 和 SCENARIO_NUM=3（多核间的确定性计算）
这两个场景的核心逻辑相同，区别在于累加的数据分布在多个AIV核或AIC核上。相较于单个核场景，多核场景需要在AIV核间或AIC核间进行同步，因此输入新增了sync_buf参数，用于存储GM上核间同步信号量，其初始值必须为0。

- **样例规格**：
  <table border="1">
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center">SetAtomicAddMultiCore</td></tr>
  <tr><td rowspan="6" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">src0</td><td align="center">[8]</td><td align="center">float32</td><td align="center">ND</td></tr>
  <tr><td align="center">src1</td><td align="center">[8]</td><td align="center">float32</td><td align="center">ND</td></tr>
  <tr><td align="center">src2</td><td align="center">[8]</td><td align="center">float32</td><td align="center">ND</td></tr>
  <tr><td align="center">src3</td><td align="center">[8]</td><td align="center">float32</td><td align="center">ND</td></tr>
  <tr><td align="center">sync_buf</td><td align="center">[256]</td><td align="center">int32</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">z</td><td align="center">[8]</td><td align="center">float32</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">核数(numBlocks)</td><td colspan="4" align="center">4</td></tr>
  </table>

### 2.3 注意事项

#### (1) 当 `SCENARIO_NUM=3` 时，样例不支持 Ascend 950PR/Ascend 950DT。
当`SCENARIO_NUM=1` 和 `SCENARIO_NUM=2`样例支持 Ascend 950PR/Ascend 950DT。
当`SCENARIO_NUM=3` 时，调用 `DataCopy` 从L1向GM搬出数据，然而在Ascend 950PR/Ascend 950DT架构下，DataCopy接口不支持L1 Buffer -> GM通路，因此当 `SCENARIO_NUM=3` 时，样例不支持 Ascend 950PR/Ascend 950DT。如果要支持Ascend 950PR/Ascend 950DT，可以参考[基础API迁移指导](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/900beta2/opdevg/Ascendcopdevg/atlas_ascendc_compatibility_10_00005.html)中的兼容性方案。

#### (2) 当 `SCENARIO_NUM=2` 时，样例不支持静态 tensor 编程方式。
当SCENARIO_NUM=2 时，调用了`IBSet` 和 `IBWait` 做核间同步，然而以上两个接口内部实现需要借助TPipe框架做核内同步，因此当SCENARIO_NUM=2 时，样例不支持静态 tensor 编程方式。

## 编译运行  
在本样例根目录下执行如下步骤，编译并执行样例。
- 配置环境变量  
  请根据当前环境上CANN开发套件包的[安装方式](../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。
    
- 样例执行

  在本样例目录下执行如下命令。
  ```bash
  SCENARIO_NUM=1 # 默认演示单个AIV核内的确定性计算
  mkdir -p build && cd build;   # 创建并进入build目录
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=${SCENARIO_NUM};make -j;  # 编译工程
  python3 ../scripts/gen_data.py
  ./demo                        # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # 验证输出结果是否正确，确认算法逻辑正确
  ```
  执行结果如下，说明精度对比成功。
  ```bash
  test pass!
  ```
