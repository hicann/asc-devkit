# 浮点操作

## 概述

本章介绍Ascend C中浮点操作，按API与编程范式分为以下三部分：

- **Reg运算**：Reg矢量计算API，以Reg矢量计算寄存器为操作对象的矢量计算，如Add、Mul、Axpy、Cast等。该能力为Ascend 950PR&950DT系列产品新增，属SIMD编程范式。
- **SIMT运算**：SIMT编程范式下以线程为粒度的运算，包括内置算术运算符与数学函数，使用方式与C/C++惯用写法一致。
- **Cube运算**：Cube核的矩阵计算编程模型，覆盖数据搬入、矩阵乘加到结果搬出（Fixpipe随路量化与ReLU）的完整通路，属SIMD编程范式。

## Reg运算

Reg矢量计算（Reg Vector Compute）是以Reg矢量计算寄存器为操作对象的矢量计算方式，属SIMD编程范式，通过[Reg矢量计算基础API](../../../api/SIMD-API/basic_api/reg_vector_compute/overview.md)与[Reg矢量计算C API](../../../api/SIMD-API/c_api/reg_compute/reg_compute.md)使用。该能力为Ascend 950PR&950DT系列产品新增。

### 相关API

Reg运算相关API按层级链接如下，各接口的功能、支持的浮点数据类型及产品支持情况请参见对应文档：

- **基础API**：[Reg矢量计算](../../../api/SIMD-API/basic_api/reg_vector_compute/reg_vector_compute.md)，涵盖Reg数据搬入与搬出、基础算术（Add、Mul、Div、Exp、Ln、Log等）、复合计算（Axpy、MulDstAdd、Fma等）、精度转换（Cast）、比较与选择、规约、逻辑计算、掩码寄存器操作等；
- **C API**：[Reg矢量计算C API](../../../api/SIMD-API/c_api/reg_compute/reg_compute.md)，与基础API功能对应，同名函数加`asc_`前缀（asc_add、asc_fma、asc_float2half等）；
- **高阶API**：[Fma](../../../api/SIMD-API/adv_api/math_compute/Fma_interface/Fma.md)按元素计算src0 × src1 + src2（单次舍入），操作对象为LocalTensor，通过调用基础API实现，支持half、float。

各接口的精度标准请参见[Reg矢量计算接口精度标准汇总](../../../api/appendix/reg_vector_compute_interface_precision_standard_summary.md)。

## SIMT运算

SIMT运算指SIMT编程范式下以线程为粒度执行的浮点运算，运行在AIV核上，通过[SIMT API](../../../api/SIMT-API/overview.md)使用。SIMT运算不区分API层次，浮点运算通过两类方式表达：

- **C/C++运算符与隐式类型转换**：float、half、bfloat16_t等类型支持`+`、`-`、`*`、`/`等内置算术运算符和隐式类型转换，使用方式与标准C/C++一致。
- **数学函数与类型转换函数**：数学函数与C标准数学库形式对齐。

### 相关API

SIMT运算相关API链接如下，各函数的精度标准、Subnormal支持情况及编译选项影响请参见对应文档：

- **数学函数**：[SIMT数学函数](../../../api/SIMT-API/math_functions/math_functions.md)，按数据类型组织：
    - float类型数学库函数（`expf`、`logf`、`sinf`、`fmaf`、`floorf`、`isfinite`等，与C标准数学库对齐）；
    - half类型函数（`__hadd`、`__hfma`等算术函数，`hexp`、`hlog`等数学库函数，`__float2half_rn`等精度转换函数，及half短向量版本）；
    - bfloat16类型函数（组织方式与half类型一致，前缀同名但作用类型为bfloat16_t，含bfloat16x2短向量版本）；
    - fp8类型函数（fp8数据类型简介与数据转换函数）。
- **数据类型转换**：[SIMT数据类型转换](../../../api/SIMT-API/math_functions/data_type_conversion/overview.md)，显式转换API按"源类型2目标类型_舍入模式[_饱和后缀]"命名（如__float2half_rn_sat），隐式转换由编译器自动执行。
- **其他SIMT API**（同步、原子操作、Warp函数、访存等）：[SIMT API总览](../../../api/SIMT-API/overview.md)。

各接口的精度标准汇总请参见[SIMT接口精度标准汇总](../../../api/appendix/SIMT_interface_precision_standard_summary.md)。

## Cube运算

Cube运算对应Ascend C的矩阵计算编程模型，覆盖Cube核从数据搬入、矩阵乘加到结果搬出的完整通路，属SIMD编程范式。

### Cube计算单元

AI Core内有三种基础计算单元：Cube计算单元、Vector计算单元和Scalar计算单元。Cube计算单元负责执行矩阵运算，以half数据类型为例，Cube每次执行可完成两个half类型的16x16矩阵的乘法操作。Cube计算单元只能直接访问三个专用缓存：L0A Buffer存储左矩阵，L0B Buffer存储右矩阵，L0C Buffer存储计算结果与累加初值；输入矩阵一般先从Global Memory搬运到空间更大的L1 Buffer，再加载到L0A或L0B。硬件架构详见[矩阵计算单元](../../../api/SIMD-API/basic_api/cube_compute_ISASI/overview/cube_compute_unit.md)。

### 矩阵计算通路

Ascend C针对矩阵计算编程模型提供了四类接口，分别承载Cube核中各个通路的搬运能力和计算能力：

1. **数据搬入（DataCopy）**：将A、B原始矩阵及Bias、量化系数等数据从Global Memory搬运到L1 Buffer；
2. **数据加载（LoadData）**：将A、B矩阵从L1 Buffer加载到L0A Buffer/L0B Buffer准备计算，加载过程中完成分形格式转换；Bias搬运到BiasTable Buffer，量化系数搬运到Fixpipe Buffer；
3. **矩阵乘加（Mmad）**：对L0A Buffer、L0B Buffer、BiasTable Buffer中的数据执行矩阵乘加C = A × B + C，结果输出到L0C Buffer；
4. **数据搬出（Fixpipe）**：将L0C Buffer的结果搬运至GM、UB或L1，搬运过程中可利用Fixpipe Buffer中的量化系数、偏置等执行随路量化、随路ReLU、随路通道合并等随路计算。

矩阵计算的完整流程与各接口详见[矩阵计算流程](../../../api/SIMD-API/basic_api/cube_compute_ISASI/overview/cube_compute_flow.md)与[矩阵计算（ISASI）](../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_ISASI.md)。

### 相关API

Cube运算相关API按层级链接如下，各接口的功能与产品支持情况请参见对应文档：

- **基础API**：[矩阵计算（ISASI）](../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_ISASI.md)，涵盖矩阵计算搬入（DataCopy与LoadData）、Mmad计算（Mmad、MmadMx、MmadBitMode、MmadWithSparse）、矩阵计算搬出（Fixpipe）及分形、辅助配置等；
- **C API**：[C API矩阵计算](../../../api/SIMD-API/c_api/cube_compute/cube_compute.md)，与基础API对应；
- **高阶API**：[Matmul（矩阵计算高阶API）](../../../api/SIMD-API/adv_api/cube_compute/cube_compute.md)，涵盖Matmul的Kernel侧与Tiling侧接口，封装Tiling切分与流水优化，支持常规Matmul、HF32模式与MxMatmul场景。
