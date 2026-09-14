# asc_reduce_sum

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-04T11:09:31.702Z pushedAt=2026-09-05T12:14:28.931Z -->

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

Performs a reduction sum operation on the source operand **src** according to **mask**, summing all elements of **src** that participate in the computation and storing the result at position 0 of **dst**.

## Prototype

```cpp
__simd_callee__ inline void asc_reduce_sum(vector_int32_t& dst, vector_int16_t src, vector_bool mask)
__simd_callee__ inline void asc_reduce_sum(vector_int32_t& dst, vector_int32_t src, vector_bool mask)
__simd_callee__ inline void asc_reduce_sum(vector_uint32_t& dst, vector_uint16_t src, vector_bool mask)
__simd_callee__ inline void asc_reduce_sum(vector_uint32_t& dst, vector_uint32_t src, vector_bool mask)
__simd_callee__ inline void asc_reduce_sum(vector_half& dst, vector_half src, vector_bool mask)
__simd_callee__ inline void asc_reduce_sum(vector_float& dst, vector_float src, vector_bool mask)
```

## Parameters

| Parameter  | Input/Output | Description |
| :----- | :------- | :------- |
| dst | Output | Destination operand (vector data register). |
| src | Input | Source operand (vector data register). |
| mask | Input | Source operand mask (mask register), used to indicate which elements participate in the computation. Elements at positions with a value of 1 participate in the computation, and those with a value of 0 do not. |

For details about vector data registers and mask registers, see [reg Data Definition](../reg_data_definition.md).

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

When no element participates in the computation, the result is 0.

## Example

```cpp
vector_half dst;
vector_half src;
vector_bool mask = asc_create_mask_b16(PAT_ALL);
asc_loadalign(src, src_addr); // src_addr is the externally input UB memory space address.
asc_reduce_sum(dst, src, mask);
```
