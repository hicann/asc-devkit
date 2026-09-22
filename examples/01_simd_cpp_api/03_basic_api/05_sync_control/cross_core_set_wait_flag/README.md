# CrossCoreSetFlag和CrossCoreWaitFlag核间同步样例

## 概述
本样例首先介绍核间同步接口CrossCoreSetFlag和CrossCoreWaitFlag支持的三种同步模式(详见下表)，随后分别在纯 Vector 计算场景以及Cube 与 Vector 融合计算场景这两个实际业务中，说明上述三种同步模式的具体使用方法。
<table border="1" align="center">
  <tr bgcolor="lightgray">
    <td>同步控制模式</td>
    <td align="center">说明</td>
  </tr>
  <tr>
    <td rowspan="2">mode 0</td>
    <td>对于AIC场景，同步所有的AIC核，直到所有的AIC核都执行到CrossCoreSetFlag时，CrossCoreWaitFlag后续的指令才会执行。</td>
  </tr>
  <tr>
    <td>对于AIV场景，同步所有的AIV核，直到所有的AIV核都执行到CrossCoreSetFlag时，CrossCoreWaitFlag后续的指令才会执行。</td>
  </tr>
  <tr>
    <td>mode 1</td>
    <td>单个AI Core内部，AIV核之间的同步控制。如果两个AIV核都运行了CrossCoreSetFlag，CrossCoreWaitFlag后续的指令才会执行。</td>
  </tr>
  <tr>
    <td rowspan="2">mode 2</td>
    <td>在AIC核执行CrossCoreSetFlag之后， 两个AIV上CrossCoreWaitFlag后续的指令才会继续执行。</td>
  </tr>
  <tr>
    <td>两个AIV都执行CrossCoreSetFlag后，AIC上CrossCoreWaitFlag后续的指令才能执行。</td>
  </tr>
</table>

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | >= CANN 9.0.0 |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```
├── cross_core_set_wait_flag
│   ├── scripts
│   │   ├── gen_data.py              // 输入数据和真值数据生成脚本
│   │   └── verify_result.py         // 验证输出数据和真值数据是否一致的验证脚本
│   ├── CMakeLists.txt               // 编译工程文件
│   ├── data_utils.h                 // 数据读入写出函数
│   ├── figures                      // 图示
│   ├── cross_core_set_wait_flag.h   // Ascend C样例实现
│   ├── cross_core_set_wait_flag.asc // 调用样例以及结果校验
│   └── README.md                    // 样例说明文档
```

## 样例描述
<table border="1" style="text-align: center;">
  <tr>
    <td>SCENARIO_NUM取值</td>
    <td>业务场景</td>
    <td>使用的同步模式</td>
  </tr>
  <tr>
    <td>0</td>
    <td>纯 Vector 计算场景(16个AIV)</td>
    <td>mode 0（AIV全核同步）</td>
  </tr>
  <tr>
    <td>1</td>
    <td>纯 Vector 计算场景(2个AIV)</td>
    <td>mode 1</td>
  </tr>
  <tr>
    <td>2</td>
    <td>Cube 与 Vector 融合计算场景</td>
    <td>mode 2（AIC等AIV）、mode2（AIV等AIC）、mode0（AIC全核同步）</td>
  </tr>
</table>
本样例通过SCENARIO_NUM控制执行分支，该变量的不同取值对应不同的业务场景和同步模式。如上表所示，当 SCENARIO_NUM取不同值时，会分别演示纯 Vector 计算场景和 Cube 与 Vector 融合计算场景下三种同步模式的具体使用方法。

### 计算公式与样例规格

#### SCENARIO_NUM=0（纯 Vector 计算场景，模式0）
- 计算公式：  
  $$
  z = \sum_{i=0}^{15} (x \times i)
  $$
  - x 为输入向量，全为1
  - i 为每个AIV的BlockIdx（取值范围0-15）
  - z 为所有AIV计算结果的累加值

- 样例规格：
  <table border="1" align="center">
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center">CrossCoreSetFlagMode0</td></tr>
  <tr><td rowspan="2" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x</td><td align="center">[32]</td><td align="center">float32</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">z</td><td align="center">[32]</td><td align="center">float32</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">cross_core_set_wait_flag_custom</td></tr>
  </table>

#### SCENARIO_NUM=1（纯 Vector 计算场景，模式1）
- 计算公式：  
  $$
  z = (x \times 2) + (x \times 3)
  $$
  - x 为输入向量，全为1
  - 仅BlockIdx=2和3的AIV参与计算
  - z 为这两个AIV计算结果的累加值

- 样例规格：
  <table border="1" align="center">
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center">CrossCoreSetFlagMode1</td></tr>
  <tr><td rowspan="2" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x</td><td align="center">[32]</td><td align="center">float32</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">z</td><td align="center">[32]</td><td align="center">float32</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">cross_core_set_wait_flag_custom</td></tr>
  </table>

#### SCENARIO_NUM=2（Cube 与 Vector 融合计算场景）
- 计算公式：  
  $$
  C = \text{LeakyRelu}(Cast(A) \times Cast(B))
  $$
  - A 为左矩阵，形状为[M, K]，数据类型为uint8
  - B 为右矩阵，形状为[K, N]，数据类型为uint8
  - 首先将A和B数据类型从uint8转换为half
  - 然后执行矩阵乘：A × B
  - 最后对结果执行LeakyRelu运算
  - C 为最终结果，形状为[M, N]，数据类型为float32

- 样例规格：
  <table border="1" align="center">
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="5" align="center">CrossCoreSetFlagMode2</td></tr>
  <tr><td rowspan="3" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td><td align="center">isTrans</td></tr>
  <tr><td align="center">a</td><td align="center">[32, 32]</td><td align="center">uint8</td><td align="center">ND</td><td align="center">false</td></tr>
  <tr><td align="center">b</td><td align="center">[32, 64]</td><td align="center">uint8</td><td align="center">ND</td><td align="center">false</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">c</td><td align="center">[32, 64]</td><td align="center">float32</td><td align="center">ND</td><td align="center">-</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="5" align="center">mmad_custom</td></tr>
  </table>
## 样例实现
### 1. Cube与Vector融合计算场景
#### 1.1 整体逻辑
<p align="center">
  <img src="figures/融合场景_示意图.png" width="100%">
   </p>
<p align="center">
图1：Cube与Vector场景融合计算场景，整体计算逻辑示意图
</p>

本样例针对融合算子(配置__mix__(1,2))场景展开，每个AI Core内部有1个AIC和2个AIV。配置逻辑核数numBlocks = 8，对应 8 个 AIC 与 16 个 AIV。如图1所示，整体计算逻辑分为三个核心阶段：精度转换阶段、分块矩阵乘与原子累加阶段、LeakyRelu运算与结果回写阶段。
#### 1.2 精度转换阶段(模式 2，单个AI Core内部AIC等AIV)
由于GM上左右矩阵的数据类型为uint8,并不满足mmad指令对输入数据类型的要求，需要先将GM上的数据搬运到AIV中进行精度转换后，才能在AIC中进行分块矩阵乘计算。因此，
每个AI Core内部，一个AIC 需要等待该AI Core内部其它2 个 AIV 完成数据精度转换后，才能开始分块矩阵乘计算。
具体来说：将 GM 中左矩阵(A矩阵)数据沿 K 轴切分 8 份，分配至 BlockIdx 为偶数的 AIV 中完成 uint8 到 half 的精度转换；将 GM 中右矩阵(B矩阵)数据沿 K 轴切分 8 份，分配至 BlockIdx 为奇数的 AIV 中完成 uint8 到 half 的精度转换。如图2所示，根据以上描述，需要使用核间同步模式2。
上述描述对应的代码段如下：

        if (blockIdx % 2 == 0) {
            ...
            AscendC::Cast(castALocal, aLocal, AscendC::RoundMode::CAST_NONE, A_BLOCKS_LENGTH);
            ...
            AscendC::DataCopy(AVectorGM, castALocal, A_BLOCKS_LENGTH);
        } else {
            ...
            AscendC::Cast(castBLocal, bLocal, AscendC::RoundMode::CAST_NONE, B_BLOCKS_LENGTH);
            ...
            AscendC::DataCopy(BVectorGM, castBLocal, B_BLOCKS_LENGTH);
        }
        // 模式2,每一个AICore内部，一个AIC等2个AIV
        AscendC::CrossCoreSetFlag<2, PIPE_MTE3>(SYNC_AIV_AIC_FLAG);

<p align="center">
  <img src="figures/融合场景_精度转换阶段.png" width="100%">
   </p>
<p align="center">
图2：精度转换阶段，模式2示意图
</p>

#### 1.3 分块矩阵乘与原子累加阶段(模式 0，AIC全核同步)
每个 AIC 执行分块矩阵乘计算后，开启原子累加求和机制，将计算结果搬运至同一块 GM 区域，将8个AIC各自分块矩阵乘的结果在GM上进行累加，最终得到完整的C矩阵。为了获取正确的C矩阵，需要等待8个AIC都完成分块矩阵乘计算并且将结果通过FixPipe搬运到GM上。如图3所示，根据以上描述，需要使用核间同步模式0（AIC全核同步）。上述描述对应的代码段如下：
$$
C = \sum_{i=1}^{8} A_i \cdot B_i
$$

        // 模式2,每一个AICore内部，AIC等2个AIV
        AscendC::CrossCoreWaitFlag<2>(SYNC_AIV_AIC_FLAG);

        CopyIn(a1Local, b1Local);
        SplitA(a1Local, a2Local);
        SplitBTranspose(b1Local, b2Local);
        Compute(a2Local, b2Local, c1Local);
        CopyOut(c1Local);
        // 模式0,8个AICore 包含的8个AIC同步
        AscendC::CrossCoreSetFlag<0, PIPE_FIX>(SYNC_AIC_FLAG);  
        AscendC::CrossCoreWaitFlag<0>(SYNC_AIC_FLAG);

        // 模式2,每一个AICore内部，2个AIV等AIC
        AscendC::CrossCoreSetFlag<2, PIPE_FIX>(SYNC_AIC_AIV_FLAG);  

<p align="center">
  <img src="figures/融合场景_分块矩阵乘与原子累加阶段.png" width="100%">
   </p>
<p align="center">
图3：分块矩阵乘与原子累加阶段，模式0示意图
</p>

#### 1.4 LeakyRelu与结果回写阶段(模式 2，2个AIV等AIC)
每个 AI Core 内部，2 个 AIV 需等待 AIC 完成分块矩阵乘及原子累加操作后，才能对C矩阵分块执行LeakyRelu运算。
具体来说是对累加得到的C矩阵，沿 M 轴切分 16 份，分配至 16 个 AIV 中分别执行 LeakyRelu 运算。如图4所示，根据以上描述，需要使用核间同步模式2（单个AI Core内部2个AIV等一个AIC）。上述描述对应的代码段如下：

        // 模式2,每一个AICore内部，2个AIV等AIC
        AscendC::CrossCoreWaitFlag<2>(SYNC_AIC_AIV_FLAG);

        // 进行LeakyRelu运算
        float alpha = 0.001;
        ...
        AscendC::LeakyRelu(reluCLocal, cLocal, alpha, C_AIV_BLOCKS_LENGTH);
        ...

<p align="center">
  <img src="figures/融合场景_LeakyRelu运算与结果回写阶段.png" width="100%">
   </p>
<p align="center">
图4：LeakyRelu运算与结果回写阶段，模式2示意图
</p>

### 2. 纯Vector计算场景
#### 2.1 模式0和模式1的对比
本样例设置NUM\_BLOCKS为8(8个AI Core)，每一个AI Core内AIC与AIV配比为1:2，即本样例总共起了8个AIC和16个AIV，AIV的BlockIdx范围为0\~15。
如下图5所示，本样例中模式0和模式1计算逻辑几乎相同，区别仅在于参与同步的AIV数目：模式0时，全部的16个AIV参与同步；模式1时，仅有第二个AI Core中的2个AIV(BlockIdx=2和3)参与同步。因此，下一节将详细介绍模式0的整体逻辑，模式1就不再详细介绍。
<p align="center">
  <img src="figures/纯aiv_模式1和模式0的区别.png" width="100%">
</p>
<p align="center">
图5：纯AIV场景，模式0与模式1的区别示意图
</p>

#### 2.2 模式0的整体逻辑
本样例用到的GM分为2块，一块用于存储输入数据(initialDataGm)，一块用于存储所有AIV的累加结果(atomicResultGm)。
如下图6所示，模式0的整体逻辑分为以下步骤：
(1) 每个AIV均从initialDataGm上搬运全为1的数据到UB上，即为PIPE\_MTE2的流水操作。

(2) 每个AIV对UB上的数据进行矢量计算：通过Muls指令乘以每个核对应的BlockIdx，即为PIPE\_V的流水操作。

(3) 步骤 3 对应代码片段如下，atomicResultGm 用于存储 16 个 AIV 全部搬运完成后的累加结果。通过调用 CrossCoreSetFlag 与 CrossCoreWaitFlag 接口，实现同步控制：确保 16 个 AIV 均完成 PIPE_MTE3 搬运指令后，方可执行 CrossCoreWaitFlag 后续指令

         // UB 到 GM 搬运启用原子累加：搬运至 atomicResult 的数据与原值累加后覆盖原值
        AscendC::SetAtomicAdd<float>(); 
        // DataCopy属于PIPE_MTE3流水操作
        AscendC::DataCopy(atomicResultGm, xLocal, this->blockLength);   
        // 当本AIV完成前置PIPE_MTE3(DataCopy)流水操作后，通知其他AIV核，本AIV已经完成
        AscendC::CrossCoreSetFlag<0, PIPE_MTE3>(0);  
        // 阻塞本AIV继续往下执行指令，直到其他AIV全部都完成PIPE_MTE3流水操作，才解除阻塞往下执行。
        AscendC::CrossCoreWaitFlag<0>(0);

上述同步完成之后，此时atomicResultGm已经是16个AIV中矢量计算结果的累加值。
如果上一步骤的同步插入不正确，那么从atomicResultGm往AIV搬运的数据可能是部分AIV中矢量计算结果的累加值，导致结果不准确。

        if (AscendC::GetBlockIdx() == 0) {
            AscendC::DataCopy(yLocal, atomicResultGm, this->blockLength);   // PIPE_MTE2
            return;
        }
<p align="center">
  <img src="figures/纯aiv_模式0示意图.png" width="100%">
</p>
<p align="center">
图6：纯AIV场景，模式0计算逻辑示意图
</p>

### 3. 注意事项
#### 3.1 Cube与Vector融合计算场景
(1) Cube与Vector融合计算场景中，融合算子(配置__mix__(1,2))中需要通过ASCEND_IS_AIV/ ASCEND_IS_AIC进行AIV和AIC核代码的隔离。
```
KernelMmad op;
if ASCEND_IS_AIC {
    op.InitAIC(A, B, c);
    op.ProcessAIC();
} 
if ASCEND_IS_AIV {
    op.InitAIV(a, b, A, B, c);
    op.ProcessAIV();
}
```
(2) GetBlockIdx(获取当前核的index)在AIC和AIV的取值范围不同，其取值与算子设置的逻辑核数和一个AI Core中的AIC与AIV的比例有关。本样例中设置NUM_BLOCKS=8、AIC与AIV的比例为1:2，因此GetBlockIdx在AIC和AIV的取值范围分别为0-7和0-15。

(3) 样例采用静态Tensor编程范式需要手动插入核内同步。另外，静态Tensor编程方式中需要开发者手动调用InitSocState()接口初始化全局状态寄存器。

#### 3.2 纯Vector计算场景
(1) 在使用 CrossCoreSetFlag 与 CrossCoreWaitFlag 核间同步接口时，即使是纯 Vector 计算场景，核函数也不能使用__vector__ 修饰符。
本样例核函数采用__mix__(1,2) 修饰，但在纯 Vector 场景下，由于仅执行矢量计算，必须通过 ASCEND_IS_AIV 宏确保程序仅在 AIV 核上运行，否则会导致程序卡死。
```
if ASCEND_IS_AIV {
        op.Init(x, z, dataLength);
        op.Process();
}
```
 (2) 模式 1 要求参与同步的 2个AIV 必须属于同一个AI Core，否则程序会卡死。本样例中，参与同步的两个AIV的 GetBlockIdx=2、3 同属第 2 个 AI Core(下标从 1 开始)；若将GetBlockIdx改为 3 和 4(分别属于两个不同的AI Core)，程序将卡死。

 (3) 样例采用静态Tensor编程范式需要手动插入核内同步。另外，静态Tensor编程方式中需要开发者手动调用InitSocState()接口初始化全局状态寄存器。

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
  SCENARIO_NUM=0  # 设置场景编号（取值为0、1、2）
  mkdir -p build && cd build;      # 创建并进入build目录
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM;make -j;    # 编译工程
  python3 ../scripts/gen_data.py -scenarioNum $SCENARIO_NUM   # 生成测试输入数据
  ./demo                           # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py ./output/output.bin ./output/golden.bin  # 验证输出结果是否正确
  ```

  使用CPU调试或NPU仿真模式时，添加 `-DCMAKE_ASC_RUN_MODE=cpu` 或 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。
  
  示例如下：
  ```bash
  cmake .. -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM;make -j; # CPU调试模式
  cmake .. -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM;make -j; # NPU仿真模式
  ```
  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU 运行、CPU调试、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU 架构：dav-2201 对应 Atlas A2 训练系列产品/Atlas A2 推理系列产品和 Atlas A3 训练系列产品/Atlas A3 推理系列产品，dav-3510 对应 Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | `0`（默认）、`1`、`2` | 场景编号：0（纯Vector模式0）、1（纯Vector模式1）、2（Cube+Vector融合） |

- 执行结果

  执行结果如下，说明精度对比成功。
  ```bash
  test pass!
  ```
