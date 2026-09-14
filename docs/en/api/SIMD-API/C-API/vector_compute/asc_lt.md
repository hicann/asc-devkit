# asc_lt

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T14:46:23.890Z pushedAt=2026-09-08T03:45:05.536Z -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Not supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
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

Determines element-wise whether **src0** < **src1** holds. If it holds, the corresponding bit in the output **dst** is set to 1; otherwise, it is set to 0.

## Prototype


- High-dimensional split computation

  ```cpp
  //Store the result in the register.
  __aicore__ inline void asc_lt(__ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride)
  __aicore__ inline void asc_lt(__ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride)
  // Output the result to the destination address.
  __aicore__ inline void asc_lt(__ubuf__ uint8_t* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride)
  __aicore__ inline void asc_lt(__ubuf__ uint8_t* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride)
  ```

- Synchronous computation

  ```cpp
  //Store the result in the register.
  __aicore__ inline void asc_lt_sync(__ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride)
  __aicore__ inline void asc_lt_sync(__ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride)
  // Output the result to the destination address.
  __aicore__ inline void asc_lt_sync(__ubuf__ uint8_t* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride)
  __aicore__ inline void asc_lt_sync(__ubuf__ uint8_t* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride)
  ```

## Parameters

| Parameter  | Input/Output | Description |
| ------------ | ------------ | ------------ |
| dst | Output | Starting address of the destination operand (vector). |
| src0 | Input | Starting address of the source operand (vector). |
| src1 | Input | Starting address of the source operand (vector). |
| repeat | Input    | Number of iterations. |
| dst_block_stride | Input    | Address stride between different DataBlocks of the destination operand within a single iteration. |
| src0_block_stride | Input    | Address stride between different DataBlocks of source operand 0 within a single iteration. |
| src1_block_stride | Input    | Address stride between different DataBlocks of source operand 1 within a single iteration. |
| dst_repeat_stride | Input    | Address stride of the same DataBlock of the destination operand between adjacent iterations. |
| src0_repeat_stride | Input    | Address stride of the same DataBlock of source operand 0 between adjacent iterations. |
| src1_repeat_stride | Input    | Address stride of the same DataBlock of source operand 1 between adjacent iterations. |

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).
- The start addresses of **dst**, **src0**, and **src1** must be 32-byte aligned.
- **dst** is sorted into a binary result in little-endian order, corresponding to the comparison result of the data at the corresponding position in **src**.
- When the result is output to the destination address, the **mask** parameter does not take effect.
- When the register-form API is used, [asc_get_cmp_mask](asc_get_cmp_mask.md) must be used to obtain the comparison result.
- When the result is output to a register, 256B of data is processed by default, and the **repeat** field does not take effect.
- When the result is output to a register, if a bit of **mask** is 0 and the value of the corresponding bit of the output register does not change, the final result may contain residue from the previous computation. To initialize the register, call the [asc_set_cmp_mask](asc_set_cmp_mask.md) API.

## Example

```cpp
// The input is fixed to 128 elements.
constexpr uint32_t total_length = 128;
__ubuf__ uint8_t dst[total_length / 8];
__ubuf__ half src0[total_length];
__ubuf__ half src1[total_length];
uint8_t repeat = 1;
uint8_t dst_block_stride = 1;
uint8_t src0_block_stride = 1;
uint8_t src1_block_stride = 1;
uint8_t dst_repeat_stride = 8;
uint8_t src0_repeat_stride = 8;
uint8_t src1_repeat_stride = 8;
... // Perform data transfer and synchronization.

// Store the result in a register and obtain the comparison result through the register.
asc_lt(src0, src1, repeat, dst_block_stride, src0_block_stride, src1_block_stride, dst_repeat_stride, src0_repeat_stride, src1_repeat_stride);
... // Perform synchronization.
asc_get_cmp_mask(dst); // Read the result.

// Output the result to the destination address.
asc_lt(dst, src0, src1, repeat, dst_block_stride, src0_block_stride, src1_block_stride, dst_repeat_stride, src0_repeat_stride, src1_repeat_stride);
```
