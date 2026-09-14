# asc_sub

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-04T11:20:43.243Z pushedAt=2026-09-05T12:14:28.943Z -->

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

Performs element-wise subtraction on the source operands **src0** and **src1** according to **mask**, and writes the result to the destination operand **dst**.
When the source operands are of type uint32_t or int32_t, an API with borrow data is provided. If a borrow occurs during the subtraction of **src0** and **src1**, 0 is written to the least significant bit of each 4-bit group at the corresponding position in **carry**; otherwise, 1 is written.

The calculation formula is as follows:

$$
dst_i = src0_i - src1_i
$$

Example description of the borrow value output by carry:

| Data Type | Whether Borrowed | Example Description |
| :----- | :------- | :------- |
| uint32_t data type | No borrow | src0_i = 5, src1_i = 2 <br> dst_i = src0_i - src1_i = 3 <br> 1 is written to the least significant bit of the 4-bit group at the corresponding position in carry: carry_i = 1 |
| uint32_t data type | Borrow occurs | src0_i = 5, src1_i = 7 <br> dst_i = src0_i - src1_i = -2 <br> 0 is written to the least significant bit of the 4-bit group at the corresponding position in carry: carry_i = 0 |
| int32_t data type | No borrow | src0_i = 5, src1_i = 2 <br> dst_i = src0_i - src1_i = 3 <br> 1 is written to the least significant bit of the 4-bit group at the corresponding position in carry: carry_i = 1 |
| int32_t data type | Borrow occurs | src0_i = 5, src1_i = -7 <br> dst_i = src0_i - src1_i = 12 <br> 0 is written to the least significant bit of the 4-bit group at the corresponding position in carry: carry_i = 0 |

## Prototype

- API without borrow data

    ```cpp
    __simd_callee__ inline void asc_sub(vector_uint8_t& dst, vector_uint8_t src0, vector_uint8_t src1, vector_bool mask)
    __simd_callee__ inline void asc_sub(vector_int8_t& dst, vector_int8_t src0, vector_int8_t src1, vector_bool mask)
    __simd_callee__ inline void asc_sub(vector_uint16_t& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask)
    __simd_callee__ inline void asc_sub(vector_int16_t& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask)
    __simd_callee__ inline void asc_sub(vector_half& dst, vector_half src0, vector_half src1, vector_bool mask)
    __simd_callee__ inline void asc_sub(vector_bfloat16_t& dst, vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask)
    __simd_callee__ inline void asc_sub(vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask)
    __simd_callee__ inline void asc_sub(vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask)
    __simd_callee__ inline void asc_sub(vector_float& dst, vector_float src0, vector_float src1, vector_bool mask)
    ```

- API with borrow data

    ```cpp
    __simd_callee__ inline void asc_sub(vector_bool& carry, vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask)
    __simd_callee__ inline void asc_sub(vector_bool& carry, vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask)
    ```

## Parameters

| Parameter | Input/Output | Description |
| :----- | :------- | :------- |
| carry | Output | Destination borrow value (mask register). |
| dst | Output | Destination operand (vector data register). |
| src0 | Input | Source operand 0 (vector data register). |
| src1 | Input | Source operand 1 (vector data register). |
| mask | Input | Source operand mask (mask register). Indicates which elements participate in the computation. Elements with the corresponding bit set to 1 participate in the computation, and those set to 0 do not. Elements not selected by mask are set to zero in the output. |

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
vector_half src0;
vector_half src1;
vector_bool mask = asc_create_mask_b16(PAT_ALL);
asc_loadalign(src0, src0_addr); // src0_addr is the externally input UB memory space address.
asc_loadalign(src1, src1_addr); // src1_addr is the externally input UB memory space address.
// mask is the source operand mask.
asc_sub(dst, src0, src1, mask);
```
