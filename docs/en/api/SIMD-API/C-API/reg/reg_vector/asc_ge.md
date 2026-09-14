# asc_ge

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-04T09:52:54.539Z pushedAt=2026-09-05T12:14:28.837Z -->

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

Greater than or equal to (ge) performs an element-wise comparison on the source operands. For src0_i >= src1_i, if the condition holds, the corresponding result bit of the destination operand is 1; otherwise, it is 0. The comparison result of each element occupies one bit.

The calculation formula is as follows:

$$
dst_i = src0_i >= src1_i
$$

## Prototype

```cpp
__simd_callee__ inline void asc_ge(vector_bool& dst, vector_uint8_t src0, vector_uint8_t src1, vector_bool mask)
__simd_callee__ inline void asc_ge(vector_bool& dst, vector_int8_t src0, vector_int8_t src1, vector_bool mask)
__simd_callee__ inline void asc_ge(vector_bool& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask)
__simd_callee__ inline void asc_ge(vector_bool& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask)
__simd_callee__ inline void asc_ge(vector_bool& dst, vector_half src0, vector_half src1, vector_bool mask)
__simd_callee__ inline void asc_ge(vector_bool& dst, vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask)
__simd_callee__ inline void asc_ge(vector_bool& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask)
__simd_callee__ inline void asc_ge(vector_bool& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask)
__simd_callee__ inline void asc_ge(vector_bool& dst, vector_float src0, vector_float src1, vector_bool mask)
```

## Parameters

| Parameter  | Input/Output | Description |
| :----- | :------- | :------- |
| dst | Output | Destination operand (mask register). |
| src0 | Input | Source operand 0 (vector data register). |
| src1 | Input | Source operand 1 (vector data register). |
| mask | Input | Source operand mask (mask register), which indicates which elements participate in the computation. When the corresponding position is 1, the element participates in the computation; when it is 0, the element does not participate. Elements not selected by mask are set to zero in the output. |

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
vector_half src1;
vector_bool mask = asc_create_mask_b16(PAT_ALL);
asc_loadalign(src0, src0_addr); // src0_addr is the externally input UB memory space address.
asc_loadalign(src1, src1_addr); // src1_addr is the externally input UB memory space address.
asc_ge(dst, src0, src1, mask);
```
