# Built-in Data Types<a name="ZH-CN_TOPIC_0000002313437229"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T14:08:32.711Z pushedAt=2026-10-26T07:41:44.274Z nl=2 -->

## Data Type List<a name="section16395539499"></a>

Ascend C provides b8\~b64 (8bit\~64bit) data types with four different bit widths. The data types corresponding to each bit width are shown in the following table.

**Table 1**  Data types corresponding to different bit widths

| Bit Width | Data Type |
| :--: | :------------: |
| b8 | bool, int4x2_t, int8_t, uint8_t, fp4x2_e2m1_t, fp4x2_e1m2_t, hifloat8_t, fp8_e8m0_t, fp8_e5m2_t, fp8_e4m3fn_t. |
| b16 | int16_t, uint16_t, half, bfloat16_t. |
| b32 | int32_t, uint32_t, float, complex32. |
| b64 | int64_t, uint64_t, double, complex64. |

> [!NOTE] Note
> The int4x2_t data type packs two 4-bit integers into one 8-bit storage unit.

<a id="datatype-abbreviation"></a>

To facilitate the description of these data types, the following data type abbreviations are provided:

**Table 2**  Data type abbreviations

| Data Type Abbreviation (bit width from low to high) | Corresponding Data Type |
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

> [!NOTE] Note
> Among them, only bool, int8\_t, uint8\_t, int16\_t, uint16\_t, half, int32\_t, uint32\_t, float, int64\_t, uint64\_t support assignment and initialization using immediate values.
> Example:
>
>```cpp
>int8_t scalar = 1;
>int32_t valueOut = AscendC::Cast<float, int32_t, AscendC::RoundMode::CAST_ROUND>((float)1);
>```

## Applicable Products of Data Types<a name="section137212564579"></a>

The data types supported by each product are as follows:

<!-- npu="950" id2 -->
- Ascend 950PR & 950DT products: bool, int4b_t, int4x2_t, int8_t, uint8_t, fp4x2_e2m1_t, fp4x2_e1m2_t, hifloat8_t, fp8_e8m0_t, fp8_e5m2_t, fp8_e4m3fn_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, complex32, int64_t, uint64_t, double, complex64.
<!-- end id2 -->
<!-- npu="A3" id3 -->
- Atlas A3 products: bool, int4b_t, int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, double.
<!-- end id3 -->
<!-- npu="910b" id4 -->
- Atlas A2 products: bool, int4b_t, int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, double.
<!-- end id4 -->
<!-- npu="310b" id5 -->
- Atlas 200I/500 A2 inference products: bool, int4b_t, int8_t, uint8_t, int16_t, uint16_t, half, int32_t, uint32_t, float, int64_t, uint64_t, double.
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products AI Core: bool, int4b_t, int8_t, uint8_t, int16_t, uint16_t, half, int32_t, uint32_t, float, int64_t, uint64_t, double.
<!-- end id6 -->
<!-- npu="310p" id7 -->
- Atlas inference products Vector Core: bool, int4b_t, int8_t, uint8_t, int16_t, uint16_t, half, int32_t, uint32_t, float, int64_t, uint64_t, double.
<!-- end id7 -->
<!-- npu="910" id8 -->
- Atlas training products: bool, int4b_t, int8_t, uint8_t, int16_t, uint16_t, half, int32_t, uint32_t, float, int64_t, uint64_t, double.
<!-- end id8 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/data_structures/builtin_data_types_res.md#id1 -->

## Boolean<a name="section82692022135113"></a>

The **bool** type occupies 8 bits. When all bits are zero, it represents false; otherwise, it represents true.

## Integer<a name="section1529635411502"></a>

An integer consists of a sign bit (S) and a value part (M). Different integer types differ in the bit allocation between the sign bit and the value part. An unsigned integer has no sign bit, and all bits are used to represent the value.

The following figure shows an example of the int8\_t type, whose sign bit occupies 1 bit and value part occupies 7 bits. $S_v=0$, $M_v = 2^5 + 2^6$, and the represented result is 96. The subscript v indicates the specific values of the sign bit and the value part.

**Figure 1** Binary representation of the int8_t data type, with a value of 96

![Binary representation of the int8_t data type](../../../figures/int8_t-binary-96.png)

The value ranges of integer data types are listed in the following table.

**Table 4** Value ranges of integer data types

| Type | Value Range |
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

## Floating-Point Numbers<a name="section1330111025115"></a>

A floating-point number consists of three parts: the sign bit (S), the exponent bits (E), and the mantissa bits (M). For different floating-point types, the number of bits occupied by these three parts may differ.

Based on the bit patterns of the exponent bits and mantissa bits (all zero, all ones, or mixed), floating-point numbers are classified into three categories:

- Normal Numbers: The exponent bits E are not all zero and not all ones. They represent floating-point numbers within the conventional range, accounting for the vast majority of valid values. The mantissa has an implicit leading 1, in the form of $1 + M_v$.
- <a name="p7381131713310"></a>Subnormal Numbers: The exponent bits E are all zero, and the mantissa bits M are not all zero. They represent extremely small values close to zero. The mantissa has no leading 1, in the form of $M_v$.
- Special values:
    - Zeros: The exponent bits E are all zero, and the mantissa bits M are all zero.
    - Infinities: The exponent bits E are all ones, and the mantissa bits M are all zero.
    -   Quiet NaN \(qNaN\): The exponent bits are all ones, and the most significant bit of the mantissa is 1.
    -   Signaling NaN \(sNaN\): The exponent bits are all ones, the most significant bit of the mantissa is 0, and the mantissa is not 0.

The calculation formula for floating-point numbers is as follows:

- For normal numbers:

    $$
    Value = (-1)^{S} \times (1 + M_v) \times 2^{(E_v - Bias)}
    $$

- For subnormal numbers:

    $$
    Value = (-1)^{S} \times M_v \times 2^{(1 - Bias)}
    $$

Where:

$$
M_v = \sum_{i=0}^{Mbits - 1}{m_i \times 2^{-Mbits + i}}, Mbits indicates the mantissa bit width
$$

$$
E_v = \sum_{i=0}^{Ebits - 1}{e_i \times 2^i}, Ebits indicates the exponent width
$$

$$
Bias = 2^{Ebits - 1} -1, Ebits indicates the exponent width
$$

The value range of floating-point data types is shown in the following table.

**Table 5** Floating-point data types

| Type | Sign Width | Exponent Width | Mantissa Width | Value Range |
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

> [!NOTE] Note
>fp4x2\_e2m1\_t and fp4x2\_e1m2\_t data types pack two independent four-bit floating-point numbers into one 8-bit storage unit.

- fp4x2\_e2m1\_t

    The following figure shows an example of the fp4x2\_e2m1\_t type, whose sign bit occupies 1 bit, exponent bits occupy 2 bits, and mantissa bits occupy 1 bit.

    **Figure 2** Binary representation of the fp4x2_e2m1_t data type

    ![](../../../figures/f8_1.png)

- fp4x2\_e1m2\_t

    The following figure shows an example of the fp4x2\_e1m2\_t type, whose sign bit occupies 1 bit, exponent bits occupy 1 bit, and mantissa bits occupy 2 bits.

    **Figure 3** Binary representation of the fp4x2_e1m2_t data type

    ![](../../../figures/f8_1-0.png)

- fp8\_e8m0\_t (fp8\_e8m0\_t binary is obtained from the bfloat16 type by discarding the sign bit and mantissa bits)

    The following figure shows an example of the fp8\_e8m0\_t type, which has no sign bit or mantissa bits, with the exponent bits occupying 8 bits.

    **Figure 4** Binary representation of the fp8_e8m0_t data type

    ![](../../../figures/f8_1-1.png)

- fp8\_e5m2\_t

    The following figure shows an example of the fp8\_e5m2\_t type, whose sign bit occupies 1 bit, exponent bits occupy 5 bits, and mantissa bits occupy 2 bits. The represented result is $(-1)^0 \times (1 + 0.5 + 0.25) \times 2^{30-15}=1.75 \times 2^{15}$.

    **Figure 5** Binary representation of the fp8_e5m2_t data type

    ![](../../../figures/f8_1-2.png)

    The bit representation of the special values of fp8\_e5m2\_t is as follows:

    **Figure 6** Special value bit representation of fp8_e5m2_t

    ![](../../../figures/fp8_e5m2_special_values.png)

- fp8\_e4m3fn\_t

    The following figure shows an example of the fp8\_e4m3fn\_t type, whose sign bit occupies 1 bit, exponent bits occupy 4 bits, and mantissa bits occupy 3 bits. The represented result is $(-1)^1 \times 2^{-3} \times 2^{-6}$.

    **Figure 7** Binary representation diagram of the fp8_e4m3fn_t data type

    ![](../../../figures/fp8_e4m3fn_binary.png)

    The special value bit representation of fp8\_e4m3fn\_t is as follows:

    **Figure 8** Special value bit representation of fp8_e4m3fn_t

    ![](../../../figures/fp8_e4m3fn_special_values.png)

- hifloat8\_t

    Compared with other types, the hifloat8_t type adds an exponent width control field D, which is used to indicate the encoding method of the exponent bits and mantissa bits.

    The hifloat8\_t type has different encoding methods depending on the point domain, which are listed below. The sign, exponent, and mantissa are abbreviated as 'S', 'E', and 'M', respectively.

    **Figure 9** Bit distribution of S, E, and M under different point domain D values<a name="fig261021710100"></a>

    ![](../../../figures/sem_bit_distribution.png "Bit distribution of S, E, and M under different point domain D values")

    In the following example, the sign bit occupies 1 bit, the exponent bits occupy 2 bits, the mantissa bits occupy 3 bits, the D field is 2 bits b01, $S_v=1$, $E_v=3$, $M_v = 2^{-1} + 2^{-2}$. According to the actual value calculation formula $S_v \times 2^{E_v} \times (1 + M_v)$, the calculated result is 14. The subscript v indicates the specific value of each part.

    **Figure 10** Binary representation of the hifloat8_t data type, with a value of 14

    ![](../../../figures/hifloat8_binary.png)

    The value range of the hifloat8_t type is shown in the following table:

    **Table 6** Value Range of hifloat8_t Type

    <a name="table0308852103016"></a>

    | sign width | exponent width control field width | exponent width | mantissa width | exponent width control field value | sign value range ($S_v$) | exponent value range ($E_v$) | mantissa value range ($M_v$) | actual value calculation formula |
    | -------- | -------------------- | -------- | -------- | -------------------- | ----------------------------- | ----------------------------- | ----------------------------- | ---------------- |
    | 1 | 4 | 0 | 3 | 4'b0000 | ±1 | - | [0, 7] | $S_v \times 2^{M_v - 23}$ |
    | 1 | 4 | 0 | 3 | 4'b0001 | ±1 | 0 | [0, $7 * 2^{-3}$] | $S_v \times 2^{E_v} \times (1 + M_v)$ |
    | 1 | 3 | 1 | 3 | 4'b001 | ±1 | ±1 | [0, $7 * 2^{-3}$] | $S_v \times 2^{E_v} \times (1 + M_v)$ |
    | 1 | 2 | 2 | 3 | 2'b01 | ±1 | ±[2, 3] | [0, $7 * 2^{-3}$] | $S_v \times 2^{E_v} \times (1 + M_v)$ |
    | 1 | 2 | 3 | 2 | 2'b10 | ±1 | ±[4, 7] | [0, $3 * 2^{-2}$] | $S_v \times 2^{E_v} \times (1 + M_v)$ |
    | 1 | 2 | 4 | 1 | 2'b11 | ±1 | ±[8, 15] | [0, $2^{-1}$] | $S_v \times 2^{E_v} \times (1 + M_v)$ |

    The bit representation of special values of hifloat8_t is as follows:

    **Figure 11** Bit representation of hifloat8_t special values

    ![](../../../figures/hifloat8_special_values.png)

    The calculation formulas for the hifloat8_t data type are as follows:

    - **Sign bit $S_v$**

        A value of 1 for s\_bit\_val indicates a negative number, and a value of 0 for s\_bit\_val indicates a non-negative number.

        $S_v = (-1)^{s\_bit\_val}$

    - **Exponent bits $E_v$**

        It consists of Es and Em.

        **Table 7** Different D values result in different **Es** and **Em** bit sizes.

        <a name="table15755618132"></a>

        | D Value | Es Value Range | Em Value Range |
        | ---- | --------- | --------- |
        | 3b001 | 0-1 | - |
        | 2b01 | 0-1 | 10~11 |
        | 2b10 | 0-1 | 100~111 |
        | 2b11 | 0-1 | 1000~1111 |

        The es\_bit\_val value represents the most significant bit of E and is used to calculate the sign value of **Ev**. For example, if E is 0b1100, the es\_bit\_val value is the most significant bit 1; if E is 0b011, the es\_bit\_val value is the most significant bit 0.

        The **Es** calculation formula:

        (1)$Es\_value = (-1)^{es\_bit\_val}$

        The em\_bit\_val value represents the remaining bits of E excluding the most significant bit, plus a leading 1, and is used to calculate the actual value of **Ev**. For example, if E is 0b1100, the em\_bit\_val value is the remaining bits 100, which becomes 1100 after adding a leading 1; if E is 0b011, the em\_bit\_val value is the remaining bits 11, which becomes 111 after adding a leading 1.

        The **Em** calculation formula is as follows, where **Emi** represents the value of each bit of **Em** (0 or 1), and **D** represents the point domain value. See [Table 7](#table15755618132).

        (2)$Em\_value = E|(1<<\textit{bit width of E})$

        The **Ev** calculation formula:

        (3)$Ev\_value = Es\_value * Em\_value$

        The **Mv** calculation formula is as follows, where **M** represents the bit value, and bitwidth of M represents the bit width of **M**. See [Table 7](#table15755618132).

        (4)$Mv\_value = M / 2^{\textit{bitwidth of M}}$

    - In Normal and Subnormal modes, the value calculation formulas for floating-point numbers differ:

        Normal mode: composed of Sv, Ev, and Mv.

        (1)$value = S_v * 2^{E_v} * (1 + M_v)$

        Subnormal mode, composed of Sv and Mv.

        $Mv = M$

        (2)$value = S_v * 2^{M_v - 23}$

- half

    The half data type consists of a 1-bit sign bit (S), 5-bit exponent bits (E), and 10-bit mantissa bits (M). The following figure shows an example of a half-type normal number, whose represented result is $(-1)^0 \times 2^{(15 - 15)} \times (1+ 0.75) = 1.75$. The subscript v indicates the specific value of each part.

    **Figure 12** Binary representation of the half data type, with a value of 1.75

    ![half data type binary representation - value 1.75](../../../figures/half-binary-1-75.png)

    **Table 8** Special value bit representation of the half type

    | S | E | M | Special value |
    | - | - | - | ------ |
    | 0 | all zero | all zero | +0 |
    | 1 | all zero | all zero | -0 |
    | 0 | all ones | all zero | +inf |
    | 1 | all ones | all zero | -inf |
    | any | all ones | not all zero | nan |
    | 0 | 5'h00 | 10'h001 | smallest positive subnormal number |
    | 0 | 5'h00 | 10'h3FF | largest positive subnormal number |
    | 0 | 5'h01 | 10'h000 | smallest positive normal number |
    | 0 | 5'h1E | 10'h3FF | largest positive normal number |

- bfloat16\_t

    The bfloat16 data type consists of a 1-bit sign bit (S), 8-bit exponent bits (E), and 7-bit mantissa bits (M). The following figure shows an example of a bfloat16\_t data type normal number, whose represented result is $(-1)^0 \times 2^{(127 - 127)} \times (1+ 0.75) = 1.75$.

    **Figure 13** bfloat16\_t data type binary representation, value 1.75<a name="zh-cn_topic_0000002563730205_fig760011915344"></a>

    ![bfloat16_t data type binary representation - value 1.75](../../../figures/bfloat16_t-binary-1-75.png)

    **Table 9** bfloat16\_t type special values

    | S | E | M | Special value |
    | - | - | - | ------ |
    | 0 | all zero | all zero | +0 |
    | 1 | all zero | all zero | -0 |
    | 0 | all ones | all zero | +inf |
    | 1 | all ones | all zero | -inf |
    | any | all ones | not all zero | nan |
    | 0 | 8'h00 | 7'h01 | smallest positive subnormal number |
    | 0 | 8'h00 | 7'h7F | largest positive subnormal number |
    | 0 | 8'h01 | 7'h00 | smallest positive normal number |
    | 0 | 8'hFE | 7'h7F | largest positive normal number |

- float

    The float data type consists of a 1-bit sign bit (S), 8-bit exponent bits (E), and 23-bit mantissa bits (M). The following figure shows an example of a float normal number, whose represented result is $(-1)^0 \times 2^{(127 - 127)} \times (1+ 0.75) = 1.75$.

    **Figure 14** Binary representation of the float data type, with a value of 1.75<a name="zh-cn_topic_0000002563730205_fig54861751133412"></a>

    ![Binary representation of the float data type - value 1.75](../../../figures/float-binary-1-75.png)

    **Table 10** Special values of the float type

    | S | E | M | Special value |
    | - | - | - | ------ |
    | 0 | all zero | all zero | +0 |
    | 1 | all zero | all zero | -0 |
    | 0 | all ones | all zero | +inf |
    | 1 | all ones | all zero | -inf |
    | any | all ones | not all zero | nan |
    | 0 | 8'h00 | 23'h000001 | smallest positive subnormal number |
    | 0 | 8'h00 | 23'h7FFFFF | largest positive subnormal number |
    | 0 | 8'h01 | 23'h000000 | smallest positive normal number |
    | 0 | 8'hFE | 23'h7FFFFF | largest positive normal number |

<!-- npu="950" id1 -->
## Complex Numbers<a name="section8666633175111"></a>

Ascend C provides the complex data types **complex32** and **complex64**.

Defined as follows:

```cpp
using complex32 = AscendC::Complex<half>;
using complex64 = AscendC::Complex<float>;
```

For the definition structure of Complex, see [complex32/complex64](../aux_data_structures/complex32-complex64.md).

**complex32** is a complex number whose real and imaginary parts are both of the half type.

**complex64** is a complex number whose real and imaginary parts are both of the float type.

Example:

```cpp
complex32 value0(1, 2);
// value0 represents a complex number with a real part of 1 and an imaginary part of 2, that is, 1+2j.
complex32 value1(3);
// value1 represents a complex number with a real part of 3 and an imaginary part of 0, that is, 3+0j.
complex64 value2 = 4;
// value2 represents a complex number with a real part of 4 and an imaginary part of 0, that is, 4+0j.
```

Currently only the Ascend 950PR & 950DT products is supported.
<!-- end id1 -->
