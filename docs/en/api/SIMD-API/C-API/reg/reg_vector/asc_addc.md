# asc_addc

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-04T09:05:23.465Z pushedAt=2026-09-05T12:14:28.782Z -->

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

Based on the mask, performs element-wise addition on the input data **src0** and **src1** and the carry data **src2**, where i is the element index. The result is written to **dst1**. If **src0** and **src1** are of the uint32_t type and the sum of **src0**, **src1**, and the carry value **src2** exceeds the maximum value of uint32_t, 1 is written to every 4 bits at the corresponding position in **dst**; otherwise, 0 is written.

The calculation formula is as follows:

$$
\{dst0_i, dst1_i\} = src0_i + src1_i + src2_i
$$

## Prototype

```cpp
__simd_callee__ inline void asc_addc(vector_bool& dst0, vector_uint32_t& dst1, vector_uint32_t src0, vector_uint32_t src1, vector_bool src2, vector_bool mask)
__simd_callee__ inline void asc_addc(vector_bool& dst0, vector_int32_t& dst1, vector_int32_t src0, vector_int32_t src1, vector_bool src2, vector_bool mask)
```

## Parameters

| Parameter  | Input/Output | Description |
| :----- | :------- | :------- |
| dst0 | Output | Destination operand (mask register). Stores the carry data after the addition. |
| dst1 | Output | Destination operand (vector data register). Stores the result after the addition. |
| src0 | Input | Source operand (vector data register). |
| src1 | Input | Source operand (vector data register). |
| src2 | Input | Source operand (mask register), carry data. |
| mask | Input | Source operand mask (mask register), used to indicate which elements participate in the calculation. A value of 1 at the corresponding position indicates participation in the calculation, and 0 indicates no participation. Elements not selected by mask are set to zero in the output.  |

For details about vector data registers and mask registers, see [reg Data Definition](../reg_data_definition.md).

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

None

## Example

```cpp
vector_bool dst0;
vector_int32_t dst1;
vector_int32_t src0, src1;
vector_bool src2;
vector_bool mask = asc_create_mask_b32(PAT_ALL);
asc_loadalign(src0, src0_addr); // src0_addr is the UB memory space address input from outside.
asc_loadalign(src1, src1_addr); // src1_addr is the UB memory space address input from outside.
asc_loadalign(src2, src2_addr); // src2_addr is the UB memory space address input from outside.
asc_addc(dst0, dst1, src0, src1, src2, mask);
```
