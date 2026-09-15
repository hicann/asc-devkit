# asc_copy_l0c2ub样例

## 概述

本样例介绍如何使用SIMD C API `asc_copy_l0c2ub`将矩阵乘的结果从L0C Buffer搬出到UB（Unified Buffer），支持多种输出格式（Nz、ND）以及双目标模式（按M维度或N维度拆分）等功能。该接口用于将L0C Buffer中的矩阵乘计算结果高效地传输到统一缓存区，并支持各种数据格式转换和拆分能力。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |

## 目录结构介绍

```
├── asc_copy_l0c2ub
│   ├── scripts
│   │   ├── gen_data.py                // 输入数据和真值数据生成脚本
│   │   └── verify_result.py           // 验证输出数据和真值数据是否一致的验证脚本
│   ├── CMakeLists.txt                 // 编译工程文件
│   ├── data_utils.h                   // 数据读入写出函数
│   ├── figures                        // 图示
│   ├── asc_copy_l0c2ub.asc             // SIMD C API样例实现及调用样例
│   └── README.md                      // 样例说明文档
```

## 场景详细说明

本样例通过编译参数 `SCENARIO_NUM` 选择不同的输出场景，SCENARIO_NUM不同取值对应的含义如下表所示。所有场景基于相同的矩阵乘规格：[M, N, K] = [128, 256, 128]，核函数名为 `asc_copy_l0c2ub_custom`。

<a name="表1"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">📌 表1：scenarioNum不同取值的含义</span></caption>
<tr><td rowspan="1" align="center">scenarioNum</td><td align="center">L0C Buffer数据类型</td><td align="center">输出数据类型</td><td align="center">输出格式</td><td align="center">双目标模式</td><td align="center">拆分维度</td></tr>
<tr><td align="center">1</td><td align="center">float</td><td align="center">float</td><td align="center">Nz</td><td align="center">否</td><td align="center">-</td></tr>
<tr><td align="center">2</td><td align="center">float</td><td align="center">float</td><td align="center">ND</td><td align="center">否</td><td align="center">-</td></tr>
<tr><td align="center">3</td><td align="center">float</td><td align="center">float</td><td align="center">ND</td><td align="center">是</td><td align="center">M维度</td></tr>
<tr><td align="center">4</td><td align="center">float</td><td align="center">float</td><td align="center">ND</td><td align="center">是</td><td align="center">N维度</td></tr>
</table>

**场景1：输出格式Nz，输出数据类型float**
- 输入：A [128, 128] half类型，ND格式；B [128, 256] half类型，ND格式
- 输出：C [128, 256] float类型，Nz格式
- 实现：调用`asc_copy_l0c2ub`，设置`dual_dst_mode = DUAL_DST_DISABLE`且关闭Nz2ND，直接输出Nz格式
- 说明：L0C Buffer数据为Nz格式直接输出到UB的Nz格式，数据保持原格式不变

**场景2：输出格式ND，输出数据类型float**
- 输入：A [128, 128] half类型，ND格式；B [128, 256] half类型，ND格式
- 输出：C [128, 256] float类型，ND格式
- 实现：调用`asc_set_l0c_copy_nz_para(1, 0, 0)`，再调用`asc_copy_l0c2ub`并设置`enable_nz2nd = true`、`dual_dst_mode = DUAL_DST_DISABLE`
- 说明：将CO1中的Nz格式数据转换为ND格式输出到UB

**场景3：输出格式ND，输出数据类型float，使能双目标模式，按M维度拆分，同时写入两个子块（SUB BLOCK）的UB中**
- 输入：A [128, 128] half类型，ND格式；B [128, 256] half类型，ND格式
- 输出：单个子块上C [64, 256] float类型，ND格式（双目标模式，按M维度拆分，每个目标输出64行）
- 实现：调用`asc_copy_l0c2ub`时设置`dual_dst_mode = asc_dual_dst_mode::DUAL_DST_SPLIT_M`和`enable_nz2nd = true`，M必须为2的倍数
- 说明：使用双目标模式将数据拆分输出到UB，按M维度拆分的两个核各处理一半数据
<p align="center">
  <img src="figures/asc_copy_l0c2ub_split_m.png" width="500">
</p>

**场景4：输出格式ND，输出数据类型float，使能双目标模式，按N维度拆分，同时写入两个子块（SUB BLOCK）的UB中**
- 输入：A [128, 128] half类型，ND格式；B [128, 256] half类型，ND格式
- 输出：单个子块上C [128, 128] float类型，ND格式（双目标模式，按N维度拆分，每个目标输出128列）
- 实现：调用`asc_copy_l0c2ub`时设置`dual_dst_mode = asc_dual_dst_mode::DUAL_DST_SPLIT_N`和`enable_nz2nd = true`，N必须为32的倍数
- 说明：使用双目标模式将数据拆分输出到UB，按N维度拆分的两个核各处理一半数据
<p align="center">
  <img src="figures/asc_copy_l0c2ub_split_n.png" width="500">
</p>

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。
- 配置环境变量
  请根据当前环境上CANN开发套件包的[安装方式](../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录；root用户默认为`/usr/local/Ascend`，非root用户默认为`${HOME}/Ascend`。
- 样例执行

  在本样例目录下执行如下命令。
  ```bash
  SCENARIO_NUM=1
  mkdir -p build && cd build;      # 创建并进入build目录
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # 编译工程，默认npu模式
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM   # 生成测试输入数据
  ./demo                           # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py output/output.bin ./output/golden.bin  # 验证输出结果是否正确
  ```

  使用NPU仿真模式时，添加`-DCMAKE_ASC_RUN_MODE=sim`参数即可。
  
  示例如下：

  ```bash
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # NPU仿真模式
  ```
  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`sim` | 运行模式：NPU运行、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510`（默认） | NPU 架构，dav-3510 对应 Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | 1-4 | 场景编号 |

  执行结果如下，说明精度对比成功。

  ```bash
  test pass!
  ```
