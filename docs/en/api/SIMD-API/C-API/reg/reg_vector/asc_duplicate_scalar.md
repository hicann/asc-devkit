# asc_duplicate_scalar

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-04T09:25:46.029Z pushedAt=2026-09-05T12:14:28.807Z -->

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

Fills **dst** with **value** according to **mask**.

The calculation formula is as follows:

$$
dst_i = value
$$

## Prototype

```cpp
__simd_callee__ inline void asc_duplicate_scalar(vector_uint8_t& dst, uint8_t value, vector_bool mask)
__simd_callee__ inline void asc_duplicate_scalar(vector_int8_t& dst, int8_t value, vector_bool mask)
__simd_callee__ inline void asc_duplicate_scalar(vector_fp8_e4m3fn_t& dst, fp8_e4m3fn_t value, vector_bool mask)
__simd_callee__ inline void asc_duplicate_scalar(vector_fp8_e5m2_t& dst, fp8_e5m2_t value, vector_bool mask)
__simd_callee__ inline void asc_duplicate_scalar(vector_uint16_t& dst, uint16_t value, vector_bool mask)
__simd_callee__ inline void asc_duplicate_scalar(vector_int16_t& dst, int16_t value, vector_bool mask)
__simd_callee__ inline void asc_duplicate_scalar(vector_half& dst, half value, vector_bool mask)
__simd_callee__ inline void asc_duplicate_scalar(vector_bfloat16_t& dst, bfloat16_t value, vector_bool mask)
__simd_callee__ inline void asc_duplicate_scalar(vector_uint32_t& dst, uint32_t value, vector_bool mask)
__simd_callee__ inline void asc_duplicate_scalar(vector_int32_t& dst, int32_t value, vector_bool mask)
__simd_callee__ inline void asc_duplicate_scalar(vector_float& dst, float value, vector_bool mask)
__simd_callee__ inline void asc_duplicate_scalar(vector_uint8_t& dst, uint8_t value)
__simd_callee__ inline void asc_duplicate_scalar(vector_int8_t& dst, int8_t value)
__simd_callee__ inline void asc_duplicate_scalar(vector_uint16_t& dst, uint16_t value)
__simd_callee__ inline void asc_duplicate_scalar(vector_int16_t& dst, int16_t value)
__simd_callee__ inline void asc_duplicate_scalar(vector_uint32_t& dst, uint32_t value)
__simd_callee__ inline void asc_duplicate_scalar(vector_int32_t& dst, int32_t value)
__simd_callee__ inline void asc_duplicate_scalar(vector_half& dst, half value)
__simd_callee__ inline void asc_duplicate_scalar(vector_float& dst, float value)
__simd_callee__ inline void asc_duplicate_scalar(vector_bfloat16_t& dst, bfloat16_t value)
__simd_callee__ inline void asc_duplicate_scalar(vector_fp8_e4m3fn_t& dst, fp8_e4m3fn_t value)
__simd_callee__ inline void asc_duplicate_scalar(vector_fp8_e5m2_t& dst, fp8_e5m2_t value)
__simd_callee__ inline void asc_duplicate_scalar(vector_fp8_e8m0_t& dst, fp8_e8m0_t value)
```

## Parameters

| Parameter  | Input/Output | Description |
| :----- | :------- | :------- |
| dst | Output | Destination operand (vector data register). |
| value | Input | Source operand (scalar). |
| mask | Input | Source operand mask (mask register). Indicates which elements participate in the calculation. A value of 1 at the corresponding position means the element participates in the calculation, and 0 means it does not. Elements not selected by **mask** are set to zero in the output. |

For details about vector data registers and mask registers, see [reg Data Definition](../reg_data_definition.md).

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

None

## Example

```cpp
vector_half dst;
half value = 1.0;
vector_bool mask = asc_create_mask_b16(PAT_ALL);
asc_duplicate_scalar(dst, value, mask);
```