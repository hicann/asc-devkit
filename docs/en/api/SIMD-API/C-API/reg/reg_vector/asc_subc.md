# asc_subc

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-04T11:21:14.122Z pushedAt=2026-09-05T12:14:28.945Z -->

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

Performs element-wise subtraction on the source operands **src0**, **src1**, and the carry data **carry_src** according to **mask**, and writes the result to the destination operand **dst**. If a borrow occurs when subtracting **src0**, **src1**, and **carry_src**, 0 is written to the least significant bit of the corresponding 4-bit position in **carry**; otherwise, 1 is written.

The calculation formula is as follows:

$$
dst_i = src0_i - src1_i - carry_src_i
$$

Example description of the borrow value output by carry:

| data type | Whether borrow occurs | example description |
| -------- | -------- | ------------------------ |
| uint32_t | No borrow occurs | src0_i = 5, src1_i = 2, carry_src_i = 1 <br> dst_i = src0_i - src1_i - carry_src_i = 2 <br> Write 1 to the least significant bit of the corresponding 4-bit position in carry: carry_i = 1 |
| uint32_t | Borrow occurs | src0_i = 5, src1_i = 7, carry_src_i = 1 <br> dst_i = src0_i - src1_i - carry_src_i = -3 <br> Write 0 to the least significant bit of the corresponding 4-bit position in carry: carry_i = 0 |
| int32_t | No borrow occurs | src0_i = 5, src1_i = 2, carry_src_i = 1 <br> dst_i = src0_i - src1_i - carry_src_i = 2 <br> Write 1 to the least significant bit of the corresponding 4-bit position in carry: carry_i = 1 |
| int32_t | Borrow occurs | src0_i = 5, src1_i = -7, carry_src_i = 1 <br> dst_i = src0_i - src1_i - carry_src_i = 11 <br> Write 0 to the least significant bit of the corresponding 4-bit position in carry: carry_i = 0 |

## Prototype

```cpp
__simd_callee__ inline void asc_subc(vector_bool& carry, vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool carry_src, vector_bool mask)
__simd_callee__ inline void asc_subc(vector_bool& carry, vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1, vector_bool carry_src, vector_bool mask)
```

## Parameters

| Parameter  | Input/Output | Description |
| :----- | :------- | :------- |
| carry | Output | Destination borrow value (mask register). |
| dst | Output | Destination operand (vector data register). |
| src0 | Input | Source operand 0 (vector data register). |
| src1 | Input | Source operand 1 (vector data register). |
| carry_src | Input | Source borrow value (mask register). |
| mask | Input | Source operand mask (mask register). Indicates which elements participate in the computation. Elements at positions where the corresponding bit is 1 participate in the computation, and elements at positions where the corresponding bit is 0 do not. Elements not selected by mask are set to zero in the output. |

For details about vector data registers and mask registers, see [reg Data Definition](../reg_data_definition.md).

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

None

## Example

```cpp
vector_bool carry;
vector_uint32_t dst;
vector_uint32_t src0;
vector_uint32_t src1;
vector_bool carry_src = asc_create_mask_b32(PAT_ALLF);
vector_bool mask = asc_create_mask_b32(PAT_ALL);
asc_loadalign(src0, src0_addr); // src0_addr is the UB memory space address input from outside.
asc_loadalign(src1, src1_addr); // src1_addr is the UB memory space address input from outside.
// mask is the source operand mask.
asc_subc(carry, dst, src0, src1, carry_src, mask);
```
