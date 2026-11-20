---
okftype: reference.api
description: "Deprecated synchronous comparison interface that determines element-wise whether src0 is less than or equal to src1 and compresses each element's comparison result into 1 bit. It supports writing the result to a register or a destination address, provides two overloads for half and float, and its replacement is asc_le used together with asc_sync."
tag:
  - roles: [compute]
  - operations: [comparison]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "For the operand address overlap constraint, see the general address overlap constraint section on that page"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: related
    condition: "Used together with the non-synchronous interface to complete synchronization"
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_compare/asc_get_cmp_mask.md
    kind: related
    condition: "After the result is written to a register, use this interface to read the comparison result"
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_compare/asc_le.md
    kind: replaced_by
    condition: "The non-synchronous interface that replaces this synchronous comparison interface"
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_compare/asc_set_cmp_mask.md
    kind: related
    condition: "When the result is written to a register, this interface can be used to initialize the comparison mask"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "All four prototypes carry the ASC_DEPRECATED(9.2.0, 2027/09/07, asc_le and asc_sync) marker (the macro line before the corresponding @override_id in include/c_api/composite/vector_compute_composite.h). The replacement is the non-synchronous comparison interface asc_le plus asc_sync. All prototypes on this page fall within the deprecation scope, which is a page-wide deprecation with no upper availability limit."
---
# asc_le_sync (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T14:03:55.828Z pushedAt=2026-11-18T03:12:16.587Z nl=2 -->

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

**The `asc_le_sync` interface is deprecated. Use the [corresponding non-synchronous interface](../vector_compute/vector_compare/asc_le.md) and the synchronous interface [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Determines element by element whether src0 <= src1 holds. If it holds, the output result is 1; otherwise, it is 0. The comparison result of each element occupies 1 bit.
When the result is output to a register, you can call [asc_get_cmp_mask](../vector_compute/vector_compare/asc_get_cmp_mask.md) after the computation is complete to obtain the comparison result.

## Prototype

- Synchronous computation

    ```cpp
    // Output the result to a register.
    __aicore__ inline void asc_le_sync(__ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride)
    __aicore__ inline void asc_le_sync(__ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride)

    // Output the result to the destination address.
    __aicore__ inline void asc_le_sync(__ubuf__ uint8_t* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride)
    __aicore__ inline void asc_le_sync(__ubuf__ uint8_t* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride)
    ```

## Parameters

**Table 1** Parameter description

|Parameter|Input/Output|Description|
| ------------ | ------------ | ------------ |
|dst|Output|Start address of the destination operand (vector).|
|src0|Input|Start address of the source operand (vector).|
|src1|Input|Start address of the source operand (vector).|
| repeat | Input | Number of iterations. |
| dst_block_stride | Input | Address stride between different DataBlocks of the destination operand within a single iteration. |
| src0_block_stride | Input | Address stride between different DataBlocks of source operand 0 within a single iteration. |
| src1_block_stride | Input | Address stride between different DataBlocks of source operand 1 within a single iteration. |
| dst_repeat_stride | Input | Address stride of the same DataBlock of the destination operand between adjacent iterations. |
| src0_repeat_stride | Input | Address stride of the same DataBlock of source operand 0 between adjacent iterations. |
| src1_repeat_stride | Input | Address stride of the same DataBlock of source operand 1 between adjacent iterations. |

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
- When the result is output to a register, if a certain bit of the mask is 0 and the value of the corresponding bit of the output register does not change, the final result may contain residue from the previous calculation. To initialize the register, call the [asc_set_cmp_mask](../vector_compute/vector_compare/asc_set_cmp_mask.md) interface.

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
…… // Data movement and synchronization operations

// Store the result in a register and obtain the comparison result through the register.
asc_le_sync(src0, src1, repeat, dst_block_stride, src0_block_stride, src1_block_stride, dst_repeat_stride, src0_repeat_stride, src1_repeat_stride);
…… // Synchronization operations
asc_get_cmp_mask(dst); // Read the result.

// Output the result to the destination address.
asc_le_sync(dst, src0, src1, repeat, dst_block_stride, src0_block_stride, src1_block_stride, dst_repeat_stride, src0_repeat_stride, src1_repeat_stride);
```
