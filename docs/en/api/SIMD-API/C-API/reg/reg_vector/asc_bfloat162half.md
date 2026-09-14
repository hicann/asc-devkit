# asc_bfloat162half

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-04T09:15:07.995Z pushedAt=2026-09-05T12:14:28.792Z -->

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

Converts the **bfloat16_t** type to the **half** type, and supports multiple rounding modes as well as saturation and non-saturation modes.

- **RINT** rounding mode: round to nearest, ties to even.
- **ROUND** rounding mode: round half up.
- **FLOOR** rounding mode: round toward negative infinity.
- **CEIL** rounding mode: round toward positive infinity.
- **TRUNC** rounding mode: round toward zero.

Based on the magnitude of the input data, the operation is divided into a non-saturation mode and a saturation mode.

- Non-saturation mode:
  - When the input data exceeds the range of the output type, the corresponding signed inf value of the output type is returned.
  - When the input data is +/-inf, +/-inf is returned.
  - When the input data is nan, nan is returned.
- Saturation mode:
  - When the input data exceeds the range of the output type, the corresponding extreme value of the output type is returned.
  - When the input data is +/-inf, the corresponding extreme value of the output type is returned.
  - When the input data is nan, 0 is returned.

## Prototype

```cpp
// RINT rounding mode, non-saturation mode.
__simd_callee__ inline void asc_bfloat162half_rn(vector_half& dst, vector_bfloat16_t src, vector_bool mask)
// RINT rounding mode, saturation mode.
__simd_callee__ inline void asc_bfloat162half_rn_sat(vector_half& dst, vector_bfloat16_t src, vector_bool mask)
// ROUND rounding mode, non-saturation mode.
__simd_callee__ inline void asc_bfloat162half_rna(vector_half& dst, vector_bfloat16_t src, vector_bool mask)
// ROUND rounding mode, saturation mode.
__simd_callee__ inline void asc_bfloat162half_rna_sat(vector_half& dst, vector_bfloat16_t src, vector_bool mask)
// FLOOR rounding mode, non-saturation mode.
__simd_callee__ inline void asc_bfloat162half_rd(vector_half& dst, vector_bfloat16_t src, vector_bool mask)
// FLOOR rounding mode, saturation mode.
__simd_callee__ inline void asc_bfloat162half_rd_sat(vector_half& dst, vector_bfloat16_t src, vector_bool mask)
// CEIL rounding mode, non-saturation mode.
__simd_callee__ inline void asc_bfloat162half_ru(vector_half& dst, vector_bfloat16_t src, vector_bool mask)
// CEIL rounding mode, saturation mode.
__simd_callee__ inline void asc_bfloat162half_ru_sat(vector_half& dst, vector_bfloat16_t src, vector_bool mask)
// TRUNC rounding mode, non-saturation mode.
__simd_callee__ inline void asc_bfloat162half_rz(vector_half& dst, vector_bfloat16_t src, vector_bool mask)
// TRUNC rounding mode, saturation mode.
__simd_callee__ inline void asc_bfloat162half_rz_sat(vector_half& dst, vector_bfloat16_t src, vector_bool mask)
```

## Parameters

| Parameter    | Input/Output | Description                |
| :------ | :----- | :----------------- |
| dst    | Output    | Destination operand (vector data register).            |
| src    | Input    | Source operand (vector data register).             |
| mask | Input | Source operand mask (mask register), which indicates which elements participate in the computation. A value of 1 at the corresponding position means the element participates in the computation, and 0 means it does not. Elements not selected by the mask are set to zero in the output. |

For details about vector data registers and mask registers, see [reg Data Definition](../reg_data_definition.md).

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

None

## Example

```cpp
vector_bfloat16_t src;
vector_half dst;
vector_bool mask = asc_create_mask_b16(PAT_ALL);
asc_loadalign(src, src_addr); // src_addr is the UB memory space address input from outside.
asc_bfloat162half_rn(dst, src, mask);
```
