# asc_add

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-04T09:05:25.727Z pushedAt=2026-09-05T12:14:28.783Z -->

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

Performs a vector addition operation on elements at corresponding positions, where i is the element index. The calculation uses the ZEROING mode (elements not selected by the mask are set to 0 in the destination operand). When the inputs **src0** and **src1** are converted to the **uint32_t** type and their sum exceeds the maximum value of **uint32_t**, an addition operation with carry is performed, 1 is written to every 4 bits at the corresponding position in **dst**; otherwise, 0 is written.

The calculation formula is as follows:

$$
dst_i = src0_i + src1_i
$$

## Prototype

```cpp
// Vector addition without carry.
__simd_callee__ inline void asc_add(vector_uint8_t& dst, vector_uint8_t src0, vector_uint8_t src1, vector_bool mask)
__simd_callee__ inline void asc_add(vector_int8_t& dst, vector_int8_t src0, vector_int8_t src1, vector_bool mask)
__simd_callee__ inline void asc_add(vector_uint16_t& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask)
__simd_callee__ inline void asc_add(vector_int16_t& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask)
__simd_callee__ inline void asc_add(vector_half& dst, vector_half src0, vector_half src1, vector_bool mask)
__simd_callee__ inline void asc_add(vector_float& dst, vector_float src0, vector_float src1, vector_bool mask)
__simd_callee__ inline void asc_add(vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask)
__simd_callee__ inline void asc_add(vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask)
__simd_callee__ inline void asc_add(vector_bfloat16_t& dst, vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask)
// Vector addition with carry.
__simd_callee__ inline void asc_add(vector_bool& dst0, vector_int32_t& dst1, vector_int32_t src0, vector_int32_t src1, vector_bool mask)
__simd_callee__ inline void asc_add(vector_bool& dst0, vector_uint32_t& dst1, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask)
```

## Parameters

| Parameter  | Input/Output | Description |
| :----- | :------- | :------- |
| dst| Output | Destination operand (vector data register). |
| dst0| Output | Destination operand (mask register). Stores the carry data after the addition operation. |
| dst1| Output | Destination operand (vector data register). |
| src0 | Input | Source operand (vector data register). |
| src1 | Input | Source operand (vector data register). |
| mask | Input | Source operand mask (mask register), which indicates the elements that participate in the calculation. A value of 1 at the corresponding position means the element participates in the calculation, and 0 means it does not. Elements not selected by the mask are set to 0 in the output. |

For details about vector data registers and mask registers, see [reg Data Definition](../reg_data_definition.md).

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- The addition operation with carry supports only the **int32_t** and **uint32_t** types.
- The addition operation with carry must be invoked manually; the API does not automatically detect and trigger the operation.

## Example

```cpp
vector_half dst;
vector_half src0, src1;
asc_loadalign(src0, src0_addr); // src0_addr is the externally input UB memory space address.
asc_loadalign(src1, src1_addr); // src1_addr is the externally input UB memory space address.
vector_bool mask = asc_create_mask_b16(PAT_ALL);
asc_add(dst, src0, src1, mask);
```