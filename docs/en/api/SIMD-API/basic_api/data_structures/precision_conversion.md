# Precision Conversion<a name="ZH-CN_TOPIC_0000002603424969"></a>

<!-- md-trans-meta sourceCommit=ee355529a5d6098b1c84bfe577b9776e4ec7583b translatedAt=2026-11-02T05:44:50.011Z pushedAt=2026-11-18T01:25:06.957Z nl=2 -->

## Binary Precision Rounding Rules

Before learning the conversion rules between different types, you need to first understand the binary precision rounding rules.

As shown in the following figure, the trailing bits of a binary number may be discarded because the target precision is insufficient. This part is called the **part to be rounded**, and the remaining retained part is called the **non-rounded part**. Based on the value of the part to be rounded and the selected **precision rounding mode**, it is determined whether the last bit of the non-rounded part needs **+1 (carry)**.

**Figure 1** Schematic diagram of binary precision rounding rules<a name="fig1"></a>

![Schematic diagram of binary precision rounding rules](../../../figures/binary_rounding.png)

The judgment rules of each precision rounding mode for the part to be rounded are shown in the following table:

**Table 1** Precision rounding modes<a id="tab1"></a>

| Mode | Description |
| ---- | ---- |
| CAST_NONE | When there is precision loss during conversion, it is processed according to the CAST_RINT mode; when there is no precision loss, no rounding is performed. |
| CAST_RINT | Round to the nearest even number.<br>&bull;If the first bit of the part to be rounded is 0, then no carry is performed.<br>&bull;If the first bit of the part to be rounded is 1 and the subsequent bits are not all 0, then carry is performed.<br>&bull;If the first bit of the part to be rounded is 1 and the subsequent bits are all 0: when the last bit of the non-rounded part is 0, then no carry is performed; when the last bit of the non-rounded part is 1, then carry is performed. |
| CAST_FLOOR | Round toward negative infinity.<br>&bull;If the sign bit (S) is 0 (positive number), then no carry is performed.<br>&bull;If the sign bit (S) is 1 (negative number): when the part to be rounded is all 0, then no carry is performed; otherwise, carry is performed. |
| CAST_CEIL | Round toward positive infinity.<br>&bull;If the sign bit (S) is 1 (negative number), then no carry is performed.<br>&bull;If the sign bit (S) is 0 (positive number): when the part to be rounded is all 0, then no carry is performed; otherwise, carry is performed. |
| CAST_ROUND | Round half up. If the first bit of the part to be rounded is 0, then no carry is performed; otherwise, carry is performed. |
| CAST_TRUNC | Truncation mode. Directly discard the part to be rounded. |
| CAST_ODD | Round to the nearest odd number.<br>&bull;If the part to be rounded is all 0, then no carry is performed.<br>&bull;If the part to be rounded is not all 0: when the last bit of the non-rounded part is 1, then no carry is performed; when the last bit of the non-rounded part is 0, then carry is performed. |
| CAST_HYBRID | Random rounding, which currently specifically refers to a random rounding used when the output result is of the hifloat8_t data type. |

## Precision Conversion Rules

This section mainly introduces the rounding behavior during precision conversion between different data types. For the representation of different data types, see [Built-in Data Types](builtin_data_types.md). When the source value exceeds the representable range of the target type, overflow occurs, and overflow is handled in saturation mode by default.

### int4b_t → int16_t

Store the source value in the destination operand in int16_t format, with no precision loss.

**Example**: Input: 1, output: 1.

---

### int4b_t → half

Store the source value in half format into the destination operand, with no precision loss.

**Example**: Input: 1, output: 1.0.

---

### int4b_t → bfloat16_t

Store the source value in the destination operand in bfloat16_t format, with no precision loss.

**Example**: Input: 1, output: 1.0.

---

### int8_t → half

Store the source value in half format into the destination operand, with no precision loss.

**Example**: Input: -1, output: -1.0.

---

### int8_t → int16_t

Store the source value in the destination operand in int16_t format, with no precision loss.

**Example**: Input: $2^7 - 1$, output: $2^7 - 1$.

---

### int8_t → int32_t

Store the source value in **int32_t** format into the destination operand, with no precision loss.

**Example**: Input: $2^7 - 1$, output: $2^7 - 1$.

---

### uint8_t → half

Store the source value in half format into the destination operand, with no precision loss.

**Example**: Input: 1, output: 1.0.

---

### uint8_t → uint16_t

Store the source value in the destination operand in **uint16_t** format, with no precision loss.

**Example**: Input: $2^8 - 1$, output: $2^8 - 1$.

---

### uint8_t → uint32_t

Store the source value in the destination operand in uint32_t format, with no precision loss.

**Example**: Input: $2^8 - 1$, output: $2^8 - 1$.

---

### fp4x2_e2m1_t → bfloat16_t

Store the source value in the destination operand in bfloat16_t format, with no precision loss.

**Example**: Input: 2, output: 2.

---

### fp4x2_e1m2_t → bfloat16_t

Store the source value in the destination operand in bfloat16_t format, with no precision loss.

**Example**: Input: 2, output: 2.

---

### hifloat8_t → float

Store the source value in the destination operand in **float** format, with no precision loss.

**Example**: Input: 2, output: 2.

---

### hifloat8_t → half

Store the source value in half format into the destination operand, with no precision loss.

**Example**: Input: 2, output: 2.

---

### fp8_e5m2_t → float

Store the source value in the destination operand in **float** format, with no precision loss.

**Example**: Input: 2, output: 2.

---

### fp8_e4m3fn_t → float

Store the source value in the destination operand in **float** format, with no precision loss.

**Example**: Input: 2, output: 2.

---

### int16_t → int4b_t

Store the source value in int4b_t format in the destination operand.

**Example**: Input: $2^{15} - 1$, output: $2^3 - 1$.

---

### int16_t → uint8_t

Store the source value in uint8_t format in the destination operand, with truncation of high-order bits and precision loss. **Negative input will be treated as an exception**.

**Example**: Input: $2^{15} - 1$, output: $2^8 - 1$.

---

### int16_t → half

Convert the source value according to the precision rounding mode to a number that can be represented by half, and store it in the destination operand in half format.

**Example**: Input: $2^{12} + 2$, written as the half representation form: $2^{12} \times (1 + 2^{-11})$, requiring $E_v = 12 + 15 = 27$ and $M_v = 2^{-11}$.

**Figure 2** int16_t to half

![int16_t to half](../../../figures/int16_to_half.png)

Because half has only a 10-bit mantissa, the gray part needs to be rounded:

- **CAST_RINT mode**: The input yields the mantissa `0000000000`, $E_v = 27$, $M_v = 0$, and the final result is: $2^{12}$.
- **CAST_FLOOR mode**: The input yields the mantissa `0000000000`, $E_v = 27$, $M_v = 0$, and the final result is: $2^{12}$.
- **CAST_CEIL mode**: Rounding yields the mantissa `0000000001`, $E_v = 27$, $M_v = 2^{-10}$, and the final result is: $2^{12} + 4$.
- **CAST_ROUND mode**: Rounding yields the mantissa `0000000001`, $E_v = 27$, $M_v = 2^{-10}$, and the final result is: $2^{12} + 4$.
- **CAST_TRUNC mode** rounding yields the mantissa `0000000000`, $E_v = 27$, $M_v = 0$, and the final result is: $2^{12}$.

---

### int16_t → uint32_t

Store the source value in the destination operand in uint32_t format, with no precision loss. Negative input will be treated as 0 by default.

**Example**: Input: $2^{15} - 1$, output: $2^{15} - 1$.

---

### int16_t → int32_t

Store the source value in **int32_t** format into the destination operand, with no precision loss.

**Example**: Input: $2^{15} - 1$, output: $2^{15} - 1$.

---

### int16_t → float

Store the source value in the destination operand in **float** format, with no precision loss.

**Example**: Input: $2^{15} - 1$, output: $2^{15} - 1$.

---

### uint16_t → uint8_t

Store the source value in uint8_t format into the destination operand.

**Example**: Input: $2^{16} - 1$, output: $2^8 - 1$.

---

### uint16_t → uint32_t

Store the source value in the destination operand in uint32_t format, with no precision loss.

**Example**: Input: $2^{16} - 1$, output: $2^{16} - 1$.

---

### half → int4b_t

Round the source value to an integer according to the precision rounding mode, and store it in int4b_t format into the destination operand.

**Example**: Input: $1.5$, the fractional part needs to be rounded.

- **CAST_RINT mode** output: $2$.
- **CAST_FLOOR mode** output: $1$.
- **CAST_CEIL mode** output: $2$.
- **CAST_ROUND mode** output: $2$.
- **CAST_TRUNC mode** output: $1$.

---

### half → int8_t

Convert the source value to an integer according to the precision rounding mode, and store it in int8_t format in the destination operand.

**Example**: Input: $2^{7} - 0.5$.

- **CAST_RINT** mode output: $2^{7} - 1$ (overflow handling).
- **CAST_FLOOR** mode output: $2^{7} - 1$.
- **CAST_CEIL** mode output: $2^{7} - 1$ (overflow handling).
- **CAST_ROUND** mode output: $2^{7} - 1$ (overflow handling).
- **CAST_TRUNC** mode output: $2^{7} - 1$.

---

### half → uint8_t

Convert the source value according to the precision rounding mode, and store it in uint8_t format in the destination operand. **Negative input will be treated as an exception**.

**Example**: Input: $1.75$.

- **CAST_RINT mode** output: $2$.
- **CAST_FLOOR mode** output: $1$.
- **CAST_CEIL mode** output: $2$.
- **CAST_ROUND mode** output: $2$.
- **CAST_TRUNC mode** output: $1$.

---

### half → hifloat8_t

Round the source value according to the precision rounding mode, and store it in the destination operand in hifloat8_t format.

**Example**: Input: $1.75$.

- **CAST_ROUND mode** output: $2$.
- **CAST_HYBRID mode**: refer to the output of the [half to hifloat8_t type conversion rule](../reg_vector_compute/type_conversion/Cast.md#half-to-hifloat8_t-conversion-rule).

---

### half → int16_t

Convert the source value according to the precision rounding mode, and store it in int16_t format in the destination operand.

**Example**: Input: $2^{7} - 0.5$.

- **CAST_RINT mode** output: $2^{7}$.
- **CAST_FLOOR** mode output: $2^{7} - 1$.
- **CAST_CEIL mode** output: $2^{7}$.
- **CAST_ROUND mode** output: $2^{7}$.
- **CAST_TRUNC** mode output: $2^{7} - 1$.

---

### half → bfloat16_t

Convert the source value according to the precision rounding mode, and store it in bfloat16_t format into the destination operand.

**Example**: Input: $1.75$.

- **CAST_RINT mode** output: $2$.
- **CAST_FLOOR mode** output: $1$.
- **CAST_CEIL mode** output: $2$.
- **CAST_ROUND mode** output: $2$.
- **CAST_TRUNC mode** output: $1$.

---

### half → int32_t

Convert the source value according to the precision rounding mode and store it in int32_t format into the destination operand.

- **Example**: Input: $-1.5$.

  - **CAST_RINT mode** output: $-2$.
  - **CAST_FLOOR mode** output: $-2$.
  - **CAST_CEIL mode** output: $-1$.
  - **CAST_ROUND mode** output: $-2$.
  - **CAST_TRUNC mode** output: $-1$.

---

### half → float

Store the source value in the destination operand in **float** format, with no precision loss.

**Example**: Input: $1.5 - 2^-10$, output: $1.5 - 2^-10$.

---

### bfloat16_t → fp4x2_e2m1_t

Convert the source value according to the precision rounding mode and store it in the destination operand in fp4x2_e2m1_t format.

**Example**: Input: 2.5.

- **CAST_RINT mode** output: 2.
- **CAST_FLOOR mode** output: 2.
- **CAST_CEIL mode** output: 3.
- **CAST_ROUND mode** output: 3.
- **CAST_TRUNC mode** output: 2.

---

### bfloat16_t → fp4x2_e1m2_t

Convert the source value according to the precision rounding mode, and store it in the destination operand in fp4x2_e1m2_t format.

**Example**: Input: 1.375.

- **CAST_RINT mode** output: 1.5.
- **CAST_FLOOR mode** output: 1.25.
- **CAST_CEIL mode** output: 1.5.
- **CAST_ROUND mode** output: 1.5.
- **CAST_TRUNC mode** output: 1.25.

---

### bfloat16_t → half

Convert the source value according to the precision rounding mode, and store it in the destination operand in half format.

**Example**: Input: $2.90573e-06$.

- **CAST_RINT mode** output: $2.9e-06$.
- **CAST_FLOOR mode** output: $2.861e-06$.
- **CAST_CEIL mode** output: $2.9e-06$.
- **CAST_ROUND mode** output: $2.9e-06$.
- **CAST_TRUNC mode** output: $2.861e-06$.

---

### bfloat16_t → int32_t

Convert the source value according to the precision rounding mode and store it in int32_t format into the destination operand.

**Example**: Input: $2^{6} + 0.5$.

- **CAST_RINT mode** output: $2^{6}$.
- **CAST_FLOOR mode** output: $2^{6}$.
- **CAST_CEIL mode** output: $2^{6} + 1$.
- **CAST_ROUND mode** output: $2^{6} + 1$.
- **CAST_TRUNC mode** output: $2^{6}$.

---

### bfloat16_t → float

Store the source value in the destination operand in **float** format, with no precision loss.

**Example**: Input: $1.5 - 2^{-6}$, output: $1.5 - 2^{-6}$.

---

### int32_t → uint8_t

Store the source value in uint8_t format in the destination operand, with truncation of high-order bits and precision loss. **Negative input will be treated as an exception**.

**Example**: Input: $2^{31} - 1$, Output: $2^8 - 1$.

---

### int32_t → uint16_t

Store the source value in the destination operand in uint16_t format, with truncation of high-order bits, so there is precision loss. **Negative input will be treated as an exception**.

**Example**: Input: $2^{31} - 1$, output: $2^{16} - 1$.

---

### int32_t → int16_t

Store the source value in int16_t format into the destination operand.

**Example**: Input: $2^{31} - 1$, output: $2^{15} - 1$.

---

### int32_t → half

The current data type combination supports only data quantization and precision conversion when used with the [SetDeqScale](../memory_vector_compute/type_conversion_aux_config/SetDeqScale.md) API, and the precision rounding mode does not take effect.

Output: $src / 2^{17} \times scale \times 2^{17}$.

---

### int32_t → float

Convert the source value according to the precision rounding mode to a number that can be represented by float, and store it in the destination operand in float format.

**Example**: Input: $2^{25} + 3$, written in float representation: $2^{25} \times (1 + 2^{-24} + 2^{-25})$, requiring $E_v = 25 + 127 = 152$ and $M_v = 2^{-24} + 2^{-25}$.

**Figure 3** int32_t to float

![int32_t to float](../../../figures/int32_to_float.png)

Because float has only a 23-bit mantissa, the gray part needs to be rounded:

- **CAST_RINT mode**: Rounding yields the mantissa `00000000000000000000001`, $E_v = 152$, $M_v = 2^{-23}$, and the final result is $2^{25} + 4$.
- **CAST_FLOOR mode**: Rounding yields the mantissa `00000000000000000000000`, $E_v = 152$, $M_v = 0$, and the final result is $2^{25}$.
- **CAST_CEIL mode**: Rounding yields the mantissa `00000000000000000000001`, $E_v = 152$, $M_v = 2^{-23}$, and the final result is $2^{25} + 4$.
- **CAST_ROUND mode**: Rounding yields the mantissa `00000000000000000000001`, $E_v = 152$, $M_v = 2^{-23}$, and the final result is $2^{25} + 4$.
- **CAST_TRUNC mode** rounding yields the mantissa `00000000000000000000000`, $E_v = 152$, $M_v = 0$, and the final result is $2^{25}$.

---

### int32_t → int64_t

Store the source value in int64_t format in the destination operand, with no precision loss.

**Example**: Input: $2^{31} - 1$, output: $2^{31} - 1$.

---

### uint32_t → uint8_t

Store the source value in uint8_t format into the destination operand.

**Example**: Input: $2^{32} - 1$, output: $2^8 - 1$.

---

### uint32_t → uint16_t

Store the source value in uint16_t format in the destination operand.

**Example**: Input: $2^{32} - 1$, output: $2^{16} - 1$.

---

### uint32_t → int16_t

Store the source value in int16_t format into the destination operand.

**Example**: Input: $2^{32} - 1$, output: $2^{15} - 1$.

---

### float → hifloat8_t

Round the source value according to the precision rounding mode, and store it in the destination operand in hifloat8_t format.

**Example**: Input: $1.75$.

- **CAST_ROUND mode** output: $2$.
- **CAST_HYBRID mode**: refer to the output of [float to hifloat8_t type conversion rule](../reg_vector_compute/type_conversion/Cast.md#float-to-hifloat8_t-conversion-rule).

---

### float → fp8_e4m3fn_t

Convert the source value according to the precision rounding mode, and store it in the destination operand in fp8_e4m3fn_t format.

**Example**: Input: $2.5$, **CAST_RINT mode** output: $2$.

---

### float → fp8_e5m2_t

Round the source value according to the precision rounding mode, and store it in the destination operand in fp8_e5m2_t format.

**Example**: Input: $2.5$, **CAST_RINT mode** output: $2$.

---

### float → int16_t

Convert the source value according to the precision rounding mode, and store it in int16_t format in the destination operand.

**Example**: Input: $2^{22} + 0.5$.

- **CAST_RINT mode** output: $2^{15} - 1$ (overflow handling).
- **CAST_FLOOR mode** output: $2^{15} - 1$ (overflow handling).
- **CAST_CEIL mode** output: $2^{15} - 1$ (overflow handling).
- **CAST_ROUND mode** output: $2^{15} - 1$ (overflow handling).
- **CAST_TRUNC mode** output: $2^{15} - 1$ (overflow handling).

---

### float → half

Convert the source value according to the precision rounding mode to a number that can be represented by half, and store it in the destination operand in half format.

**Example**: Input: $0.5 + 2^{-12}$, written as the float representation form: $2^{-1} \times (1 + 2^{-11})$, therefore $E_v = -1 + 127 = 126$, $M_v = 2^{-11}$.

**Figure 4** float to half

![float to half](../../../figures/float_to_half.png)

The exponent bits of half can represent $2^{-1}$, $E_v = -1 + 15 = 14$, but half has only 10 mantissa bits, therefore the gray part needs to be rounded:

- **CAST_RINT mode** rounding yields the mantissa `0000000000`, $E_v = 14$, $M_v = 0$, and the final result is $0.5$.
- **CAST_FLOOR mode** rounding yields the mantissa `0000000000`, $E_v = 14$, $M_v = 0$, and the final result is $0.5$.
- **CAST_CEIL mode** rounding yields the mantissa `0000000001`, $E_v = 14$, $M_v = 2^{-10}$, and the final result is $0.5 + 2^{-11}$.
- **CAST_ROUND mode** rounding yields the mantissa `0000000001`, $E_v = 14$, $M_v = 2^{-10}$, and the final result is $0.5 + 2^{-11}$.
- In **CAST_TRUNC** mode, rounding yields the mantissa `0000000000`, $E_v = 14$, $M_v = 0$, and the final result is $0.5$.
- In **CAST_ODD** mode, rounding yields the mantissa `0000000001`, $E_v = 14$, $M_v = 2^{-10}$, and the final result is $0.5 + 2^{-11}$.

---

### float → bfloat16_t

Convert the source value according to the precision rounding mode to a number that can be represented by bfloat16_t, and store it in the destination operand in bfloat16_t format.

**Example**: Input: $0.5 + 2^{-9} + 2^{-11}$, written as the float representation form: $2^{-1} \times (1 + 2^{-8} + 2^{-10})$, therefore $E_v = -1 + 127 = 126$, $M_v = 2^{-8} + 2^{-10}$.

**Figure 5** float to bfloat16_t

![float to bfloat16_t](../../../figures/float_to_bfloat16_t.png)

The number of exponent bits of bfloat16_t is the same as that of float, with $E_v = 126$, but bfloat16_t has only a 7-bit mantissa, therefore the gray part needs to perform rounding:

- **CAST_RINT mode**: rounding yields the mantissa `0000001`, $E_v = 126$, $M_v = 2^{-7}$, and the final result is $0.5 + 2^{-8}$.
- **CAST_FLOOR mode**: rounding yields the mantissa `0000000`, $E_v = 126$, $M_v = 0$, and the final result is $0.5$.
- **CAST_CEIL mode**: rounding yields the mantissa `0000001`, $E_v = 126$, $M_v = 2^{-7}$, and the final result is $0.5 + 2^{-8}$.
- **CAST_ROUND mode**: rounding yields the mantissa `0000001`, $E_v = 126$, $M_v = 2^{-7}$, and the final result is $0.5 + 2^{-8}$.
- **CAST_TRUNC mode** rounding yields the mantissa `0000000`, $E_v = 126$, $M_v = 0$, and the final result is $0.5$.

---

### float → int32_t

Convert the source value according to the precision rounding mode and store it in int32_t format into the destination operand.

**Example**: Input: $2^{22} + 0.5$.

- **CAST_RINT mode**: Output: $2^{22}$.
- **CAST_FLOOR mode**: Output: $2^{22}$.
- **CAST_CEIL mode**: Output: $2^{22} + 1$.
- **CAST_ROUND mode**: Output: $2^{22} + 1$.
- **CAST_TRUNC mode**: Output: $2^{22}$.

---

### float -> float

Convert the source value according to the precision rounding mode, and store it in the destination operand in float format.

**Example**: Input: $0.5$.

- **CAST_RINT mode** output: $0.0$.
- **CAST_FLOOR mode** output: $0.0$.
- **CAST_CEIL mode** output: $1.0$.
- **CAST_ROUND mode** output: $1.0$.
- **CAST_TRUNC mode** output: $0.0$.

---

### float → int64_t

Convert the source value according to the precision rounding mode and store it in the destination operand in int64_t format.

**Example**: Input: $2^{22} + 0.5$.

- **CAST_RINT mode**: Output: $2^{22}$.
- **CAST_FLOOR mode**: Output: $2^{22}$.
- **CAST_CEIL mode**: Output: $2^{22} + 1$.
- **CAST_ROUND mode**: Output: $2^{22} + 1$.
- **CAST_TRUNC mode**: Output: $2^{22}$.

---

### complex32 → complex64

The real part and imaginary part of complex64 are both of the float type, and the real part and imaginary part of complex32 are both of the half type. Refer to the precision conversion rules from half to float.

---

### int64_t → int32_t

Store the source value in int32_t format into the destination operand.

**Example**: Input: $2^{31}$, output: $2^{31} - 1$.

---

### int64_t → float

Convert the source value according to the precision rounding mode to a number that can be represented by float, and store it in the destination operand in float format.

**Example**: Input: $2^{35} + 2^{12} + 2^{11}$, written as the float representation form: $2^{35} \times (1 + 2^{-23} + 2^{-24})$, requiring $E_v = 35 + 127 = 162$, $M_v = 2^{-23} + 2^{-24}$.

**Figure 6** int64_t to float

![int64_t to float](../../../figures/int64_t_to_float.png)

Because float has only a 23-bit mantissa, the gray part needs to be rounded:

- **CAST_RINT mode**: Rounding yields the mantissa `00000000000000000000010`, $E_v = 162$, $M_v = 2^{-22}$, and the final result is $2^{35} + 2^{13}$.
- **CAST_FLOOR mode**: Rounding yields the mantissa `00000000000000000000001`, $E_v = 162$, $M_v = 2^{-23}$, and the final result is $2^{35} + 2^{12}$.
- **CAST_CEIL mode**: Rounding yields the mantissa `00000000000000000000010`, $E_v = 162$, $M_v = 2^{-22}$, and the final result is $2^{35} + 2^{13}$.
- **CAST_ROUND mode**: Rounding yields the mantissa `00000000000000000000010`, $E_v = 162$, $M_v = 2^{-22}$, and the final result is $2^{35} + 2^{13}$.
- **CAST_TRUNC mode** rounding yields the mantissa `00000000000000000000001`, $E_v = 162$, $M_v = 2^{-23}$, and the final result is $2^{35} + 2^{12}$.

---

### int64_t → double

Convert the source value according to the precision rounding mode to a number that can be represented by double, and store it in the destination operand in double format.

**Example**: Input: $2^{61} + 2^9 + 2^8$, written as the double representation form: $2^{61} \times (1 + 2^{-52} + 2^{-53})$, requiring $E_v = 61 + 1023 = 1084$, $M_v = 2^{-52} + 2^{-53}$.

**Figure 7** Schematic diagram of int64_t to double conversion

![Schematic diagram of int64_t to double conversion](../../../figures/int64.png)

Because double has only 52 bits of mantissa, the gray part needs to be rounded:

- **CAST_RINT mode**: Rounding yields the mantissa `...010`, $E_v = 1084$, $M_v = 2^{-51}$, and the final representation result is $2^{61} + 2^{10}$.
- **CAST_FLOOR mode**: Rounding yields the mantissa `...001`, $E_v = 1084$, $M_v = 2^{-52}$, and the final representation result is $2^{61} + 2^9$.
- **CAST_CEIL mode**: Rounding yields the mantissa `...010`, $E_v = 1084$, $M_v = 2^{-51}$, and the final representation result is $2^{61} + 2^{10}$.
- **CAST_ROUND mode**: Rounding yields the mantissa `...010`, $E_v = 1084$, $M_v = 2^{-51}$, and the final representation result is $2^{61} + 2^{10}$.
- **CAST_TRUNC mode** rounding yields the mantissa `...001`, $E_v = 1084$, $M_v = 2^{-52}$, and the final representation result is $2^{61} + 2^9$.

---

### double → float

Convert the source value according to the precision rounding mode to a number that can be represented by float, and store it in the destination operand in float format.

**Example**: Input: $2^{35} + 2^{12} + 2^{11}$, written in float representation: $2^{35} \times (1 + 2^{-23} + 2^{-24})$, requiring $E_v = 1058 - 1023 + 127 = 162$, $M_v = 2^{-23} + 2^{-24}$.

**Figure 8** Schematic diagram of double to float conversion

![Schematic diagram of double to float conversion](../../../figures/float.png)

Because float has only an 8-bit exponent, the exponent part needs to be converted, and only a 23-bit mantissa, the gray part needs to be rounded:

- In **CAST_RINT** mode, rounding yields the mantissa 00000000000000000000010, $E_v = 162$, $M_v = 2^{-22}$, and the final representation result is $2^{35} + 2^{13}$.
- In **CAST_FLOOR** mode, rounding yields the mantissa 00000000000000000000001, $E_v = 162$, $M_v = 2^{-23}$, and the final representation result is $2^{35} + 2^{12}$.
- In **CAST_CEIL** mode, rounding yields the mantissa 00000000000000000000010, $E_v = 162$, $M_v = 2^{-22}$, and the final representation result is $2^{35} + 2^{13}$.
- In **CAST_ROUND** mode, rounding yields the mantissa 00000000000000000000010, $E_v = 162$, $M_v = 2^{-22}$, and the final representation result is $2^{35} + 2^{13}$.
- **CAST_TRUNC mode** rounding yields the mantissa 00000000000000000000001, $E_v = 162$, $M_v = 2^{-23}$, and the final representation result is $2^{35} + 2^{12}$.

---

### double → bfloat16_t

Convert the source value according to the precision rounding mode to a number that can be represented by bfloat16_t, and store it in the destination operand in bfloat16_t format.

**Example**: Input: $2^{35} + 2^{28} + 2^{27}$, written as the bfloat16_t representation form: $2^{35} \times (1 + 2^{-7} + 2^{-8})$, requiring $E_v = 1058 - 1023 + 127 = 162$, $M_v = 2^{-7} + 2^{-8}$.

**Figure 9** Schematic diagram of double to bfloat16_t conversion

![Schematic diagram of double to bfloat16_t conversion](../../../figures/double2bfloat16.png)

Because bfloat16_t has only an 8-bit exponent, the exponent part needs to be converted, and it has only a 7-bit mantissa, therefore the gray part needs to be rounded:

- **CAST_RINT mode**: rounding yields the mantissa 0000010, $E_v = 162$, $M_v = 2^{-6}$, and the final representation result is $2^{35} + 2^{29}$.
- **CAST_FLOOR mode**: rounding yields the mantissa 0000001, $E_v = 162$, $M_v = 2^{-7}$, and the final representation result is $2^{35} + 2^{28}$.
- **CAST_CEIL mode**: rounding yields the mantissa 0000010, $E_v = 162$, $M_v = 2^{-6}$, and the final representation result is $2^{35} + 2^{29}$.
- **CAST_ROUND mode**: rounding yields the mantissa 0000010, $E_v = 162$, $M_v = 2^{-6}$, and the final representation result is $2^{35} + 2^{29}$.
- **CAST_TRUNC mode** rounding yields the mantissa 0000001, $E_v = 162$, $M_v = 2^{-7}$, and the final representation result is $2^{35} + 2^{28}$.

---

### double → int32_t

Convert the source value according to the precision rounding mode and store it in int32_t format into the destination operand.

**Example**: Input: -1.5, **CAST_TRUNC mode** output: -1.

---

### double → int64_t

Convert the source value according to the precision rounding mode and store it in the destination operand in int64_t format.

**Example**: Input: -1.5, **CAST_TRUNC mode** output: -1.

---

### complex64 → complex64

The real part and imaginary part of **complex64** are both of the **float** type. Refer to the precision conversion rules for float.

---

### complex64 → complex32

The real part and imaginary part of complex64 are both of the float type, and the real part and imaginary part of complex32 are both of the half type. Refer to the precision conversion rules from float to half.
