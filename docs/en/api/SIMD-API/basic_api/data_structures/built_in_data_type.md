# Built-in Data Types<a name="ZH-CN_TOPIC_0000002313437229"></a>

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-08-27T13:16:36.335Z -->

## Data Type List<a name="section16395539499"></a>

Ascend C provides four data types with different bit widths, ranging from b8 to b64 (8-bit to 64-bit). The data types corresponding to each bit width are listed in the following table.

**Table 1** Data types corresponding to different bit widths

| Bit Width | Data Type |
| :--: | :------------: |
| b8 | bool, int4x2_t, int8_t, uint8_t, fp4x2_e2m1_t, fp4x2_e1m2_t, hifloat8_t, fp8_e8m0_t, fp8_e5m2_t, fp8_e4m3fn_t. |
| b16 | int16_t, uint16_t, half, bfloat16_t. |
| b32 | int32_t, uint32_t, float, complex32. |
| b64 | int64_t, uint64_t, double, complex64. |

> [!NOTE]NOTE
> The int4x2_t data type packs two 4-bit integers into a single 8-bit storage unit.

<a id="datatype-abbreviation"></a>

For ease of describing these data types, the following data type abbreviations are provided:

**Table 2** Data type abbreviations

| Data Type Abbreviation (from low to high bit width) | Corresponding Data Type |
| :---------------: | :------------: |
| S4        | int4b_t |
| S8        | int8_t |
| U8        | uint8_t |
| HiF8      | hifloat8_t |
| FP8_E4M3  | fp8_e4m3fn_t |
| S16       | int16_t |
| U16       | uint16_t |
| FP16      | half |
| BF16      | bfloat16_t |
| S32       | int32_t |
| U32       | uint32_t |
| FP32      | float |
| S64       | int64_t |
| U64       | uint64_t |

> [!NOTE]NOTE
> Among these, only the following data types support assignment and initialization using immediate values: bool, int8\_t, uint8\_t, int16\_t, uint16\_t, half, int32\_t, uint32\_t, float, int64\_t, and uint64\_t.
> Example:
>
> ```cpp
> int8_t scalar = 1;
> int32_t valueOut = AscendC::Cast<float, int32_t, AscendC::RoundMode::CAST_ROUND>((float)1);
> ```

## Applicable Products<a name="section137212564579"></a>

**Table 3** Data types supported by products

| Product | Supported Data Types |
| :----: | :--------------: |
| Ascend 950PR/Ascend 950DT | bool, int4b_t, int4x2_t, int8_t, uint8_t, fp4x2_e2m1_t, fp4x2_e1m2_t, hifloat8_t, fp8_e8m0_t, fp8_e5m2_t, fp8_e4m3fn_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, complex32, int64_t, uint64_t, double, complex64. |
|Atlas A3 training products/Atlas A3 inference products | bool, int4b_t, int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, double. |
|Atlas A2 training products/Atlas A2 inference products | bool, int4b_t, int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, double.|
|Atlas 200I/500 A2 inference products | bool, int4b_t, int8_t, uint8_t, int16_t, uint16_t, half, int32_t, uint32_t, float, int64_t, uint64_t, double. |
|Atlas inference products AI Core | bool, int4b_t, int8_t, uint8_t, int16_t, uint16_t, half, int32_t, uint32_t, float, int64_t, uint64_t, double.|
|Atlas inference products Vector Core | bool, int4b_t, int8_t, uint8_t, int16_t, uint16_t, half, int32_t, uint32_t, float, int64_t, uint64_t, double.|
|Atlas training products | bool, int4b_t, int8_t, uint8_t, int16_t, uint16_t, half, int32_t, uint32_t, float, int64_t, uint64_t, double.|

## Boolean<a name="section82692022135113"></a>

The **bool** type occupies 8 bits. When all bits are 0, it represents **false**; otherwise, it represents **true**.

## Integer<a name="section1529635411502"></a>

An integer consists of a sign bit (S) and a value part (M). Different integer types differ in the bit allocation between the sign bit and the value part. Unsigned integers have no sign bit, and all bits are used to represent the value.

The following figure shows an example of the int8\_t type, whose sign bit occupies 1 bit and value part occupies 7 bits. $S_v=0$ and $M_v = 2^5 + 2^6$, indicating a value of 96. The subscript v indicates the specific values of the sign bit and the value part.

**Figure 1** Binary representation of the int8_t data type with a value of 96

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

A floating-point number consists of three parts: the sign bit (S), the exponent bits (E), and the mantissa bits (M). Different floating-point types may allocate different numbers of bits to these three parts.

Based on the bit patterns of the exponent bits and mantissa bits (all zeros, all ones, or mixed), floating-point numbers are classified into three categories:

- Normal numbers: The exponent bits E are neither all zeros nor all ones. They represent floating-point numbers within the conventional range and account for the vast majority of valid values. The mantissa has an implicit leading 1, taking the form $1 + M_v$.
- <a name="p7381131713310"></a>Subnormal numbers: The exponent bits E are all zeros and the mantissa bits M are not all zeros. They represent extremely small values close to zero. The mantissa has no leading 1, taking the form $M_v$.
- Special values:
    - Zeros: The exponent bits E are all zeros and the mantissa bits M are all zeros.
    - Infinities: The exponent bits E are all ones and the mantissa bits M are all zeros.
    - NaN: The exponent bits E are all ones and the mantissa bits M are not all zeros.

The formula for calculating a floating-point number is as follows:

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
M_v = \sum_{i=0}^{Mbits - 1}{m_i \times 2^{-Mbits + i}}, where Mbits is the mantissa bit width
$$

$$
E_v = \sum_{i=0}^{Ebits - 1}{e_i \times 2^i}, where Ebits is the exponent bit width
$$

$$
Bias = 2^{Ebits - 1} -1, where Ebits is the exponent bit width
$$

The value ranges of floating-point data types are shown in the following table.

**Table 5** Floating-point data types

| Type | Sign Bit Width | Exponent Bit Width | Mantissa Bit Width | Value Range |
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

> [!NOTE]Note
>The `fp4x2_e2m1_t` and `fp4x2_e1m2_t` data types pack two independent 4-bit floating-point numbers into a single 8-bit storage unit.

- `fp4x2_e2m1_t`

    The following figure shows an example of the `fp4x2_e2m1_t` type, whose sign bit occupies 1 bit, exponent bits occupy 2 bits, and mantissa bit occupies 1 bit.

    **Figure 2** Binary layout of the fp4x2_e2m1_t data type

    ![](../../../figures/f8_1.png)

- `fp4x2_e1m2_t`

    The following figure shows an example of the `fp4x2_e1m2_t` type, whose sign bit occupies 1 bit, exponent bit occupies 1 bit, and mantissa bits occupy 2 bits.

    **Figure 3** Binary layout of the fp4x2_e1m2_t data type

    ![](../../../figures/f8_1-0.png)

- fp8\_e8m0\_t (the binary representation of fp8\_e8m0\_t is obtained by discarding the sign bit and mantissa bits from the bfloat16 type)

    The following figure shows an example of the fp8\_e8m0\_t type. This type has no sign bit or mantissa bits, and the exponent occupies 8 bits.

    **Figure 4** Binary representation of the fp8_e8m0_t data type

    ![](../../../figures/f8_1-1.png)

- fp8\_e5m2\_t

    The following figure shows an example of the fp8\_e5m2\_t type. Its sign bit occupies 1 bit, the exponent occupies 5 bits, and the mantissa occupies 2 bits. The represented result is $(-1)^0 \times (1 + 0.5 + 0.25) \times 2^{30-15}=1.75 \times 2^{15}$.

    **Figure 5** Binary representation of the fp8_e5m2_t data type

    ![](../../../figures/f8_1-2.png)

    The bit representation of the special values of fp8\_e5m2\_t is as follows:

    **Figure 6** Bit representation of the special values of fp8_e5m2_t

    ![](../../../figures/hifi8_special_value.png)

- fp8\_e4m3fn\_t

    The following figure shows an example of the fp8\_e4m3fn\_t type, where the sign bit occupies 1 bit, the exponent bits occupy 4 bits, and the mantissa bits occupy 3 bits. The represented value is $(-1)^1 \times 2^{-3} \times 2^{-6}$.

    **Figure 7** Binary representation of the fp8_e4m3fn_t data type

    ![](../../../figures/F8_2.png)

    The bit representation of special values of fp8\_e4m3fn\_t is as follows:

    **Figure 8** Bit representation of special values of fp8_e4m3fn_t

    ![](../../../figures/hifi8_special_value_3.png)

- hifloat8\_t

    Compared with other types, the hifloat8\_t type adds an exponent bit-width control field **D**, which is used to indicate the encoding method of the exponent bits and mantissa bits.

    The **hifloat8\_t** type has different encoding methods depending on the point domain, which are listed below. The sign, exponent, and mantissa are abbreviated as 'S', 'E', and 'M', respectively.

    **Figure 9** Bit distribution of S, E, and M under different point domain D values<a name="fig261021710100"></a>

    ![](../../../figures/sem_bit_distribution_d_value.png "Bit distribution of S, E, and M under different point domain D values")

    In the following example, the sign bit occupies 1 bit, the exponent bits occupy 2 bits, the mantissa bits occupy 3 bits, the D field is 2 bits b01, $S_v=1$, $E_v=3$, $M_v = 2^{-1} + 2^{-2}$, and the result calculated according to the actual value formula $S_v \times 2^{E_v} \times (1 + M_v)$ is 14. The subscript v indicates the specific value of each part.

    **Figure 10** Binary representation of the hifloat8_t data type, with a value of 14

    ![](../../../figures/pipeline_task_run_4.png)

    The value range of the **hifloat8\_t** type is shown in the following table:

    **Table 6** Value range of the hifloat8\_t type

    <a name="table0308852103016"></a>

    | Sign bit width | Exponent bit width control field bit width | Exponent bit width | Mantissa bit width | Exponent bit width control field value | Sign value range ($S_v$) | Exponent value range ($E_v$) | Mantissa value range ($M_v$) | Actual value formula |
    | -------- | -------------------- | -------- | -------- | -------------------- | ----------------------------- | ----------------------------- | ----------------------------- | ---------------- |
    | 1 | 4 | 0 | 3 | 4'b0000 | ±1 | - | [0, 7] | $S_v \times 2^{M_v - 23}$ |
    | 1 | 4 | 0 | 3 | 4'b0001 | ±1 | 0 | [0, $7 * 2^{-3}$] | $S_v \times 2^{E_v} \times (1 + M_v)$ |
    | 1 | 3 | 1 | 3 | 4'b001 | ±1 | ±1 | [0, $7 * 2^{-3}$] | $S_v \times 2^{E_v} \times (1 + M_v)$ |
    | 1 | 2 | 2 | 3 | 2'b01 | ±1 | ±[2, 3] | [0, $7 * 2^{-3}$] | $S_v \times 2^{E_v} \times (1 + M_v)$ |
    | 1 | 2 | 3 | 2 | 2'b10 | ±1 | ±[4, 7] | [0, $3 * 2^{-2}$] | $S_v \times 2^{E_v} \times (1 + M_v)$ |
    | 1 | 2 | 4 | 1 | 2'b11 | ±1 | ±[8, 15] | [0, $2^{-1}$] | $S_v \times 2^{E_v} \times (1 + M_v)$ |

    The special value bit representation of **hifloat8_t** is as follows:

    **Figure 11** Special value bit representation of **hifloat8_t**

    ![](../../../figures/hifi8_special_value_5.png)

    The calculation formula of the **hifloat8_t** data type is as follows:

    - **Sign bit $S_v$**

        **s_bit_val** of 1 indicates a negative number, and **s_bit_val** of 0 indicates a non-negative number.

        ![Formula for the sign bit S_v](../../../figures/zh-cn_formulaimage_0000002530886571.png)

    - **Exponent bit $E_v$**

        It consists of **Es** and **Em**.

        **Table 7** Different D-domain values result in different bit sizes of **Es** and **Em**

        <a name="table15755618132"></a>

        | D Value | Es Value Range | Em Value Range |
        | ---- | --------- | --------- |
        | 3b001 | 0-1 | - |
        | 2b01 | 0-1 | 10~11 |
        | 2b10 | 0-1 | 100~111 |
        | 2b11 | 0-1 | 1000~1111 |

        The **es\_bit\_val** value represents the most significant bit of **E** and is used to calculate the sign value of **Ev**. For example, if **E** is 0b1100, the **es\_bit\_val** value is the most significant bit 1; if **E** is 0b011, the **es\_bit\_val** value is the most significant bit 0.

        The **Es** calculation formula:

        ![Es calculation formula](../../../figures/zh-cn_formulaimage_0000002531046525.png)

        The **em\_bit\_val** value represents the remaining bits of **E** excluding the most significant bit, plus a leading 1, and is used to calculate the actual value of **Ev**. For example, if **E** is 0b1100, the **em\_bit\_val** value is the remaining bits 100, which becomes 1100 after adding a leading 1; if **E** is 0b011, the **em\_bit\_val** value is the remaining bits 11, which becomes 111 after adding a leading 1.

        The **Em** calculation formula is as follows, where **Emi** represents the value of each bit of **Em** (0 or 1), and **D** represents the dot-domain value. See [Table 7](#table15755618132).

        ![Em calculation formula](../../../figures/zh-cn_formulaimage_0000002530886573.png)

        The **Ev** calculation formula:

        ![Ev calculation formula](../../../figures/zh-cn_formulaimage_0000002531046527.png)

        Mv calculation formula, where M represents the bit value and bitwidth of M represents the bit width of M. See [Table 7](#table15755618132).

        ![M_v calculation formula](../../../figures/zh-cn_formulaimage_0000002530886575.png)

    - In Normal and Subnormal modes, the value calculation formulas for floating-point numbers differ:

        Normal mode: composed of Sv, Ev, and Mv.

        ![Normal mode value calculation formula](../../../figures/zh-cn_formulaimage_0000002531046529.png)

        Subnormal mode: composed of Sv and Mv.

        ![Subnormal mode value calculation formula](../../../figures/zh-cn_formulaimage_0000002530886577.png)

        ![hifloat8_t value condition calculation formula](../../../figures/zh-cn_formulaimage_0000002531046531.png)

- half

    The half data type consists of a 1-bit sign bit (S), a 5-bit exponent bit (E), and a 10-bit mantissa bit (M). The following figure shows an example of a normal half value, whose represented result is $(-1)^0 \times 2^{(15 - 15)} \times (1+ 0.75) = 1.75$. The subscript v indicates the specific value of each part.

    **Figure 12** Binary representation of the half data type, with a value of 1.75

    ![Binary representation of the half data type, with a value of 1.75](../../../figures/half-binary-1-75.png)

    **Table 8** Special value bit representation of the half type

    | S | E | M | Special Value |
    | - | - | - | ------ |
    | 0 | All 0s | All 0s | +0 |
    | 1 | All 0s | All 0s | -0 |
    | 0 | All 1s | All 0s | +INF |
    | 1 | All 1s | All 0s | -INF |
    | Any | All 1s | Not all 0s | NaN |
    | 0 | 5'h00 | 10'h001 | Minimum positive subnormal number |
    | 0 | 5'h00 | 10'h3FF | Maximum positive subnormal number |
    | 0 | 5'h01 | 10'h000 | Minimum positive normal number |
    | 0 | 5'h1E | 10'h3FF | Maximum positive normal number |

- bfloat16\_t

    The bfloat16 data type consists of 1 sign bit (S), 8 exponent bits (E), and 7 mantissa bits (M). The following figure shows an example of a normal number of the bfloat16\_t data type, whose represented result is $(-1)^0 \times 2^{(127 - 127)} \times (1+ 0.75) = 1.75$.

    **Figure 13** Binary representation of the bfloat16\_t data type, with a value of 1.75<a name="zh-cn_topic_0000002563730205_fig760011915344"></a>

    ![Binary representation of the bfloat16_t data type, with a value of 1.75](../../../figures/bfloat16_t-binary-1-75.png)

    **Table 9** Special values of the bfloat16\_t type

    | S | E | M | Special Value |
    | - | - | - | ------ |
    | 0 | All 0s | All 0s | +0 |
    | 1 | All 0s | All 0s | -0 |
    | 0 | All 1s | All 0s | +INF |
    | 1 | All 1s | All 0s | -INF |
    | Any | All 1s | Not all 0s | NaN |
    | 0 | 8'h00 | 7'h01 | Minimum positive subnormal number |
    | 0 | 8'h00 | 7'h7F | Maximum positive subnormal number |
    | 0 | 8'h01 | 7'h00 | Minimum positive normal number |
    | 0 | 8'hFE | 7'h7F | Maximum positive normal number |

- float

    The float data type consists of a 1-bit sign bit (S), an 8-bit exponent bit (E), and a 23-bit mantissa bit (M). The following figure shows an example of a normal number of the float type, whose represented result is $(-1)^0 \times 2^{(127 - 127)} \times (1+ 0.75) = 1.75$.

    **Figure 14** Binary representation diagram of the float data type, with a value of 1.75<a name="zh-cn_topic_0000002563730205_fig54861751133412"></a>

    ![Binary representation diagram of the float data type, with a value of 1.75](../../../figures/float-binary-1-75.png)

    **Table 10** Special values of the float type

    | S | E | M | Special Value |
    | - | - | - | ------ |
    | 0 | All 0s | All 0s | +0 |
    | 1 | All 0s | All 0s | -0 |
    | 0 | All 1s | All 0s | +INF |
    | 1 | All 1s | All 0s | -INF |
    | Any | All 1s | Not all 0s | NaN |
    | 0 | 8'h00 | 23'h000001 | Minimum positive subnormal number |
    | 0 | 8'h00 | 23'h7FFFFF | Maximum positive subnormal number |
    | 0 | 8'h01 | 23'h000000 | Minimum positive normal number |
    | 0 | 8'hFE | 23'h7FFFFF | Maximum positive normal number |

<!-- npu="950" id1 -->
## Complex Numbers<a name="section8666633175111"></a>

Ascend C provides the following complex data types: **complex32** and **complex64**.

Defined as follows:

```cpp
using complex32 = AscendC::Complex<half>;
using complex64 = AscendC::Complex<float>;
```

For the definition of the Complex structure, see [complex32/complex64](../aux_data_structures/complex32-complex64.md).

**complex32** is a complex number whose real and imaginary parts are both of the **half** type.

**complex64** is a complex number whose real and imaginary parts are both of the **float** type.

Example:

```cpp
complex32 value0(1, 2);
// value0 represents a complex number with a real part of 1 and an imaginary part of 2, that is, 1+2j.
complex32 value1(3);
// value1 represents a complex number with a real part of 3 and an imaginary part of 0, that is, 3+0j.
complex64 value2 = 4;
// value2 represents a complex number with a real part of 4 and an imaginary part of 0, that is, 4+0j.
```

Currently supported only on Ascend 950PR/Ascend 950DT.
<!-- end id1 -->
