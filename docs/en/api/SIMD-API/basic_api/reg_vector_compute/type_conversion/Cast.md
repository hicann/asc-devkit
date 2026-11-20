# Cast<a name="ZH-CN_TOPIC_0000001956827113"></a>

<!-- md-trans-meta sourceCommit=bca6c1424f07e7236ea79f6afa0b44e2c7c5eaa2 translatedAt=2026-11-05T03:55:53.588Z pushedAt=2026-11-18T01:25:30.985Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- npu="9050" id8 -->
- Kirin 9050 processor family products: Supported
<!-- end id8 -->
<!-- npu="kirin1" id9 -->
- KirinDev0001 processor family products: Supported
<!-- end id9 -->
<!-- npu="kirin2" id10 -->
- KirinDev0002 processor family products: Supported
<!-- end id10 -->
<!-- npu="kirin3" id11 -->
- KirinDev0003 processor family products: Supported
<!-- end id11 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/type_conversion/Cast_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

Header file path: `"basic_api/reg_compute/kernel_reg_compute_vec_vconv_intf.h"`.

Cast is used for data type precision conversion, converting the source operand data type into the destination operand data type. It supports [float-to-integer conversion](#table-6-float-to-integer-conversion), [float-to-float conversion](#table-7-float-to-float-conversion), [integer-to-float conversion](#table-8-integer-to-float-conversion), and [integer-to-integer conversion](#table-9-integer-to-integer-conversion). During the conversion process, due to bit width and precision changes, the following parameters can be configured to implement the functionality:
- [RegLayout](../aux_data_types/RegLayout.md): When the source operand and destination operand have different bit widths, the computation amount of a single instruction is based on the data type with the larger bit width. RegLayout controls the arrangement of elements with smaller bit width in the register.
- [SatMode](../aux_data_types/SatMode.md): Used to set the saturation and non-saturation modes.
- [MaskMergeMode](../aux_data_types/MaskMergeMode.md): Used to specify the register data write mode. Elements not selected by the mask are set to zero in dst or retain the original dst value.
- [RoundMode](../aux_data_types/RoundMode.md): Used to set the rounding mode.

The mask bit width corresponding to an element varies with the data type. During type conversion by Cast, MaskReg filters valid elements based on the input source operand.
Figure [b16-to-b32 type conversion process](#fig1) and Figure [b32-to-b16 type conversion process](#fig2) show the b16 and b32 type conversion process when MaskReg and RegLayout take effect together.

**Figure 1** b16-to-b32 type conversion process<a id="fig1"></a>  

![](../../../../figures/b16_to_b32_conversion.png)

**Figure 2** b32-to-b16 type conversion process<a id="fig2"></a>  

![](../../../../figures/b32_to_b16_conversion.png)

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T = DefaultType, typename U = DefaultType, const CastTrait& trait = castTrait, typename S, typename V>
__simd_callee__ inline void Cast(S& dstReg, V& srcReg, MaskReg& mask);
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the destination operand. For supported data types, see [Data Type](#data-type). |
| U | Data type of the source operand. For supported data types, see [Data Type](#data-type). |
| trait | CastTrait type, a type conversion mode structure. It includes [RegLayout](../aux_data_types/RegLayout.md), [SatMode](../aux_data_types/SatMode.md), [MaskMergeMode](../aux_data_types/MaskMergeMode.md), and [RoundMode](../aux_data_types/RoundMode.md).<br><code>struct CastTrait {<br>    RegLayout layoutMode = RegLayout::UNKNOWN;<br>    SatMode satMode = SatMode::UNKNOWN;<br>    MaskMergeMode mrgMode = MaskMergeMode::UNKNOWN;<br>    RoundMode roundMode = RoundMode::UNKNOWN;<br>};</code><br>To make SatMode take effect, use it together with SetCtrlSpr. For the specific configuration of non-saturation mode and saturation mode, see [Table 5 Saturation mode global or single instruction take effect configuration table](#table-5-saturation-mode-global-or-single-effective-instruction-configuration-table).<br>Note: SetCtrlSpr must be called outside SIMD_VF. |
| S | [RegTensor](../register_data_types/RegTensor.md) type of the destination operand, for example, RegTensor&lt;float&gt;, which is automatically deduced by the compiler and does not need to be specified by the user. |
| V | [RegTensor](../register_data_types/RegTensor.md) type of the source operand, for example, RegTensor&lt;int32_t&gt;, which is automatically deduced by the compiler and does not need to be specified by the user. |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstReg | Output | Destination operand.<br>The type is [RegTensor](../register_data_types/RegTensor.md). |
| srcReg | Input | Source operand.<br>The type is [RegTensor](../register_data_types/RegTensor.md). |
| mask | Input | Valid indication of the source operand element operation. For details, see [MaskReg](../register_data_types/MaskReg.md).<br>Note: mask filters elements based on the input. |

## Data Type

**Table 3**  Data type combinations

| src | dst |
| --- | --- |
| int4x2_t | int16_t, half, bfloat16_t |
| int8_t | int16_t, half, int32_t |
| uint8_t | uint16_t, half, uint32_t |
| fp4x2_e2m1_t | bfloat16_t |
| fp4x2_e1m2_t | bfloat16_t |
| hifloat8_t | half, float |
| fp8_e8m0_t | bfloat16_t |
| fp8_e5m2_t | float |
| fp8_e4m3fn_t | float |
| int16_t | int4x2_t, uint8_t, half, int32_t, uint32_t, float |
| uint16_t | uint8_t, uint32_t |
| half | int4x2_t, int8_t, uint8_t, hifloat8_t, int16_t, bfloat16_t, int32_t, float |
| bfloat16_t | fp4x2_e2m1_t, fp4x2_e1m2_t, fp8_e8m0_t, half, int32_t, float |
| int32_t | uint8_t, int16_t, uint16_t, float, int64_t |
| uint32_t | uint8_t, int16_t, uint16_t |
| float | hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t, int16_t, half, bfloat16_t, int32_t, int64_t |
| int64_t | int32_t, float |

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- When the destination operand bit width is smaller than the source operand, in MaskMergeMode::ZEROING mode, the invalid destination operand elements corresponding to MaskReg and RegLayout are all 0; in MaskMergeMode::MERGING mode, the destination operand elements not selected by the mask retain the original dstReg values.
- This instruction must be used together with the SetCtrlSpr instruction to control the non-saturation mode and saturation mode of Cast by setting register values.
- The rounding mode takes effect only in conversions that may cause precision loss and support the rounding mode. For details, see [Key Features](#key-features).
- The descriptions of non-saturation and saturation modes differ for different type conversions, and the global or single instruction configuration methods also differ. For details, see [Table 4 Non-saturation and Saturation Modes in Different Type Conversion Scenarios](#table-4-non-saturation-mode-and-saturation-mode-in-different-type-conversion-scenarios) and [Table 5 Saturation Mode Global or Single Instruction Configuration Table](#table-5-saturation-mode-global-or-single-effective-instruction-configuration-table).
- float-to-float conversion:
    - When the output type is float32, only non-saturation mode is supported.
    - Non-saturation mode: When the output type is fp8_e4m3fn_t, because fp8_e4m3fn_t has no inf representation format, the output is nan.
    - Saturation mode: When the output type is fp8_e5m2_t/fp8_e4m3fn_t and the input is nan, the default output is 0. If CTRL\[50\] = 1'b1, the output is nan.
    - The fp4x2_e2m1_t/fp4x2_e1m2_t data types have no definitions for inf and nan. For bfloat16-to-float4 type conversion, when the input bfloat16 value is inf or exceeds the type range of fp4x2_e2m1_t/fp4x2_e1m2_t, the corresponding signed fp4x2_e2m1_t/fp4x2_e1m2_t range limit is returned; when the input is nan, fp4x2_e2m1_t/fp4x2_e1m2_t outputs 0.
    - For the fp8_e8m0_t type: When the absolute value of the input bfloat16_t exceeds the maximum value of the fp8_e8m0_t type, the maximum value of fp8_e8m0_t, 0b11111110, is returned; when the input bfloat16_t is +/-inf or nan, the output is fp8_e8m0_t nan = 0b11111111.

- Integer-to-integer conversion

    For a narrow data type such as int16_t\(2Byte\) converted to a wide data type uint32_t\(4Byte\), only saturation mode is supported, and a negative input is saturated to 0.

- In particular, for conversions between int4x2_t/fp4x2_e2m1_t/fp4x2_e1m2_t and b16, the instruction reads and writes every two elements as a pair. When converting from a larger to a smaller type, the valid mask bits are determined by even-numbered bits.

    Figure [fp4x2_e2m1_t-to-bfloat16_t conversion process](#fig3) and Figure [bfloat16_t-to-fp4x2_e2m1_t conversion process](#fig4) show the conversion between fp4x2_e2m1_t and bfloat16_t when MaskReg and RegLayout act simultaneously.

    **Figure 3**  fp4x2_e2m1_t-to-bfloat16_t conversion process<a id="fig3"></a>  
    
    ![](../../../../figures/reg_cast_1.png)

    **Figure 4**  bfloat16_t-to-fp4x2_e2m1_t conversion process<a id="fig4"></a>  
    ![](../../../../figures/reg_cast_2.png)

## Key Features

### Non-saturation Mode and Saturation Mode in Different Type Conversion Scenarios

<a id="table-4-non-saturation-mode-and-saturation-mode-in-different-type-conversion-scenarios"></a>
**Table 4** Non-saturation mode and saturation mode in different type conversion scenarios

| Scenario | Non-saturation mode | Saturation mode |
| --- | --- | --- |
| Float-to-integer conversion | When the input data exceeds the output type range, the result is truncated to the data width of the target format (the least significant bits are retained). For example, if the input half value is 257, the output uint8_t value is 1. If the input is +/-inf, the corresponding extreme value of the output type is returned. If the input is nan, 0 is returned. | When the input data exceeds the output type range, the corresponding extreme value of the output type is returned. For example, if the input half value is 257, the output uint8 value is 255; if the input half value is -inf, the output uint8_t value is 0. If the input is nan, 0 is returned. |
| Float-to-float conversion | If the input data is nan, the output is nan. If the input is +/-inf, the output is +/-inf. | If the input is nan, the output is 0. When the input data exceeds the output type range, the corresponding extreme value of the output type is returned. |
| Integer-to-float conversion | Non-saturation mode is not supported. | When the input data exceeds the output type range, the corresponding extreme value of the output type is returned. This scenario uses saturation mode by default and requires no configuration. |
| Integer-to-integer conversion | The input data is truncated to the target data width. For example, if the input int32_t value is 256, the output uint8_t value is 0. | If the input data exceeds the target data range, it is saturated to the extreme value of the target data type. |

### Non-saturation Mode and Saturation Mode Configuration

<a id="table-5-saturation-mode-global-or-single-effective-instruction-configuration-table"></a>
**Table 5** Saturation mode global or single effective instruction configuration table

| Global enable bit | Control bit | Function Description |
| --- | --- | --- |
| CTRL[60] = 1'b0 | In the trait template parameter of the **Reg** vector calculation **Cast** API, **satMode** is set to **SatMode::NO_SAT**. | Single-instruction non-saturation mode. |
| CTRL[60] = 1'b0 | In the trait template parameter of the **Reg** vector calculation **Cast** API, **satMode** is set to **SatMode::SAT**. | Single-instruction saturation mode. |
| CTRL[60] = 1'b1 | CTRL[48] = 1'b1 | Global non-saturation mode (floating-point calculation and floating-point precision conversion). |
| CTRL[60] = 1'b1 | CTRL[48] = 1'b0 | Global saturation mode (floating-point calculation and floating-point precision conversion). |
| CTRL[60] = 1'b1 | CTRL[59] = 1'b1 | Global non-saturation mode (precision conversion from floating-point to integer or integer to integer). |
| CTRL[60] = 1'b1 | CTRL[59] = 1'b0 | Global saturation mode (precision conversion from floating-point to integer or integer to integer). |

### CastTrait Type Values Supported for Different Type Conversions

<a id="table-6-float-to-integer-conversion"></a>
**Table 6**  Float-to-Integer Conversion

| src dtype | dst dtype | layoutMode | satMode | mrgMode | roundMode |
| --- | --- | --- | --- | --- | --- |
| half | int4x2_t | RegLayout::ZERO, RegLayout::ONE, RegLayout::TWO, RegLayout::THREE | SatMode::NO_SAT, SatMode::SAT | MaskMergeMode::ZEROING, MaskMergeMode::MERGING | RoundMode::CAST_RINT, RoundMode::CAST_ROUND, RoundMode::CAST_FLOOR, RoundMode::CAST_CEIL, RoundMode::CAST_TRUNC |
| half | int8_t | RegLayout::ZERO, RegLayout::ONE | SatMode::NO_SAT, SatMode::SAT | MaskMergeMode::ZEROING, MaskMergeMode::MERGING | RoundMode::CAST_RINT, RoundMode::CAST_ROUND, RoundMode::CAST_FLOOR, RoundMode::CAST_CEIL, RoundMode::CAST_TRUNC |
| half | uint8_t | RegLayout::ZERO, RegLayout::ONE | SatMode::NO_SAT, SatMode::SAT | MaskMergeMode::ZEROING, MaskMergeMode::MERGING | RoundMode::CAST_RINT, RoundMode::CAST_ROUND, RoundMode::CAST_FLOOR, RoundMode::CAST_CEIL, RoundMode::CAST_TRUNC |
| half | int16_t | RegLayout::UNKNOWN | SatMode::NO_SAT, SatMode::SAT | MaskMergeMode::ZEROING, MaskMergeMode::MERGING | RoundMode::CAST_RINT, RoundMode::CAST_ROUND, RoundMode::CAST_FLOOR, RoundMode::CAST_CEIL, RoundMode::CAST_TRUNC |
| half | int32_t | RegLayout::ZERO, RegLayout::ONE | SatMode::UNKNOWN | MaskMergeMode::ZEROING, MaskMergeMode::MERGING | RoundMode::CAST_RINT, RoundMode::CAST_ROUND, RoundMode::CAST_FLOOR, RoundMode::CAST_CEIL, RoundMode::CAST_TRUNC |
| bfloat16_t | int32_t | RegLayout::ZERO, RegLayout::ONE | SatMode::NO_SAT, SatMode::SAT | MaskMergeMode::ZEROING, MaskMergeMode::MERGING | RoundMode::CAST_RINT, RoundMode::CAST_ROUND, RoundMode::CAST_FLOOR, RoundMode::CAST_CEIL, RoundMode::CAST_TRUNC |
| float | int16_t | RegLayout::ZERO, RegLayout::ONE | SatMode::NO_SAT, SatMode::SAT | MaskMergeMode::ZEROING, MaskMergeMode::MERGING | RoundMode::CAST_RINT, RoundMode::CAST_ROUND, RoundMode::CAST_FLOOR, RoundMode::CAST_CEIL, RoundMode::CAST_TRUNC |
| float | int32_t | RegLayout::UNKNOWN | SatMode::NO_SAT, SatMode::SAT | MaskMergeMode::ZEROING, MaskMergeMode::MERGING | RoundMode::CAST_RINT, RoundMode::CAST_ROUND, RoundMode::CAST_FLOOR, RoundMode::CAST_CEIL, RoundMode::CAST_TRUNC |
| float | int64_t | RegLayout::ZERO, RegLayout::ONE | SatMode::NO_SAT, SatMode::SAT | MaskMergeMode::ZEROING, MaskMergeMode::MERGING | RoundMode::CAST_RINT, RoundMode::CAST_ROUND, RoundMode::CAST_FLOOR, RoundMode::CAST_CEIL, RoundMode::CAST_TRUNC |

<a id="table-7-float-to-float-conversion"></a>
**Table 7**  Float-to-Float Conversion

| src dtype | dst dtype | layoutMode | satMode | mrgMode | roundMode |
| --- | --- | --- | --- | --- | --- |
| hifloat8_t | half | RegLayout::ZERO, RegLayout::ONE | SatMode::UNKNOWN | MaskMergeMode::ZEROING, MaskMergeMode::MERGING | RoundMode::UNKNOWN |
| hifloat8_t | float | RegLayout::ZERO, RegLayout::ONE, RegLayout::TWO, RegLayout::THREE | SatMode::UNKNOWN | MaskMergeMode::ZEROING, MaskMergeMode::MERGING | RoundMode::UNKNOWN |
| fp8_e4m3fn_t | float | RegLayout::ZERO, RegLayout::ONE, RegLayout::TWO, RegLayout::THREE | SatMode::UNKNOWN | MaskMergeMode::ZEROING, MaskMergeMode::MERGING | RoundMode::UNKNOWN |
| fp8_e5m2_t | float | RegLayout::ZERO, RegLayout::ONE, RegLayout::TWO, RegLayout::THREE | SatMode::UNKNOWN | MaskMergeMode::ZEROING, MaskMergeMode::MERGING | RoundMode::UNKNOWN |
| fp8_e8m0_t | bfloat16_t | RegLayout::ZERO, RegLayout::ONE | SatMode::UNKNOWN | MaskMergeMode::ZEROING, MaskMergeMode::MERGING | RoundMode::UNKNOWN |
| fp4x2_e2m1_t | bfloat16_t | RegLayout::ZERO, RegLayout::ONE, RegLayout::TWO, RegLayout::THREE | SatMode::UNKNOWN | MaskMergeMode::ZEROING, MaskMergeMode::MERGING | RoundMode::UNKNOWN |
| fp4x2_e1m2_t | bfloat16_t | RegLayout::ZERO, RegLayout::ONE, RegLayout::TWO, RegLayout::THREE | SatMode::UNKNOWN | MaskMergeMode::ZEROING, MaskMergeMode::MERGING | RoundMode::UNKNOWN |
| half | hifloat8_t | RegLayout::ZERO, RegLayout::ONE | SatMode::NO_SAT, SatMode::SAT | MaskMergeMode::ZEROING, MaskMergeMode::MERGING | RoundMode::CAST_ROUND,  RoundMode::CAST_HYBRID |
| half | bfloat16_t | RegLayout::UNKNOWN | SatMode::UNKNOWN | MaskMergeMode::ZEROING, MaskMergeMode::MERGING | RoundMode::CAST_RINT, RoundMode::CAST_ROUND, RoundMode::CAST_FLOOR, RoundMode::CAST_CEIL, RoundMode::CAST_TRUNC |
| bfloat16_t | half | RegLayout::UNKNOWN | SatMode::NO_SAT, SatMode::SAT | MaskMergeMode::ZEROING, MaskMergeMode::MERGING | RoundMode::CAST_RINT, RoundMode::CAST_ROUND, RoundMode::CAST_FLOOR, RoundMode::CAST_CEIL, RoundMode::CAST_TRUNC |
| bfloat16_t | fp4x2_e2m1_t | RegLayout::ZERO, RegLayout::ONE, RegLayout::TWO, RegLayout::THREE | SatMode::UNKNOWN | MaskMergeMode::ZEROING, MaskMergeMode::MERGING | RoundMode::CAST_RINT, RoundMode::CAST_ROUND, RoundMode::CAST_FLOOR, RoundMode::CAST_CEIL, RoundMode::CAST_TRUNC |
| bfloat16_t | fp4x2_e1m2_t | RegLayout::ZERO, RegLayout::ONE, RegLayout::TWO, RegLayout::THREE | SatMode::UNKNOWN | MaskMergeMode::ZEROING, MaskMergeMode::MERGING | RoundMode::CAST_RINT, RoundMode::CAST_ROUND, RoundMode::CAST_FLOOR, RoundMode::CAST_CEIL, RoundMode::CAST_TRUNC |
| bfloat16_t | fp8_e8m0_t | RegLayout::ZERO, RegLayout::ONE | SatMode::UNKNOWN | MaskMergeMode::ZEROING, MaskMergeMode::MERGING | RoundMode::UNKNOWN |
| half | float | RegLayout::ZERO, RegLayout::ONE | SatMode::UNKNOWN | MaskMergeMode::ZEROING, MaskMergeMode::MERGING | RoundMode::UNKNOWN |
| bfloat16_t | float | RegLayout::ZERO, RegLayout::ONE | SatMode::UNKNOWN | MaskMergeMode::ZEROING, MaskMergeMode::MERGING | RoundMode::UNKNOWN |
| float | hifloat8_t | RegLayout::ZERO, RegLayout::ONE, RegLayout::TWO, RegLayout::THREE | SatMode::NO_SAT, SatMode::SAT | MaskMergeMode::ZEROING, MaskMergeMode::MERGING | RoundMode::CAST_ROUND,  RoundMode::CAST_HYBRID |
| float | fp8_e4m3fn_t | RegLayout::ZERO, RegLayout::ONE, RegLayout::TWO, RegLayout::THREE | SatMode::NO_SAT, SatMode::SAT | MaskMergeMode::ZEROING, MaskMergeMode::MERGING | RoundMode::CAST_RINT |
| float | fp8_e5m2_t | RegLayout::ZERO, RegLayout::ONE, RegLayout::TWO, RegLayout::THREE | SatMode::NO_SAT, SatMode::SAT | MaskMergeMode::ZEROING, MaskMergeMode::MERGING | RoundMode::CAST_RINT |
| float | half | RegLayout::ZERO, RegLayout::ONE | SatMode::NO_SAT, SatMode::SAT | MaskMergeMode::ZEROING, MaskMergeMode::MERGING | RoundMode::CAST_ODD, RoundMode::CAST_RINT, RoundMode::CAST_ROUND, RoundMode::CAST_FLOOR, RoundMode::CAST_CEIL, RoundMode::CAST_TRUNC |
| float | bfloat16_t | RegLayout::ZERO, RegLayout::ONE | SatMode::NO_SAT, SatMode::SAT | MaskMergeMode::ZEROING, MaskMergeMode::MERGING | RoundMode::CAST_RINT, RoundMode::CAST_ROUND, RoundMode::CAST_FLOOR, RoundMode::CAST_CEIL, RoundMode::CAST_TRUNC |

<a id="table-8-integer-to-float-conversion"></a>
**Table 8**  Integer-to-Float Conversion

| src dtype | dst dtype | layoutMode | satMode | mrgMode | roundMode |
| --- | --- | --- | --- | --- | --- |
| int4x2_t | half | RegLayout::ZERO, RegLayout::ONE, RegLayout::TWO, RegLayout::THREE | SatMode::UNKNOWN | MaskMergeMode::ZEROING, MaskMergeMode::MERGING | RoundMode::UNKNOWN |
| int4x2_t | bfloat16_t | RegLayout::ZERO, RegLayout::ONE, RegLayout::TWO, RegLayout::THREE | SatMode::UNKNOWN | MaskMergeMode::ZEROING, MaskMergeMode::MERGING | RoundMode::UNKNOWN |
| int8_t | half | RegLayout::ZERO, RegLayout::ONE | SatMode::UNKNOWN | MaskMergeMode::ZEROING, MaskMergeMode::MERGING | RoundMode::UNKNOWN |
| uint8_t | half | RegLayout::ZERO, RegLayout::ONE | SatMode::UNKNOWN | MaskMergeMode::ZEROING, MaskMergeMode::MERGING | RoundMode::UNKNOWN |
| int16_t | half | RegLayout::UNKNOWN | SatMode::UNKNOWN | MaskMergeMode::ZEROING, MaskMergeMode::MERGING | RoundMode::CAST_RINT, RoundMode::CAST_ROUND, RoundMode::CAST_FLOOR, RoundMode::CAST_CEIL, RoundMode::CAST_TRUNC |
| int16_t | float | RegLayout::ZERO, RegLayout::ONE | SatMode::UNKNOWN | MaskMergeMode::ZEROING, MaskMergeMode::MERGING | RoundMode::UNKNOWN |
| int32_t | float | RegLayout::UNKNOWN | SatMode::UNKNOWN | MaskMergeMode::ZEROING, MaskMergeMode::MERGING | RoundMode::CAST_RINT, RoundMode::CAST_ROUND, RoundMode::CAST_FLOOR, RoundMode::CAST_CEIL, RoundMode::CAST_TRUNC |
| int64_t | float | RegLayout::ZERO, RegLayout::ONE | SatMode::UNKNOWN | MaskMergeMode::ZEROING, MaskMergeMode::MERGING | RoundMode::CAST_RINT, RoundMode::CAST_ROUND, RoundMode::CAST_FLOOR, RoundMode::CAST_CEIL, RoundMode::CAST_TRUNC |

<a id="table-9-integer-to-integer-conversion"></a>
**Table 9**  Integer-to-Integer Conversion

| src dtype | dst dtype | layoutMode | satMode | mrgMode | roundMode |
| --- | --- | --- | --- | --- | --- |
| int4x2_t | int16_t | RegLayout::ZERO, RegLayout::ONE, RegLayout::TWO, RegLayout::THREE | SatMode::UNKNOWN | MaskMergeMode::ZEROING, MaskMergeMode::MERGING | RoundMode::UNKNOWN |
| int8_t | int16_t | RegLayout::ZERO, RegLayout::ONE | SatMode::UNKNOWN | MaskMergeMode::ZEROING, MaskMergeMode::MERGING | RoundMode::UNKNOWN |
| int8_t | int32_t | RegLayout::ZERO, RegLayout::ONE, RegLayout::TWO, RegLayout::THREE | SatMode::UNKNOWN | MaskMergeMode::ZEROING, MaskMergeMode::MERGING | RoundMode::UNKNOWN |
| uint8_t | uint16_t | RegLayout::ZERO, RegLayout::ONE | SatMode::UNKNOWN | MaskMergeMode::ZEROING, MaskMergeMode::MERGING | RoundMode::UNKNOWN |
| uint8_t | uint32_t | RegLayout::ZERO, RegLayout::ONE, RegLayout::TWO, RegLayout::THREE | SatMode::UNKNOWN | MaskMergeMode::ZEROING, MaskMergeMode::MERGING | RoundMode::UNKNOWN |
| int16_t | int4x2_t | RegLayout::ZERO, RegLayout::ONE, RegLayout::TWO, RegLayout::THREE | SatMode::NO_SAT, SatMode::SAT | MaskMergeMode::ZEROING, MaskMergeMode::MERGING | RoundMode::UNKNOWN |
| int16_t | uint8_t | RegLayout::ZERO, RegLayout::ONE | SatMode::NO_SAT, SatMode::SAT | MaskMergeMode::ZEROING, MaskMergeMode::MERGING | RoundMode::UNKNOWN |
| int16_t | int32_t | RegLayout::ZERO, RegLayout::ONE | SatMode::UNKNOWN | MaskMergeMode::ZEROING, MaskMergeMode::MERGING | RoundMode::UNKNOWN |
| int16_t | uint32_t | RegLayout::ZERO, RegLayout::ONE | SatMode::UNKNOWN | MaskMergeMode::ZEROING, MaskMergeMode::MERGING | RoundMode::UNKNOWN |
| uint16_t | uint8_t | RegLayout::ZERO, RegLayout::ONE | SatMode::NO_SAT, SatMode::SAT | MaskMergeMode::ZEROING, MaskMergeMode::MERGING | RoundMode::UNKNOWN |
| uint16_t | uint32_t | RegLayout::ZERO, RegLayout::ONE | SatMode::UNKNOWN | MaskMergeMode::ZEROING, MaskMergeMode::MERGING | RoundMode::UNKNOWN |
| int32_t | uint8_t | RegLayout::ZERO, RegLayout::ONE, RegLayout::TWO, RegLayout::THREE | SatMode::NO_SAT, SatMode::SAT | MaskMergeMode::ZEROING, MaskMergeMode::MERGING | RoundMode::UNKNOWN |
| int32_t | int16_t | RegLayout::ZERO, RegLayout::ONE | SatMode::NO_SAT, SatMode::SAT | MaskMergeMode::ZEROING, MaskMergeMode::MERGING | RoundMode::UNKNOWN |
| int32_t | uint16_t | RegLayout::ZERO, RegLayout::ONE | SatMode::NO_SAT, SatMode::SAT | MaskMergeMode::ZEROING, MaskMergeMode::MERGING | RoundMode::UNKNOWN |
| int32_t | int64_t | RegLayout::ZERO, RegLayout::ONE | SatMode::UNKNOWN | MaskMergeMode::ZEROING, MaskMergeMode::MERGING | RoundMode::UNKNOWN |
| uint32_t | uint8_t | RegLayout::ZERO, RegLayout::ONE, RegLayout::TWO, RegLayout::THREE | SatMode::NO_SAT, SatMode::SAT | MaskMergeMode::ZEROING, MaskMergeMode::MERGING | RoundMode::UNKNOWN |
| uint32_t | int16_t | RegLayout::ZERO, RegLayout::ONE | SatMode::NO_SAT, SatMode::SAT | MaskMergeMode::ZEROING, MaskMergeMode::MERGING | RoundMode::UNKNOWN |
| uint32_t | uint16_t | RegLayout::ZERO, RegLayout::ONE | SatMode::NO_SAT, SatMode::SAT | MaskMergeMode::ZEROING, MaskMergeMode::MERGING | RoundMode::UNKNOWN |
| int64_t | int32_t | RegLayout::ZERO, RegLayout::ONE | SatMode::NO_SAT, SatMode::SAT | MaskMergeMode::ZEROING, MaskMergeMode::MERGING | RoundMode::UNKNOWN |

### float-to-hifloat8_t conversion rule

```
tmp2[31 : 0] = f32_src_data[i][31 : 0];
Ev = tmp2[30 : 23] - 8'b01111111, thr = tmp2[13 : 0];
if (Ev == 128 && tmp2[22 : 0] != 23'b0) then
    tmp3[7 : 0] = HiF8NAN(8'b10000000);
else if ((Ev == 128 && tmp2[22 : 0] == 23'b0) || (Ev > 15)) then
    tmp3[7 : 0] = (tmp2[31] == 1'b0) ? HiF8+inf(8'b01101111) : HiF8-inf(8'b11101111);
else if (Ev < -23) then
    tmp3[7 : 0] = HiF8ZERO (8'b00000000);
else if (Ev == -23) then
    if ((Half To Away Round) || (Hybrid Round && {1'b1, tmp2[22 : 10]} >= thr)) then
        tmp3[7 : 0] = (tmp2[31] == 1'b0) ? 8'b00000001 : 8'b10000001; // min subnormal
    else
        tmp3[7 : 0] = HiF8ZERO(8'b00000000);
    end if
else
    if (Ev == 0) then
        M = tmp2[22 : 20], T A_bit = tmp2[19], frac = tmp2[19 : 6];
    else if (Ev == ±1) then
        M = tmp2[22 : 20], T A_bit = tmp2[19], frac = tmp2[19 : 6];
    else if (Ev == ±[2, 3]) then
        M = tmp2[22 : 20], T A_bit = tmp2[19], frac = tmp2[19:6];
    else if (Ev == ±[4, 7]) then
        M = tmp2[22 : 21], T A_bit = tmp2[20], frac = tmp2[20 : 7];
    else if (Ev == ±[8, 15]) then
        M = tmp2[22], T A_bit = tmp2[21], frac = tmp2[21 : 8];
    else if (Ev == [-16, -22]) then
        M = Ev + 23, T A_bit = tmp2[22], frac = tmp2[22 : 9]; // subnormal
    end if
    if (Ev == ±[0, 3]) then
        if (T A_bit == 1'b1) then
            M_tmp = M + 1, Ev = Ev + carry of M_tmp, M = (carry of M_tmp) ? 0 : M_tmp;
        end if
    else if (Ev == ±[4, 15]) then
        if (HALF To Away Round && T A_bit == 1'b1) || (Hybrid Round && frac >= thr) then
            M_tmp = M + 1, Ev = Ev + carry of M_tmp, M = (carry of M_tmp) ? 0 : M_tmp;
        end if
    else if (Ev == [-16, -22]) then
        if ((Half To Away Round) && T A_bit == 1'b1) || (Hybrid Round && frac >= thr) then
            M_tmp = Ev + 23, Ev = Ev + 1, M = (Ev == -15) ? 0 : M_tmp;
        end if
    end if
    encode {tmp3[31], Ev, M} to tmp3[7 : 0] following HiF encoding rule;
end if
result[i][7 :0] = saturation(tmp[7 : 0]) according to control bit;
```

### half to hifloat8_t Conversion Rule

```
tmp2[15 : 0] = f16_src_data[i][15 : 0];
Ev = tmp2[14 : 10] - 5'b01111, thr = {tmp2[0], 1'b1};
if (Ev == 16 && tmp2[9 : 0] != 10'b0) then
    tmp3[7 : 0] = HiF8NAN(8'b10000000);
else if (Ev == 16 && tmp2[9:0] == 10'b0) && (Ev > 15) then
    tmp3[7 : 0] = (tmp2[31] == 1'b0) ? HiF8+inf(8'b01101111) : HiF8-inf(8'b11101111);
else if (Ev < -23) then
    tmp3[7 : 0] = HiF8ZERO(8'b00000000);
else if (Ev == -23) then
    if ((Half To Away Round) || (Hybrid Round && {1'b1, tmp2[9]} >= thr)) then
        tmp3[7 : 0] = (tmp2[31] == 1'b0) ? 8'b00000001 : 8'b10000001; // min subnormal
    else
        tmp3[7 : 0] = HiF8ZERO(8'b00000000);
    end if
else
    if (Ev == 0) then
        M = tmp2[9 : 7], T A_bit = tmp2[6], frac = tmp2[6 : 5];
    else if (Ev == ±1) then
        M = tmp2[9 : 7], T A_bit = tmp2[6], frac = tmp2[6 : 5];
    else if (Ev == ±[2, 3]) then
        M = tmp2[9 : 7], T A_bit = tmp2[6], frac = tmp2[6 : 5];
    else if (Ev == ±[4, 7]) then
        M = tmp2[9 : 8], T A_bit = tmp2[7], frac = tmp2[7 : 6];
    else if (Ev == ±[8, 15]) then
        M = tmp2[9], T A_bit = tmp2[8], frac = tmp2[8 : 7];
    else if (Ev == [-16, -22]) then
        M = Ev + 23, T A_bit = tmp2[9], frac = tmp2[9 : 8]; // subnormal
    end if
    if (Ev == ±[0, 3]) then
        if (T A_bit == 1'b1) then
            M_tmp = M + 1, Ev = Ev + carry of M_tmp, M = (carry of M_tmp) ? 0 : M_tmp;
        end if
    else if (Ev == ±[4, 15]) then
        if (HALF To Away Round && T A_bit == 1'b1) || (Hybrid Round && frac >= thr) then
            M_tmp = M + 1, Ev = Ev + carry of M_tmp, M = (carry of M_tmp) ? 0 : M_tmp;
        end if
    else if (Ev == [-16, -22]) then
        if ((Half To Away Round) && T A_bit == 1'b1) || (Hybrid Round && frac >= thr) then
            M_tmp = Ev + 23, Ev = Ev + 1, M = (Ev == -15) ? 0 : M_tmp;
        end if
    end if
    encode {tmp3[31], Ev, M} to tmp3[7 : 0] following HiF encoding rule;
end if
result[i][7 : 0] = saturation(tmp3[7 : 0]) according to control bit; 
```

## Example<a name="section642mcpsimp"></a>

-   Scenario 1: Converting from a smaller bit width to a larger bit width, using half to int32_t as an example.

    ```cpp
    __simd_vf__ inline void CastVFF162S32(__ubuf__ half* xAddr, __ubuf__ int32_t* yAddr,
        uint32_t repeatTimes, uint16_t oneRepeatSize)
    {
        AscendC::Reg::MaskReg mask = AscendC::Reg::CreateMask<int32_t, AscendC::Reg::MaskPattern::ALL>();
        AscendC::Reg::RegTensor<half> xReg;
        AscendC::Reg::RegTensor<int32_t> yReg;
        static constexpr AscendC::Reg::CastTrait castTrait = {
            AscendC::Reg::RegLayout::ZERO,
            AscendC::Reg::SatMode::UNKNOWN,
            AscendC::Reg::MaskMergeMode::ZEROING,
            AscendC::RoundMode::CAST_FLOOR
        };
        for (uint16_t i = 0; i < repeatTimes; ++i) {
            AscendC::Reg::LoadAlign<half, AscendC::Reg::PostLiteral::POST_MODE_UPDATE,
                AscendC::Reg::LoadDist::DIST_UNPACK_B16>(xReg, xAddr, oneRepeatSize);
            AscendC::Reg::Cast<int32_t, half, castTrait>(yReg, xReg, mask);
            AscendC::Reg::StoreAlign<int32_t, AscendC::Reg::PostLiteral::POST_MODE_UPDATE>(
                yAddr, yReg, oneRepeatSize, mask);
        }
    }
    ```

-   Scenario 2: Converting from a larger bit width to a smaller bit width, using float to int16_t as an example.

    ```cpp
    __simd_vf__ inline void CastVFF322S16(__ubuf__ float* xAddr, __ubuf__ int16_t* yAddr,
        uint32_t repeatTimes, uint16_t oneRepeatSize)
    {
        AscendC::Reg::MaskReg mask = AscendC::Reg::CreateMask<float, AscendC::Reg::MaskPattern::ALL>();
        AscendC::Reg::RegTensor<float> xReg;
        AscendC::Reg::RegTensor<int16_t> yReg;
        static constexpr AscendC::Reg::CastTrait castTrait = {
            AscendC::Reg::RegLayout::ZERO,
            AscendC::Reg::SatMode::SAT,
            AscendC::Reg::MaskMergeMode::ZEROING,
            AscendC::RoundMode::CAST_ROUND
        };
        for (uint16_t i = 0; i < repeatTimes; ++i) {
            AscendC::Reg::LoadAlign<float, AscendC::Reg::PostLiteral::POST_MODE_UPDATE>(xReg, xAddr, oneRepeatSize);
            AscendC::Reg::Cast<int16_t, float, castTrait>(yReg, xReg, mask);
            AscendC::Reg::StoreAlign<int16_t, AscendC::Reg::PostLiteral::POST_MODE_UPDATE,
                AscendC::Reg::StoreDist::DIST_PACK_B32>(yAddr, yReg, oneRepeatSize, mask);
        }
    }
    ```
