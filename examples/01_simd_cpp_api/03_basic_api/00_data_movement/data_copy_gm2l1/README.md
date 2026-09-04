# data_copy_gm2l1样例

## 概述

本样例介绍如何使用DataCopy将输入数据从GM（Global Memory）搬出到L1（L1 Buffer），支持多种输入格式（Nz、ND、DN）、Bias矩阵搬入、量化矩阵搬入等功能。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | >= CANN 9.2.0 |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | >= CANN 9.2.0 |

## 目录结构介绍

```
├── data_copy_gm2l1
│   ├── figures                        // 图示
│   ├── scripts
│   │   ├── gen_data.py                // 输入数据和真值数据生成脚本
│   │   └── verify_result.py           // 验证输出数据和真值数据是否一致的验证脚本
│   ├── data_utils.h                   // 数据读入写出函数
│   ├── CMakeLists.txt                 // cmake编译文件
│   ├── data_copy_gm2l1.asc            // Ascend C样例实现 & 调用样例
│   ├── README.md                      // 中文样例说明文档
│   └── README_en.md                   // 英文样例说明文档
```

## 场景详细说明

本样例通过编译参数 `SCENARIO_NUM` 选择不同的输入场景, SCENARIO_NUM不同取值对应的含义如下表所示。所有场景基于相同的矩阵乘规格：[M, K, N] = [128, 128, 256]，核函数名为 `data_copy_gm2l1`。

<table>
<caption style="font-weight: normal;">
  	     <span style="font-weight: bold; font-size: 1.2em;">📌 表1：scenarioNum不同取值的含义</span></caption>
<tr><td rowspan="1" align="center">scenarioNum</td><td align="center">输入格式</td><td align="center">输入数据类型</td><td align="center">输出数据类型</td><td align="center">是否使能Bias</td><td align="center">是否使能Vector量化</td></tr>
<tr><td align="center">1</td><td align="center">Nz</td><td align="center">half</td><td align="center">float</td><td align="center">否</td><td align="center">否</td></tr>
<tr><td align="center">2</td><td align="center">ND</td><td align="center">half</td><td align="center">float</td><td align="center">否</td><td align="center">否</td></tr>
<tr><td align="center">3</td><td align="center">DN</td><td align="center">half</td><td align="center">float</td><td align="center">否</td><td align="center">否</td></tr>
<tr><td align="center">4</td><td align="center">ND</td><td align="center">half</td><td align="center">float</td><td align="center">是</td><td align="center">否</td></tr>
<tr><td align="center">5</td><td align="center">ND</td><td align="center">half</td><td align="center">int8_t</td><td align="center">否</td><td align="center">是</td></tr>
</table>

**场景1：输入格式Nz，输入数据类型half**
- 输入：A [128, 128] half类型，Nz格式；B [128, 256] half类型，Nz格式
- 输出：C [128, 256] float类型，ND格式
- 实现：使用 `DataCopyParams` 将NZ格式的输入数据从GM搬运到L1
- 说明：输入数据为Nz格式，直接搬运到L1，无需格式转换
<p align="center">
  <img src="figures/data_copy_gm2l1_NZ2NZ.png" width="800">
</p>

**场景2：输入格式ND，输入数据类型half**
- 输入：A [128, 128] half类型，ND格式；B [128, 256] half类型，ND格式
- 输出：C [128, 256] float类型，ND格式
- 实现：使用 `Nd2NzParams` 将ND格式的输入数据从GM搬运到L1，自动转换为Nz格式
- 说明：输入数据为ND格式，在搬运过程中自动转换为Nz格式
<p align="center">
  <img src="figures/data_copy_gm2l1_ND2NZ.png" width="800">
</p>

**场景3：输入格式DN，输入数据类型half（仅Ascend 950PR/Ascend 950DT支持）**
- 输入：A [128, 128] half类型，DN格式；B [128, 256] half类型，DN格式
- 输出：C [128, 256] float类型，ND格式
- 实现：使用 `Dn2NzParams` 将DN格式的输入数据从GM搬运到L1，自动转换为Nz格式
- 说明：输入数据为DN格式，在搬运过程中自动转换为Nz格式，DN格式仅Ascend 950PR/Ascend 950DT支持
<p align="center">
  <img src="figures/data_copy_gm2l1_DN2NZ.png" width="800">
</p>

**场景4：输入格式ND，输入数据类型half，使能Bias**
- 输入：A [128, 128] half类型，ND格式；B [128, 256] half类型，ND格式；Bias [256] float类型
- 输出：C [128, 256] float类型，ND格式
- 实现：使用 `Nd2NzParams` 将输入数据和Bias从GM搬运到L1，执行矩阵乘计算时通过 `Mmad(c, a, b, bias, mmadParams)` 加入Bias
- 说明：在矩阵乘计算过程中加入Bias，Bias通过C1搬运到C2，然后参与矩阵乘计算

**场景5：输入格式ND，输入数据类型half，使能Vector量化**
- 输入：A [128, 128] half类型，ND格式；B [128, 256] half类型，ND格式；量化参数 [256] uint64_t类型
- 输出：C [128, 256] int8_t类型，ND格式
- 实现：使用 `Nd2NzParams` 将输入数据从GM搬运到L1，执行矩阵乘计算，量化参数通过 `DataCopy` 从GM搬运到L1，然后使用 `Fixpipe<outputType, l0cType, AscendC::CFG_ROW_MAJOR>(cGM, c, quantAlphaTensor, fixpipeParams)` 进行Vector量化输出
- 说明：矩阵乘计算结果为float类型，通过Vector量化转换为int8_t类型，C矩阵的每一列对应一个量化参数

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
  | `SCENARIO_NUM` | 1-5 | 场景编号，场景3仅支持dav-3510架构 |

- 执行结果

  执行结果如下，说明精度对比成功。
  ```bash
  test pass!
  ```
