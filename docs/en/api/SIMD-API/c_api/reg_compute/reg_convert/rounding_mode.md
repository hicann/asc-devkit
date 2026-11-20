---
okftype: reference.concept
description: "Describes the seven rounding modes supported by data type conversion—RINT, ROUND, FLOOR, CEIL, TRUNC, ODD, and HYBRID—and their function suffixes, uses integer and floating-point examples to illustrate the result differences for overflow, positive and negative infinity, and NaN inputs in saturation and non-saturation modes, and explains how to configure saturation mode through the asc_set_saturation_flag API."
---
# Rounding Modes and Saturation Modes

<!-- md-trans-meta sourceCommit=59317b6fda711b248a190c30d2db6306b5b584f1 translatedAt=2026-11-16T01:40:03.002793+00:00 pushedAt=2026-11-16T02:10:56.554Z nl=2 -->

## Overview

When performing data type conversion, Ascend C API supports multiple rounding modes and saturation modes. These modes determine how to handle rounding and overflow when a source data type is converted to a destination data type with different precision or range.

- **Rounding mode**: Mainly used for conversions between floating-point numbers or from floating-point to integer, determining how to round a high-precision value to a low-precision representation.
- **Saturation mode/non-saturation mode**: Handles overflow behavior when a value exceeds the representable range of the destination data type.

## Rounding Mode Description

Ascend C API supports the following rounding modes:

**Table 1** Rounding mode description

| Rounding Mode | Function Suffix | Description | Example |
| :------ | :------ | :--- | :--- |
| RINT | `_rn` | Round half to even (Banker's Rounding). When the fractional part is 0.5, rounds to the nearest even number. | 1.5 → 2, 2.5 → 2, 3.5 → 4 |
| ROUND | `_rna` | Round half up. When the fractional part is ≥ 0.5, rounds up; when < 0.5, rounds down. | 1.4 → 1, 1.5 → 2, 1.6 → 2 |
| FLOOR | `_rd` | Round toward negative infinity. Always rounds down. | 1.7 → 1, -1.3 → -2 |
| CEIL | `_ru` | Round toward positive infinity. Always rounds up. | 1.3 → 2, -1.7 → -1 |
| TRUNC | `_rz` | Round toward zero. Directly truncates the fractional part. | 1.7 → 1, -1.7 → -1 |
| ODD | `_ro` | Round to nearest odd. Rounds to the nearest odd number. | 1.5 → 1, 2.5 → 3, 3.5 → 3 |
| HYBRID | `_rh` | Hybrid rounding mode. A stochastic rounding mode used specifically when the output result is hifloat8_t data. | - |

### Detailed Rounding Mode Description

#### RINT (Round Half to Even)

Also known as "banker's rounding", it is the most statistically unbiased rounding method.
- When the fractional part is less than 0.5, rounds down.
- When the fractional part is greater than 0.5, rounds up.
- When the fractional part is equal to 0.5, rounds to the nearest even number.

**Example:**

```
1.4 → 1  (fractional part < 0.5, round down)
1.5 → 2  (fractional part = 0.5, round to even)
2.5 → 2  (fractional part = 0.5, round to even)
3.5 → 4  (fractional part = 0.5, round to even)
-1.5 → -2 (fractional part = 0.5, round to even)
```

#### ROUND (Round Half up)

The most commonly used rounding method, consistent with everyday practice.
- When the fractional part is less than 0.5, rounds down.
- When the fractional part is greater than or equal to 0.5, round up.

**Example:**

```
1.4 → 1  (fractional part < 0.5)
1.5 → 2  (fractional part ≥ 0.5)
1.6 → 2  (fractional part ≥ 0.5)
-1.5 → -2 (round toward the larger absolute value)
```

#### FLOOR (Round Down)

Always rounds toward negative infinity, even if the result becomes smaller.

**Example:**

```
1.7 → 1   (downward)
-1.3 → -2 (toward negative infinity, becoming smaller)
```

#### CEIL (Round up)

Always rounds toward positive infinity, even if the result becomes larger.

**Example:**

```
1.3 → 2   (upward)
-1.7 → -1 (toward positive infinity, becoming larger)
```

#### TRUNC (Truncate Toward Zero)

Directly truncates the fractional part and rounds toward zero.

**Example:**

```
1.7 → 1   (toward zero)
-1.7 → -1 (toward zero)
```

#### ODD (Round to Odd)

Rounds to the nearest odd number, mainly used in certain special numerical computation scenarios.

**Example:**

```
1.5 → 1  (nearest odd number)
2.5 → 3  (nearest odd number)
3.5 → 3  (nearest odd number)
```

## Saturation Mode and Non-Saturation Mode

During data type conversion, the source data value may exceed the representable range of the target data type. In this case, saturation mode or non-saturation mode is required to handle the overflow.

### Configuration Method

When using saturation or non-saturation mode, configure it through the [asc_set_saturation_flag](../../spr/asc_set_saturation_flag.md) API. For configuration requirements, see the API description.

### Non-Saturation Mode

In non-saturation mode, when the data exceeds the target type range:

#### Integer Conversion

- **The Result Is Truncated**: The least significant bits (LSBs) are retained, and the high-order bits are discarded directly.
- **Binary Truncation Example**: When converting int32 to int8, only the lower 8 bits are retained.

**Binary truncation example (int32 → int8):**

```
Input value (int32):  0x000001FF (511)
Binary representation:       0000 0000 0000 0000 0000 0001 1111 1111
After truncation (int8):   1111 1111 (0xFF, -1)
Note: Only the lower 8 bits are retained, and the high-order bits are discarded.

Input value (int32):  0x00000100 (256)
Binary representation:       0000 0000 0000 0000 0000 0001 0000 0000
After truncation (int8):   0000 0000 (0x00, 0)
Note: Only the lower 8 bits are retained, and the high-order bits are discarded.

Input value (int32):  0xFFFFFF00 (-256)
Binary representation:       1111 1111 1111 1111 1111 1111 0000 0000
After truncation (int8):   0000 0000 (0x00, 0)
Note: Only the lower 8 bits are retained, and the high-order bits are discarded.
```

#### Floating-Point Conversion

- **Out of Range**: Returns the corresponding signed inf value (positive/negative infinity) of the output type.
- **±inf Input**: Returns ±inf.
- **NaN Input**: Returns NaN.

**Example (float → half):**

```
Input value: 70000.0 (exceeds the half range)
Output value: +inf

Input value: -70000.0
Output value: -inf

Input value: NaN
Output value: NaN
```

#### Floating-Point to Integer Conversion

- **Out of Range**: The result is truncated to the data width of the target format.
- **±inf Input**: Returns the maximum or minimum value of the output type.
- **NaN Input**: Returns 0.

**Example (float → int16):**

```
Input value: 100000.5 (out of int16 range)
Non-saturation mode: The result is truncated (the specific value depends on the binary representation).

Input value: +inf
Output value: 32767 (maximum int16 value)

Input value: NaN
Output value: 0
```

### Saturation Mode

In saturation mode, when data exceeds the target type range, the result is clamped to the representable range of the target type:

#### Integer Conversion

- **Out of Range**: Returns the maximum or minimum value of the target type.
- **Keeps Values Safe**: No unexpected results caused by truncation.

**Example (int32 → int8):**

```
Input value: 511 (out of int8 range)
Saturated output: 127 (maximum int8 value)

Input value: -300 (out of int8 range)
Saturated output: -128 (minimum int8 value)

Input value: 100 (within int8 range)
Saturated output: 100 (unchanged)
```

**Example (int32 → uint8):**

```
Input value: 500 (out of uint8 range)
Saturated output: 255 (maximum uint8 value)

Input value: -10 (out of uint8 range)
Saturated output: 0 (minimum uint8 value)

Input value: 100 (within uint8 range)
Saturated output: 100 (unchanged)
```

#### Floating-Point Conversion

- **Out of Range**: Returns the corresponding extreme value of the output type.
- **±inf Input**: Returns the corresponding extreme value of the output type.
- **NaN Input**: Returns 0.

**Example (float → half):**

```
Input value: 70000.0 (exceeds the half range)
Saturated output: 65504.0 (maximum half value)

Input value: -70000.0
Saturated output: -65504.0 (minimum half value)

Input value: +inf
Saturated output: 65504.0 (maximum half value)

Input value: NaN
Saturated output: 0
```

#### Floating-Point to Integer Conversion

- **Out of Range**: Returns the corresponding extreme value of the output type.
- **±inf Input**: Returns the corresponding extreme value of the output type.
- **NaN Input**: Returns 0.

**Example (float → int16):**

```
Input value: 100000.5 (out of int16 range)
Saturated output: 32767 (maximum int16 value)

Input value: -50000.0
Saturated output: -32768 (minimum int16 value)

Input value: +inf
Saturated output: 32767.

Input value: NaN
Saturated output: 0
```

### Mode Comparison Summary

**Table 2** Description of saturation mode and non-saturation mode

| Scenario | Non-Saturation Mode | Saturation Mode |
| :--- | :--- | :--- |
| **Integer to integer (overflow)** | Truncates the low bits and discards the high bits. | Returns the maximum or minimum value of the destination type. |
| **Floating-point to floating-point (out of range)** | Returns ±inf. | Returns the maximum or minimum value of the destination type. |
| **Floating-point to integer (out of range)** | Truncates to the destination width. | Returns the maximum or minimum value of the destination type. |
| **Floating-point to integer (±inf)** | Returns the maximum or minimum value of the destination type. | Returns the maximum or minimum value of the destination type. |
| **Floating-point to floating-point (±inf)** | Returns ±inf. | Returns the maximum or minimum value of the destination type. |
| **NaN input** | Returns NaN or 0. | Returns 0. |
