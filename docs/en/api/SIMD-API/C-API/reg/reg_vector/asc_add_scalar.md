# asc_add_scalar

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-04T09:05:20.735Z pushedAt=2026-09-05T12:14:28.780Z -->

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

Performs addition of a vector and a scalar. The calculation formula is as follows:

$$
dst_i = src_i + value
$$

## Prototype

```cpp
__simd_callee__ inline void asc_add_scalar(vector_int8_t& dst, vector_int8_t src, int8_t value, vector_bool mask)
__simd_callee__ inline void asc_add_scalar(vector_uint8_t& dst, vector_uint8_t src, uint8_t value, vector_bool mask)
__simd_callee__ inline void asc_add_scalar(vector_int16_t& dst, vector_int16_t src, int16_t value, vector_bool mask)
__simd_callee__ inline void asc_add_scalar(vector_uint16_t& dst, vector_uint16_t src, uint16_t value, vector_bool mask)
__simd_callee__ inline void asc_add_scalar(vector_half& dst, vector_half src, half value, vector_bool mask)
__simd_callee__ inline void asc_add_scalar(vector_bfloat16_t& dst, vector_bfloat16_t src, bfloat16_t value, vector_bool mask)
__simd_callee__ inline void asc_add_scalar(vector_int32_t& dst, vector_int32_t src, int32_t value, vector_bool mask)
__simd_callee__ inline void asc_add_scalar(vector_uint32_t& dst, vector_uint32_t src, uint32_t value, vector_bool mask)
__simd_callee__ inline void asc_add_scalar(vector_float& dst, vector_float src, float value, vector_bool mask)
```

## Parameters

| Parameter       | Input/Output | Description               |
| --------- | ----- | ---------------- |
| dst       | Output    | Destination operand (vector data register).            |
| src | Input    | Source operand (vector data register).            |
| value | Input    | Source operand (scalar).       |
| mask     | Input    | Source operand mask (mask register), which indicates the elements that participate in the calculation. An element participates in the calculation when the corresponding bit is 1, and does not participate when the bit is 0. Elements not selected by mask are set to zero in the output.        |

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
vector_half src;
half scalar_val = 0.0f;
vector_bool mask = asc_create_mask_b16(PAT_ALL);
asc_loadalign(src, src_addr);  // src_addr is the externally input UB memory space address.
asc_add_scalar(dst, src, scalar_val, mask);
```
