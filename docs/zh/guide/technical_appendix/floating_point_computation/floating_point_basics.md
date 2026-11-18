# 浮点计算基础

自1985年IEEE-754二进制浮点运算标准发布以来，几乎所有主流计算系统都实现了该标准。IEEE-754规定了浮点运算的结果应如何近似表示。在异构计算环境，同一运算可能在不同硬件上执行，为了在获得正确结果的同时以所需精度达成最高性能，必须充分考虑浮点计算的诸多特性。

本章介绍浮点计算的基础性质，包括浮点格式、规格数与非规格数、特殊值、结合律与运算顺序、乘加融合、饱和与溢出处理以及舍入模式。

## 浮点格式

浮点数的格式与功能由IEEE-754标准定义。标准规定二进制浮点数据由三个字段编码而成：

- **符号（sign）**：1位，指示正数或负数。
- **指数（exponent）**：以偏置（bias）形式编码的以2为底的指数。
- **尾数（significand，亦称mantissa）**：编码数值的小数部分。

**图1** 浮点数格式示意图

![](../../figures/floating_point_format.png)


IEEE-754标准定义了以下二进制格式的编码与属性：

- 16位，即半精度（binary16），对应Ascend C中的数据类型`half`；
- 32位，即单精度（binary32），对应C/C++和Ascend C中的数据类型`float`；
- 64位，即双精度（binary64），对应C/C++中的数据类型`double`；
- 128位，即四精度（binary128）。

**表1** IEEE-754二进制浮点格式

| 格式 | 总位数 | 符号位 | 指数位 | 尾数位 | 指数偏置 |
| --- | --- | --- | --- | --- | --- |
| 半精度（binary16） | 16 | 1 | 5 | 10 | 15 |
| 单精度（binary32） | 32 | 1 | 8 | 23 | 127 |
| 双精度（binary64） | 64 | 1 | 11 | 52 | 1023 |
| 四精度（binary128） | 128 | 1 | 15 | 112 | 16383 |

**图2** IEEE-754二进制浮点格式示意图

![](../../figures/IEEE-754_binary_floating_point_format.png)

Ascend C支持的浮点数及其格式详细请参见[Ascend C数据类型汇总](./ascendc_data_type_summary.md)。

对[规格数](#规格数与非规格数)而言，编码对应的数值按下式计算：

`(-1)^sign × 1.mantissa × 2^(exponent - bias)`

对[非规格数](#规格数与非规格数)，公式修改为：

`(-1)^sign × 0.mantissa × 2^(1 - bias)`

尾数中"1."的整数部分是隐含的，不占用编码位。

例如，-192 = (-1)^1 × 2^7 × 1.5，编码为负的符号位、指数7以及小数部分0.5。指数7在单精度中表示为7 + 127 = 134 = `10000110`，在双精度中表示为7 + 1023 = 1030 = `10000000110`。尾数0.5 = 2^-1，即尾数位串的最高位为1、其余位为0。-192在单精度和双精度下的二进制编码如表2所示。

**表2** -192的二进制编码示例

| 格式 | 符号 | 指数 | 尾数 |
| --- | --- | --- | --- |
| 单精度 | 1 | 10000110 | 10000000000000000000000 |
| 双精度 | 1 | 10000000110 | 1000000000000000000000000000000000000000000000000000 |

由于尾数字段的位数有限，并非所有实数都能被精确表示。例如，三分之二的二进制表示为0.10101010…，小数点后有无穷多位。因此，三分之二在存入有限精度的浮点数之前必须先舍入。舍入的规则与模式由IEEE-754规定，其中最常用的是"舍入到最近、平局取偶"（round-to-nearest-ties-to-even），简称舍入到最近（round-to-nearest），参见本章[舍入模式](#舍入模式)一节。

## 规格数与非规格数

指数字段既非全0也非全1的浮点值称为规格数（Normal Number）。

浮点数的一个重要特性是：最小可表示正规格数（单精度下即FLT_MIN）与0之间的间隔，远大于FLT_MIN与次小规格数之间的间隔。换言之，如果不支持非规格数，数值在下溢时将突然塌缩为0，而不是渐进地损失精度。

非规格数（Subnormal Number，亦称Denormal）正是为解决这一问题而引入。非规格数的指数字段全为0，且尾数中至少有1位为1。非规格数是IEEE-754标准的必要组成部分。

**表3** 单精度下0附近的编码示意

| 数值类别 | 符号 | 指数 | 尾数 |
| --- | --- | --- | --- |
| 最小规格数（2^-126） | 0 | 00000001 | 00000000000000000000000 |
| 最大非规格数 | 0 | 00000000 | 11111111111111111111111 |
| 最小非规格数（2^-149） | 0 | 00000000 | 00000000000000000000001 |
| +0 | 0 | 00000000 | 00000000000000000000000 |

非规格数的取值在最小与最大非规格数之间连续变化：指数字段保持全0，数值信息全部由尾数承载，随尾数增大而逐步逼近最小规格数，从而实现"渐进下溢"。

Ascend C支持的各浮点数据类型中，除fp8_e8m0_t（无尾数位，仅能表示2的幂次）外，其余浮点类型均按IEEE-754风格编码非规格数。各类型的非规格数范围汇总如下：

**表4** Ascend C浮点数据类型的非规格数范围

| 类型 | 符号位 | 指数位 | 尾数位 | 最小正非规格数 | 最大正非规格数 | 非规格数支持 |
| --- | --- | --- | --- | --- | --- | --- |
| half | 1 | 5 | 10 | 2^-24 | 2^-14 - 2^-24 | 支持 |
| bfloat16_t | 1 | 8 | 7 | 2^-133 | 2^-126 - 2^-133 | 支持 |
| float | 1 | 8 | 23 | 2^-149 | 2^-126 - 2^-149 | 支持 |
| double | 1 | 11 | 52 | 2^-1074 | 2^-1022 - 2^-1074 | 支持 |
| fp8_e5m2_t | 1 | 5 | 2 | 2^-16 | 2^-14 - 2^-16 | 支持 |
| fp8_e4m3fn_t | 1 | 4 | 3 | 2^-9 | 2^-6 - 2^-9 | 支持 |
| fp8_e8m0_t | 0 | 8 | 0 | - | - | 不支持，仅纯2的幂次，无尾数 |
| fp4x2_e2m1_t | 1 | 2 | 1 | 0.5 | 0.5 | 支持 |
| fp4x2_e1m2_t | 1 | 1 | 2 | 0.25 | 0.75 | 支持 |

## 浮点数的特殊值

IEEE-754标准为浮点数定义了三类特殊值：零、无穷与NaN。

**零（zero）**：

- 数学意义上的零。
- 浮点零有两种表示：+0与-0，这与整数零只有一种表示不同。
- `+0 == -0`的比较结果为true。
- 零的编码为指数字段与尾数字段全部为0。

**无穷（infinity）**：

- 当结果超出可表示范围时，运算结果会得到+Infinity或-Infinity。
- 无穷的编码为指数字段全1、尾数字段全0，恰有两种编码：+∞与-∞。
- 无穷与有限非零值的算术运算通常结果为无穷；不定式如Inf × 0.0、Inf - Inf、Inf / Inf、0.0 / 0.0的结果为NaN。

**非数（NaN，Not-a-Number）**：

NaN以两种形式提供：

- **静默NaN（quiet NaN，qNaN）**：用于传播由无效运算或无效值产生的错误。无效的算术运算通常产生静默NaN，其编码为尾数最高有效位置1。
- **信号NaN（signaling NaN，sNaN）**：用于触发无效运算异常，一般由程序显式创建，其编码为尾数最高有效位置0。

NaN有以下四种特性：

- 表示未定义或不可表示的值，常见的产生例子有0.0 / 0.0、sqrt(-1.0)、Inf - Inf。
- NaN的编码为指数字段全1、尾数为任意非全0模式，共有2^(尾数位数+1) - 2种可能的编码。以单精度为例，共有2^24 - 2 = 16777214种。
- 任何涉及NaN的算术运算，其结果均为NaN。
- 任何涉及NaN的有序比较（<、<=、>、>=、==）的结果均为false，包括`NaN == NaN`（非自反性）；无序比较`NaN != NaN`返回true。

**表5** 特殊值编码一览

| 特殊值 | 符号 | 指数 | 尾数 | 说明 |
| --- | --- | --- | --- | --- |
| ±0 | 0或1 | 全0 | 全0 | +0与-0相等 |
| 非规格数 | 0或1 | 全0 | 至少1位为1 | 渐进下溢 |
| 规格数 | 0或1 | 非0非全1 | 任意 | 常规数值 |
| ±∞ | 0或1 | 全1 | 全0 | 恰有两种编码 |
| NaN | 0或1 | 全1 | 非全0 | 尾数最高位为1是qNaN，为0是sNaN |

## 结合律与运算顺序

需要注意的是，由于浮点数的精度有限，数学算术的规则与性质并不能直接套用到浮点算术上。下面的例子展示了单精度值A、B、C在采用不同结合顺序求和时的情形：

作为参照，下面给出了精确的数学结果，展示在纯数学，即无限精度下两种结合顺序的结果：

```
A = 2^1 × 1.00000000000000000000001

B = 2^0 × 1.00000000000000000000001

C = 2^3 × 1.00000000000000000000001

(A + B) + C = 2^3 × 1.01100000000000000000001011

A + (B + C) = 2^3 × 1.01100000000000000000001011
```

在数学上，(A + B) + C与A + (B + C)相等。

设rn(x)表示对x执行一次舍入。在单精度浮点算术中，按IEEE-754规定的`就近舍入`模式执行同样的计算，得到：

```
A + B = 2^1 × 1.1000000000000000000000110000…

rn(A + B) = 2^1 × 1.10000000000000000000010

B + C = 2^3 × 1.0010000000000000000000100100…

rn(B + C) = 2^3 × 1.00100000000000000000001

A + B + C = 2^3 × 1.0110000000000000000000101100…

rn(rn(A + B) + C) = 2^3 × 1.01100000000000000000010

rn(A + rn(B + C)) = 2^3 × 1.01100000000000000000001
```

可以看到：按IEEE-754计算的结果与精确的数学结果不同；更重要的是，rn(rn(A + B) + C)与rn(A + rn(B + C))这两个结果彼此也不相同。在本例中，rn(A + rn(B + C))比rn(rn(A + B) + C)更接近正确的数学结果。

这个例子说明，结合律在浮点算术中不成立：改变加法的结合顺序，即使每次运算都严格执行IEEE-754规定的舍入，也会得到不同的结果。

## 乘加融合

乘加融合（Fused Multiply-Add，FMA）运算只需一次舍入即可得到结果。若不使用FMA，结果需要两次舍入：乘法一次、加法一次。由于只执行一次舍入，FMA能产生更精确的结果。

先看一个十进制的例子来说明FMA的工作方式。以五位有效数字，即小数点后四位计算x^2 - 1：

当x = 1.0008时，正确的数学结果是$x^2 - 1 = 1.60064 \times 10^{-3}$。仅用小数点后四位能表示的最接近的数是$1.6006 \times 10^{-3}$。

FMA只经一次舍入即可得到正确结果：$rn(x \times x - 1) = 1.6006 \times 10^{-3}$。

另一种做法是分开执行乘法和加法。x^2 = 1.00160064，舍入后$rn(x \times x) = 1.0016$。最终结果为$rn(rn(x \times x) - 1) = 1.6000 \times 10^{-3}$。

将乘法和加法分别舍入，得到的结果偏差$0.00064 \times 10^{-3}$（即$6.4 \times 10^{-7}$）；而FMA计算仅偏差$0.00004 \times 10^{-3}$（即$4 \times 10^{-8}$），其结果最接近正确的数学答案。结果汇总如下：

$$
\begin{aligned}
x &= 1.0008 \\
x^2 &= 1.00160064 \\
x^2 - 1 &= 1.60064 \times 10^{-3} \quad \text{（真值）} \\
rn(x^2 - 1) &= 1.6006 \times 10^{-3} \quad \text{（乘加融合）} \\
rn(x^2) &= 1.0016 \\
rn(rn(x^2) - 1) &= 1.6000 \times 10^{-3} \quad \text{（先乘后加）}
\end{aligned}
$$

下面是另一个采用二进制单精度值的例子：

$$
\begin{aligned}
A &= 2^0 \times 1.00000000000000000000001 \\
B &= -2^0 \times 1.00000000000000000000010 \\
rn(A \times A + B) &= 2^{-46} \times 1.00000000000000000000000 \quad \text{（乘加融合）} \\
rn(rn(A \times A) + B) &= 0 \quad \text{（先乘后加）}
\end{aligned}
$$

分开计算乘法与加法会导致全部精度丢失，结果为0；而使用FMA计算，得到的结果等于数学真值。

FMA有助于防止相消造成的精度损失。相消是指大小相近、符号相反的量相加时，前导有效位大量抵消，导致结果中的有效位数变少。FMA在乘法阶段计算的是双宽度的乘积，因此即使加法阶段发生相消，乘积中仍有足够多的有效位来产生精确的结果。

### SIMD与SIMT乘加融合的区别

SIMD与SIMT两种编程范式中，乘加融合的获取方式与控制粒度存在本质区别。

**SIMD范式**：融合与否由用户编写的API显式决定。SIMD的计算以API调用表达，每次调用独立发射指令，融合路径与分步路径清晰可分。

- 以下API是融合类API：
    - 基础API [Axpy](../../../api/SIMD-API/basic_api/memory_vector_compute/composite_compute/Axpy.md)：单条乘加指令。
    - 基础API [MulDstAdd](../../../api/SIMD-API/basic_api/reg_vector_compute/composite_compute/MulDstAdd.md)：三操作数融合乘加。
    - 高阶API [Fma](../../../api/SIMD-API/adv_api/math_compute/Fma_interface/Fma.md)：按元素计算两个输入相乘后与第三个输入相加。
- 以下是分步计算API：分开调用[Mul](../../../api/SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Mul.md)与[Add](../../../api/SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Add.md)，经过两次舍入，且可获取中间结果。

**SIMT范式**：标量表达式`x * y + z`会被编译器优化为乘加指令，即默认获得融合乘加。也可以显式调用函数：
- [fmaf](../../../api/SIMT-API/math_functions/float_math_functions/fmaf.md)(x, y, z)：float的融合乘加。
- `__hfma(x, y, z)`：[bfloat16数据类型](../../../api/SIMT-API/math_functions/bfloat16_type/bfloat16_arithmetic_functions/__hfma.md)、[half数据类型](../../../api/SIMT-API/math_functions/half_type/half_arithmetic_functions/__hfma.md)的融合乘加，遵循CAST_RINT模式对结果舍入，另有[bfloat16数据类型__hfma_relu](../../../api/SIMT-API/math_functions/bfloat16_type/bfloat16_arithmetic_functions/__hfma_relu.md)、[half数据类型__hfma_relu](../../../api/SIMT-API/math_functions/half_type/half_arithmetic_functions/__hfma_relu.md)变体。

## 饱和与溢出处理

当运算结果超出浮点类型可表示的范围时，IEEE-754的运算结果不会回绕或报错，而是返回为+Infinity或-Infinity，详细参见[特殊值](#浮点数的特殊值)章节。

在Ascend C的浮点运算中，存在饱和模式和非饱和模式。在类型转换场景中，饱和模式控制对nan、inf特殊值以及转换后超出目标类型范围结果的处理方式，分为两种模式：

**表6** 饱和与非饱和模式下的转换结果

| 输入情形 | 非饱和模式 | 饱和模式 |
| --- | --- | --- |
| 输入超出目标类型所能表示的最大值 | +inf | 目标类型最大值 |
| 输入超出目标类型所能表示的最小值 | -inf | 目标类型最小值 |
| 输入为+inf | +inf | 目标类型最大值 |
| 输入为-inf | -inf | 目标类型最小值 |
| 输入为nan | nan | 0 |

以float转half为例，两种模式的转换结果如下：

**表7** float转half的饱和与非饱和模式转换结果示例

| 输入 | 饱和模式 | 非饱和模式 |
| --- | --- | --- |
| inf | 65504（half最大值） | inf |
| -inf | -65504 | -inf |
| nan | 0 | nan |

### 设置饱和模式

饱和模式的设置按编程范式分为SIMD与SIMT两种场景：SIMD场景下分为单指令与寄存器两种设置方式；SIMT场景不支持设置饱和模式，仅在SIMD与SIMT混合编程时借助SIMD侧的饱和设置生效。

#### SIMD场景

**寄存器方式**：
- 通过[SetSaturationFlag](../../../api/SIMD-API/basic_api/special_register_access/SetSaturationFlag_ISASI.md)按模式设置全局饱和开关，本质是对CTRL寄存器饱和相关比特位的封装；
- 通过基础API [SetCtrlSpr](../../../api/SIMD-API/basic_api/special_register_access/SetCtrlSpr_ISASI.md)或C API [asc_set_ctrl](../../../api/SIMD-API/c_api/spr/asc_set_ctrl.md)直接读写相应比特位设置饱和模式。

**单指令方式**：饱和模式在编译期作为指令操作数编码进指令，仅用于数据类型转换的饱和模式设置。
- [Reg矢量计算Cast](../../../api/SIMD-API/basic_api/reg_vector_compute/type_conversion/Cast.md)通过模板参数`CastTrait::satMode`设置：`SatMode::SAT`为饱和模式，`SatMode::NO_SAT`为非饱和模式；
- [Memory矢量计算Cast](../../../api/SIMD-API/basic_api/memory_vector_compute/type_conversion/Cast.md)不提供单接口饱和模式参数，其单接口配置固定为饱和模式。

#### SIMT场景

SIMT编程场景不支持设置饱和模式：暂不支持设置CTRL寄存器。在SIMD与SIMT混合编程场景下可通过[SetSaturationFlag](../../../api/SIMD-API/basic_api/special_register_access/SetSaturationFlag_ISASI.md)或[SetCtrlSpr(ISASI)](../../../api/SIMD-API/basic_api/special_register_access/SetCtrlSpr_ISASI.md)接口控制全局饱和模式。具体请参见[SIMT饱和模式设置](../../../api/SIMT-API/math_functions/common_constraints.md#饱和模式设置)。

## 舍入模式

IEEE-754标准定义了四种舍入模式：

**表8** IEEE-754舍入模式

| 舍入模式 | 含义 |
| --- | --- |
| rn | 舍入到最近，平局取偶（round to nearest, ties to even） |
| rz | 向零舍入（round towards zero） |
| ru | 向上舍入，朝正无穷方向（round towards +∞） |
| rd | 向下舍入，朝负无穷方向（round towards -∞） |

Ascend C的类型转换接口在此基础上提供了更丰富的舍入模式：

**表9** 类型转换类接口支持的舍入模式

| 舍入模式 | 描述 |
| --- | --- |
| CAST_NONE | 当转换过程存在精度损失时，按CAST_RINT模式处理；当不存在精度损失时，不进行舍入 |
| CAST_RINT | 四舍六入五成双取整 |
| CAST_FLOOR | 向下取整 |
| CAST_CEIL | 向上取整 |
| CAST_ROUND | 四舍五入取整 |
| CAST_TRUNC | 向零取整 |
| CAST_ODD | 最近邻奇数舍入 |
| CAST_HYBRID | 目前特指输出结果为hi8数据时，会用到的一种随机舍入 |

各舍入模式的行为示例（float转half，输入0.5 + 2^(-12)，写成float的表示形式2^(-1) × (1 + 2^(-11))；half的指数位可以表示出2^(-1)，但只有10 bit尾数位，因此要进行舍入）：

- CAST_RINT模式舍入得尾数0000000000，最终表示的结果为0.5；
- CAST_FLOOR模式舍入得尾数0000000000，最终表示的结果为0.5；
- CAST_CEIL模式舍入得尾数0000000001，最终表示的结果为0.5 + 2^(-11)；
- CAST_ROUND模式舍入得尾数0000000001，最终表示的结果为0.5 + 2^(-11)；
- CAST_TRUNC模式舍入得尾数0000000000，最终表示的结果为0.5；
- CAST_ODD模式舍入得尾数0000000001，最终表示的结果为0.5 + 2^(-11)。

舍入模式处理的是目标类型可表示范围内的精度损失；当输入或转换结果超出目标类型的表示范围时，如float转half时输入1e10，舍入模式不再适用，溢出行为由饱和模式决定。浮点转浮点场景下，两类模式对溢出及特殊值的处理行为如下：

**表10** 浮点转浮点的饱和溢出行为

| 输入情形 | 饱和模式 | 不饱和模式 |
| --- | --- | --- |
| 绝对值超过目标类型最值（如float 1e10转half） | 饱和为±MAX（目标类型最大/最小有限值，如half为±65504） | 溢出为±inf |
| 输入±inf | 饱和为±MAX | 保持±inf |
| 输入nan | 饱和为0 | 保持nan |

注：上表适用于half、bfloat16_t等具备inf编码的目标类型。fp8_e4m3fn_t没有inf编码，不饱和模式下超限或输入±inf时输出为nan；其饱和模式下输入nan默认输出0。fp8_e8m0_t、fp4x2_e2m1_t等类型的特殊溢出行为见[精度转换](../../../api/SIMD-API/basic_api/data_structures/precision_conversion.md)。

浮点数据类型的具体转换规则示例，详见[SIMD精度转换](../../../api/SIMD-API/basic_api/data_structures/precision_conversion.md)与[SIMT精度转换](../../../api/SIMT-API/math_functions/data_type_conversion/overview.md)。

具体的舍入类型支持参见API手册介绍：[Reg矢量计算Cast](../../../api/SIMD-API/basic_api/reg_vector_compute/type_conversion/Cast.md)、[Memory矢量计算Cast](../../../api/SIMD-API/basic_api/memory_vector_compute/type_conversion/Cast.md)、[SIMT数据类型转换API](../../../api/SIMT-API/math_functions/data_type_conversion/type_conversion_functions/type_conversion_functions.md)。
