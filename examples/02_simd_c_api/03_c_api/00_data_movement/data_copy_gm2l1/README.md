# data_copy_gm2l1样例

## 概述

本样例介绍如何使用Ascend C C API将输入数据从GM（Global Memory）搬运到L1（L1 Buffer），支持多种输入格式（Nz、ND、DN）和向量量化参数搬入等功能。数据搬入后，样例继续完成L1 Buffer到L0A Buffer和L0B Buffer的数据搬运、矩阵乘计算，以及通过Fixpipe将L0C Buffer中的计算结果搬运到GM（Global Memory）。

本样例适用于Ascend 950PR/Ascend 950DT（`dav-3510`），可在NPU运行模式或NPU仿真模式下执行，不提供CPU域调试模式。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |

## 目录结构介绍

```
├── data_copy_gm2l1
│   ├── figures                        // 图示
│   ├── scripts
│   │   ├── gen_data.py                // 输入数据和真值数据生成脚本
│   │   └── verify_result.py           // 验证输出数据和真值数据是否一致的验证脚本
│   ├── data_utils.h                   // 数据读入写出函数
│   ├── CMakeLists.txt                 // cmake编译文件
│   ├── data_copy_gm2l1.asc            // C API样例实现及调用代码
│   ├── README.md                      // 中文样例说明文档
│   └── README_en.md                   // 英文样例说明文档
```

## 场景详细说明

本样例通过编译参数`SCENARIO_NUM`选择不同的输入场景，`SCENARIO_NUM`不同取值对应的含义如下表所示。所有场景基于相同的矩阵乘规格：[M, K, N] = [128, 128, 256]，核函数名为`data_copy_gm2l1`。

**表1：SCENARIO_NUM不同取值的含义**

| SCENARIO_NUM | 输入格式 | 输入数据类型 | 输出数据类型 | 是否使能Vector量化 |
|---|---|---|---|---|
| 1 | Nz | half | float | 否 |
| 2 | ND | half | float | 否 |
| 3 | DN | half | float | 否 |
| 4 | ND | half | int8_t | 是 |

`SCENARIO_NUM`由CMake作为编译期宏传入；Kernel使用`if constexpr`选择对应场景。切换场景后，需要重新编译。

设备侧搬运和计算均使用Ascend C C API异步接口，通过`asc_sync_notify`和`asc_sync_wait`建立`MTE2→MTE1→M→FIX`的必要依赖；场景4还使用`MTE2→FIX`保证量化参数就绪，并在量化参数从L1 Buffer搬运到Fixpipe Buffer后通过`asc_sync_pipe(PIPE_FIX)`等待该搬运完成，随后通过Fixpipe将L0C Buffer中的计算结果搬运到GM。核函数结束时调用`asc_sync_pipe(PIPE_ALL)`，确保全部流水完成。

**场景1：输入格式Nz，输入数据类型half**

- 输入：A [128, 128] half类型，Nz格式；B [128, 256] half类型，Nz格式
- 输出：C [128, 256] float类型，ND格式
- 实现：使用`asc_copy_gm2l1`将Nz格式的输入数据从GM搬运到L1
- 说明：输入数据为Nz格式，直接搬运到L1，无需格式转换
<p align="center">
  <img src="figures/data_copy_gm2l1_nz2nz.png" width="800">
</p>

**场景2：输入格式ND，输入数据类型half**

- 输入：A [128, 128] half类型，ND格式；B [128, 256] half类型，ND格式
- 输出：C [128, 256] float类型，ND格式
- 实现：使用`asc_set_gm2l1_nz_para`配置ND2NZ参数，再使用`asc_copy_gm2l1_nd2nz`将ND格式的输入数据从GM搬运到L1，并转换为Nz格式
- 说明：输入数据为ND格式，在搬运过程中自动转换为Nz格式
<p align="center">
  <img src="figures/data_copy_gm2l1_nd2nz.png" width="800">
</p>

**场景3：输入格式DN，输入数据类型half（仅Ascend 950PR/Ascend 950DT支持）**

- 输入：A [128, 128] half类型，DN格式；B [128, 256] half类型，DN格式
- 输出：C [128, 256] float类型，ND格式
- 实现：使用`asc_set_gm2l1_nz_para`配置DN2NZ参数，再使用`asc_copy_gm2l1_dn2nz`将DN格式的输入数据从GM搬运到L1，并转换为Nz格式
- 说明：输入数据为DN格式，在搬运过程中自动转换为Nz格式
<p align="center">
  <img src="figures/data_copy_gm2l1_dn2nz.png" width="800">
</p>

**场景4：输入格式ND，输入数据类型half，使能Vector量化**

- 输入：A [128, 128] half类型，ND格式；B [128, 256] half类型，ND格式；量化参数 [256] uint64_t类型
- 输出：C [128, 256] int8_t类型，ND格式
- 实现：使用`asc_set_gm2l1_nz_para`配置ND2NZ参数，再使用`asc_copy_gm2l1_nd2nz`将输入数据从GM搬运到L1，执行矩阵乘计算；使用`asc_copy_gm2l1`和`asc_copy_l12fb`将量化参数从GM经L1搬运到Fixpipe Buffer；调用`asc_set_l0c2gm_config`配置参数地址，并通过`asc_copy_l0c2gm`的`VQF322B8_PRE`模式进行Vector量化输出
- 说明：矩阵乘计算结果为float类型，随后通过Vector量化转换为int8_t类型；C矩阵的每一列使用一个量化参数。`asc_set_l0c2gm_config`通过`quant_pre`配置Fixpipe矢量量化参数地址，该参数以128B为地址单位，因此量化参数文件需按128B向上对齐，不足部分补0。本样例的`[256] uint64_t`参数共2048B，已满足该要求。量化参数从L1搬运至Fixpipe Buffer时，需要搬运2048B数据，数据搬运单位为64B，对应`asc_copy_l12fb`的`len_burst`配置为32。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

- 配置环境变量
  请根据当前环境上CANN开发套件包的[安装方式](../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}`为CANN包安装目录；root用户默认为`/usr/local/Ascend`，非root用户默认为`${HOME}/Ascend`。

- 样例执行

  在本样例目录下执行如下命令。

  ```bash
  SCENARIO_NUM=1
  mkdir -p build && cd build;      # 创建并进入build目录
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;    # 编译工程，默认NPU模式
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM   # 生成测试输入数据
  ./demo                           # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py output/output.bin ./output/golden.bin  # 验证输出结果是否正确
  ```

  将`SCENARIO_NUM`和`-scenarioNum`同时设置为1至4，即可运行对应场景。

  使用NPU仿真模式时，添加`-DCMAKE_ASC_RUN_MODE=sim`参数即可。

  示例：

  ```bash
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换运行模式前需清理CMake缓存，可在build目录下执行`rm CMakeCache.txt`后重新执行CMake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`sim` | 运行模式：NPU运行、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU架构：对应Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | 1-4 | 场景编号 |

- 执行结果

  场景1-3的浮点结果满足精度要求，或场景4的int8_t结果与真值完全一致时，执行结果如下，说明精度对比成功。

  ```bash
  test pass!
  ```
