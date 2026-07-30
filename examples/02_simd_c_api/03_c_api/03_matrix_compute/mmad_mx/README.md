# Mmad MX 样例

## 概述

本样例演示如何使用C API实现带量化功能的MX矩阵乘法。A、B矩阵使用FP4或FP8数据类型，量化系数矩阵ScaleA、ScaleB使用fp8_e8m0_t数据类型；A和ScaleA不转置，B和ScaleB转置。

本样例还介绍了FP4和FP8场景在K轴上的搬运对齐与补零处理：FP4场景通过打包数据的32字节搬运对齐覆盖计算所需范围，无需额外调用清零接口；FP8场景则需要使用`asc_fill_l1`显式清零未被搬运覆盖的尾部补齐区域。详细说明请参见[K轴对齐与FP8尾部清零](#k-axis-alignment-and-fp8-tail-clearing)。

## 支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |

## 目录结构

```text
├── mmad_mx
│   ├── figures
│   │   └── mmad-mx.png             // MX矩阵乘示意图
│   ├── scripts
│   │   ├── gen_data.py             // 输入数据和真值数据生成脚本
│   │   └── verify_result.py        // 精度验证脚本
│   ├── CMakeLists.txt              // 编译工程文件
│   ├── data_utils.h                // 数据读写函数
│   ├── mmad_mx.asc                 // C API样例实现
│   ├── README_en.md                // 英文说明文档
│   └── README.md                   // 中文说明文档
```

## 样例描述

MX矩阵乘法计算公式为：

$$ C = (ScaleA \otimes A) \times (ScaleB \otimes B) + Bias $$

A、ScaleA、B、ScaleB、Bias、C矩阵的shape分别为[M, K]、[M, ScaleK]、[K, N]、[ScaleK, N]、[N]和[M, N]，其中`ScaleK = CeilAlign(CeilDiv(K, 32), 2)`。本样例使用[M, N, K] = [40, 50, 70]。

一次完整计算包含以下数据搬运和计算阶段：

1. 使用`asc_copy_gm2l1_nd2nz`将A、B和Bias从GM搬运到L1，使用`asc_copy_gm2l1_dn2nz`搬运ScaleA、ScaleB；
2. 场景3使用`asc_fill_l1`将FP8数据在K轴上的额外补齐区域清零；
3. 使用`asc_copy_l12l0a`和`asc_copy_l12l0a_mx`将A、ScaleA搬运到L0A和L0AMX；
4. 使用`asc_copy_l12l0b`和`asc_copy_l12l0b_mx`将B、ScaleB搬运到L0B和L0BMX；
5. 场景2使用`asc_copy_l12bt`将Bias搬运到BiasTable Buffer；
6. 使用`asc_mmad_mx`执行MX矩阵乘法；
7. 使用`asc_copy_l0c2gm`将结果从L0C搬运到GM，并转换为ND排布。

不同存储单元的数据排布如下：

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
    <td>L0A、L0AMX</td>
    <td>A、ScaleA矩阵分别为Nz、Zz排列。</td>
  </tr>
  <tr>
    <td>L0B、L0BMX</td>
    <td>B、ScaleB矩阵分别为Zn、Nn排列。</td>
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

编译参数`SCENARIO_NUM`对应以下场景：

<a name="表2"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">📌 表2：SCENARIO_NUM不同取值的含义</span></caption>
  <tr>
    <td >SCENARIO_NUM</td>
    <td>A Type</td>
    <td>ScaleA Type</td>
    <td>B Type</td>
    <td>ScaleB Type</td>
    <td>Bias Type</td>
    <td>C Type</td>
    <td>Bias说明</td>
  </tr>
  <tr>
    <td>1</td>
    <td>fp4x2_e1m2_t</td>
    <td>fp8_e8m0_t</td>
    <td>fp4x2_e2m1_t</td>
    <td>fp8_e8m0_t</td>
    <td>float</td>
    <td>float</td>
    <td>不带Bias，C矩阵的初始值为0</td>
  </tr>
  <tr>
    <td>2</td>
    <td>fp4x2_e2m1_t</td>
    <td>fp8_e8m0_t</td>
    <td>fp4x2_e1m2_t</td>
    <td>fp8_e8m0_t</td>
    <td>float</td>
    <td>float</td>
    <td>带Bias，矩阵计算时不传入biasTensor，C矩阵的初始值来源于C2（BiasTable Buffer），<br>从C2的起始位置0开始读取</td>
  </tr>
  <tr>
    <td>3</td>
    <td>fp8_e4m3fn_t</td>
    <td>fp8_e8m0_t</td>
    <td>fp8_e5m2_t</td>
    <td>fp8_e8m0_t</td>
    <td>float</td>
    <td>float</td>
    <td>不带Bias，C矩阵累加来源于CO1初始值</td>
  </tr>
</table>

### 场景详细说明

  本样例通过编译参数 `SCENARIO_NUM` 选择不同的场景，所有场景基于相同的矩阵乘规格：[M, N, K] = [40, 50, 70]。

**场景1 C矩阵初始值为0**

- A：[40, 70]，`fp4x2_e1m2_t`，ND格式；
- ScaleA：[40, 4]，`fp8_e8m0_t`，ND格式；
- B转置后输入：[50, 70]，`fp4x2_e2m1_t`，ND格式；
- ScaleB转置后输入：[50, 4]，`fp8_e8m0_t`，ND格式；
- 调用`asc_mmad_mx`时设置`c_matrix_init_val = true`。

**场景2 C矩阵初始值来源于C2**

- A：[40, 70]，`fp4x2_e2m1_t`，ND格式；
- ScaleA：[40, 4]，`fp8_e8m0_t`，ND格式；
- B转置后输入：[50, 70]，`fp4x2_e1m2_t`，ND格式；
- ScaleB转置后输入：[50, 4]，`fp8_e8m0_t`，ND格式；
- Bias：[50]，`float`；
- 调用`asc_mmad_mx`时设置`c_matrix_init_val = false`、`c_matrix_source = true`。

**场景3 C矩阵初始值来源于CO1**

- A：[40, 70]，`fp8_e4m3fn_t`，ND格式；
- ScaleA：[40, 4]，`fp8_e8m0_t`，ND格式；
- B转置后输入：[50, 70]，`fp8_e5m2_t`，ND格式；
- ScaleB转置后输入：[50, 4]，`fp8_e8m0_t`，ND格式；
- 第一次`asc_mmad_mx`将C初始化为0，第二次设置`c_matrix_init_val = false`、`c_matrix_source = false`，将计算结果累加到L0C。

### 矩阵乘法（MmadMx）

`asc_mmad_mx`指令会自动完成左、右矩阵与对应Scale矩阵的广播乘法，K轴上每32个元素共享一个量化因子。以A、B矩阵均为FP4类型为例，下图展示了A、ScaleA、B、ScaleB的分形排布格式和量化功能原理。

<a id="k-axis-alignment-and-fp8-tail-clearing"></a>

#### K轴对齐与FP8尾部清零

MX矩阵乘要求计算使用的K轴长度按64个逻辑元素对齐，因此传给`asc_mmad_mx`的K维参数必须使用`ALIGN_K = CeilAlign(K, 64)`，不能直接使用原始K。本样例中原始`K = 70`，实际计算参数为`ALIGN_K = 128`。L1和L0A/L0B空间也按照`ALIGN_K`申请，原始数据之外的补齐区域必须为0，否则无效数据会参与矩阵乘并影响结果。

Scale在K轴上以32个逻辑元素为一组，并要求组数按2对齐。因此本样例需要的有效Scale组数为`CeilDiv(70, 32) = 3`，实际`ScaleK = CeilAlign(3, 2) = 4`。

FP4和FP8数据在K轴上的存储宽度不同，GM到L1搬运后的补齐范围也不同：

| 项目 | FP4场景（场景1、2） | FP8场景（场景3） |
|------|--------------------|------------------|
| GM中每行的K轴存储量 | 70个FP4逻辑元素，打包为`PACKED_K = 35`字节 | 70个FP8元素，即70字节 |
| `asc_copy_gm2l1_nd2nz`按32字节对齐后的范围 | 64字节，对应128个FP4逻辑元素 | 96字节，对应96个FP8逻辑元素 |
| `asc_mmad_mx`使用的K轴长度 | 128 | 128 |
| 是否需要额外清零 | 不需要 | 需要将K轴的`[96, 128)`区域清零 |

场景3中，GM到L1完成`K = 70`的数据搬运及其32字节粒度补齐后，L1中已初始化的K轴范围到96元素边界，而`asc_mmad_mx`会按照128个元素进行计算。样例分别对A和B调用`asc_fill_l1`，从K轴偏移96处开始，为每个对齐行写入一个32字节的全0数据块，从而清除`[96, 128)`区域。

`asc_fill_l1`的目的地址按`uint16_t`寻址，起始偏移通过`AlignHeight * (CeilAlign(PACKED_K, 32) / 2)`计算。对于FP8场景，A和B的`AlignHeight`分别为`M_ALIGN = 48`和`N_ALIGN = 64`，所以起始偏移分别为2304和3072个`uint16_t`元素。两次清零的`repeat`分别设置为48和64，`blk_num = 1`、`dst_gap = 0`，连续覆盖每个对齐行对应的一个32字节数据块。清零完成后调用`asc_sync_pipe(PIPE_MTE1)`，确保写L1操作结束后再执行L1到L0A/L0B的搬运。

需要特别注意：如果修改样例中的K值，必须同步重新计算`PACKED_K`、`ScaleK`和`ALIGN_K`，并根据GM到L1搬运实际覆盖的K轴范围处理剩余补齐区域。

<p align="center">
  <img src="figures/mmad-mx.png" width="1000">
</p>

<p align="center">图1：带量化功能的MX矩阵乘法</p>

## 编译运行

- 配置环境变量：

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

- 安装输入数据生成脚本依赖：

  ```bash
  python3 -m pip install ml_dtypes==0.2.0 en_dtypes==0.0.4
  ```

- 在本样例目录下编译并运行：

  ```bash
  SCENARIO_NUM=1
  mkdir -p build && cd build
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM
  make -j
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM
  ./demo
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin
  ```

  使用NPU仿真模式时，在CMake命令中添加`-DCMAKE_ASC_RUN_MODE=sim`。切换运行模式前需要清理CMake缓存。

| 选项 | 可选值 | 说明 |
|------|--------|------|
| `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU架构 |
| `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`sim` | NPU运行或NPU仿真 |
| `SCENARIO_NUM` | `1`（默认）、`2`、`3` | 场景编号 |

精度对比成功时输出：

```text
test pass!
```
