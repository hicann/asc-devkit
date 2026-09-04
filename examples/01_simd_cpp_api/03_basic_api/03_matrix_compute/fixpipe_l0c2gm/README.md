# fixpipe_l0c2gm样例

## 概述

本样例介绍如何使用Fixpipe将矩阵乘的结果从L0C Buffer搬出到GM（Global Memory），支持多种输出格式（Nz、ND、DN）、数据类型转换、随路量化、ReLU以及ChannelSplit等功能。这些接口用于将L0C中的矩阵乘计算结果高效地传输到全局内存，并支持各种数据格式转换和预处理能力。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | >= CANN 9.2.0 |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | >= CANN 9.2.0 |

## 目录结构介绍

```
├── fixpipe_l0c2gm
│   ├── figures                        // 图示
│   ├── scripts
│   │   ├── gen_data.py                // 输入数据和真值数据生成脚本
│   │   └── verify_result.py           // 验证输出数据和真值数据是否一致的验证脚本
│   ├── CMakeLists.txt                 // 编译工程文件
│   ├── data_utils.h                   // 数据读入写出函数
│   ├── fixpipe_l0c2gm.asc             // Ascend C样例实现 & 调用样例
│   └── README.md                      // 样例说明文档
```

## FixpipeParamsV220 与 FixpipeParamsArch3510 结构体对比

不同产品支持的参数结构体不同：
- **Ascend 950PR/Ascend 950DT**：支持 `FixpipeParamsV220` 和 `FixpipeParamsArch3510` 两种参数结构体，推荐使用 `FixpipeParamsArch3510`
- **Atlas A3 训练/推理系列、Atlas A2 训练/推理系列**：仅支持 `FixpipeParamsV220`

本样例通过编译参数 `CMAKE_ASC_ARCHITECTURES` 选择不同架构，根据架构自动选择对应的参数结构体：
- `dav-2201` 架构：使用 `FixpipeParamsV220`
- `dav-3510` 架构：使用 `FixpipeParamsArch3510`

<a name="表1"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">📌 表1：参数结构体对比</span></caption>
<tr><td rowspan="1" align="center">成员名</td><td align="center">FixpipeParamsV220</td><td align="center">FixpipeParamsArch3510</td><td align="center">说明</td></tr>
<tr><td align="center"><code>nSize</code></td><td align="center">✅</td><td align="center">✅</td><td>输出矩阵在N方向上的大小</td></tr>
<tr><td align="center"><code>mSize</code></td><td align="center">✅</td><td align="center">✅</td><td>输出矩阵在M方向上的大小</td></tr>
<tr><td align="center"><code>srcStride</code></td><td align="center">✅</td><td align="center">✅</td><td>源Nz矩阵中相邻Z排布的起始地址偏移</td></tr>
<tr><td align="center"><code>dstStride</code></td><td align="center">✅</td><td align="center">✅</td><td>目的矩阵中相邻Z排布的起始地址偏移（Nz格式）或每行元素个数（ND/DN格式）</td></tr>
<tr><td align="center"><code>quantPre</code></td><td align="center">✅</td><td align="center">✅</td><td>量化模式控制</td></tr>
<tr><td align="center"><code>deqScalar</code></td><td align="center">✅</td><td align="center">✅</td><td>scalar量化参数</td></tr>
<tr><td align="center"><code>reluEn</code></td><td align="center">✅</td><td align="center">✅</td><td>ReLU开关</td></tr>
<tr><td align="center"><code>unitFlag</code></td><td align="center">✅</td><td align="center">✅</td><td>Mmad与Fixpipe细粒度并行控制</td></tr>
<tr><td align="center"><code>isChannelSplit</code></td><td align="center">✅</td><td align="center">✅</td><td>通道拆分开关</td></tr>
<tr><td align="center"><code>ndNum</code> / <code>srcNdStride</code> / <code>dstNdStride</code></td><td align="center">✅</td><td align="center">✅（在<code>TransformParams</code>中）</td><td>NZ2ND场景下控制多矩阵传输的参数，在V220中为独立成员，在Arch3510中被整合到<code>TransformParams</code>结构体中</td></tr>
<tr><td align="center"><code>dnNum</code> / <code>srcNzMatrixStride</code> / <code>dstDnMatrixStride</code> / <code>srcNzC0Stride</code></td><td align="center">❌</td><td align="center">✅（在<code>TransformParams</code>中）</td><td>NZ2DN场景下控制多矩阵传输的参数，仅Arch3510支持</td></tr>
<tr><td align="center"><code>TransformParams</code></td><td align="center">❌</td><td align="center">✅</td><td>基于模板参数的类型选择器，根据CO2Layout自动选择参数类型</td></tr>
<tr><td align="center"><code>dualDstCtrl</code></td><td align="center">❌</td><td align="center">✅</td><td>双目标模式控制，支持M维度拆分或N维度拆分</td></tr>
<tr><td align="center"><code>subBlockId</code></td><td align="center">❌</td><td align="center">✅</td><td>单目标模式下指示目标UB的编号</td></tr>
</table>

## 场景详细说明

本样例通过编译参数 `SCENARIO_NUM` 选择不同的输出场景，SCENARIO_NUM不同取值对应的含义如下表所示。所有场景基于相同的矩阵乘规格：[M, N, K] = [128, 256, 128]，核函数名为 `fixpipe_l0c2gm`。

<a name="表2"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">📌 表2：scenarioNum不同取值的含义</span></caption>
<tr><td rowspan="1" align="center">scenarioNum</td><td align="center">L0C数据类型</td><td align="center">输出数据类型</td><td align="center">输出格式</td><td align="center">是否使能量化</td><td align="center">是否使能ReLU</td><td align="center">是否使能ChannelSplit</td></tr>
<tr><td align="center">1</td><td align="center">float</td><td align="center">float</td><td align="center">Nz</td><td align="center">否</td><td align="center">否</td><td align="center">否</td></tr>
<tr><td align="center">2</td><td align="center">float</td><td align="center">float</td><td align="center">ND</td><td align="center">否</td><td align="center">否</td><td align="center">否</td></tr>
<tr><td align="center">3</td><td align="center">float</td><td align="center">float</td><td align="center">DN</td><td align="center">否</td><td align="center">否</td><td align="center">否</td></tr>
<tr><td align="center">4</td><td align="center">float</td><td align="center">int8_t</td><td align="center">ND</td><td align="center">是</td><td align="center">否</td><td align="center">否</td></tr>
<tr><td align="center">5</td><td align="center">float</td><td align="center">int8_t</td><td align="center">ND</td><td align="center">是</td><td align="center">否</td><td align="center">否</td></tr>
<tr><td align="center">6</td><td align="center">float</td><td align="center">float</td><td align="center">ND</td><td align="center">否</td><td align="center">是</td><td align="center">否</td></tr>
<tr><td align="center">7</td><td align="center">float</td><td align="center">float</td><td align="center">Nz</td><td align="center">否</td><td align="center">否</td><td align="center">是</td></tr>
</table>

**场景1：输出格式Nz，输出数据类型float**
- 输入：A [128, 128] half类型，ND格式；B [128, 256] half类型，ND格式
- 输出：C [128, 256] float类型，Nz格式
- 实现：使用 `Fixpipe<outputType, l0cType, AscendC::CFG_NZ>` 将数据从L0C搬出到GM，输出为Nz格式
- 说明：L0C数据为Nz格式直接输出到GM的Nz格式，数据保持原格式不变
<p align="center">
  <img src="figures/fixpipe_l0c2gm_NZ2NZ.png" width="800">
</p>

**场景2：输出格式ND，输出数据类型float**
- 输入：A [128, 128] half类型，ND格式；B [128, 256] half类型，ND格式
- 输出：C [128, 256] float类型，ND格式
- 实现：使用 `Fixpipe<outputType, l0cType, AscendC::CFG_ROW_MAJOR>` 指定ROW_MAJOR格式转换
- 说明：将CO1中的Nz格式数据转换为ND格式输出到GM，ND格式没有Nz格式的对齐要求，输出时需要按实际大小配置参数
<p align="center">
  <img src="figures/fixpipe_l0c2gm_NZ2ND.png" width="800">
</p>

**场景3：输出格式DN，输出数据类型float（仅Ascend 950PR/Ascend 950DT支持）**
- 输入：A [128, 128] half类型，ND格式；B [128, 256] half类型，ND格式
- 输出：C [256, 128] float类型，DN格式
- 实现：使用 `Fixpipe<outputType, l0cType, AscendC::CFG_COLUMN_MAJOR>` 指定COLUMN_MAJOR格式转换
- 说明：将CO1中的Nz格式数据转换为DN格式输出到GM
<p align="center">
  <img src="figures/fixpipe_l0c2gm_NZ2DN.png" width="800">
</p>

**场景4：输出格式ND，输出数据类型int8_t，使能Scalar量化**
- 输入：A [128, 128] half类型，ND格式；B [128, 256] half类型，ND格式
- 输出：C [128, 256] int8_t类型，ND格式
- 实现：设置 `fixpipeParams.quantPre = QuantMode_t::QF322B8_PRE`，使用Scalar量化模式
- 说明：将float类型数据量化为int8_t类型，整个C矩阵使用一个量化参数

**场景5：输出格式ND，输出数据类型int8_t，使能Vector量化**
- 输入：A [128, 128] half类型，ND格式；B [128, 256] half类型，ND格式
- 输出：C [128, 256] int8_t类型，ND格式
- 实现：设置 `fixpipeParams.quantPre = QuantMode_t::VQF322B8_PRE`，使用Vector量化模式，并通过quantAlphaTensor传入每列的量化参数
- 说明：将float类型数据量化为int8_t类型，C矩阵的每一列对应一个量化参数，使用的量化参数需要从GM拷贝量化参数到L1

**场景6：输出格式ND，输出数据类型float，使能ReLU**
- 输入：A [128, 128] half类型，ND格式；B [128, 256] half类型，ND格式
- 输出：C [128, 256] float类型，ND格式
- 实现：设置 `fixpipeParams.reluEn = true` 开启ReLU功能
- 说明：在数据从L0C搬出到GM的过程中执行ReLU操作，即将负值置为0

**场景7：输出格式Nz，输出数据类型float，使能ChannelSplit**
- 输入：A [128, 128] half类型，ND格式；B [128, 256] half类型，ND格式
- 输出：C [128, 512] float类型，Nz格式（使能通道拆分）
- 实现：设置 `fixpipeParams.isChannelSplit = true` 开启ChannelSplit功能
- 说明：在数据从L0C搬出到GM的过程中开启通道拆分功能，即将16x16小z分形矩阵拆分成两个独立的16x8小z分形矩阵输出到GM，Fixpipe接口的输入输出必须均为float类型，且仅支持Nz格式

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
  SCENARIO_NUM=1
  mkdir -p build && cd build;      # 创建并进入build目录
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;    # 编译工程，默认npu模式
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM   # 生成测试输入数据
  ./demo                           # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py output/output.bin ./output/golden.bin  # 验证输出结果是否正确
  ```

  使用 CPU调试 或 NPU仿真 模式时，添加 `-DCMAKE_ASC_RUN_MODE=cpu` 或 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。
  
  示例如下：

  ```bash
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # CPU调试模式
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU仿真模式
  ```
  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU 运行、CPU调试、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU 架构：dav-2201 对应 Atlas A2 训练系列产品/Atlas A2 推理系列产品和 Atlas A3 训练系列产品/Atlas A3 推理系列产品，dav-3510 对应 Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | 1-7 | 场景编号 |

  执行结果如下，说明精度对比成功。

  ```bash
  test pass!
  ```
