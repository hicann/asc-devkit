# asc_half2int16

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-04T10:01:01.689Z pushedAt=2026-09-05T12:14:28.847Z -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->

## Description

Converts **half** data to **int16_t** and supports multiple rounding modes.

- **RINT** rounding mode: round to nearest, ties to even.
- **ROUND** rounding mode: round half up.
- **FLOOR** rounding mode: round toward negative infinity.
- **CEIL** rounding mode: round toward positive infinity.
- **TRUNC** rounding mode: round toward zero.

Based on the magnitude of the input data, the conversion is divided into non-saturation mode and saturation mode.

- Non-saturation mode:
  - When the input data exceeds the range of the output type, the result is truncated to the data width of the target format.
  - When the input data is +/-inf, the maximum or minimum value of the output type is returned.
  - When the input data is nan, 0 is returned.
- Saturation mode:
  - When the input data exceeds the range of the output type, the corresponding maximum or minimum value of the output type is returned.
  - When the input data is +/-inf, the maximum or minimum value of the output type is returned.
  - When the input data is nan, 0 is returned.

## Prototype

```cpp
// RINT rounding mode, non-saturation mode.
__simd_callee__ inline void asc_half2int16_rn(vector_int16_t& dst, vector_half src, vector_bool mask)
// RINT rounding mode, saturation mode.
__simd_callee__ inline void asc_half2int16_rn_sat(vector_int16_t& dst, vector_half src, vector_bool mask)
// ROUND rounding mode, non-saturation mode.
__simd_callee__ inline void asc_half2int16_rna(vector_int16_t& dst, vector_half src, vector_bool mask)
// ROUND rounding mode, saturation mode.
__simd_callee__ inline void asc_half2int16_rna_sat(vector_int16_t& dst, vector_half src, vector_bool mask)
// FLOOR rounding mode, non-saturation mode.
__simd_callee__ inline void asc_half2int16_rd(vector_int16_t& dst, vector_half src, vector_bool mask)
// FLOOR rounding mode, saturation mode.
__simd_callee__ inline void asc_half2int16_rd_sat(vector_int16_t& dst, vector_half src, vector_bool mask)
// CEIL rounding mode, non-saturation mode.
__simd_callee__ inline void asc_half2int16_ru(vector_int16_t& dst, vector_half src, vector_bool mask)
// CEIL rounding mode, saturation mode.
__simd_callee__ inline void asc_half2int16_ru_sat(vector_int16_t& dst, vector_half src, vector_bool mask)
// TRUNC rounding mode, non-saturation mode.
__simd_callee__ inline void asc_half2int16_rz(vector_int16_t& dst, vector_half src, vector_bool mask)
// TRUNC rounding mode, saturation mode.
__simd_callee__ inline void asc_half2int16_rz_sat(vector_int16_t& dst, vector_half src, vector_bool mask)
```

## Parameters

|Parameter|Input/Output|Description|
| ------------ | ------------ | ------------ |
| dst        | Output    | Destination operand (vector data register). |
| src        | Input    | Source operand (vector data register). |
| mask       | Input    | Source operand mask (mask register), which indicates which elements participate in the computation. A value of 1 at the corresponding position means the element participates in the computation, and 0 means it does not. Elements not selected by the mask are set to zero in the output. |

For details about vector data registers and mask registers, see [reg Data Definition](../reg_data_definition.md).

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- When enabling saturation mode and non-saturation mode, configure the ctrl register. For details about the ctrl register, see [asc_set_ctrl.md](../../sys_var/asc_set_ctrl.md).

## Example

```cpp
vector_half src;
vector_int16_t dst;
vector_bool mask = asc_create_mask_b16(PAT_ALL);
asc_loadalign(src, src_addr); // src_addr is the UB memory space address input from outside.
asc_half2int16_rn(dst, src, mask);
```
