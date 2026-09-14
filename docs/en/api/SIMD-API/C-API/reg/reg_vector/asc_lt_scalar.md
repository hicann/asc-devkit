# asc_lt_scalar

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-04T10:42:32.429Z pushedAt=2026-09-05T12:14:28.897Z -->

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

Less than (lt) performs an element-wise comparison on the source operand. For src_i < value, if the condition holds, the corresponding result bit of the destination operand is set to 1; otherwise, it is set to 0. The comparison result of each element occupies one bit.

## Prototype

```cpp
__simd_callee__ inline void asc_lt_scalar(vector_bool& dst, vector_uint8_t src, uint8_t value, vector_bool mask)
__simd_callee__ inline void asc_lt_scalar(vector_bool& dst, vector_int8_t src, int8_t value, vector_bool mask)
__simd_callee__ inline void asc_lt_scalar(vector_bool& dst, vector_half src, half value, vector_bool mask)
__simd_callee__ inline void asc_lt_scalar(vector_bool& dst, vector_uint16_t src, uint16_t value, vector_bool mask)
__simd_callee__ inline void asc_lt_scalar(vector_bool& dst, vector_int16_t src, int16_t value, vector_bool mask)
__simd_callee__ inline void asc_lt_scalar(vector_bool& dst, vector_bfloat16_t src, bfloat16_t value, vector_bool mask)
__simd_callee__ inline void asc_lt_scalar(vector_bool& dst, vector_float src, float value, vector_bool mask)
__simd_callee__ inline void asc_lt_scalar(vector_bool& dst, vector_uint32_t src, uint32_t value, vector_bool mask)
__simd_callee__ inline void asc_lt_scalar(vector_bool& dst, vector_int32_t src, int32_t value, vector_bool mask)
```

## Parameters

| Parameter  | Input/Output | Description |
| :----- | :------- | :------- |
| dst | Output | Destination operand (mask register). |
| src | Input | Source operand (vector data register). |
| value | Input | Source operand (scalar). |
| mask | Input | Source operand mask (mask register), which indicates the elements that participate in the computation. A value of 1 at the corresponding position indicates participation in the computation, and 0 indicates no participation. Elements not selected by mask are set to zero in the output. |

For details about vector data registers and mask registers, see [reg Data Definition](../reg_data_definition.md).

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

None

## Example

```cpp
vector_bool dst;
vector_half src0;
half value = 0.0;
vector_bool mask = asc_create_mask_b16(PAT_ALL);
asc_loadalign(src0, src0_addr); // src0_addr is the UB memory space address input from outside.
asc_lt_scalar(dst, src0, value, mask);
```