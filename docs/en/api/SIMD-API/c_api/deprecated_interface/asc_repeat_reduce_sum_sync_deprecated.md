---
okftype: reference.api
description: "Deprecated per-iteration synchronous reduction interface that sums elements within each Repeat using binary-tree pairwise addition. Half-precision intermediate results exceeding 65504 are stored with saturation. The replacement is asc_repeat_reduce_sum combined with asc_sync."
tag:
  - roles: [compute]
  - operations: [reduction, synchronization]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: replaced_by
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_reduce/asc_repeat_reduce_sum.md
    kind: replaced_by
version: ">=8.5.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The synchronous composite reduction interface is deprecated; use asc_repeat_reduce_sum and asc_sync instead. Both declarations in include/c_api/composite/vector_compute_composite.h carry ASC_DEPRECATED(9.2.0)."
---
# asc_repeat_reduce_sum_sync (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T14:41:00.970Z pushedAt=2026-11-18T03:12:16.645Z nl=2 -->

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

**The `asc_repeat_reduce_sum_sync` interface is deprecated. Use the [corresponding non-sync interface](../vector_compute/vector_reduce/asc_repeat_reduce_sum.md) together with the sync interface [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Sums all elements within each Repeat.

The source operands are added using a binary tree approach, with pairwise addition. Taking the summation of 256 half-precision data as an example, each Repeat can compute 128 half-precision data, which are divided into 2 Repeats for computation. Within each Repeat, pairwise addition is performed using a binary tree approach.

Note that during pairwise addition, when the computation result is greater than 65504, the result is saved as 65504. For example, if the source operands are [60000, 60000, -30000, 100], first 60000+60000 overflows and the result is 65504, then -30000+100=-29900 is computed, and finally 65504-29900=35604.

## Prototype

- Synchronous computation

    ```cpp
    __aicore__ inline void asc_repeat_reduce_sum_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count)
    __aicore__ inline void asc_repeat_reduce_sum_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
    ```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
|----|-----|-----|
| dst | Output | Start address of the destination operand (vector). |
| src  | Input | Start address of the source operand (vector).|
| count | Input | Number of elements involved in the computation. |

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).
- The starting address of dst must be 2-byte aligned when the input type is half, and 4-byte aligned when the input type is float.
- The starting address of src must be 32-byte aligned.

## Examples

```cpp
// Example: compute the sum of 256 half values in 2 iterations, producing 2 groups of summation results.
constexpr uint32_t total_length = 256;
__ubuf__ half src[total_length];
constexpr uint32_t dst_length = 2;
__ubuf__ half dst[dst_length];

// The sync interface uses count to specify the total number of elements involved in the computation, so there is no need to set the mask manually.
asc_repeat_reduce_sum_sync(dst, src, total_length);
```

Result example:

```
Input data src: [1, 1, 1, ..., 1]
Output data dst: [128, 128]
```
