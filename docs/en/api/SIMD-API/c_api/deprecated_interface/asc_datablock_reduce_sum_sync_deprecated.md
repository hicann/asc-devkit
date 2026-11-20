---
okftype: reference.api
description: "Deprecated synchronous reduction sum API within DataBlock. Elements in each 32-byte data block are added pairwise in a binary tree, and half overflow is saturated to 65504. The alternative is asc_datablock_reduce_sum combined with asc_sync."
tag:
  - roles: [compute]
  - operations: [reduction]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "generic address overlap constraint"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: replaced_by
    condition: "synchronous part of the alternative"
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_reduce/asc_datablock_reduce_sum.md
    kind: replaced_by
    condition: "asynchronous API declared as the alternative in the body"
version: ">=8.5.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "Since 9.2.0, include marks ASC_DEPRECATED(9.2.0, \"2027/09/07\", asc_datablock_reduce_sum and asc_sync), and the body provides the replacement accordingly; the declaration is retained, so this is deprecated rather than removed."
---
# asc_datablock_reduce_sum_sync (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T12:44:02.523Z pushedAt=2026-11-18T03:12:16.459Z nl=2 -->

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

**The `asc_datablock_reduce_sum_sync` API is deprecated. Use the [corresponding non-synchronous API](../vector_compute/vector_reduce/asc_datablock_reduce_sum.md) together with the synchronous API [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Sums all elements in each DataBlock.

The source operands are added using a binary tree, with pairwise addition. Taking the summation of 128 half-type data as an example, each DataBlock can compute 16 half-type data, which are divided into 8 DataBlocks for computation. Within each DataBlock, pairwise addition is performed using a binary tree.

Note that during pairwise addition, when the computation result is greater than 65504, the result is saved as 65504. For example, if the source operands are [60000, 60000, -30000, 100], first 60000+60000 overflows and the result is 65504, then -30000+100=-29900 is computed, and finally 65504-29900=35604 is computed.

## Prototype

- Synchronous computation

    ```cpp
    __aicore__ inline void asc_datablock_reduce_sum_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count)
    __aicore__ inline void asc_datablock_reduce_sum_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
    ```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| :----| :-----| :-----|
| dst | Output | Start address of the destination operand (vector). |
| src  | Input | Start address of the source operand (vector). |
| count | Input | Number of elements involved in the computation. |

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- The start addresses of dst and src must be 32-byte aligned.
- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).

## Examples

```cpp
constexpr uint32_t src_length = 256;
constexpr uint32_t dst_length = 16;
__ubuf__ half src[src_length];
__ubuf__ half dst[dst_length];

// The synchronous API uses count to specify the total number of elements involved in the computation, without the need to manually set the mask.
asc_datablock_reduce_sum_sync(dst, src, src_length);
```

Result example:

```
Input data src: [1 1 1 ... 1]
Output data dst: [16 16 16 ... 16]
```
