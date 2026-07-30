# Mmad样例

## 概述

本样例以 int8_t 和 bfloat16 两种输入数据类型为例，演示如何通过C API实现矩阵乘法（C = A × B + Bias）。每个场景的流水线函数拆分到独立的头文件中（`mmad_s1.h` / `mmad_s2.h`），主文件 `mmad.asc` 通过 `if constexpr (scenario_num)` 在核函数入口处按场景分发。

## 支持的产品及CANN软件版本

| 产品 | CANN 软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |

## 目录结构

```
├── mmad
│   ├── figures                     // 图示
│   ├── scripts
│   │   ├── gen_data.py             // 输入数据和真值数据生成脚本
│   │   ├── mmad.py                 // 矩阵乘法辅助脚本
│   │   └── verify_result.py        // 精度验证脚本
│   ├── CMakeLists.txt              // 编译工程文件
│   ├── data_utils.h                // 数据读写函数
│   ├── mmad_s1.h                   // 场景1的常量与完整流水线函数
│   ├── mmad_s2.h                   // 场景2的常量与完整流水线函数
│   ├── mmad.asc                    // 样例主文件（公共常量、核函数入口、Host侧main函数）
│   ├── README_en.md                // 英文样例说明文档
│   └── README.md                   // 样例说明文档
```

## 样例描述

一次完整的矩阵乘法涉及的数据搬运过程包括：Global Memory -> L1 Buffer、L1 Buffer -> L0A / L0B Buffer、L1 Buffer -> BiasTable Buffer、L0C Buffer -> Global Memory。不同存储单元的数据排布格式如下表所示。

<a name="表1"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">表1：不同存储单元的数据排布格式</span></caption>
  <tr>
    <td>存储单元</td>
    <td>数据排布格式</td>
  </tr>
  <tr>
    <td>Global Memory（GM）</td>
    <td>输入 A、B 矩阵和输出 C 矩阵为 ND 排列。</td>
  </tr>
  <tr>
    <td>L1 Buffer（L1）</td>
    <td>A、B 矩阵为 Nz 排列。</td>
  </tr>
  <tr>
    <td>L0A Buffer（L0A）</td>
    <td>A 矩阵为 Nz 排列。</td>
  </tr>
  <tr>
    <td>L0B Buffer（L0B）</td>
    <td>B 矩阵为 Zn 排列。</td>
  </tr>
  <tr>
    <td>BiasTable Buffer（BT）</td>
    <td>Bias 是 shape 为 [N] 的一维 Tensor。</td>
  </tr>
  <tr>
    <td>L0C Buffer（L0C）</td>
    <td>C 矩阵为 Nz 排列。</td>
  </tr>
</table>

矩阵乘法计算公式：C = A × B + Bias，其中 A、B、Bias、C 矩阵需满足的 shape 分别为 [M, K]、[K, N]、[N] 和 [M, N]。Bias 仅在场景 1 中启用，其数据类型与 C 矩阵的对应关系如下表所示。

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
    <td>bfloat16_t</td>
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

编译参数 `SCENARIO_NUM` 的不同取值对应如下场景：

<a name="表3"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">表3：SCENARIO_NUM 不同取值的含义</span></caption>
  <tr>
    <td>SCENARIO_NUM</td>
    <td>输入数据类型</td>
    <td>输出数据类型</td>
    <td>A 矩阵</td>
    <td>B 矩阵</td>
    <td>Bias</td>
  </tr>
  <tr>
    <td>1</td>
    <td>int8_t</td>
    <td>int32_t</td>
    <td>不转置</td>
    <td>不转置</td>
    <td>带 Bias，C 矩阵初始值来源于 BT</td>
  </tr>
  <tr>
    <td>2</td>
    <td>bfloat16_t</td>
    <td>float</td>
    <td>不转置</td>
    <td>转置</td>
    <td>不带 Bias，C 矩阵初始值来源于 CO1（两次 Mmad 累加）</td>
  </tr>
</table>

### 场景详细说明

本样例通过编译参数 `SCENARIO_NUM` 选择场景，所有场景基于相同的矩阵乘规格：[M, N, K] = [30, 40, 70]，核函数名为 `mmad_custom`。

**场景 1：int8_t 输入，int32_t 输出，带 Bias**

- 输入：
  - A 不转置 [30, 70] int8_t，ND 格式；
  - B 不转置 [70, 40] int8_t，ND 格式；
  - Bias [1, 40] int32_t；
- 输出：C [30, 40] int32_t，ND 格式；
- 实现（详见 `mmad_s1.h`）：
  1. `asc_copy_gm2l1_nd2nz` + `asc_set_gm2l1_nz_para`：A、B、Bias 从 GM 搬运至 L1（ND → Nz）；
  2. `asc_copy_l12l0a`：A 从 L1 搬运至 L0A；`asc_copy_l12l0b_trans`：B 从 L1 转置搬运至 L0B；`asc_copy_l12bt`：Bias 从 L1 搬运至 BT；
  3. `asc_mmad`：矩阵乘加，C 矩阵初始值来源于 BT；
  4. `asc_copy_l0c2gm` + `asc_set_l0c2gm_nz2nd`：结果从 L0C 搬运至 GM（Nz → ND）；
- 说明：int8_t类型输入，B矩阵不转置场景下，N轴向2 * 16对齐，填充了全部是无效数据的32 * 16的分形。如下图1所示，如果设置`right_width = N`，就会导致读入编号为3、7的分形，同时又没能将包含有效数据的编号为9、10的分形读入。因此需要设置：`right_width = CeilAlign(N, BLOCK_CUBE * fractalNum)`，此时会读入全部分形，虽然矩阵计算结果中包含了无效数据参与计算的结果，但是在`asc_copy_l0c2gm`指令搬出数据时通过设置`n_size = N`来保证无效数据参与计算的结果不会被搬出。
<p align="center">
  <img src="figures/mmad_s8_L0B_转置.png" width="700">
</p>
<p align="center">
图1：int8_t类型，B不转置，N轴实际对齐要求与Mmad指令默认不一致
</p>

**场景 2：bfloat16_t 输入，float 输出，无 Bias，两次 Mmad 累加**

- 输入：
  - A 不转置 [30, 70] bfloat16_t，ND 格式；
  - B 转置 [40, 70] bfloat16_t，ND 格式；
- 输出：C [30, 40] float，ND 格式；
- 实现（详见 `mmad_s2.h`）：
  1. `asc_copy_gm2l1_nd2nz` + `asc_set_gm2l1_nz_para`：A、B 从 GM 搬运至 L1（B 以转置后的 ND 排布存放于 GM）；
  2. `asc_copy_l12l0a`：A 从 L1 搬运至 L0A；`asc_copy_l12l0b`：B 从 L1 直接搬运至 L0B（B 已在 L1 中转置为 Nz 排布）；
  3. `asc_mmad`：两次调用。首次 `c_matrix_init_val = true` 将 C 初始化为 0 并计算 A×B；第二次 `c_matrix_init_val = false`、`c_matrix_source = false` 以 CO1 为初值累加第二次 A×B；
  4. `asc_copy_l0c2gm` + `asc_set_l0c2gm_nz2nd`：结果从 L0C 搬运至 GM；

### 矩阵乘法（Mmad）

下面将介绍如何配置
`asc_mmad`指令的入参，入参的具体含义这里不再赘述。

需要注意的是，执行`asc_mmad`指令时，矩阵计算单元会从L0A/L0B连续读入多个分形参与矩阵乘计算。读入分形的数量由`left_height`、`n_dim`、`right_width`的取值，以及`asc_mmad`指令对L0A/L0B上A矩阵、B矩阵各轴的对齐要求共同决定。

如图2所示，以b16输入类型为例，`asc_mmad`指令会按照A矩阵分形[16, 16]、B矩阵分形[16, 16]连续读入数据。此时，矩阵计算单元从L0A/L0B读入的分形总数分别为2 x 5 = 10和5 x 3 = 15，写入L0C的分形总数为2 x 3 = 6。

<p align="center">
  <img src="figures/mmad_f16_A5.png" width="900">
</p>
<p align="center">
图2：bfloat16类型，L0A上Nz排布，Mmad数据排布示意图
</p>

Mmad计算中包含了补齐的无效数据，需要配合Fixpipe指令在L0C搬出到GM过程中，排除参数Mmad计算过程中填充的无效数据。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行算子。

- 配置环境变量
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为 CANN 包安装目录，未指定时root用户默认安装至 `/usr/local/Ascend`，非root用户默认安装至`$HOME/Ascend`。

- 样例执行

  ```bash
  SCENARIO_NUM=1
  mkdir -p build && cd build
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM .. && make -j
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM
  ./demo
  python3 ../scripts/verify_result.py -scenarioNum=$SCENARIO_NUM output/output.bin output/golden.bin
  ```

  使用NPU仿真模式时，添加 `-DCMAKE_ASC_RUN_MODE=sim` 参数：

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM .. && make -j
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`sim` | 运行模式：NPU 运行、NPU 仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510`（默认） | NPU 架构，对应 Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | `1`（默认）、`2` | 场景编号，分别对应int8_t 、 bfloat16输入数据类型 |

- 执行结果

  精度对比成功时的输出：

  ```bash
  test pass!
  ```
