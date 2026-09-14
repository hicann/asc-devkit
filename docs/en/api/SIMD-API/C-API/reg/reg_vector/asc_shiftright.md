# asc_shiftright

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-04T11:16:19.602Z pushedAt=2026-09-05T12:14:28.939Z -->

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

Performs a right shift operation on the input data **src0** according to the corresponding elements of **src1** based on the mask, and writes the result to **dst**.

## Prototype

```cpp
__simd_callee__ inline void asc_shiftright(vector_uint8_t& dst, vector_uint8_t src0, vector_int8_t src1, vector_bool mask)
__simd_callee__ inline void asc_shiftright(vector_int8_t& dst, vector_int8_t src0, vector_int8_t src1, vector_bool mask)
__simd_callee__ inline void asc_shiftright(vector_uint16_t& dst, vector_uint16_t src0, vector_int16_t src1, vector_bool mask)
__simd_callee__ inline void asc_shiftright(vector_int16_t& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask)
__simd_callee__ inline void asc_shiftright(vector_uint32_t& dst, vector_uint32_t src0, vector_int32_t src1, vector_bool mask)
__simd_callee__ inline void asc_shiftright(vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask)
```

## Parameters

| Parameter  | Input/Output | Description |
| :----- | :------- | :------- |
| dst | Output | Destination operand (vector data register).|
| src0 | Input | Source operand (vector data register).|
| src1 | Input | Source operand (vector data register).|
| mask | Input | Source operand mask (mask register), which indicates which elements participate in the computation. A value of 1 at the corresponding position means the element participates in the computation, and 0 means it does not. Elements not selected by the mask are set to zero in the output. |

For details about vector data registers and mask registers, see [reg Data Definition](../reg_data_definition.md).

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- When the data type is unsigned, a logical right shift is performed. A logical right shift moves the binary number to the right by the specified number of bits, discarding the least significant bits and filling the most significant bits with 0.
- For a logical shift (unsigned data type), if the shift amount is greater than or equal to the bit width of the data type, the output is 0.
- When the data type is signed, an arithmetic right shift is performed. An arithmetic right shift moves the binary number to the right by the specified number of bits, discarding the least significant bits and filling the most significant bits with the original sign bit.
- For an arithmetic shift (signed data type), if **src0** is less than 0, **src1** is greater than 0, and the shift amount of **src1** is greater than the bit width of the data type, the output is -1.
- For an arithmetic shift (signed data type), if **src0** is greater than 0 and the shift amount of **src1** is greater than the bit width of the data type, the output is 0.

## Example

```cpp
vector_uint32_t dst;
vector_uint32_t src0;
vector_int32_t src1;
vector_bool mask = asc_create_mask_b32(PAT_ALL);
asc_loadalign(src0, src0_addr); // src0_addr is the externally input UB memory space address.
asc_loadalign(src1, src1_addr); // src1_addr is the externally input UB memory space address.
asc_shiftright(dst, src0, src1, mask);
```