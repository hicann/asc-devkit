# Ascend C数据类型汇总

## 类型总览与标准符合性

Ascend C提供不同位宽的浮点数据类型。浮点相关数据类型如下表所示。

**表1** 不同位宽对应的浮点数据类型

| 位宽 | 数据类型 |
| --- | --- |
| b8 | fp4x2_e2m1_t、fp4x2_e1m2_t、hifloat8_t、fp8_e8m0_t、fp8_e5m2_t、fp8_e4m3fn_t。 |
| b16 | half、bfloat16_t。 |
| b32 | float。 |

为了方便描述这些数据类型，提供如下的数据类型简写：

**表2** 浮点数据类型简写

| 数据类型简写 | 对应数据类型 |
| --- | --- |
| HiF8 | hifloat8_t |
| FP8_E4M3 | fp8_e4m3fn_t |
| FP8_E5M2 | fp8_e5m2_t |
| FP8_E8M0 | fp8_e8m0_t |
| FP4_E2M1 | fp4x2_e2m1_t |
| FP4_E1M2 | fp4x2_e1m2_t |
| FP16 | half |
| BF16 | bfloat16_t |
| FP32 | float |

说明：各产品支持的数据类型范围不同，详细支持情况请参见[内置数据类型](../../../api/SIMD-API/basic_api/data_structures/builtin_data_types.md)。

## 数据范围与精度属性

Ascend C中各浮点数据类型的比特分布、表达范围及特殊值支持汇总如下。表中给出各类型完整的对称取值区间（fp8_e8m0_t无符号位除外）。

**表3** 各浮点数据类型的比特分布与表达范围

| 类型 | 符号位 | 指数位 | 尾数位 | 取值范围 |
| --- | --- | --- | --- | --- |
| half | 1 | 5 | 10 | [$2^{5} - 2^{16}$, $2^{16} - 2^{5}$] |
| bfloat16_t | 1 | 8 | 7 | [$2^{120} - 2^{128}$, $2^{128} - 2^{120}$] |
| float | 1 | 8 | 23 | [$2^{104} - 2^{128}$, $2^{128} - 2^{104}$] |
| fp8_e5m2_t | 1 | 5 | 2 | [$2^{13} - 2^{16}$, $2^{16} - 2^{13}$] |
| fp8_e4m3fn_t | 1 | 4 | 3 | [$2^{6} - 2^{9}$, $2^{9} - 2^{6}$] |
| fp8_e8m0_t | 0 | 8 | 0 | [$2^{-127}$, $2^{127}$] |
| fp4x2_e2m1_t | 1 | 2 | 1 | [$-6$, $6$] |
| fp4x2_e1m2_t | 1 | 1 | 2 | [$-7 * 2^{-2}$, $7 * 2^{-2}$] |
| hifloat8_t | 1 | 由点域D决定 | 由点域D决定 | 由点域D决定 |

**表4** 各浮点数据类型的特殊值支持

| 类型 | +0/-0 | ±inf | NaN |
| --- | --- | --- | --- |
| half | 支持 | 支持 | 支持 |
| bfloat16_t | 支持 | 支持 | 支持 |
| float | 支持 | 支持 | 支持 |
| fp8_e5m2_t | 支持 | 支持 | 支持 |
| fp8_e4m3fn_t | 支持 | 不支持（最大规格数448替代） | 支持（仅sNaN） |
| fp8_e8m0_t | 支持（0x00表示0） | 不支持（0xFF表示NaN） | 支持（仅0xFF） |
| fp4x2_e2m1_t | 支持（指数字段全0） | 不支持 | 不支持 |
| fp4x2_e1m2_t | 支持（指数字段全0） | 不支持 | 不支持 |
| hifloat8_t | 支持 | 不支持 | 不支持 |

> [!NOTE]说明  
> fp8_e4m3fn_t的"FN"表示finite：该格式没有inf，超出±448的值饱和处理；NaN仅有sNaN一种编码（尾数=0b111，指数全1）。  
> fp8_e8m0_t：0x00编码为0，0xFF编码为NaN，其余254个编码均为2的幂（2^-127至2^127）。  
> hifloat8_t的取值范围与精度由点域D的取值动态决定，具体编码方式请参见[hifloat8数据格式](#hifloat8_t)。

### float

float数据类型由1位符号位（S）、8位指数位（E）、23位尾数位（M）组成。下图是一个float类型规格数的示例，其表示的结果为$(-1)^0 \times 2^{(127 - 127)} \times (1+ 0.75) = 1.75$。

**图1** float数据类型二进制示意图，值为1.75

![float数据类型二进制示意图-值为1-75](../../../api/figures/float-binary-1-75.png)

**表5** float类型特殊值

| 特殊值 | S | E | M |
| - | - | - | - |
| +0 | 0 | 全为0 | 全为0 |
| -0 | 1 | 全为0 | 全为0 |
| +inf | 0 | 全为1 | 全为0 |
| -inf | 1 | 全为1 | 全为0 |
| nan | 任意 | 全为1 | 不全为0 |
| 最小正非规格数 | 0 | 8'h00 | 23'h000001 |
| 最大正非规格数 | 0 | 8'h00 | 23'h7FFFFF |
| 最小正规格数 | 0 | 8'h01 | 23'h000000 |
| 最大正规格数 | 0 | 8'hFE | 23'h7FFFFF |

### half

half数据类型由1位符号位（S）、5位指数位（E）、10位尾数位（M）组成。下图是一个half类型规格数的示例，其表示的结果为 $(-1)^0 \times 2^{(15 - 15)} \times (1+ 0.75) = 1.75$。下标v表示各部分的具体数值。

**图2** half数据类型二进制示意图，值为1.75

![half数据类型二进制示意图-值为1-75](../../../api/figures/half-binary-1-75.png)

**表6** half类型特殊值bit位表示

| 特殊值 | S | E | M |
| - | - | - | - |
| +0 | 0 | 全为0 | 全为0 |
| -0 | 1 | 全为0 | 全为0 |
| +inf | 0 | 全为1 | 全为0 |
| -inf | 1 | 全为1 | 全为0 |
| nan | 任意 | 全为1 | 不全为0 |
| 最小正非规格数 | 0 | 5'h00 | 10'h001 |
| 最大正非规格数 | 0 | 5'h00 | 10'h3FF |
| 最小正规格数 | 0 | 5'h01 | 10'h000 |
| 最大正规格数 | 0 | 5'h1E | 10'h3FF |

### bfloat16_t

bfloat16数据类型由1位符号位（S）、8位指数位（E）、7位尾数位（M）组成。下图是一个bfloat16_t数据类型规格数的示例，其表示的结果为 $(-1)^0 \times 2^{(127 - 127)} \times (1+ 0.75) = 1.75$。

**图3** bfloat16_t数据类型二进制示意图，值为1.75

![bfloat16_t数据类型二进制示意图-值为1.75](../../../api/figures/bfloat16_t-binary-1-75.png)

**表7** bfloat16_t类型特殊值

| 特殊值 | S | E | M |
| - | - | - | - |
| +0 | 0 | 全为0 | 全为0 |
| -0 | 1 | 全为0 | 全为0 |
| +inf | 0 | 全为1 | 全为0 |
| -inf | 1 | 全为1 | 全为0 |
| nan | 任意 | 全为1 | 不全为0 |
| 最小正非规格数 | 0 | 8'h00 | 7'h01 |
| 最大正非规格数 | 0 | 8'h00 | 7'h7F |
| 最小正规格数 | 0 | 8'h01 | 7'h00 |
| 最大正规格数 | 0 | 8'hFE | 7'h7F |

## FP8与FP4浮点数格式

FP8与FP4是为降低数据存储与搬运开销而引入的低精度浮点格式，主要面向量化计算场景。Ascend C支持的FP8格式包括fp8_e5m2_t、fp8_e4m3fn_t和fp8_e8m0_t，FP4格式包括fp4x2_e2m1_t和fp4x2_e1m2_t。

**表8** FP8/FP4格式概览

| 类型 | 符号位 | 指数位 | 尾数位 | 取值范围 |
| --- | --- | --- | --- | --- |
| fp8_e5m2_t | 1 | 5 | 2 | [$2^{13} - 2^{16}$, $2^{16} - 2^{13}$] |
| fp8_e4m3fn_t | 1 | 4 | 3 | [$2^{6} - 2^{9}$, $2^{9} - 2^{6}$] |
| fp8_e8m0_t | 0 | 8 | 0 | [$2^{-127}$, $2^{127}$] |
| fp4x2_e2m1_t | 1 | 2 | 1 | [$-6$, $6$] |
| fp4x2_e1m2_t | 1 | 1 | 2 | [$-7 * 2^{-2}$, $7 * 2^{-2}$] |

> [!NOTE]说明
> fp4x2_e2m1_t和fp4x2_e1m2_t数据类型会将两个独立的四位浮点数打包为一个8比特存储单元。

### fp8_e5m2_t

fp8_e5m2_t由1位符号位、5位指数位、2位尾数位组成，编码风格与IEEE-754一致：支持Subnormal，指数偏置为15，特殊值包括±0、±inf和NaN。下图是一个fp8_e5m2_t类型的示例，其符号位占用1位，指数占用5位，尾数占用2位，表示的结果为$(-1)^0 \times (1 + 0.5 + 0.25) \times 2^{30-15}=1.75 \times 2^{15}$。

**图4** fp8_e5m2_t数据类型二进制示意图

![](../../../api/figures/f8_1-2.png)

fp8_e5m2_t的特殊值bit位表示如下：

**表9** fp8_e5m2_t特殊值bit位表示

| 特殊值 | S | E | M |
| --- | --- | --- | --- |
| +0 | 0 | 全为0 | 全为0 |
| -0 | 1 | 全为0 | 全为0 |
| +inf | 0 | 全为1 | 全为0 |
| -inf | 1 | 全为1 | 全为0 |
| nan | 任意 | 全为1 | 不全为0 |
| 最小正非规格数 | 0 | 5'b00000 | 2'b01 |
| 最大正非规格数 | 0 | 5'b00000 | 2'b11 |
| 最小正规格数 | 0 | 5'b00001 | 2'b00 |
| 最大正规格数 | 0 | 5'b11110 | 2'b11 |

### fp8_e4m3fn_t

fp8_e4m3fn_t由1位符号位、4位指数位、3位尾数位组成。该格式没有inf；支持Subnormal，非规格数范围为[$2^{-9}$, $2^{-6} - 2^{-9}$]，最小规格数为$2^{-6}$；下图是一个fp8_e4m3fn_t类型的示例，其符号位占用1位，指数占用4位，尾数占用3位，表示的结果为 $(-1)^1 \times 2^{-3} \times 2^{-6}$。

**图5** fp8_e4m3fn_t数据类型二进制示意图

![](../../../api/figures/fp8_e4m3fn_binary.png)

fp8_e4m3fn_t的特殊值bit位表示如下：

**表10** fp8_e4m3fn_t特殊值bit位表示

| 特殊值 | S | E | M |
| --- | --- | --- | --- |
| +0 | 0 | 全为0 | 全为0 |
| -0 | 1 | 全为0 | 全为0 |
| nan | 任意 | 全为1 | 全为1 |
| 最小正非规格数 | 0 | 4'b0000 | 3'b001 |
| 最大正非规格数 | 0 | 4'b0000 | 3'b111 |
| 最小正规格数 | 0 | 4'b0001 | 3'b000 |
| 最大正规格数 | 0 | 4'b1111 | 3'b110 |

### fp8_e8m0_t

fp8_e8m0_t没有符号位和尾数位，仅由8位指数组成（其编码可视为由bfloat16类型舍弃符号位、尾数位得到），不支持Subnormal，只能表示2的幂次：0x00编码为0，0xFF编码为NaN，其余254个编码均为$2^{-127}$至$2^{127}$之间的2的幂。该格式主要用作MX（微缩放）格式的共享缩放因子，不用于通用数值计算。

**图6** fp8_e8m0_t数据类型二进制示意图

![](../../../api/figures/f8_1-1.png)

### fp4x2_e2m1_t

fp4x2_e2m1_t的每个元素由1位符号位、2位指数位、1位尾数位组成，取值范围为[$-6$, $6$]，没有inf和NaN；支持Subnormal，非规格数仅有±0.5，最小规格数为1。

**图7** fp4x2_e2m1_t数据类型二进制示意图

![](../../../api/figures/f8_1.png)

### fp4x2_e1m2_t

fp4x2_e1m2_t的每个元素由1位符号位、1位指数位、2位尾数位组成，取值范围为[$-7 * 2^{-2}$, $7 * 2^{-2}$]，没有inf和NaN；支持Subnormal，非规格数为±0.25、±0.5、±0.75，最小规格数为1。

**图8** fp4x2_e1m2_t数据类型二进制示意图

![](../../../api/figures/f8_1-0.png)

### 应用场景

FP8/FP4主要与量化/反量化（Quantize/AntiQuantize）类接口配合使用：支持从half、bfloat16_t、float类型量化到fp8_e5m2_t、fp8_e4m3fn_t、hifloat8_t、int8_t等类型，以及相应的反量化。FP8与FP4还可作为MX格式的私有元素，与fp8_e8m0_t共享缩放因子组合使用，参见本节"MX缩放计算"部分。矩阵计算（Mmad）同样支持fp4x2_e2m1_t、fp4x2_e1m2_t、fp8_e4m3fn_t、fp8_e5m2_t作为输入。具体接口请参见[数据类型转换](../../../api/SIMT-API/math_functions/data_type_conversion/overview.md)与[量化API](../../../api/SIMD-API/adv_api/quantization/quantization.md)。

### MX缩放计算（Microscaling Matmul）

#### MX数据格式（Microscaling）

现有浮点数格式或数值范围不足，或精度不高；若同时满足数值范围和精度的要求，则内存占用过大，数据存储与传输成本高。为此，业内提出了微缩放（Microscaling，MX）格式：以块为单位编码数据，在保持低位宽的同时通过共享缩放因子灵活表示不同范围的数据，使8位及以下位宽的数据也能支撑稳健的AI训练和推理精度。

MX的核心思想：

- 张量切分固定大小Block（块），每块共用一个独立缩放因子；
- 块内元素用极低比特浮点（4bit/8bit）存储；
- 缩放因子用E8M0（纯指数8bit，仅代表2的幂次），硬件乘算极快。

MX格式的数据由三部分构成：

- 共享缩放因子X，位宽为w bits：块级缩放比例因子，决定数据块中所有元素的动态范围；
- 私有元素P<sub>i</sub>，位宽为d bits：数据块中的每个低位宽数据元素，所有k个元素Pi有相同的位宽和数据类型，并且共享一个缩放因子X，经X调整后共同表示高精度的浮点数或整数；
- 块大小k：组成一个数据块的低比特数据的数量。

每个包含k个元素的块使用（w + k\*d）位编码，元素的数据类型和缩放因子可以独立选择。

**图9** MX格式组成示意图

![](../../figures/mx_format.png "MX格式组成示意图")

Ascend C支持的MX数据类型如下，块大小k固定为32，共享缩放因子固定为fp8_e8m0_t：

**表11** MxMatmul支持的MX数据类型

| 数据类型 | 私有元素数据类型 | 私有元素位宽（d） | 块大小（k） | 共享缩放因子数据类型 | 共享缩放因子位宽（w） |
| --- | --- | --- | --- | --- | --- |
| MXFP8 | fp8_e5m2_t | 8 | 32 | fp8_e8m0_t | 8 |
| MXFP8 | fp8_e4m3fn_t | 8 | 32 | fp8_e8m0_t | 8 |
| MXFP4 | fp4x2_e1m2_t | 4 | 32 | fp8_e8m0_t | 8 |
| MXFP4 | fp4x2_e2m1_t | 4 | 32 | fp8_e8m0_t | 8 |

以MXFP8为例，每个元素平均占用（8 + 8/32）= 8.25位；MXFP4每元素仅（4 + 8/32）= 4.25位，相比half（16位）可节省一半以上的存储与搬运开销。MXFP8/MXFP4不是独立的数据类型，而是"低位宽私有元素 + fp8_e8m0_t共享缩放因子"的组合，各底层类型参见本节各FP8/FP4数据类型小节。

将长度为k的原始数据向量V转换为MX格式{X, [P<sub>i</sub>]}遵循OCP MX规范定义的算法，分两步：

1. 计算缩放因子X。记amax为块内绝对值最大值、$2^{E_{max}}$为私有元素类型能表示的最大2的幂，则：

$$X = 2^{\lfloor \log_2 amax \rfloor - E_{max}}$$

其中$\lfloor \log_2 amax \rfloor$为amax的整数指数，即不大于amax的最大2的幂的次数。e8m0的编码为"指数+127"。

2. 计算私有元素P<sub>i</sub>：将$V_i / X$量化到私有元素类型，超出最大规格数时钳位到最大规格值并保留符号，舍入采用roundTiesToEven（就近舍入到偶数）。

各私有元素类型对应的最大规格数与最大2的幂如下：

**表12** 各私有元素类型对应的最大规格数与最大2的幂

| 私有元素类型 | 最大规格数 | 最大2的幂（$2^{E_{max}}$） |
| --- | --- | --- |
| fp8_e4m3fn_t | 448 | $2^8$ = 256 |
| fp8_e5m2_t | 57344 | $2^{15}$ = 32768 |
| fp4x2_e2m1_t | 6 | $2^2$ = 4 |
| fp4x2_e1m2_t | $7 \times 2^{-2}$ = 1.75 | $2^0$ = 1 |

以amax = 120000、私有元素为fp8_e4m3fn_t为例：$\lfloor \log_2 120000 \rfloor = 16$，$X = 2^{16-8} = 2^8 = 256$；amax除以X得468.75，超出fp8_e4m3fn_t最大规格数448，钳位为448。对应地，缩放因子X编码为fp8_e8m0_t时，8位编码值为8 + 127 = 135（0x87）。此时块内最大元素有约4.4%的表示误差，其余元素均满精度表示。当块内数值整体较小时（如amax = 100），$X = 2^{6-8} = 2^{-2} < 1$，缩放因子将整块数值放大至私有元素类型精度最密集的区间，小数值块同样可保持精度。

#### MX缩放计算（MxMatmul）

MxMatmul（全称Microscaling Matmul）为带有量化系数的矩阵乘法：左矩阵和右矩阵均有对应的量化系数矩阵，计算公式为：

C = (scaleA ⊗ A) \* (scaleB ⊗ B) + Bias

其中"⊗"表示广播乘法，左/右矩阵与左/右量化系数矩阵做逐元素乘积时，K方向上每32个元素共享一个量化因子。各操作数的形状为：

- A为左矩阵，形状为[M, K]；scaleA为左量化系数矩阵，形状为[M, K/32]；
- B为右矩阵，形状为[K, N]；scaleB为右量化系数矩阵，形状为[K/32, N]；
- C为目的操作数，形状为[M, N]；
- Bias为矩阵乘偏置，形状为[1, N]，对结果矩阵的每一行进行偏置。

**图10** MxMatmul矩阵乘示意图

![](../../figures/mx_matmul.png "MxMatmul矩阵乘示意图")

MxMatmul支持的输入输出类型组合如下（仅Ascend 950PR&950DT系列产品支持）：

**表13** MxMatmul支持的量化场景

| A矩阵 | B矩阵 | ScaleA/ScaleB矩阵 | Bias矩阵 | C矩阵 |
| --- | --- | --- | --- | --- |
| fp4x2_e1m2_t | fp4x2_e1m2_t/fp4x2_e2m1_t | fp8_e8m0_t | float/half/bfloat16_t | float/half/bfloat16_t |
| fp4x2_e2m1_t | fp4x2_e2m1_t/fp4x2_e1m2_t | fp8_e8m0_t | float/half/bfloat16_t | float/half/bfloat16_t |
| fp8_e4m3fn_t | fp8_e4m3fn_t/fp8_e5m2_t | fp8_e8m0_t | float/half/bfloat16_t | float/half/bfloat16_t |
| fp8_e5m2_t | fp8_e4m3fn_t/fp8_e5m2_t | fp8_e8m0_t | float/half/bfloat16_t | float/half/bfloat16_t |

#### MX配置方式

MX缩放计算的接口分为三类：

- 基础API：[MmadMx](../../../api/SIMD-API/basic_api/cube_compute_ISASI/mmad_compute/MmadMx.md)执行MX矩阵乘加计算；数据搬运使用[LoadData（2D MX搬运）](../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/LoadData_2D_MX.md)，左右矩阵以512字节分形为单位搬入L0A/L0B Buffer，量化系数矩阵以32字节分形为单位搬入专用的L0A_MX/L0B_MX Buffer（各4KB）。
- C API：[asc_mmad_mx](../../../api/SIMD-API/c_api/cube_compute/mmad_compute/asc_mmad_mx.md)执行MX矩阵乘加，[asc_copy_l12l0a_mx](../../../api/SIMD-API/c_api/cube_datamove/cube_compute_load/asc_copy_l12l0a_mx.md)、[asc_copy_l12l0b_mx](../../../api/SIMD-API/c_api/cube_datamove/cube_compute_load/asc_copy_l12l0b_mx.md)完成数据搬运。
- Matmul高阶API：Host侧Tiling通过[SetMadType](../../../api/SIMD-API/adv_api/cube_compute/Matmul_Tiling/SetMadType.md)配置`MatrixMadType::MXMODE`开启MxMatmul场景，通过[SetScaleAType](../../../api/SIMD-API/adv_api/cube_compute/Matmul_Tiling/SetScaleAType.md)、[SetScaleBType](../../../api/SIMD-API/adv_api/cube_compute/Matmul_Tiling/SetScaleBType.md)设置量化系数矩阵的类型信息；Kernel侧通过MatmulTypeWithScale定义A、scaleA、B、scaleB的类型，并以MatmulWithScalePolicy策略创建Matmul对象，通过[SetTensorScaleA](../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/SetTensorScaleA.md)、[SetTensorScaleB](../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/SetTensorScaleB.md)设置量化系数矩阵。

矩阵计算前，需通过量化接口将half、bfloat16_t、float等类型量化为MX格式的私有元素；计算完成后，可通过反量化接口还原。[Quantize](../../../api/SIMD-API/adv_api/quantization/Quantize.md)与[AntiQuantize](../../../api/SIMD-API/adv_api/quantization/AntiQuantize.md)支持这些类型与fp8_e5m2_t、fp8_e4m3fn_t、fp4x2_e2m1_t、fp4x2_e1m2_t、hifloat8_t、int8_t等低位宽类型的互转。

MX矩阵搬运与计算为Ascend 950PR&950DT系列产品新增能力。完整的使用流程与代码示例请参见[MxMatmul场景](../../operator_practice/simd_operator_impl/matrix_advanced_api/feature_scenarios/mx_matmul_scenario.md)与[mmad_mx样例](https://gitcode.com/cann/asc-devkit/blob/master/examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/mmad_mx)。

MX缩放计算仅适用于SIMD编程范式：MxMatmul为矩阵计算特性，相关接口均为SIMD API；SIMT编程范式不提供矩阵计算能力。SIMD与SIMT混合编程场景下，需在SIMD侧完成MX的配置与计算。

## hifloat数据格式

### hifloat8_t

hifloat8\_t类型相对其他类型增加了指数位宽控制字段D，用于指示指数位和尾数位的编码方式：通过点域D动态调整指数位与尾数位的宽度分配，在8bit位宽内实现精度与动态范围的折中。

hifloat8_t根据点域D的不同取值，有不同的编码方式。符号、指数和尾数分别缩写成'S'、'E'和'M'，其bit位分布如下：

**图11** S、E、M在不同点域D值下的bit位分布

![](../../../api/figures/sem_bit_distribution.png "S-E-M在不同点域D值下的bit位分布")

各点域取值下的位宽分配与实际取值计算公式如下表所示：

**表14** hifloat8_t类型取值范围

| 符号位宽 | 指数位宽控制字段位宽 | 指数位宽 | 尾数位宽 | 指数位宽控制字段取值 | 符号取值范围（$S_v$） | 指数取值范围（$E_v$） | 尾数取值范围（$M_v$） | 实际取值计算公式 |
| -------- | -------------------- | -------- | -------- | -------------------- | ----------------------------- | ----------------------------- | ----------------------------- | ---------------- |
| 1 | 4 | 0 | 3 | 4'b0000 | ±1 | - | [0, 7] | $S_v \times 2^{M_v - 23}$ |
| 1 | 4 | 0 | 3 | 4'b0001 | ±1 | 0 | [0, $7 * 2^{-3}$] | $S_v \times 2^{E_v} \times (1 + M_v)$ |
| 1 | 3 | 1 | 3 | 3'b001 | ±1 | ±1 | [0, $7 * 2^{-3}$] | $S_v \times 2^{E_v} \times (1 + M_v)$ |
| 1 | 2 | 2 | 3 | 2'b01 | ±1 | ±[2, 3] | [0, $7 * 2^{-3}$] | $S_v \times 2^{E_v} \times (1 + M_v)$ |
| 1 | 2 | 3 | 2 | 2'b10 | ±1 | ±[4, 7] | [0, $3 * 2^{-2}$] | $S_v \times 2^{E_v} \times (1 + M_v)$ |
| 1 | 2 | 4 | 1 | 2'b11 | ±1 | ±[8, 15] | [0, $2^{-1}$] | $S_v \times 2^{E_v} \times (1 + M_v)$ |

hifloat8_t特殊值bit位表示如下：

**表15** hifloat8_t特殊值bit位表示

| 特殊值 | S | D | E | M |
| --- | --- | --- | --- | --- |
| +0 | 0 | 0000 | - | 000 |
| nan | 1 | 0000 | - | 000 |
| +inf | 0 | 11 | 0111 | 1 |
| -inf | 1 | 11 | 0111 | 1 |

hifloat8_t数据类型计算公式如下：

- **符号位$S_v$**：s_bit_val表示符号位数值，为1表示负数，为0表示非负数，$S_v = (-1)^{s\_bit\_val}$。
- **指数位$E_v$**：由Es、Em组成。Es表示指数的符号，Em表示指数的数值。不同D域值下Es、Em的bit大小如下表所示：

    **表16** 不同D域值，Es、Em bit大小不同

    | D值 | Es 值范围 | Em值范围 |
    | ---- | --------- | --------- |
    | 3'b001 | 0~1 | - |
    | 2'b01 | 0~1 | 10~11 |
    | 2'b10 | 0~1 | 100~111 |
    | 2'b11 | 0~1 | 1000~1111 |

    es_bit_val表示指数符号位的值，是E的最高位，用于计算Ev的符号值$Es\_value$；指数的数值$Em\_value$是E除最高位外的剩余位数加上前置1，用于计算Ev的实际值。Ev的计算公式为：

    (1) $Es\_value = (-1)^{es\_bit\_val}$

    (2) $Em\_value = (1 << m) | R$，其中R为E除最高位外的剩余bit串，m为其位宽

    (3) $Ev\_value = Es\_value * Em\_value$

    以E = 0b1100（位宽4）为例：最高位为1，得 $Es\_value = -1$；剩余bit串R = 0b100前置隐含1，得 $Em\_value = 0b1100$，即12；故 $Ev\_value = -12$。

- **尾数位$M_v$**：Mv计算公式如下，M表示位数值，bit width of M表示M的bit宽度大小。

    (4) $Mv\_value = M / 2^{\textit{bit width of M}}$

- 在Normal和Subnormal模式下，浮点数的取值计算公式不同：

    Normal模式，由Sv、Ev、Mv组成：

    (1) $value = S_v * 2^{E_v} * (1 + M_v)$

    Subnormal模式，由Sv、Mv组成，$Mv = M$：

    (2) $value = S_v * 2^{M_v - 23}$

下图示例中，其符号位占用1位，指数位占用2位，尾数位占用3位，D字段为2比特b01，$S_v=1$，$E_v=3$，$M_v = 2^{-1} + 2^{-2}$，根据实际取值计算公式$S_v \times 2^{E_v} \times (1 + M_v)$计算得到的结果为14。下标v表示各部分的具体数值。

**图12** hifloat8_t数据类型二进制示意图，值为14

![](../../../api/figures/hifloat8_binary.png)


### HF32

HF32（half-fp32）是面向Cube（矩阵计算）的高性能float32数据格式，在fp32的32bit位宽内以更短的尾数换取更高的矩阵计算性能。它不是独立的用户数据类型：没有对应的C++类型名，fp32数据在参与矩阵计算前被舍入为HF32格式，中间计算使用HF32，最终运算结果仍以fp32格式输出，以保证后续处理的兼容性。

HF32与float32的差异仅在尾数位宽度：Atlas A3系列与Atlas A2系列产品的HF32格式见下图；Ascend 950PR&950DT系列产品的HF32格式尾数位为10位。

**图13** HF32数值精度示意图

![](../../../api/figures/mmad_hf32.png "HF32数值精度示意图")

**图14** Ascend 950PR&950DT系列产品HF32数值精度示意图

![](../../../api/figures/mmad_hf32_950.png "Ascend 950PR&950DT系列产品HF32数值精度示意图")

### HF计算模式

HF计算模式是面向Cube（矩阵计算）的高性能float32计算模式，其作用对象是fp32矩阵乘：在输入矩阵A、B均为fp32数据类型的场景下，开启HF计算模式后，fp32数据在执行矩阵乘法运算之前会被转换并舍入为HF32格式参与计算。矩阵乘单元因此可用接近half的吞吐处理fp32输入，可显著提升计算性能，代价是输入端的一次舍入带来的精度损失。中间计算与累加仍在HF32/更宽的内部精度上进行，最终运算结果仍以fp32格式输出，对使用者透明。

fp32转HF32的舍入模式与HF计算模式的开关通过以下接口配置，各接口的产品支持情况请参见对应接口文档：

- 基础API：[SetHF32Mode](../../../api/SIMD-API/basic_api/cube_compute_ISASI/mmad_compute_aux_config/SetHF32Mode.md)开启或关闭HF32模式，开启后L0A Buffer/L0B Buffer中的fp32数据将在参与Mmad计算之前被舍入为HF32；
- C API：[asc_enable_hf32](../../../api/SIMD-API/c_api/cube_compute/mmad_compute_aux_config/asc_enable_hf32.md)开启、[asc_disable_hf32](../../../api/SIMD-API/c_api/cube_compute/mmad_compute_aux_config/asc_disable_hf32.md)关闭、[asc_set_hf32_round_mode](../../../api/SIMD-API/c_api/cube_compute/mmad_compute_aux_config/asc_set_hf32_round_mode.md)设置舍入模式。
- Matmul高阶API：Kernel侧通过[SetHF32](../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/SetHF32.md)设置；Tiling侧的[SetMadType](../../../api/SIMD-API/adv_api/cube_compute/Matmul_Tiling/SetMadType.md)对应`MatrixMadType::HF32`，**当前版本暂不支持**HF32模式设置，仅可用于开启MxMatmul场景。

HF32的完整特性说明请参见[HF32关键特性](../../../api/SIMD-API/basic_api/cube_compute_ISASI/mmad_compute_key_features/HF32.md)。
