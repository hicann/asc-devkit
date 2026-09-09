# MatMul样例

## 概述

本样例以 float 输入数据类型为例，演示如何通过C API实现矩阵乘法（C = A × B + Bias）。A、B矩阵均不转置，Host侧负责数据读写、内存管理和核函数启动，矩阵乘加全部在单个Cube Core上完成。

## 支持的产品及CANN软件版本

| 产品 | CANN 软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |

## 目录结构

```
├── matmul
│   ├── scripts
│   │   ├── gen_data.py             // 输入数据和真值数据生成脚本
│   │   └── verify_result.py        // 精度验证脚本
│   ├── CMakeLists.txt              // 编译工程文件
│   ├── data_utils.h                // 数据读写函数
│   ├── matmul.asc                  // 样例主文件（核函数实现、Host侧main函数）
│   ├── README_en.md                // 英文样例说明文档
│   └── README.md                   // 样例说明文档
```

## 样例描述

矩阵乘法计算公式：C = A × B + Bias，本样例的矩阵乘规格为 [M, N, K] = [30, 70, 40]，核函数名为 `matmul_custom`。输入输出规格如下表所示。

| 参数 | 类型 | 数据类型 | Shape | 是否转置 |
|------|------|----------|-------|----------|
| `a` | 输入 | float | [30, 40] | 不转置 |
| `b` | 输入 | float | [40, 70] | 不转置 |
| `bias` | 输入 | float | [70] | - |
| `c` | 输出 | float | [30, 70] | - |

一次完整的矩阵乘法涉及的数据搬运过程，以及各存储单元的数据排布格式如下表所示。

| 阶段 | 数据搬运 | 使用的API | 数据排布格式 |
|------|----------|-----------|--------------|
| 1 | GM → L1 | `asc_set_gm2l1_nz_para` + `asc_copy_gm2l1_nd2nz` / `asc_copy_gm2l1_dn2nz` | GM上A、B矩阵为ND排列，L1上为Nz排列 |
| 2 | L1 → L0A / L0B / BT | `asc_copy_l12l0a`、`asc_copy_l12l0b`、`asc_copy_l12bt` | L0A上A矩阵为Nz排列，L0B上B矩阵为Zn排列，Bias为shape为[N]的一维Tensor |
| 3 | 矩阵乘加 | `asc_mmad` | C矩阵在L0C上为Nz排列，初始值来源于BT |
| 4 | L0C → GM | `asc_set_l0c2gm_nz2nd` + `asc_copy_l0c2gm` | L0C上C矩阵为Nz排列，GM上为ND排列 |

L1、L0A、L0B、L0C上的Buffer大小均按分形对齐要求计算，Mmad计算中包含了补齐的无效数据，通过在`asc_copy_l0c2gm`中设置`m_size = M`、`n_size = N`保证补齐区域的计算结果不会被搬出。另外，`asc_copy_gm2l1_nd2nz` / `asc_copy_gm2l1_dn2nz`只在K方向补零，A、B在M轴、N轴上的对齐补齐行不会被写入，本样例调用`asc_fill_l1`将其显式清零，避免未初始化数据进入Cube参与Mmad。

搬运至BiasTable的数据量需满足64字节对齐，且整个对齐区间都会被读取，因此Host侧先将bias补零到对齐后的大小再拷贝至Device，避免未初始化数据经L1进入BiasTable。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行算子。

- 配置环境变量
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}`为CANN包安装目录；root用户默认为`/usr/local/Ascend`，非root用户默认为`${HOME}/Ascend`。

- 样例执行

  ```bash
  mkdir -p build && cd build
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 .. && make -j
  python3 ../scripts/gen_data.py
  ./demo
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin
  ```

  使用NPU仿真模式时，添加 `-DCMAKE_ASC_RUN_MODE=sim` 参数：

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 .. && make -j
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`sim` | 运行模式：NPU 运行、NPU 仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510`（默认） | NPU 架构，对应 Ascend 950PR/Ascend 950DT |

- 执行结果

  精度对比成功时的输出：

  ```bash
  test pass!
  ```
