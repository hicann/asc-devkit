---
okftype: reference.api
description: "Compares two vectors element-wise for equality. The result is written to the destination address at 1 bit per element or left in the comparison register for asc_get_cmp_mask to read. Supported only on the 2201 architecture."
tag:
  - roles: [compute]
  - operations: [comparison]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: related
    condition: "The main text references the general address overlap constraint accordingly."
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_compare/asc_get_cmp_mask.md
    kind: related
    condition: "The register form requires it to read the comparison result."
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_compare/asc_set_cmp_mask.md
    kind: related
    condition: "Use it to initialize the comparison mask when the register has residual data."
version: ">=9.0.0"
version_changes: []
---
# asc_eq

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.717621+00:00 pushedAt=2026-11-16T02:10:58.323Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Not supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Supported
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

The header file path is `"c_api/vector_compute/compute/vector_compare.h"`.

Compares the element values of src0 and src1 at corresponding index positions. If the comparison result is true, the corresponding bit of the output result is set to 1; otherwise, it is set to 0.

## Prototype

- High-dimensional slicing computation

  ```cpp
  // Store the comparison result in registers.
  __aicore__ inline void asc_eq(__ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride)
  __aicore__ inline void asc_eq(__ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride)
  // Store the comparison result in dst.
  __aicore__ inline void asc_eq(__ubuf__ uint8_t* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride)
  __aicore__ inline void asc_eq(__ubuf__ uint8_t* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride)
  __aicore__ inline void asc_eq(__ubuf__ uint8_t* dst, __ubuf__ int32_t* src0, __ubuf__ int32_t* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride)
  ```

## Parameters

**Table 1** Parameters

|Parameter|Input/Output|Description|
| ------------ | ------------ | ------------ |
|dst|Output|Start address of the destination operand (vector).|
|src0|Input|Start address of the source operand (vector).|
|src1|Input|Start address of the source operand (vector).|
| repeat | Input | Number of iterations. |
| dst_block_stride | Input | Address stride between different DataBlocks of the destination operand within a single iteration. |
| src0_block_stride| Input | Address stride between different DataBlocks of the source operand within a single iteration. |
| src1_block_stride | Input | Address stride between different DataBlocks of the source operand within a single iteration. |
| dst_repeat_stride | Input | Address stride of the same DataBlock of the destination operand between adjacent iterations. |
| src0_repeat_stride | Input | Address stride of the same DataBlock of the source operand between adjacent iterations. |
| src1_repeat_stride | Input | Address stride of the same DataBlock of the source operand between adjacent iterations. |

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- For operand address overlap constraints, see [General Address Overlap Constraints](../../general_description_and_constraints.md#general-address-overlap-constraints).
- The start addresses of dst, src0, and src1 must be 32-byte aligned.
- dst is arranged into a binary result in little-endian order, corresponding to the comparison result of the data at the corresponding position in src.
- When the register-based API is used, [asc_get_cmp_mask](asc_get_cmp_mask.md) must be used to obtain the comparison result.
- When the result is output to a register, a fixed amount of 256B of data is processed, and the repeat field does not take effect.
- When the result is output to a register, if a bit of mask is 0 and the value of the corresponding bit of the output register does not change, the final result may retain residue from the previous computation. To initialize the register, call the [asc_set_cmp_mask](asc_set_cmp_mask.md) API.

## Examples

```cpp
// The input is fixed at 128 elements.
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
//... data movement and synchronization operations.

// Store the result in a register and obtain the comparison result through the register.
asc_eq(src0, src1, repeat, dst_block_stride, src0_block_stride, src1_block_stride, dst_repeat_stride, src0_repeat_stride, src1_repeat_stride);
//... Synchronous operation.
asc_get_cmp_mask(dst); // Read the result.

// Output the result to the destination address.
asc_eq(dst, src0, src1, repeat, dst_block_stride, src0_block_stride, src1_block_stride, dst_repeat_stride, src0_repeat_stride, src1_repeat_stride);
```
