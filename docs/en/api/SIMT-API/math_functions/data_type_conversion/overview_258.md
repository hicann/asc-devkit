# Overview

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-29T01:33:48.577Z -->

Type conversion includes implicit conversion and explicit conversion. Implicit conversion is performed automatically by the system when types do not match. Explicit conversion is actively completed by calling type conversion APIs, which convert the input data from the source type to the target type and return the conversion result. The entire conversion process follows the specified processing rules. The conversion result is affected by the rounding mode and the saturation mode. For details about the representation and rounding rules of floating-point numbers, see [type conversion](../../../SIMD-API/basic_api/memory_vector_compute/type_conversion/Cast.md#section618mcpsimp).

## Implicit Conversion

Implicit conversion refers to the behavior in which the system automatically converts data from the source type to the target type without explicitly specifying the target type.

The following is an example of implicit conversion.

```
int32_t a;
a = 2.2f; // a = 2
```

- Integer input and integer output:
  The input and output are int8_t/uint8_t/int16_t/uint16_t/int32_t/uint32_t/int64_t/uint64_t, and the rounding rule follows [CAST_TRUNC](../../../SIMD-API/basic_api/data_structures/precision_conversion.md#tab1).
- Floating-point input and integer output:
  - The input is float, and the output is int32_t/uint32_t/int64_t/uint64_t
  - The input is half or bfloat16_t, and the output is int32_t/uint32_t

data_structures  The rounding rule follows [CAST_TRUNC](../../../SIMD-API/basic_api/data_structures/precision_conversion.md#tab1).

  When converting wide-range data to narrow-range data:
  - If the input is +inf, or exceeds the maximum value that the type can represent, the conversion result is the maximum value.
  - If the input is -inf, or is less than the minimum value that the type can represent, the conversion result is the minimum value.
  - If the input is nan, the conversion result is 0.
- Input integer, output floating-point:
  - Input int32_t/uint32_t, output float/half/bfloat16_t
  - Input int64_t/uint64_t, output float
  The rounding rule follows [CAST_RINT](../../../SIMD-API/basic_api/data_structures/precision_conversion.md#tab1).

  When wide-range data is converted to narrow-range data:
  - If the input exceeds the range that the type can represent, the conversion result is the inf value of the corresponding sign.
- Input floating-point, output floating-point:
  - Input is float, and output is half/bfloat16_t.
  - Input is half, and output is float/bfloat16_t.
  - Input is bfloat16_t, and output is float/half.

  The rounding rule follows [CAST_RINT](../../../SIMD-API/basic_api/data_structures/precision_conversion.md#tab1).

  When wide-range data is converted to narrow-range data:
  - If the input exceeds the range that the type can represent, the conversion result is the inf value of the corresponding symbol.
  - If the input is nan, the conversion result is nan.

Examples of implicit conversion are as follows:

```
// Convert int8_t to uint8_t.
int8_t a = -100;
uint8_t b = a; // b = 156

// Convert half to int32_t.
half a = 60000.6;
int32_t b = a; // b = 60000

// Convert int32_t to half.
int32_t a = 100000;
half b = a; // b = inf, 100000 exceeds the range that the half type can represent.

// Convert float to half.
float a = -100000.0f;
half b = a; // b = -inf, -100000.0f exceeds the range that the half type can represent.
```

## Rounding Mode

The rounding mode controls how precision loss is handled during type conversion.

**Table 1** Rounding modes supported by type conversion interfaces

| Rounding Mode | Description | Interface Name Format |
| --- | --- | --- |
| CAST_RINT | Round to nearest, ties to even | srctype2dsttype_rn |
| CAST_FLOOR | Round down | srctype2dsttype_rd |
| CAST_CEIL | Round up | srctype2dsttype_ru |
| CAST_ROUND | Round half away from zero | srctype2dsttype_rna |
| CAST_TRUNC | Round toward zero | srctype2dsttype_rz |
| CAST_ODD | Round to nearest odd | srctype2dsttype_ro |
| CAST_HYBRID | Currently refers to a random rounding used when the output result is hi8 data | srctype2dsttype_rh |

The table above describes the interface name format corresponding to each input mode, where **srctype** indicates the input source operand type and **dsttype** indicates the output target type. For example, the interface name for converting the **float** type to the **half** type using the **CAST_RINT** rounding mode is **__float2half_rn**.

## Saturation Behavior

Saturation behavior controls how nan and inf special values, as well as results that exceed the range of the target type after conversion, are handled.

**Table 2**  Mode description of saturation behavior

| Mode | Description | Interface Name Format |
| --- | --- | --- |
| Non-saturation mode | Default mode.<br>If the input exceeds the range that the type can represent, the conversion result is the inf value with the corresponding sign.<br>If the input is nan, the conversion result is nan. | srctype2dsttype_r[xx] |
| Saturation mode | In saturation mode:<br>If the input is +inf, or exceeds the maximum value that the type can represent, the conversion result is the maximum value.<br>If the input is -inf, or is less than the minimum value that the type can represent, the conversion result is the minimum value.<br>If the input is nan, the conversion result is 0. | srctype2dsttype_r[xx]_sat |

## Methods for Controlling Saturation Behavior

The saturation behavior is controlled by the registers **CTRL[60]** and **CTRL[48]** together with the interface function. In mixed SIMD and SIMT programming scenarios, refer to the [SetCtrlSpr(ISASI)](../../../SIMD-API/basic_api/special_register_access/SetCtrlSpr_ISASI.md) interface for the CTRL register settings. Setting the CTRL register is not supported in SIMT programming scenarios.

When wide-range data is converted to narrow-range data and the target type of the conversion is not float, including float to half, float to bfloat16_t, bfloat16_t to half, and float or half to hifloat8_t, fp8_e4m3fn_t, or fp8_e5m2_t, the saturation behavior of the interface is affected by the hardware registers **CTRL[60]** and **CTRL[48]**. The specific behavior is as follows:

-   When **CTRL[60]** = 0, the saturation behavior is independently controlled by the interface.
    -   The conversion result of an interface with the __sat suffix is in saturation mode.
    -   The conversion result of an interface without the __sat suffix is in non-saturation mode.

-   When **CTRL[60]** = 1, the saturation behavior is globally controlled only by **CTRL[48]**.
    -   When **CTRL[48]** = 1, the interface conversion result is in non-saturation mode.
    -   When **CTRL[48]** = 0, the interface conversion result is in saturation mode.

**Table 3**  Final saturation behavior of converting wide-range data to narrow-range data

| Ctrl[60] | Ctrl[48] | interface name format | final saturation behavior |
| --- | --- | --- | --- |
| 0 | / | Without __sat | non-saturation |
| 1 | 1 | / | non-saturation |
| 0 | / | __sat | saturation |
| 1 | 0 | / | saturation |

For the following two cases, the saturation behavior is controlled only by ctrl\[48\].

-   Narrow-range data is converted to wide-range data, and the destination operand type of the conversion is not float, that is, half to bfloat16\_t, and hifloat8\_t to half.
-   half to half, and bfloat16\_t to bfloat16\_t.

The specific behavior is as follows:

-   When CTRL\[48\] = 1, the interface conversion result is in non-saturation mode.
-   When CTRL\[48\] = 0, the interface conversion result is in saturation mode.

**Table 4**  Final saturation behavior for narrow-range data to wide-range data conversion and same-type data conversion

| Ctrl[60] | Ctrl[48] | final saturation behavior |
| --- | --- | --- |
| / | 1 | non-saturation |
| / | 0 | saturation |

For cases where the target type of the conversion is float, all type conversion interfaces support only the non-saturation mode. When a floating-point number is converted to an integer, the conversion result always uses the saturation mode.

## Data Type Conversion Rule Examples

- Input data type is **float**:
  - Output data type is **float**:

    Round **src** according to the rounding mode (precision conversion processing mode; see [Table 1](#rounding-mode)) and store the result in **dst** in the float format.

    Example: For input 0.5, the CAST_RINT mode outputs 0.0, the CAST_FLOOR mode outputs 0.0, the CAST_CEIL mode outputs 1.0, the CAST_ROUND mode outputs 1.0, and the CAST_TRUNC mode outputs 0.0.
  - Output data type is **half**:

    Example 1: For input 0.5 + 2^(-12), written in the float representation form: 2^(-1) \* (1 + 2^(-11)); therefore, E = -1 + 127 = 126 and M = 2^(-11). The exponent bits of half can represent 2^(-1), with E = -1 + 15 = 14, but half has only 10 mantissa bits, so rounding is required.

    In the CAST_RINT mode, rounding yields a mantissa of 0000000000, with E = 14 and M = 0, and the final representation result is 0.5;

    In the CAST_FLOOR mode, rounding yields a mantissa of 0000000000, with E = 14 and M = 0, and the final representation result is 0.5;

    In the CAST_CEIL mode, rounding yields a mantissa of 0000000001, with E = 14 and M = 2^(-10), and the final representation result is 0.5 + 2^(-11);

    CAST_ROUND mode rounding yields a mantissa of 0000000001, E = 14, M = 2^(-10), and the final representation result is 0.5 + 2^(-11);

    CAST_TRUNC mode rounding yields a mantissa of 0000000000, E = 14, M = 0, and the final representation result is 0.5;

    CAST_ODD mode rounding yields a mantissa of 0000000001, E = 14, M = 2^(-10), and the final representation result is 0.5 + 2^(-11).

    For the final saturation behavior of float-to-half conversion, see [Table 3](#methods-for-controlling-saturation-behavior).

    Example 2: With input inf, the saturation mode outputs 65504, and the non-saturation mode outputs inf.

    Example 3: With input -inf, the saturation mode outputs -65504, and the non-saturation mode outputs -inf.

    Example 4: With input nan, the saturation mode outputs 0, and the non-saturation mode outputs nan.
  - When the output data type is bfloat16_t:

    Example 1: With input 0.5 + 2^(-9) + 2^(-11), written in float representation form: 2^(-1) \* (1 + 2^(-8) + 2^(-10)), therefore E = -1 + 127 = 126, M = 2^(-8) + 2^(-10). The number of exponent bits of bfloat16_t is the same as that of float, so E = 126, but bfloat16_t has only 7 mantissa bits, so rounding is required.

    CAST_RINT mode rounding yields a mantissa of 0000001, E = 126, M = 2^(-7), and the final representation result is 0.5 + 2^(-8);

    In CAST_FLOOR mode, rounding yields a mantissa of 0000000, E = 126, M = 0, and the final representation result is 0.5;

    In CAST_CEIL mode, rounding yields a mantissa of 0000001, E = 126, M = 2^(-7), and the final representation result is 0.5 + 2^(-8);

    In CAST_ROUND mode, rounding yields a mantissa of 0000001, E = 126, M = 2^(-7), and the final representation result is 0.5 + 2^(-8);

    In CAST_TRUNC mode, rounding yields a mantissa of 0000000, E = 126, M = 0, and the final representation result is 0.5.

    For the final saturation behavior of float-to-bfloat16_t conversion, see [Table 3](#methods-for-controlling-saturation-behavior).

    Example 2: For input inf, the saturation mode outputs approximately 3.39\*10^38, and the non-saturation mode outputs inf.

    Example 3: For input -inf, the saturation mode outputs approximately -3.39\*10^38, and the non-saturation mode outputs -inf.

    Example 4: For input nan, the saturation mode outputs 0, and the non-saturation mode outputs nan.
  - When the output data type is int32_t:

    Example 1: For input 2^22 + 0.5, the CAST_RINT mode outputs 2^22, the CAST_FLOOR mode outputs 2^22, the CAST_CEIL mode outputs 2^22 + 1, the CAST_ROUND mode outputs 2^22 + 1, and the CAST_TRUNC mode outputs 2^22.

    Example 2: Input inf, output 2^31-1.

    Example 3: Input -inf, output -2^31.

    Example 4: Input nan, output 0.
  - Output data type is uint32_t:

    Example 1: Input 2^22 + 0.5, CAST_RINT mode outputs 2^22, CAST_FLOOR mode outputs 2^22, CAST_CEIL mode outputs 2^22 + 1, CAST_ROUND mode outputs 2^22 + 1, CAST_TRUNC mode outputs 2^22.

    Example 2: Input inf, output 2^32-1.

    Example 3: Input -inf, output 0.

    Example 4: Input nan, output 0.
  - Output data type is int64_t:

    Example 1: Input 2^22 + 0.5, CAST_RINT mode outputs 2^22, CAST_FLOOR mode outputs 2^22, CAST_CEIL mode outputs 2^22 + 1, CAST_ROUND mode outputs 2^22 + 1, CAST_TRUNC mode outputs 2^22.

    Example 2: Input inf, output 2^63-1.

    Example 3: Input -inf, output -2^63.

    Example 4: Input nan, output 0.
  - Output data type is uint64_t:

    Example 1: Input 2^22 + 0.5, CAST_RINT mode outputs 2^22, CAST_FLOOR mode outputs 2^22, CAST_CEIL mode outputs 2^22 + 1, CAST_ROUND mode outputs 2^22 + 1, CAST_TRUNC mode outputs 2^22.

    Example 2: Input inf, output 2^64-1.

    Example 3: Input -inf, output 0.

    Example 4: Input nan, output 0.
  - Output data type is hifloat8_t:

    Round src according to the rounding mode, and store it in dst in hifloat8_t format (overflow is handled by saturation by default).

    Example: For input 1.75, the CAST_ROUND mode outputs 2; for CAST_HYBRID, see [Table 9](../../../SIMD-API/basic_api/reg_vector_compute/type_conversion/Cast-45.md#table1352142520363).
  - When the output data type is fp8_e4m3fn_t:

    Round src according to the rounding mode and store it into dst in the fp8_e4m3fn_t format (overflow is handled as saturation by default).

    Example: For input 2.5, the CAST_RINT mode outputs 2.
  - When the output data type is fp8_e5m2_t:

    Round src according to the rounding mode and store it into dst in the fp8_e5m2_t format (overflow is handled as saturation by default).

    Example: For input 2.5, the CAST_RINT mode outputs 2.
- When the input data type is half:
  - When the output data type is half:

    Round src according to the rounding mode and store it into dst still in the half format.

    Example: Input 0.5.

    In CAST_RINT mode, the output is 0.0; in CAST_FLOOR mode, the output is 0.0; in CAST_CEIL mode, the output is 1.0; in CAST_ROUND mode, the output is 1.0; in CAST_TRUNC mode, the output is 0.0.
  - Output data type is float:

    There is no precision conversion issue, and the rounding mode has no effect.

    Example 1: Input 1.5 - 2^(-10), output 1.5 - 2^(-10). Converting half to float supports only the non-saturation mode.

    Example 2: Input inf, output inf.

    Example 3: Input -inf, output -inf.

    Example 4: Input nan, output nan.
  - Output data type is bfloat16_t:

    Example 1: Input 0.5 + 2^(-9) + 2^(-11), written in the half representation form: 2^(-1) \* (1 + 2^(-8) + 2^(-10)), therefore E = -1 + 15 = 14, M = 2^(-8) + 2^(-10). The exponent bits of bfloat16_t can represent 2^(-1), E = -1 + 127 = 126, but bfloat16_t has only 7 mantissa bits, so rounding is required.

    In CAST_RINT mode, rounding yields the mantissa 0000001, with E = 126 and M = 2^(-7), so the final representation result is 0.5 + 2^(-8);

    In CAST_FLOOR mode, rounding yields the mantissa 0000000, with E = 126 and M = 0, so the final representation result is 0.5;

    In CAST_CEIL mode, rounding yields the mantissa 0000001, with E = 126 and M = 2^(-7), so the final representation result is 0.5 + 2^(-8);

    In CAST_ROUND mode, rounding yields the mantissa 0000001, with E = 126 and M = 2^(-7), so the final representation result is 0.5 + 2^(-8);

    In CAST_TRUNC mode, rounding yields the mantissa 0000000, with E = 126 and M = 0, so the final representation result is 0.5.

    For the final saturation behavior of converting half to bfloat16_t, see [Table 4](#methods-for-controlling-saturation-behavior).

    Example 2: For input inf, the saturation mode outputs approximately 3.39*10^38, and the non-saturation mode outputs inf.

    Example 3: For input -inf, the saturation mode outputs approximately -3.39*10^38, and the non-saturation mode outputs -inf.

    Example 4: For input nan, the saturation mode outputs 0, and the non-saturation mode outputs nan.
  - When the output data type is int32_t:

    Example 1: For input 2^9 + 0.5, the CAST_RINT mode outputs 2^9, the CAST_FLOOR mode outputs 2^9, the CAST_CEIL mode outputs 2^9 + 1, the CAST_ROUND mode outputs 2^9 + 1, and the CAST_TRUNC mode outputs 2^9.

    Example 2: For input inf, the output is 2^31-1.

    Example 3: For input -inf, the output is -2^31.

    Example 4: For input nan, the output is 0.
  - When the output data type is uint32_t:

    Example 1: For input 2^9 + 0.5, the CAST_RINT mode outputs 2^9, the CAST_FLOOR mode outputs 2^9, the CAST_CEIL mode outputs 2^9 + 1, the CAST_ROUND mode outputs 2^9 + 1, and the CAST_TRUNC mode outputs 2^9.

    Example 2: For input inf, the output is 2^32-1.

    Example 3: For input -inf, the output is 0.

    Example 4: For input nan, the output is 0.
  - When the output data type is hifloat8_t:

    Round src according to the rounding mode and store it in dst in the hifloat8_t format (overflow is handled as saturation by default).

    Example: Input 1.75.

    The CAST_ROUND mode outputs 2, and the CAST_HYBRID mode outputs as described in [Table 10](../../../SIMD-API/basic_api/reg_vector_compute/type_conversion/Cast-45.md#table163085984311).
- Input data type bfloat16_t:
  - Output data type bfloat16_t:

    Round src according to the rounding mode and store it in dst still in the bfloat16_t format.

    Example: Input 0.5.

    The CAST_RINT mode outputs 0.0, the CAST_FLOOR mode outputs 0.0, the CAST_CEIL mode outputs 1.0, the CAST_ROUND mode outputs 1.0, and the CAST_TRUNC mode outputs 0.0.
  - Output data type float:

    There is no precision conversion issue, and the rounding mode has no effect.

    Example 1: Input 1.5 - 2^(-6), output 1.5 - 2^(-6). bfloat16_t to float conversion supports only non-saturation mode.

    Example 2: Input inf, output inf.

    Example 3: Input -inf, output -inf.

    Example 4: Input nan, output nan.
  - Output data type is half:

    Example 1: Input 1.5 - 2^(-6), output 1.5 - 2^(-6). For the final saturation behavior of bfloat16 to half conversion, see [Table 3](#methods-for-controlling-saturation-behavior).

    Example 2: Input inf, saturation mode outputs 65504, non-saturation mode outputs inf.

    Example 3: Input -inf, saturation mode outputs -65504, non-saturation mode outputs -inf.

    Example 4: Input nan, saturation mode outputs 0, non-saturation mode outputs nan.
  - Output data type is int32_t:

    Example 1: Input 2^6 + 0.5. In CAST_RINT mode, the output is 2^6; in CAST_FLOOR mode, the output is 2^6; in CAST_CEIL mode, the output is 2^6 + 1; in CAST_ROUND mode, the output is 2^6 + 1; in CAST_TRUNC mode, the output is 2^6.

    Example 2: Input inf, output 2^31-1.

    Example 3: Input -inf, output -2^31.

    Example 4: Input nan, output 0.
  - Output data type is uint32_t:

    Example 1: Input 2^6 + 0.5. In CAST_RINT mode, the output is 2^6; in CAST_FLOOR mode, the output is 2^6; in CAST_CEIL mode, the output is 2^6 + 1; in CAST_ROUND mode, the output is 2^6 + 1; in CAST_TRUNC mode, the output is 2^6.

    Example 2: Input inf, output 2^32-1.

    Example 3: Input -inf, output 0.

    Example 4: Input nan, output 0.
- Input data type is hifloat8_t:
  - Output data type is float:

    Store src in dst in float format. There is no precision conversion issue and no rounding mode.

    Example: input 2, output 2.
  - Output data type is half:

    Store src in dst in half format. There is no precision conversion issue and no rounding mode.

    Example: input 2, output 2.
- Input data type is fp8_e4m3fn_t:
  - Output data type is float:

    Store src in dst in float format. There is no precision conversion issue and no rounding mode.

    Example: input 2, output 2.
- Input data type is fp8_e5m2_t:
  - Output data type is float:

    Store src in dst in float format. There is no precision conversion issue and no rounding mode.

    Example: Input 2, output 2.
- Input data type is int32_t:
  - Output data type is float:

    Example 1: Input 2^25 + 3, written in float representation form: 2^25 \* (1 + 2^(-24) + 2^(-25)), requiring E = 25 + 127 = 152 and M = 2^(-24) + 2^(-25). Since float has only 23 mantissa bits, rounding is required.

    In CAST_RINT mode, rounding yields mantissa 00000000000000000000001, E = 152, M = 2^(-23), and the final representation result is 2^25 + 4;

    In CAST_FLOOR mode, rounding yields mantissa 00000000000000000000000, E = 152, M = 0, and the final representation result is 2^25;

    In CAST_CEIL mode, rounding yields mantissa 00000000000000000000001, E = 152, M = 2^(-23), and the final representation result is 2^25 + 4;

    CAST_ROUND mode rounding yields the mantissa 00000000000000000000001, E = 152, M = 2^(-23), and the final representation result is 2^25 + 4;

    CAST_TRUNC mode rounding yields the mantissa 00000000000000000000000, E = 152, M = 0, and the final representation result is 2^25.
  - Output data type is half:

    Example 1: Input 2^12 + 3, written in the representation form of half: 2^12 \*(1 + 2^(-11) + 2^(-12)), requiring E = 12 + 15 = 27, M = 2^(-11) + 2^(-12). Since half has only 10 mantissa bits, rounding is therefore required.

    CAST_RINT mode rounding yields the mantissa 0000000001, E = 27, M = 2^(-10), and the final representation result is 2^12 + 4;

    CAST_FLOOR mode rounding yields the mantissa 0000000000, E = 27, M = 0, and the final representation result is 2^12;

    CAST_CEIL mode rounding yields the mantissa 0000000001, E = 27, M = 2^(-10), and the final representation result is 2^12 + 4;

    CAST_ROUND mode rounding yields the mantissa 0000000001, E = 27, M = 2^(-10), and the final representation result is 2^12 + 4;

    CAST_TRUNC mode rounding yields the mantissa 0000000000, E = 27, M = 0, and the final representation result is 2^12.

    Example 2: Input 2^31 - 1, SAT mode returns 65504.0, and NO_SAT mode returns inf.

    Example 3: Input -2^31. In SAT mode, the result is -65504.0; in NO_SAT mode, the result is -inf.
  - Output data type is bfloat16_t:

    Example 1: Input 2^9 + 3, written in the bfloat16_t representation form: 2^9 \*(1 + 2^(-8) + 2^(-9)), requiring E = 9 + 127 = 136 and M = 2^(-8) + 2^(-9). Since bfloat16_t has only 7 mantissa bits, rounding is required.

    In CAST_RINT mode, rounding yields mantissa 0000001, E = 136, M = 2^(-7), and the final representation result is 2^9 + 4;

    In CAST_FLOOR mode, rounding yields mantissa 0000000, E = 136, M = 0, and the final representation result is 2^9;

    In CAST_CEIL mode, rounding yields mantissa 0000001, E = 136, M = 2^(-7), and the final representation result is 2^9 + 4;

    In CAST_ROUND mode, rounding yields mantissa 0000001, E = 136, M = 2^(-7), and the final representation result is 2^9 + 4;

    In CAST_TRUNC mode, rounding yields mantissa 0000000, E = 136, M = 0, and the final representation result is 2^9.
- Input data type is uint32_t:
  - Output data type is float:

    Example 1: Input 2^25 + 3, written in the float representation form: 2^25 \* (1 + 2^(-24) + 2^(-25)), requiring E = 25 + 127 = 152, M = 2^(-24) + 2^(-25). Since float has only 23 mantissa bits, rounding is required.

    In CAST_RINT mode, rounding yields mantissa 00000000000000000000001, E = 152, M = 2^(-23), and the final representation result is 2^25 + 4;

    In CAST_FLOOR mode, rounding yields mantissa 00000000000000000000000, E = 152, M = 0, and the final representation result is 2^25;

    In CAST_CEIL mode, rounding yields mantissa 00000000000000000000001, E = 152, M = 2^(-23), and the final representation result is 2^25 + 4;

    In CAST_ROUND mode, rounding yields mantissa 00000000000000000000001, E = 152, M = 2^(-23), and the final representation result is 2^25 + 4;

    In CAST_TRUNC mode, rounding yields mantissa 00000000000000000000000, E = 152, M = 0, and the final representation result is 2^25.
  - The output data type is half:

    Example 1: Input 2^12 + 3, written in the half representation form: 2^12 \*(1 + 2^(-11) + 2^(-12)), requiring E = 12 + 15 = 27, M = 2^(-11) + 2^(-12). Since half has only 10 mantissa bits, rounding is required.

    In CAST_RINT mode, rounding yields mantissa 0000000001, E = 27, M = 2^(-10), and the final representation result is 2^12 + 4;

    In CAST_FLOOR mode, rounding yields mantissa 0000000000, E = 27, M = 0, and the final representation result is 2^12;

    CAST_CEIL mode rounding yields a mantissa of 0000000001, E = 27, M = 2^(-10), and the final representation result is 2^12+ 4;

    CAST_ROUND mode rounding yields a mantissa of 0000000001, E = 27, M = 2^(-10), and the final representation result is 2^12 + 4;

    CAST_TRUNC mode rounding yields a mantissa of 0000000000, E = 27, M = 0, and the final representation result is 2^12.

    Example 2: Input 2^32 - 1, SAT mode returns 65504.0, and NO_SAT mode returns inf.
  - Output data type is bfloat16_t:

    Example 1: Input 2^9 + 3, written in the representation form of bfloat16_t: 2^9 \*(1 + 2^(-8) + 2^(-9)), which requires E = 9 + 127 = 136 and M = 2^(-8) + 2^(-9). Since bfloat16_t has only 7 mantissa bits, rounding is required.

    CAST_RINT mode rounding yields a mantissa of 0000001, E = 136, M = 2^(-7), and the final representation result is 2^9 + 4;

    CAST_FLOOR mode rounding yields a mantissa of 0000000, E = 136, M = 0, and the final representation result is 2^9;

    CAST_CEIL mode rounding yields a mantissa of 0000001, E = 136, M = 2^(-7), and the final representation result is 2^9+ 4;

    CAST_ROUND mode rounding yields a mantissa of 0000001, E = 136, M = 2^(-7), and the final representation result is 2^9 + 4;

    In CAST_TRUNC mode, rounding yields mantissa 0000000, E = 136, M = 0, and the final representation result is 2^9.
- Input data type is int64_t:
  - Output data type is float:

    Example 1: Input 2^25 + 3, written in float representation: 2^25 \* (1 + 2^(-24) + 2^(-25)), requiring E = 25 + 127 = 152 and M = 2^(-24) + 2^(-25). Since float has only 23 mantissa bits, rounding is required.

    In CAST_RINT mode, rounding yields mantissa 00000000000000000000001, E = 152, M = 2^(-23), and the final representation result is 2^25 + 4;

    In CAST_FLOOR mode, rounding yields mantissa 00000000000000000000000, E = 152, M = 0, and the final representation result is 2^25;

    In CAST_CEIL mode, rounding yields mantissa 00000000000000000000001, E = 152, M = 2^(-23), and the final representation result is 2^25 + 4;

    In CAST_ROUND mode, rounding yields mantissa 00000000000000000000001, E = 152, M = 2^(-23), and the final representation result is 2^25 + 4;

    In CAST_TRUNC mode, rounding yields mantissa 00000000000000000000000, E = 152, M = 0, and the final representation result is 2^25.
- Input data type is uint64_t:
  - Output data type is float:

    Example 1: Input 2^25 + 3, written in float representation: 2^25 \* (1 + 2^(-24) + 2^(-25)), requiring E = 25 + 127 = 152, M = 2^(-24) + 2^(-25). Since float has only 23 mantissa bits, rounding is required.

    CAST_RINT mode rounds to mantissa 00000000000000000000001, E = 152, M = 2^(-23), and the final representation result is 2^25 + 4;

    CAST_FLOOR mode rounds to mantissa 00000000000000000000000, E = 152, M = 0, and the final representation result is 2^25;

    CAST_CEIL mode rounds to mantissa 00000000000000000000001, E = 152, M = 2^(-23), and the final representation result is 2^25 + 4;

    CAST_ROUND mode rounds to mantissa 00000000000000000000001, E = 152, M = 2^(-23), and the final representation result is 2^25 + 4;

    CAST_TRUNC mode rounds to mantissa 00000000000000000000000, E = 152, M = 0, and the final representation result is 2^25.
