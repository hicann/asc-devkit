---
okftype: reference.api
description: "Deprecated synchronous comparison interface that compares two vectors element by element to determine whether src0 is greater than src1. The result can be written bitwise to dst or left in the comparison register. It is supported only on the 2201 architecture and should be replaced by asc_gt combined with asc_sync."
tag:
  - roles: [compute]
  - operations: [comparison]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: replaced_by
    condition: "Replaces the original synchronous wait semantics of this interface"
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_compare/asc_get_cmp_mask.md
    kind: related
    condition: "Used to read the comparison result when the result is stored in register form"
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_compare/asc_gt.md
    kind: replaced_by
    condition: "Replaces the vector comparison capability of this interface; synchronization must be inserted separately"
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_compare/asc_set_cmp_mask.md
    kind: related
    condition: "Used when a comparison mask needs to be initialized or set"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The four overloads in include/c_api/composite/vector_compute_composite.h are marked ASC_DEPRECATED(9.2.0, \"2027/09/07\", asc_gt and asc_sync), and the body now uses asc_gt plus asc_sync; the declaration is still retained in 9.3.0-beta.1, which is deprecated rather than removed, with no upper version limit."
---
# asc_gt_sync (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T13:23:19.799Z pushedAt=2026-11-18T03:12:16.519Z nl=2 -->

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

**The `asc_gt_sync` interface is deprecated. Use the [corresponding non-synchronous interface](../vector_compute/vector_compare/asc_gt.md) and the synchronous interface [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Compares the magnitude relationship of two vectors element by element. If the comparison result is true, the corresponding bit of the output result is 1; otherwise, it is 0. The calculation formula is as follows:

$$
\overrightarrow{\text{result}} = \text{asc\_gt}(\overrightarrow{src0}, \overrightarrow{src1})
$$
$$
\begin{bmatrix}
\text{result}_0 \\
\text{result}_1 \\
\vdots \\
\text{result}_{n-1}
\end{bmatrix}
=
\begin{bmatrix}
\mathbb{I}_{(src0_0 > src1_0)} \\
\mathbb{I}_{(src0_1 > src1_1)} \\
\vdots \\
\mathbb{I}_{(src0_{n-1} > src1_{n-1})}
\end{bmatrix}
$$

## Prototype

- Synchronous computation

    ```cpp
    // Store the result in the register.
    __aicore__ inline void asc_gt_sync(__ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t      dst_block_stride, uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride)
    __aicore__ inline void asc_gt_sync(__ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride)
    // Output the result to the destination operand.
    __aicore__ inline void asc_gt_sync(__ubuf__ uint8_t* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride)
    __aicore__ inline void asc_gt_sync(__ubuf__ uint8_t* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride)
    ```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| :----- | :------- | :------- |
| dst | Output | Start address of the destination operand (vector). |
| src0 | Input | Start address of the source operand (vector). |
| src1 | Input | Start address of the source operand (vector). |
| repeat | Input | Number of iterations. |
| dst_block_stride | Input | Address step between different DataBlocks within a single iteration of the destination operand. |
| src0_block_stride | Input | Address step between different DataBlocks within a single iteration of source operand 0. |
| src1_block_stride | Input | Address step between different DataBlocks within a single iteration of source operand 1. |
| dst_repeat_stride | Input | Address step of the same DataBlock between adjacent iterations of the destination operand. |
| src0_repeat_stride | Input | Address step of the same DataBlock between adjacent iterations of source operand 0. |
| src1_repeat_stride | Input | Address step of the same DataBlock between adjacent iterations of source operand 1. |

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).
- The start addresses of dst, src0, and src1 must be 32-byte aligned.
- dst is sorted into a binary result in little-endian order, corresponding to the comparison result of the data at the corresponding position in src.
- When the result is output to the destination address, the mask parameter does not take effect.
- When using the register-form interface, you need to use [asc_get_cmp_mask](../vector_compute/vector_compare/asc_get_cmp_mask.md) to obtain the comparison result.
- When the result is output to a register, 256B of data is processed in a fixed manner, and the repeat field does not take effect.
- When the result is output to a register, if a bit of mask is 0 and the value of the corresponding bit of the output register does not change, the final result may contain residue from the previous calculation. To initialize the register, you can call the [asc_set_cmp_mask](../vector_compute/vector_compare/asc_set_cmp_mask.md) interface.

## Examples

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
…… // Data movement and sync operations

// Store the result in a register and obtain the comparison result through the register.
asc_gt_sync(src0, src1, repeat, dst_block_stride, src0_block_stride, src1_block_stride, dst_repeat_stride, src0_repeat_stride, src1_repeat_stride);
…… // Sync operations
asc_get_cmp_mask(dst); // Read the result.

// Output the result to the destination address.
asc_gt_sync(dst, src0, src1, repeat, dst_block_stride, src0_block_stride, src1_block_stride, dst_repeat_stride, src0_repeat_stride, src1_repeat_stride);
```
