# asc_xor

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-04T11:37:59.689Z pushedAt=2026-09-05T12:14:28.966Z -->

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

Performs an element-wise XOR (^) operation on the input **src0** and **src1** based on **mask**, and writes the result to **dst**.

## Prototype

```cpp
__simd_callee__ inline void asc_xor(vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask)
__simd_callee__ inline void asc_xor(vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask)
__simd_callee__ inline void asc_xor(vector_uint16_t& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask)
__simd_callee__ inline void asc_xor(vector_int16_t& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask)
__simd_callee__ inline void asc_xor(vector_uint8_t& dst, vector_uint8_t src0, vector_uint8_t src1, vector_bool mask)
__simd_callee__ inline void asc_xor(vector_int8_t& dst, vector_int8_t src0, vector_int8_t src1, vector_bool mask)
__simd_callee__ inline void asc_xor(vector_bool& dst, vector_bool src0, vector_bool src1, vector_bool mask)
```

## Parameters

| Parameter      | Input/Output | Description                             |
|:---------| :--- |:-------------------------------|
| dst      | Output | Destination operand (vector data register).                |
| src0     | Input | Source operand (vector data register).                                                       |
| src1     | Input | Source operand (vector data register).                                                            |
| mask     | Input | Source operand mask (mask register), which indicates which elements participate in the computation. A value of 1 at the corresponding position means the element participates in the computation, and 0 means it does not. Elements not selected by the mask are set to zero in the output. |

For details about vector data registers and mask registers, see [reg Data Definition](../reg_data_definition.md).

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

None

## Example

```cpp
vector_int8_t dst;
vector_int8_t src0;
vector_int8_t src1;
vector_bool mask = asc_create_mask_b8(PAT_ALL);
asc_loadalign(src0, src0_addr); // src0_addr is the externally input UB memory space address.
asc_loadalign(src1, src1_addr); // src1_addr is the externally input UB memory space address.
asc_xor(dst, src0, src1, mask);
```