# asc_float2half

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-04T09:41:46.280Z pushedAt=2026-09-05T12:14:28.826Z -->

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

Converts the float type to the half type and supports multiple rounding modes.

- **RINT** rounding mode: round half to even.
- **ROUND** rounding mode: round half up.
- **FLOOR** rounding mode: round toward negative infinity.
- **CEIL** rounding mode: round toward positive infinity.
- **TRUNC** rounding mode: round toward zero.
- **ODD** rounding mode: round to nearest odd.

Based on the magnitude of the input data, the conversion is divided into non-saturation mode and saturation mode.

- Non-saturation mode:
  - When the input data exceeds the range of the output type, the corresponding signed inf value of the output type is returned.
  - When the input data is +/-inf, +/-inf is returned.
  - When the input data is nan, nan is returned.
- Saturation mode:
  - When the input data exceeds the range of the output type, the corresponding maximum value of the output type is returned.
  - When the input data is +/-inf, the corresponding extreme value of the output type is returned.
  - When the input data is nan, 0 is returned.

Because the bit-width ratio between the source operand and the destination operand is 2:1, when writing data, one VL-sized data block must be divided into two parts, and index 0 or index 1 is selected depending on the API.

## Prototype

```cpp
// RINT rounding mode, non-saturation mode, data written to even-indexed positions
__simd_callee__ inline void asc_float2half_rn(vector_half& dst, vector_float src, vector_bool mask)
// RINT rounding mode, saturation mode, data written to even-indexed positions
__simd_callee__ inline void asc_float2half_rn_sat(vector_half& dst, vector_float src, vector_bool mask)
// RINT rounding mode, non-saturation mode, data written to odd-indexed positions.
__simd_callee__ inline void asc_float2half_rn_v2(vector_half& dst, vector_float src, vector_bool mask)
// RINT rounding mode, saturation mode, data written to odd-indexed positions.
__simd_callee__ inline void asc_float2half_rn_sat_v2(vector_half& dst, vector_float src, vector_bool mask)
// ROUND rounding mode, non-saturation mode, data written to even-indexed positions.
__simd_callee__ inline void asc_float2half_rna(vector_half& dst, vector_float src, vector_bool mask)
// ROUND rounding mode, saturation mode, data written to even-indexed positions.
__simd_callee__ inline void asc_float2half_rna_sat(vector_half& dst, vector_float src, vector_bool mask)
// ROUND rounding mode, non-saturation mode, data written to odd-indexed positions.
__simd_callee__ inline void asc_float2half_rna_v2(vector_half& dst, vector_float src, vector_bool mask)
// ROUND rounding mode, saturation mode, data written to odd-indexed positions.
__simd_callee__ inline void asc_float2half_rna_sat_v2(vector_half& dst, vector_float src, vector_bool mask)
// FLOOR rounding mode, non-saturation mode, data written to even-indexed positions.
__simd_callee__ inline void asc_float2half_rd(vector_half& dst, vector_float src, vector_bool mask)
// FLOOR rounding mode, saturation mode, data written to even-indexed positions.
__simd_callee__ inline void asc_float2half_rd_sat(vector_half& dst, vector_float src, vector_bool mask)
// FLOOR rounding mode, non-saturation mode, data written to odd-indexed positions.
__simd_callee__ inline void asc_float2half_rd_v2(vector_half& dst, vector_float src, vector_bool mask)
// FLOOR rounding mode, saturation mode, data written to odd-indexed positions.
__simd_callee__ inline void asc_float2half_rd_sat_v2(vector_half& dst, vector_float src, vector_bool mask)
// CEIL rounding mode, non-saturation mode, data written to even-indexed positions.
__simd_callee__ inline void asc_float2half_ru(vector_half& dst, vector_float src, vector_bool mask)
// CEIL rounding mode, saturation mode, data written to even-indexed positions.
__simd_callee__ inline void asc_float2half_ru_sat(vector_half& dst, vector_float src, vector_bool mask)
// CEIL rounding mode, non-saturation mode, data written to odd-indexed positions.
__simd_callee__ inline void asc_float2half_ru_v2(vector_half& dst, vector_float src, vector_bool mask)
// CEIL rounding mode, saturation mode, data written to odd-indexed positions.
__simd_callee__ inline void asc_float2half_ru_sat_v2(vector_half& dst, vector_float src, vector_bool mask)
// TRUNC rounding mode, non-saturation mode, data written to even-indexed positions.
__simd_callee__ inline void asc_float2half_rz(vector_half& dst, vector_float src, vector_bool mask)
// TRUNC rounding mode, saturation mode, data written to even-indexed positions.
__simd_callee__ inline void asc_float2half_rz_sat(vector_half& dst, vector_float src, vector_bool mask)
// TRUNC rounding mode, non-saturation mode, data written to odd-indexed positions.
__simd_callee__ inline void asc_float2half_rz_v2(vector_half& dst, vector_float src, vector_bool mask)
// TRUNC rounding mode, saturation mode, data written to odd-indexed positions.
__simd_callee__ inline void asc_float2half_rz_sat_v2(vector_half& dst, vector_float src, vector_bool mask)
// ODD rounding mode, non-saturation mode, data written to even-indexed positions.
__simd_callee__ inline void asc_float2half_ro(vector_half& dst, vector_float src, vector_bool mask)
// ODD rounding mode, saturation mode, data written to even-indexed positions.
__simd_callee__ inline void asc_float2half_ro_sat(vector_half& dst, vector_float src, vector_bool mask)
// ODD rounding mode, non-saturation mode, data written to odd-indexed positions.
__simd_callee__ inline void asc_float2half_ro_v2(vector_half& dst, vector_float src, vector_bool mask)
// ODD rounding mode, saturation mode, data written to odd-indexed positions.
__simd_callee__ inline void asc_float2half_ro_sat_v2(vector_half& dst, vector_float src, vector_bool mask)
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
vector_float src;
vector_half dst;
vector_bool mask = asc_create_mask_b32(PAT_ALL);
asc_loadalign(src, src_addr); // src_addr is the externally input UB memory space address.
asc_float2half_rn(dst, src, mask);
```
