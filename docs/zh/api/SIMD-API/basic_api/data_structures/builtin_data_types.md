# 内置数据类型<a name="ZH-CN_TOPIC_0000002313437229"></a>

## 数据类型列表<a name="section16395539499"></a>

Ascend C提供b8\~b64（8bit\~64bit）四种不同位宽的数据类型，不同位宽对应的数据类型如下表所示。

**表1**  不同位宽对应的数据类型

| 位宽 | 数据类型 |
| :--: | :------------: |
| b8 | bool、int4x2_t、int8_t、uint8_t、fp4x2_e2m1_t、fp4x2_e1m2_t、hifloat8_t、fp8_e8m0_t、fp8_e5m2_t、fp8_e4m3fn_t。 |
| b16 | int16_t、uint16_t、half、bfloat16_t。 |
| b32 | int32_t、uint32_t、float、complex32。 |
| b64 | int64_t、uint64_t、double、complex64。 |

> [!NOTE]说明
> int4x2_t数据类型由两个四位整型数打包为一个8比特存储单元。

<a id="datatype-abbreviation"></a>

为了方便描述这些数据类型，提供如下的数据类型简写：

**表2**  数据类型简写

| 数据类型简写（位宽从低到高） | 对应数据类型 |
| :---------------: | :------------: |
| S4        | int4b_t |
| S8        | int8_t |
| U8        | uint8_t |
| HiF8      | hifloat8_t |
| FP8_E4M3  | fp8_e4m3fn_t |
| FP8_E5M2  | fp8_e5m2_t |
| FP8_E8M0  | fp8_e8m0_t |
| FP4_E2M1  | fp4x2_e2m1_t |
| FP4_E1M2  | fp4x2_e1m2_t |
| S16       | int16_t |
| U16       | uint16_t |
| FP16      | half |
| BF16      | bfloat16_t |
| S32       | int32_t |
| U32       | uint32_t |
| FP32      | float |
| S64       | int64_t |
| U64       | uint64_t |

> [!NOTE]说明
>其中，只有bool、int8\_t、uint8\_t、int16\_t、uint16\_t、half、int32\_t、uint32\_t、float、int64\_t、uint64\_t这些数据类型支持使用立即数进行赋值和初始化。
>示例：
>
>```cpp
>int8_t scalar = 1;
>int32_t valueOut = AscendC::Cast<float, int32_t, AscendC::RoundMode::CAST_ROUND>((float)1);
>```

## 数据类型的产品支持情况<a name="section137212564579"></a>

各产品支持的数据类型如下：

<!-- npu="950" id2 -->
- Ascend 950PR/Ascend 950DT：bool、int4b_t、int4x2_t、int8_t、uint8_t、fp4x2_e2m1_t、fp4x2_e1m2_t、hifloat8_t、fp8_e8m0_t、fp8_e5m2_t、fp8_e4m3fn_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、complex32、int64_t、uint64_t、double、complex64。
<!-- end id2 -->
<!-- npu="A3" id3 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：bool、int4b_t、int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、int64_t、uint64_t、double。
<!-- end id3 -->
<!-- npu="910b" id4 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：bool、int4b_t、int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、int64_t、uint64_t、double。
<!-- end id4 -->
<!-- npu="310b" id5 -->
- Atlas 200I/500 A2 推理产品：bool、int4b_t、int8_t、uint8_t、int16_t、uint16_t、half、int32_t、uint32_t、float、int64_t、uint64_t、double。
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品AI Core：bool、int4b_t、int8_t、uint8_t、int16_t、uint16_t、half、int32_t、uint32_t、float、int64_t、uint64_t、double。
<!-- end id6 -->
<!-- npu="310p" id7 -->
- Atlas 推理系列产品Vector Core：bool、int4b_t、int8_t、uint8_t、int16_t、uint16_t、half、int32_t、uint32_t、float、int64_t、uint64_t、double。
<!-- end id7 -->
<!-- npu="910" id8 -->
- Atlas 训练系列产品：bool、int4b_t、int8_t、uint8_t、int16_t、uint16_t、half、int32_t、uint32_t、float、int64_t、uint64_t、double。
<!-- end id8 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/data_structures/builtin_data_types_res.md#id1 -->

## 布尔型<a name="section82692022135113"></a>

bool类型占8比特，全0时代表false，否则代表true。

## 整数<a name="section1529635411502"></a>

整数由符号位（S）和数值部分（M）组成，不同类型的整数在符号位和数值部分的比特分配上有所不同。无符号整数没有符号位，所有比特均用于表示数值。

下图是一个int8\_t类型的示例，其符号位占用1位，数值部分占用7位。$S_v=0$，$M_v = 2^5 + 2^6$，表示的结果为96。下标v表示符号位和数值部分的具体数值。

**图1** int8_t数据类型二进制示意图，值为96

![int8_t数据类型二进制示意图](../../../figures/int8_t-binary-96.png)

整数数据类型取值范围见下表。

**表4** 整数数据类型取值范围

| 类型 | 取值范围 |
| ---- | -------- |
| int4b_t | [-8, 7] |
| int8_t | [-128, 127] |
| uint8_t | [0, 255] |
| int16_t | [-32768, 32767] |
| uint16_t | [0, 65535] |
| int32_t | [-2147483648, 2147483647] |
| uint32_t | [0, 4294967295] |
| int64_t | [-9223372036854775808, 9223372036854775807] |
| uint64_t | [0,18446744073709551615] |

## 浮点数<a name="section1330111025115"></a>

浮点数由符号位（S）、指数位（E）、尾数位（M）三个部分组成，不同类型的浮点数，三个部分所占的比特数可能不同。

根据指数位和尾数位的位模式（全0，全1，混合），浮点数被分为三类：

- 规格数（Normal Numbers）：指数位E不全为0且不全为1。表示常规范围内的浮点数，占绝大多数有效数值，尾数隐含一个前导1，形式为$1 + M_v$。
- <a name="p7381131713310"></a>非规格数（Subnormal Numbers）：指数位E全为0，且尾数位M不全为0。表示接近零的极小值，尾数无前导1，形式为$M_v$。
- 特殊值：
    - 零（Zeros）：指数位E全为0，尾数位M全为0。
    - 无穷大（Infinites）：指数位E全为1，尾数位M全为0。
    -   Quiet NaN \(qNaN\)：指数全1，尾数最高位=1。
    -   Signaling NaN \(sNaN\)：指数全1，尾数最高位=0，尾数≠0。

浮点数的计算公式如下：

- 对于规格数：

    $$
    Value = (-1)^{S} \times (1 + M_v) \times 2^{(E_v - Bias)}
    $$

- 对于非规格数：

    $$
    Value = (-1)^{S} \times M_v \times 2^{(1 - Bias)}
    $$

其中：

$$
M_v = \sum_{i=0}^{Mbits - 1}{m_i \times 2^{-Mbits + i}}，Mbits表示尾数位宽
$$

$$
E_v = \sum_{i=0}^{Ebits - 1}{e_i \times 2^i}，Ebits表示指数位宽
$$

$$
Bias = 2^{Ebits - 1} -1，Ebits表示指数位宽
$$

浮点数数据类型的取值范围如下表所示。

**表5** 浮点数据类型

| 类型 | 符号位宽 | 指数位宽 | 尾数位宽 | 取值范围 |
| ---- | -------- | -------- | -------- | -------- |
| fp4x2_e2m1_t | 1 | 2 | 1 | [$-6$, $6$] |
| fp4x2_e1m2_t | 1 | 1 | 2 | [$-7 * 2^{-2}$, $7 * 2^{-2}$] |
| fp8_e8m0_t | 0 | 8 | 0 | [$2^{-127}$, $2^{127}$] |
| fp8_e5m2_t | 1 | 5 | 2 | [$2^{13} - 2^{16}$, $2^{16} - 2^{13}$] |
| fp8_e4m3fn_t | 1 | 4 | 3 | [$2^{6} - 2^{9}$, $2^{9} - 2^{6}$] |
| half | 1 | 5 | 10 | [$2^{5} - 2^{16}$, $2^{16} - 2^{5}$] |
| bfloat16_t | 1 | 8 | 7 | [$2^{120} - 2^{128}$, $2^{128} - 2^{120}$] |
| float | 1 | 8 | 23 | [$2^{104} - 2^{128}$, $2^{128} - 2^{104}$] |
| double | 1 | 11 | 52 | [$2^{971} - 2^{1024}$, $2^{1024} - 2^{971}$] |

> [!NOTE]说明
>fp4x2\_e2m1\_t和fp4x2\_e1m2\_t数据类型会将两个独立的四位浮点数打包为一个8比特存储单元。

- fp4x2\_e2m1\_t

    下图是一个fp4x2\_e2m1\_t类型的示例，其符号位占用1位，指数位占用2位，尾数位占用1位。

    **图2** fp4x2_e2m1_t数据类型二进制示意图

    ![](../../../figures/f8_1.png)

- fp4x2\_e1m2\_t

    下图是一个fp4x2\_e1m2\_t类型的示例，其符号位占用1位，指数位占用1位，尾数位占用2位。

    **图3** fp4x2_e1m2_t数据类型二进制示意图

    ![](../../../figures/f8_1-0.png)

- fp8\_e8m0\_t（fp8\_e8m0\_t二进制由bfloat16类型舍弃符号位、尾数位得到）

    下图是一个fp8\_e8m0\_t类型的示例，该类型没有符号位和尾数位，指数位占用8位。

    **图4** fp8_e8m0_t数据类型二进制示意图

    ![](../../../figures/f8_1-1.png)

- fp8\_e5m2\_t

    下图是一个fp8\_e5m2\_t类型的示例，其符号位占用1位，指数位占用5位，尾数位占用2位，表示的结果为$(-1)^0 \times (1 + 0.5 + 0.25) \times 2^{30-15}=1.75 \times 2^{15}$。

    **图5** fp8_e5m2_t数据类型二进制示意图

    ![](../../../figures/f8_1-2.png)

    fp8\_e5m2\_t的特殊值bit位表示如下：

    **图6** fp8_e5m2_t特殊值bit位表示

    ![](../../../figures/fp8_e5m2_special_values.png)

- fp8\_e4m3fn\_t

    下图是一个fp8\_e4m3fn\_t类型的示例，其符号位占用1位，指数位占用4位，尾数位占用3位，表示的结果为 $(-1)^1 \times 2^{-3} \times 2^{-6}$。

    **图7** fp8_e4m3fn_t数据类型二进制示意图

    ![](../../../figures/fp8_e4m3fn_binary.png)

    fp8\_e4m3fn\_t的特殊值bit位表示如下：

    **图8** fp8_e4m3fn_t特殊值bit位表示

    ![](../../../figures/fp8_e4m3fn_special_values.png)

- hifloat8\_t

    hifloat8\_t类型相对其他类型增加了指数位宽控制字段D，用于指示指数位和尾数位的编码方式。

    hifloat8\_t类型根据点域的不同，有不同的编码方式，下面一一列出。符号、指数和尾数分别缩写成‘S’，‘E’和‘M’。

    **图9** S、E、M在不同点域D值下的bit位分布<a name="fig261021710100"></a>

    ![](../../../figures/sem_bit_distribution.png "S-E-M在不同点域D值下的bit位分布")

    下图示例中，其符号位占用1位，指数位占用2位，尾数位占用3位，D字段为2比特b01，$S_v=1$，$E_v=3$，$M_v = 2^{-1} + 2^{-2}$，根据实际取值计算公式$S_v \times 2^{E_v} \times (1 + M_v)$计算得到的结果为14。下标v表示各部分的具体数值。

    **图10** hifloat8_t数据类型二进制示意图，值为14

    ![](../../../figures/hifloat8_binary.png)

    hifloat8\_t类型的取值范围如下表所示：

    **表6** hifloat8\_t类型取值范围

    <a name="table0308852103016"></a>

    | 符号位宽 | 指数位宽控制字段位宽 | 指数位宽 | 尾数位宽 | 指数位宽控制字段取值 | 符号取值范围（$S_v$） | 指数取值范围（$E_v$） | 尾数取值范围（$M_v$） | 实际取值计算公式 |
    | -------- | -------------------- | -------- | -------- | -------------------- | ----------------------------- | ----------------------------- | ----------------------------- | ---------------- |
    | 1 | 4 | 0 | 3 | 4'b0000 | ±1 | - | [0, 7] | $S_v \times 2^{M_v - 23}$ |
    | 1 | 4 | 0 | 3 | 4'b0001 | ±1 | 0 | [0, $7 * 2^{-3}$] | $S_v \times 2^{E_v} \times (1 + M_v)$ |
    | 1 | 3 | 1 | 3 | 4'b001 | ±1 | ±1 | [0, $7 * 2^{-3}$] | $S_v \times 2^{E_v} \times (1 + M_v)$ |
    | 1 | 2 | 2 | 3 | 2'b01 | ±1 | ±[2, 3] | [0, $7 * 2^{-3}$] | $S_v \times 2^{E_v} \times (1 + M_v)$ |
    | 1 | 2 | 3 | 2 | 2'b10 | ±1 | ±[4, 7] | [0, $3 * 2^{-2}$] | $S_v \times 2^{E_v} \times (1 + M_v)$ |
    | 1 | 2 | 4 | 1 | 2'b11 | ±1 | ±[8, 15] | [0, $2^{-1}$] | $S_v \times 2^{E_v} \times (1 + M_v)$ |

    hifloat8\_t特殊值bit位表示如下：

    **图11**  hifloat8_t特殊值bit位表示

    ![](../../../figures/hifloat8_special_values.png)

    hifloat8\_t数据类型计算公式如下：

    - **符号位$S_v$**

        s\_bit\_val为1表示负数，s\_bit\_val为0表示非负数。

        $S_v = (-1)^{s\_bit\_val}$

    - **指数位$E_v$**

        由Es、Em组成。

        **表7** 不同D域值，Es，Em bit大小不同

        <a name="table15755618132"></a>

        | D值 | Es 值范围 | Em值范围 |
        | ---- | --------- | --------- |
        | 3b001 | 0-1 | - |
        | 2b01 | 0-1 | 10~11 |
        | 2b10 | 0-1 | 100~111 |
        | 2b11 | 0-1 | 1000~1111 |

        es\_bit\_val值表示E的最高位，用于计算Ev的符号值，例如E为0b1100，则es\_bit\_val值为最高位1，又如E为0b011，则es\_bit\_val为最高位0。

        Es计算公式：

        (1)$Es\_value = (-1)^{es\_bit\_val}$

        em\_bit\_val值表示E除开最高位的剩余位数加上前置1，用于计算Ev的实际值，例如E为0b1100，则em\_bit\_val值为剩余位100，加上前置1为1100，又如E为0b011，em\_bit\_val值为剩余位11，加上前置1为111。

        Em计算公式如下，Emi表示Em每个bit位的值（0或1），D表示点域值，参考[表7](#table15755618132)。

        (2)$Em\_value = E|(1<<\textit{bit width of E})$
    
        Ev计算公式：
        
        (3)$Ev\_value = Es\_value * Em\_value$

        Mv计算公式，M表示位数值，bitwidth of M表示M的bit宽度大小，参考[表7](#table15755618132)。

        (4)$Mv\_value = M / 2^{\textit{bitwidth of M}}$

    - 在Normal和Subnormal模式下，浮点数的取值计算公式不同：

        Normal模式：由Sv、Ev、Mv组成。

        (1)$value = S_v * 2^{E_v} * (1 + M_v)$
        
        Subnormal模式，由Sv、Mv组成。

        $Mv = M$

        (2)$value = S_v * 2^{M_v - 23}$

- half

    half数据类型由1位符号位（S）、5位指数位（E）、10位尾数位（M）组成。下图是一个half类型规格数的示例，其表示的结果为 $(-1)^0 \times 2^{(15 - 15)} \times (1+ 0.75) = 1.75$。下标v表示各部分的具体数值。

    **图12** half数据类型二进制示意图，值为1.75

    ![half数据类型二进制示意图-值为1-75](../../../figures/half-binary-1-75.png)

    **表8** half类型特殊值bit位表示

    | S | E | M | 特殊值 |
    | - | - | - | ------ |
    | 0 | 全为0 | 全为0 | +0 |
    | 1 | 全为0 | 全为0 | -0 |
    | 0 | 全为1 | 全为0 | +inf |
    | 1 | 全为1 | 全为0 | -inf |
    | 任意 | 全为1 | 不全为0 | nan |
    | 0 | 5'h00 | 10'h001 | 最小正非规格数 |
    | 0 | 5'h00 | 10'h3FF | 最大正非规格数 |
    | 0 | 5'h01 | 10'h000 | 最小正规格数 |
    | 0 | 5'h1E | 10'h3FF | 最大正规格数 |

- bfloat16\_t

    bfloat16数据类型由1位符号位（S）、8位指数位（E）、7位尾数位（M）组成。下图是一个bfloat16\_t数据类型规格数的示例，其表示的结果为 $(-1)^0 \times 2^{(127 - 127)} \times (1+ 0.75) = 1.75$。

    **图13** bfloat16\_t数据类型二进制示意图，值为1.75<a name="zh-cn_topic_0000002563730205_fig760011915344"></a>

    ![bfloat16_t数据类型二进制示意图-值为1.75](../../../figures/bfloat16_t-binary-1-75.png)
    
    **表9** bfloat16\_t类型特殊值

    | S | E | M | 特殊值 |
    | - | - | - | ------ |
    | 0 | 全为0 | 全为0 | +0 |
    | 1 | 全为0 | 全为0 | -0 |
    | 0 | 全为1 | 全为0 | +inf |
    | 1 | 全为1 | 全为0 | -inf |
    | 任意 | 全为1 | 不全为0 | nan |
    | 0 | 8'h00 | 7'h01 | 最小正非规格数 |
    | 0 | 8'h00 | 7'h7F | 最大正非规格数 |
    | 0 | 8'h01 | 7'h00 | 最小正规格数 |
    | 0 | 8'hFE | 7'h7F | 最大正规格数 |

- float

    float数据类型由1位符号位（S）、8位指数位（E）、23位尾数位（M）组成。下图是一个float类型规格数的示例，其表示的结果为$(-1)^0 \times 2^{(127 - 127)} \times (1+ 0.75) = 1.75$。

    **图14** float数据类型二进制示意图，值为1.75<a name="zh-cn_topic_0000002563730205_fig54861751133412"></a>
    
    ![float数据类型二进制示意图-值为1-75](../../../figures/float-binary-1-75.png)

    **表10** float类型特殊值

    | S | E | M | 特殊值 |
    | - | - | - | ------ |
    | 0 | 全为0 | 全为0 | +0 |
    | 1 | 全为0 | 全为0 | -0 |
    | 0 | 全为1 | 全为0 | +inf |
    | 1 | 全为1 | 全为0 | -inf |
    | 任意 | 全为1 | 不全为0 | nan |
    | 0 | 8'h00 | 23'h000001 | 最小正非规格数 |
    | 0 | 8'h00 | 23'h7FFFFF | 最大正非规格数 |
    | 0 | 8'h01 | 23'h000000 | 最小正规格数 |
    | 0 | 8'hFE | 23'h7FFFFF | 最大正规格数 |

<!-- npu="950" id1 -->
## 复数<a name="section8666633175111"></a>

Ascend C提供了复数数据类型：complex32和complex64。

定义如下：

```cpp
using complex32 = AscendC::Complex<half>;
using complex64 = AscendC::Complex<float>;
```

其中Complex定义结构见[complex32/complex64](../aux_data_structures/complex32-complex64.md)。

complex32即实部和虚部都是half类型的复数。

complex64即实部和虚部都是float类型的复数。

示例：

```cpp
complex32 value0(1, 2);
// value0将代表实部为1，虚部为2的复数，即1+2j
complex32 value1(3);
// value1将代表实部为3，虚部为0的复数，即3+0j
complex64 value2 = 4;
// value2将代表实部为4，虚部为0的复数，即4+0j
```

当前仅Ascend 950PR/Ascend 950DT支持。
<!-- end id1 -->
