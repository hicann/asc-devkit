---
okftype: reference.api
description: "Deprecated synchronous comparison interface that determines element by element whether src is less than or equal to the scalar value and packs each element's comparison result into 1 bit written to the destination address. It provides half and float source type overloads. The replacement is asc_le_scalar used together with asc_sync."
tag:
  - roles: [compute]
  - operations: [comparison]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "For the operand address overlap constraint, see the general-purpose address overlap constraint section on that page"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: related
    condition: "Works with the non-synchronous interface to complete synchronization"
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_compare/asc_le_scalar.md
    kind: replaced_by
    condition: "The non-synchronous comparison interface that replaces this synchronous interface"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "Both source type overloads carry the ASC_DEPRECATED(9.2.0, 2027/09/07, asc_le_scalar and asc_sync) marker (the macro lines before the two @override_id entries in include/c_api/composite/vector_compute_composite.h). The replacement is the non-synchronous comparison interface asc_le_scalar combined with asc_sync. All prototypes on this page fall within the deprecation scope, which is a page-wide deprecation with no upper availability limit."
---
# asc_le_scalar_sync (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T14:03:44.325Z pushedAt=2026-11-18T03:12:16.586Z nl=2 -->

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

**The `asc_le_scalar_sync` API is deprecated. Use the [corresponding non-synchronous API](../vector_compute/vector_compare/asc_le_scalar.md) and the synchronous API [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Determines element by element whether src <= value holds. If it holds, the output result is 1; otherwise, it is 0. The comparison result of each element occupies 1 bit.

## Prototype

- Synchronous computation

    ```cpp
    __aicore__ inline void asc_le_scalar_sync(__ubuf__ uint8_t* dst, __ubuf__ half* src, half value, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
    __aicore__ inline void asc_le_scalar_sync(__ubuf__ uint8_t* dst, __ubuf__ float* src, float value, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
    ```

## Parameters

**Table 1** Parameter description

|Parameter|Input/Output|Description|
| ------------ | ------------ | ------------ |
|dst|Output|Start address of the destination operand (vector).|
|src|Input|Start address of the source operand (vector).|
|value|Input|Source operand (scalar).|
| repeat | Input | Number of iterations. |
| dst_block_stride | Input | Address stride between different DataBlocks within a single iteration of the destination operand. |
| src_block_stride | Input | Address stride between different DataBlocks within a single iteration of the source operand. |
| dst_repeat_stride | Input | Address stride of the same DataBlock between adjacent iterations of the destination operand. |
| src_repeat_stride | Input | Address stride of the same DataBlock between adjacent iterations of the source operand. |

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).
- The start addresses of dst and src must be 32-byte aligned.
- dst is arranged into a binary result in little-endian order, corresponding to the comparison results of the data at the corresponding positions in src.
- When the result is output to the destination address, the mask parameter does not take effect.

## Examples

```cpp
// Output the result to the destination address, fixed to 128 elements.
constexpr uint32_t total_length = 128;
__ubuf__ uint8_t dst[total_length / 8];
__ubuf__ half src[total_length];
half scalar = 20;
uint8_t repeat = 1;
uint8_t dst_block_stride = 1;
uint8_t src_block_stride = 1;
uint8_t dst_repeat_stride = 8;
uint8_t src_repeat_stride = 8;
…… // Data transfer and synchronization operations
asc_le_scalar_sync(dst, src, scalar, repeat, dst_block_stride, src_block_stride, dst_repeat_stride, src_repeat_stride);
…… // Synchronization operation
```
