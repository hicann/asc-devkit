# asc_float2bfloat16

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-04T09:37:31.581Z pushedAt=2026-09-05T12:14:28.822Z -->

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

Converts the **vector_float** type to the **vector_bfloat16_t** type, writes to the upper half or lower half of the destination operand, and supports multiple rounding modes and saturation modes:

- RINT rounding mode: round to nearest, ties to even.

- ROUND rounding mode: round half up.

- FLOOR rounding mode: round toward negative infinity.

- CEIL rounding mode: round toward positive infinity.

- TRUNC rounding mode: round toward zero.

Take the **asc_float2bfloat16_rd** API as an example:

- **asc_float2bfloat16_rd**: non-saturation mode, writes to the upper half of the destination operand.

- **asc_float2bfloat16_rd_sat**: saturation mode, writes to the upper half of the destination operand.

- **asc_float2bfloat16_rd_v2**: non-saturation mode, writes to the lower half of the destination operand.

- **asc_float2bfloat16_rd_sat_v2**: saturation mode, writes to the lower half of the destination operand.

## Prototype

```cpp
// FLOOR rounding mode
__simd_callee__ inline void asc_float2bfloat16_rd(vector_bfloat16_t& dst, vector_float src, vector_bool mask)
__simd_callee__ inline void asc_float2bfloat16_rd_sat(vector_bfloat16_t& dst, vector_float src, vector_bool mask)
__simd_callee__ inline void asc_float2bfloat16_rd_v2(vector_bfloat16_t& dst, vector_float src, vector_bool mask)
__simd_callee__ inline void asc_float2bfloat16_rd_sat_v2(vector_bfloat16_t& dst, vector_float src, vector_bool mask)
// RINT rounding mode
__simd_callee__ inline void asc_float2bfloat16_rn(vector_bfloat16_t& dst, vector_float src, vector_bool mask)
__simd_callee__ inline void asc_float2bfloat16_rn_sat(vector_bfloat16_t& dst, vector_float src, vector_bool mask)
__simd_callee__ inline void asc_float2bfloat16_rn_v2(vector_bfloat16_t& dst, vector_float src, vector_bool mask)
__simd_callee__ inline void asc_float2bfloat16_rn_sat_v2(vector_bfloat16_t& dst, vector_float src, vector_bool mask)
// ROUND rounding mode
__simd_callee__ inline void asc_float2bfloat16_rna(vector_bfloat16_t& dst, vector_float src, vector_bool mask)
__simd_callee__ inline void asc_float2bfloat16_rna_sat(vector_bfloat16_t& dst, vector_float src, vector_bool mask)
__simd_callee__ inline void asc_float2bfloat16_rna_v2(vector_bfloat16_t& dst, vector_float src, vector_bool mask)
__simd_callee__ inline void asc_float2bfloat16_rna_sat_v2(vector_bfloat16_t& dst, vector_float src, vector_bool mask)
// CEIL rounding mode
__simd_callee__ inline void asc_float2bfloat16_ru(vector_bfloat16_t& dst, vector_float src, vector_bool mask)
__simd_callee__ inline void asc_float2bfloat16_ru_sat(vector_bfloat16_t& dst, vector_float src, vector_bool mask)
__simd_callee__ inline void asc_float2bfloat16_ru_v2(vector_bfloat16_t& dst, vector_float src, vector_bool mask)
__simd_callee__ inline void asc_float2bfloat16_ru_sat_v2(vector_bfloat16_t& dst, vector_float src, vector_bool mask)
// TRUNC rounding mode
__simd_callee__ inline void asc_float2bfloat16_rz(vector_bfloat16_t& dst, vector_float src, vector_bool mask)
__simd_callee__ inline void asc_float2bfloat16_rz_sat(vector_bfloat16_t& dst, vector_float src, vector_bool mask)
__simd_callee__ inline void asc_float2bfloat16_rz_v2(vector_bfloat16_t& dst, vector_float src, vector_bool mask)
__simd_callee__ inline void asc_float2bfloat16_rz_sat_v2(vector_bfloat16_t& dst, vector_float src, vector_bool mask)
```

## Parameters

| Parameter       | Input/Output | Description               |
| --------- | ----- | ---------------- |
| dst       | Output    | Destination operand (vector data register).            |
| src | Input    | Source operand (vector data register).            |
| mask     | Input    | Source operand mask (mask register), which indicates which elements participate in the computation. A value of 1 at the corresponding position means the element participates in the computation, and 0 means it does not. Elements not selected by the mask are set to zero in the output.        |

For details about vector data registers and mask registers, see [reg Data Definition](../reg_data_definition.md).

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- When enabling saturation mode and non-saturation mode, the ctrl register must be configured. For details about the ctrl register, see [asc_set_ctrl.md](../../sys_var/asc_set_ctrl.md).

## Example

```cpp
vector_bfloat16_t dst;
vector_float src;
vector_bool mask;
mask = asc_create_mask_b32(PAT_ALL);
asc_float2bfloat16_rd(dst, src, mask);    // FLOOR rounding mode, non-saturation mode, converts src to the vector_bfloat16_t type and writes it to the upper half of dst.
asc_float2bfloat16_rd_sat(dst, src, mask);    // Use FLOOR rounding mode and saturation mode to convert src to the vector_bfloat16_t type and write the result to the upper half of dst.
asc_float2bfloat16_rd_v2(dst, src, mask);    // Use FLOOR rounding mode and non-saturation mode to convert src to the vector_bfloat16_t type and write the result to the lower half of dst.
asc_float2bfloat16_rd_sat_v2(dst, src, mask);    // Use FLOOR rounding mode and saturation mode to convert src to the vector_bfloat16_t type and write the result to the lower half of dst.
```