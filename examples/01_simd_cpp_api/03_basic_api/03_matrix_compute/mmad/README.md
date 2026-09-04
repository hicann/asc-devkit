# Mmad样例

## 概述

本样例介绍了输入为ND格式，B4 / B8 / B16 / B32输入数据类型（具体以int4_t / int8_t / bfloat16 / float为例），四种输入数据类型下的矩阵乘法，说明如何通过Mmad指令实现矩阵乘法计算（C = A x B + Bias）。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | >= CANN 9.2.0 |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | >= CANN 9.2.0 |

## 目录结构介绍

```
├── mmad
│   ├── figures                     // 图示
│   ├── scripts
│   │   ├── gen_data.py             // 输入数据和真值数据生成脚本
│   │   ├── mmad.py                 // 矩阵乘法辅助脚本
│   │   └── verify_result.py        // 验证输出数据和真值数据是否一致的验证脚本
│   ├── CMakeLists.txt              // 编译工程文件
│   ├── data_utils.h                // 数据读入写出函数
│   ├── mmad.asc                    // Ascend C样例实现 & 调用样例
│   └── README.md                   // 样例说明文档
```

## 样例描述

一次完整的矩阵乘法涉及的数据搬运过程包括：GM -> L1、L1 -> L0A / L0B、L1 -> BT（BiasTable Buffer）、L0C -> GM，其中不同存储单元的数据排布格式，如下表1所示：

<a name="表1"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">📌 表1：不同存储单元的数据排布格式</span></caption>
  <tr>
    <td >存储单元</td>
    <td>数据排布格式</td>
  </tr>
  <tr>
    <td>GM</td>
    <td>输入A、B矩阵和输出C矩阵为ND排列。</td>
  </tr>
  <tr>
    <td>L1</td>
    <td>A、B矩阵为Nz排列。</td>
  </tr>
  <tr>
    <td>L0A</td>
    <td>Ascend 950PR/Ascend 950DT产品，A矩阵为Nz排列；<br>Atlas A3 训练系列产品/Atlas A3 推理系列产品和Atlas A2 训练系列产品/Atlas A2 推理系列产品，A矩阵为Zz排列。</td>
  </tr>
  <tr>
    <td>L0B</td>
    <td>B矩阵为Zn排列。</td>
  </tr>
  <tr>
    <td>BT（BiasTable Buffer）</td>
    <td>Bias是shape为[N]的一维Tensor。</td>
  </tr>
  <tr>
    <td>L0C</td>
    <td>C矩阵为Nz排列。</td>
  </tr>
</table>

通常的矩阵乘法计算公式：C = A × B + Bias，其中A、B、Bias、C矩阵的需要满足的shape分别为[M,K]、[K,N]、[N]和[M,N]。Bias的数据类型与C矩阵数据类型的对应关系，如表2所示，其中Bias在GM/L1上使用 bfloat16 的组合仅支持Ascend 950PR/Ascend 950DT。
<a name="表2"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">📌 表2：L0C 与输入 Bias 的数据类型对应关系</span></caption>
  <tr>
    <td>Bias在GM/L1上的数据类型</td>
    <td>Bias在BT（BiasTable Buffer）上的数据类型</td>
    <td>矩阵计算输出到L0C上的数据类型</td>
  </tr>
  <tr>
    <td>int32_t</td>
    <td>int32_t</td>
    <td>int32_t</td>
  </tr>
  <tr>
    <td>bfloat16</td>
    <td rowspan="3">float</td>
    <td rowspan="3">float</td>
  </tr>
  <tr>
    <td>half</td>
  </tr>
    <tr>
    <td>float</td>
  </tr>
</table>

程序中scenarioNum参数不同取值对应的场景，如下表3所示：

<a name="表3"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">📌 表3：scenarioNum不同取值的含义</span></caption>
  <tr>
    <td >scenarioNum</td>
    <td>输入数据类型</td>
    <td>输出数据类型</td>
    <td>A矩阵</td>
    <td>B矩阵</td>
    <td>Bias</td>
  </tr>
  <tr>
    <td>1</td>
    <td>int8_t</td>
    <td>int32_t</td>
    <td>不转置</td>
    <td>不转置</td>
    <td>带Bias且不传入biasTensor，C矩阵的初始值来源于C2</td>
  </tr>
  <tr>
    <td>2</td>
    <td>bfloat16</td>
    <td>bfloat16</td>
    <td>不转置</td>
    <td>转置</td>
    <td>不带Bias，C矩阵累加来源于CO1初始值</td>
  </tr>
  <tr>
    <td>3</td>
    <td>float</td>
    <td>float</td>
    <td>转置</td>
    <td>转置</td>
    <td>带Bias且传入biasTensor的场景</td>
  </tr>
  <tr>
    <td>4</td>
    <td>int4b_t</td>
    <td>int32_t</td>
    <td>不转置</td>
    <td>转置</td>
    <td>不启用Bias，C矩阵的初始值为0</td>
  </tr>
</table>

### 场景详细说明

  本样例通过编译参数 `SCENARIO_NUM` 选择不同的输出场景，所有场景基于相同的矩阵乘规格：[M, N, K] = [30, 40, 70]，核函数名为 `mmad_custom`。

**场景1 int8_t输入，int32_t输出，C矩阵初始值来源于C2**
- 输入：
  - A 不转置 [30, 70] int8_t类型，ND格式
  - B 不转置 [70, 40] int8_t类型，ND格式
  - Bias [1, 40] int32_t类型
- 输出：C [30, 40] int32_t类型，ND格式
- 实现：使用`Mmad`实现矩阵乘法运算，不传入biasTensor通过参数：`mmadParams.cmatrixInitVal = false、mmadParams.cmatrixSource = true`，设置C矩阵初始值来源于C2。
- 说明：int8_t类型输入，B矩阵不转置场景下，N轴向2 * 16对齐，填充了全部是无效数据的32 * 16的分形。如下图1所示，如果设置`mmadParams.n = N`，就会导致读入编号为3、7的分形，同时又没能将包含有效数据的编号为9、10的分形读入。因此需要设置：`mmadParams.n = CeilAlign(N, BLOCK_CUBE * fractalNum)`，此时会读入全部分形，虽然矩阵计算结果中包含了无效数据参与计算的结果，但是在Fixpipe指令搬出数据时通过设置`fixpipeParams.nSize = N`来保证无效数据参与计算的结果不会被搬出。
<p align="center">
  <img src="figures/mmad_s8_L0B_转置.png" width="700">
</p>
<p align="center">
图1：int8_t类型，B不转置，N轴实际对齐要求与Mmad指令默认不一致
</p>

**场景2 bfloat16输入，float输出，A不转置，B转置，C矩阵初始值来源于CO1**
- 输入：
  - A 不转置 [30, 70] bfloat16类型，ND格式
  - B 转置 [40, 70] bfloat16类型，ND格式
- 输出：C [30, 40] float类型，ND格式
- 实现：使用`Mmad`实现矩阵乘法运算，通过参数：`mmadParams.cmatrixInitVal = false、mmadParams.cmatrixSource = false`，设置C矩阵初始值来源于CO1
- 说明：该场景进行两次Mmad计算，第一次计算结果存储在CO1，作为下一次计算的C矩阵初始值，最终累加两次Mmad计算结果。

**场景3 float输入，float输出，A转置，B转置，传入biasTensor，kDirectionAlign值设为true**
- 输入：
  - A转置 [70, 30] float类型，ND格式
  - B 转置 [40, 70] float类型，ND格式
  - Bias [1, 40] float类型
- 输出：C [30, 40] float类型，ND格式
- 实现：使用`Mmad`实现矩阵乘法运算，传入biasTensor，该场景下`mmadParams.cmatrixSource`参数无效
- 说明：float类型输入，A矩阵转置场景下，需要使用`mmadParams.kDirectionAlign`来解决K轴实际向`CeilAlign(K, 8*2)`对齐，与Mmad指令默认要求对齐到`CeilAlign(K, 8)`不一致的问题。该场景下该参数设置为真，K轴对齐到`CeilAlign(K, 16)`，矩阵计算单元从L0A读取数据会跳过填充的无效数据，其余场景下该参数默认为false，K轴仍向`CeilAlign(K, 8)`对齐，如下图2所示：

<p align="center">
  <img src="figures/mmad_f32_L0A_转置.png" width="1100">
</p>
<p align="center">
图2：float类型，A转置，K轴实际对齐与Mmad指令默认要求不一致
</p>

**场景4 int4b_t输入，int32_t输出，C矩阵初始值为0，仅支持Atlas A3 训练系列产品/Atlas A3 推理系列产品、Atlas A2 训练系列产品/Atlas A2 推理系列产品**

- 输入：
  - A 不转置 [30, 70] int4b_t类型，ND格式
  - B 转置 [40, 70] int4b_t类型，ND格式
- 输出：C [30, 40] int32_t类型，ND格式
- 实现：使用`Mmad`实现矩阵乘法运算，通过参数：`mmadParams.cmatrixInitVal = true`，设置C矩阵初始值为0
- 说明：该场景不支持使用传入biasTensor的方式加Bias（场景3）。

### 矩阵乘法（Mmad）

下面将介绍如何配置
Mmad指令的MmadParams结构体的成员，各个成员变量的具体含义这里不再赘述。

需要注意的是，执行Mmad指令时，矩阵计算单元会从L0A/L0B连续读入多个分形参与矩阵乘计算。读入分形的数量由MmadParams结构体成员m、n、k的取值，以及Mmad指令对L0A/L0B上A矩阵、B矩阵各轴的对齐要求共同决定。

以b16输入类型为例，Mmad指令会按照A矩阵分形[16, 16]、B矩阵分形[16, 16]连续读入数据。此时，矩阵计算单元从L0A/L0B读入的分形总数分别为2 x 5 = 10和5 x 3 = 15，写入L0C的分形总数为2 x 3 = 6。如下图所示，图3对应Atlas A3训练系列产品/Atlas A3推理系列产品和Atlas A2训练系列产品/Atlas A2推理系列产品，图4对应Ascend 950PR/Ascend 950DT；两者在L0A上的数据排布不同，前者为Zz，后者为Nz。

<p align="center">
  <img src="figures/mmad_f16_A3.png" width="900">
</p>
<p align="center">
图3：bfloat16类型，L0A上Zz排布，Mmad数据排布示意图
</p>
<p align="center">
  <img src="figures/mmad_f16_A5.png" width="900">
</p>
<p align="center">
图4：bfloat16类型，L0A上Nz排布，Mmad数据排布示意图
</p>

Mmad计算中包含了补齐的无效数据，需要配合Fixpipe指令在L0C搬出到GM过程中，排除参数Mmad计算过程中填充的无效数据。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行算子。

- 配置环境变量  
  请根据当前环境上CANN开发套件包的[安装方式](../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。
  
- 样例执行

  在本样例目录下执行如下命令。
  ```bash
  SCENARIO_NUM=1
  mkdir -p build && cd build;      # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j;    # 编译工程，默认npu模式
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM   # 生成测试输入数据
  ./demo                           # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py -scenarioNum=$SCENARIO_NUM output/output.bin output/golden.bin   # 验证输出结果是否正确，确认算法逻辑正确
  ```

  使用 CPU调试 或 NPU仿真 模式时，添加 `-DCMAKE_ASC_RUN_MODE=cpu` 或 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。

  示例如下：

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j; # CPU调试模式
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`sim`、`cpu` | 运行模式：NPU运行、NPU仿真、CPU调试 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU 架构：dav-2201 对应 Atlas A2 训练系列产品/Atlas A2 推理系列产品/Atlas A3 训练系列产品/Atlas A3 推理系列产品，dav-3510 对应 Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` |  `1`（默认）、`2`、`3`、`4` | 场景编号，分别对应int8_t / bfloat16 / float / int4b_t输入数据类型；`仅在CMAKE_ASC_ARCHITECTURES=dav-2201时支持设为4` |

- 执行结果

  执行结果如下，说明精度对比成功。

  ```bash
  test pass!
  ```
