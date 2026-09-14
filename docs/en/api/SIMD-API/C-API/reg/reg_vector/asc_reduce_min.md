# asc_reduce_min

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-04T11:07:24.036Z pushedAt=2026-09-05T12:14:28.930Z -->

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

Performs a reduction minimum operation on the source operand **src** based on **mask**: writes the minimum value in **src** to position 0 of the destination operand **dst**, writes the index of the minimum value in **src** to position 1 of **dst**, and sets all other positions of **dst** to 0.

## Prototype

```cpp
__simd_callee__ inline void asc_reduce_min(vector_uint16_t& dst, vector_uint16_t src, vector_bool mask)
__simd_callee__ inline void asc_reduce_min(vector_int16_t& dst, vector_int16_t src, vector_bool mask)
__simd_callee__ inline void asc_reduce_min(vector_half& dst, vector_half src, vector_bool mask)
__simd_callee__ inline void asc_reduce_min(vector_uint32_t& dst, vector_uint32_t src, vector_bool mask)
__simd_callee__ inline void asc_reduce_min(vector_int32_t& dst, vector_int32_t src, vector_bool mask)
__simd_callee__ inline void asc_reduce_min(vector_float& dst, vector_float src, vector_bool mask)
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

- When no element participates in the computation, the maximum value of the data type is written to **dst**.
- When multiple minimum values exist, the first minimum value and its index are saved in **dst**.

## Example

```cpp
vector_half dst;
vector_half src;
vector_bool mask = asc_create_mask_b16(PAT_ALL);
asc_loadalign(src, src_addr); // src_addr is the externally input UB memory space address.
asc_reduce_min(dst, src, mask);
```
