# asc_shiftright_scalar

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-04T11:16:05.644Z pushedAt=2026-09-05T12:14:28.937Z -->

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

Performs a right shift on the source operand **src** based on **mask**, with the number of shift bits determined by the input parameter **value**, and writes the result to the destination operand **dst**.
For unsigned data types, a logical right shift is performed, removing the low-order bits and filling the high-order bits with 0.
For signed data types, an arithmetic right shift is performed, removing the low-order bits and filling the high-order bits with the sign bit.

The calculation formula is as follows:

$$
dst_i = src_i >> value
$$

## Prototype

```cpp
__simd_callee__ inline void asc_shiftright_scalar(vector_uint8_t& dst, vector_uint8_t src, int16_t value, vector_bool mask)
__simd_callee__ inline void asc_shiftright_scalar(vector_int8_t& dst, vector_int8_t src, int16_t value, vector_bool mask)
__simd_callee__ inline void asc_shiftright_scalar(vector_uint16_t& dst, vector_uint16_t src, int16_t value, vector_bool mask)
__simd_callee__ inline void asc_shiftright_scalar(vector_int16_t& dst, vector_int16_t src, int16_t value, vector_bool mask)
__simd_callee__ inline void asc_shiftright_scalar(vector_uint32_t& dst, vector_uint32_t src, int16_t value, vector_bool mask)
__simd_callee__ inline void asc_shiftright_scalar(vector_int32_t& dst, vector_int32_t src, int16_t value, vector_bool mask)
```

## Parameters

| Parameter  | Input/Output | Description |
| :----- | :------- | :------- |
| dst | Output | Destination operand (vector data register). |
| src | Input | Source operand (vector data register). |
| value | Input | Source operand (scalar). Setting it to a negative number is not supported. |
| mask | Input | Source operand mask (mask register). Indicates which elements participate in the calculation. A value of 1 at the corresponding position indicates participation in the calculation, and 0 indicates no participation. Elements not selected by **mask** are set to zero in the output. |

For details about vector data registers and mask registers, see [reg Data Definition](../reg_data_definition.md).

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- **value** cannot be set to a negative value.

## Example

```cpp
vector_uint16_t dst;
vector_uint16_t src;
int16_t value = 1;
vector_bool mask = asc_create_mask_b16(PAT_ALL);
asc_loadalign(src, src_addr); // src_addr is the UB memory space address input from outside.
asc_shiftright_scalar(dst, src, value, mask);
```