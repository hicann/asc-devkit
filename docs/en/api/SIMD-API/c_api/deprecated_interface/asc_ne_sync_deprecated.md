---
okftype: reference.api
description: "Deprecated vector not-equal comparison synchronous interface that determines element by element whether two source vectors are not equal. The result can be written bitwise to the destination address or output to the comparison mask register. It supports half and float, and the replacement is asc_ne combined with asc_sync."
tag:
  - roles: [compute]
  - operations: [comparison, synchronization]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: replaced_by
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_compare/asc_get_cmp_mask.md
    kind: related
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_compare/asc_ne.md
    kind: replaced_by
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_compare/asc_set_cmp_mask.md
    kind: related
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The synchronous comparison composite interface is deprecated; use asc_ne and asc_sync instead. All 4 declarations in include/c_api/composite/vector_compute_composite.h carry ASC_DEPRECATED(9.2.0)."
---
# asc_ne_sync (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T14:29:54.989Z pushedAt=2026-11-18T03:12:16.629Z nl=2 -->

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

The header file path is `"c_api/composite/vector_compute_composite.h"`.

**The `asc_ne_sync` interface is deprecated. Use the [corresponding non-synchronous interface](../vector_compute/vector_compare/asc_ne.md) and the synchronous interface [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Determines element by element whether src0 != src1 holds. If it holds, the output result is 1; otherwise, it is 0. The comparison result of each element occupies 1 bit.
When the result is output to a register, you can call [asc_get_cmp_mask](../vector_compute/vector_compare/asc_get_cmp_mask.md) after the computation is complete to obtain the comparison result.

## Prototype

- Synchronous computation

    ```cpp
    // Output the result to a register.
    __aicore__ inline void asc_ne_sync(__ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride)
    __aicore__ inline void asc_ne_sync(__ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride)

    // Output the result to the destination address.
    __aicore__ inline void asc_ne_sync(__ubuf__ uint8_t* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride)
    __aicore__ inline void asc_ne_sync(__ubuf__ uint8_t* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride)
    ```

## Parameters

**Table 1** Parameter description

|Parameter|Input/Output|Description|
| ------------ | ------------ | ------------ |
|dst| Output |Start address of the destination operand (vector).|
|src0| Input |Start address of the source operand (vector).|
|src1| Input |Start address of the source operand (vector).|
| repeat | Input | Repeat count. |
| dst_block_stride | Input | Address stride between different DataBlocks within a single iteration of dst. |
| src0_block_stride | Input | Address stride between different DataBlocks within a single iteration of src0. |
| src1_block_stride | Input | Address stride between different DataBlocks within a single iteration of src1. |
| dst_repeat_stride | Input | Address stride of the same DataBlock between adjacent iterations of dst. |
| src0_repeat_stride | Input | Address stride of the same DataBlock between adjacent iterations of src0. |
| src1_repeat_stride | Input | Address stride of the same DataBlock between adjacent iterations of src1. |

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).
- The start addresses of dst, src0, and src1 must be 32-byte aligned.
- dst is arranged in little-endian order into a binary result, corresponding to the comparison result of the data at the corresponding position in src.
- When the result is output to the destination address, the mask parameter is not effective.
- When using the register-form interface, you need to use [asc_get_cmp_mask](../vector_compute/vector_compare/asc_get_cmp_mask.md) to obtain the comparison result.
- When the result is output to a register, 256B data is processed in a fixed manner, and the repeat field is not effective.
- When the result is output to a register, if a bit of the mask is 0 and the value of the corresponding bit of the output register does not change, the final result may contain residue from the previous calculation. To initialize the register, you can call the [asc_set_cmp_mask](../vector_compute/vector_compare/asc_set_cmp_mask.md) interface.

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
…… // Data transfer and synchronization operations

// Store the result in a register and obtain the comparison result through the register.
asc_ne_sync(src0, src1, repeat, dst_block_stride, src0_block_stride, src1_block_stride, dst_repeat_stride, src0_repeat_stride, src1_repeat_stride);
…… // Synchronization operations
asc_get_cmp_mask(dst); // Read the result.

// Output the result to the destination address.
asc_ne_sync(dst, src0, src1, repeat, dst_block_stride, src0_block_stride, src1_block_stride, dst_repeat_stride, src0_repeat_stride, src1_repeat_stride);
```
