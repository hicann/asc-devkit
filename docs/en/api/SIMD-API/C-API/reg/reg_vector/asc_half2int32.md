# asc_half2int32

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-04T10:02:12.082Z pushedAt=2026-09-05T12:14:28.848Z -->

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

Divides the source operand of the **vector_half** type into two parts in units of 256B, reads the elements of one part, converts them into the **vector_int32_t** type, and writes them to the destination operand. It supports multiple rounding modes:

- RINT rounding mode: round to nearest, ties to even.

- ROUND rounding mode: round half up.

- FLOOR rounding mode: round toward negative infinity.

- CEIL rounding mode: round toward positive infinity.

- TRUNC rounding mode: round toward zero.

Take the **asc_half2int32_rd** API as an example:

- **asc_half2int32_rd**: Writes the upper half of the source operand to the destination operand.

- **asc_half2int32_rd_v2**: Writes the lower half of the source operand to the destination operand.

## Prototype

```cpp
// FLOOR rounding mode.
__simd_callee__ inline void asc_half2int32_rd(vector_int32_t& dst, vector_half src, vector_bool mask)
__simd_callee__ inline void asc_half2int32_rd_v2(vector_int32_t& dst, vector_half src, vector_bool mask)
// RINT rounding mode.
__simd_callee__ inline void asc_half2int32_rn(vector_int32_t& dst, vector_half src, vector_bool mask)
__simd_callee__ inline void asc_half2int32_rn_v2(vector_int32_t& dst, vector_half src, vector_bool mask)
// ROUND rounding mode.
__simd_callee__ inline void asc_half2int32_rna(vector_int32_t& dst, vector_half src, vector_bool mask)
__simd_callee__ inline void asc_half2int32_rna_v2(vector_int32_t& dst, vector_half src, vector_bool mask)
// CEIL rounding mode.
__simd_callee__ inline void asc_half2int32_ru(vector_int32_t& dst, vector_half src, vector_bool mask)
__simd_callee__ inline void asc_half2int32_ru_v2(vector_int32_t& dst, vector_half src, vector_bool mask)
// TRUNC rounding mode.
__simd_callee__ inline void asc_half2int32_rz(vector_int32_t& dst, vector_half src, vector_bool mask)
__simd_callee__ inline void asc_half2int32_rz_v2(vector_int32_t& dst, vector_half src, vector_bool mask)
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

- When saturation mode and non-saturation mode are enabled, the ctrl register must be configured. For details about the ctrl register, see [asc_set_ctrl.md](../../sys_var/asc_set_ctrl.md).

## Example

```cpp
vector_int32_t dst;
vector_half src;
vector_bool mask;
mask = asc_create_mask_b16(PAT_ALL);
asc_half2int32_rd(dst, src, mask);    // FLOOR rounding mode. Convert the upper half of src to the vector_int32_t type and write it to dst.
asc_half2int32_rd_v2(dst, src, mask);    // FLOOR rounding mode. Convert the lower half of src to the vector_int32_t type and write it to dst.
```