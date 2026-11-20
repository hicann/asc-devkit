---
okftype: reference.api
description: "Deprecated synchronous adjacent element sum interface that adds each pair of odd-even adjacent elements in the source vector and writes the results to the destination vector. It supports half and float. The replacement is asc_pair_reduce_sum combined with asc_sync."
tag:
  - roles: [compute]
  - operations: [reduction, synchronization]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: replaced_by
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_reduce/asc_pair_reduce_sum.md
    kind: replaced_by
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The synchronous composite reduction interface is deprecated; use asc_pair_reduce_sum and asc_sync instead. Both declarations in include/c_api/composite/vector_compute_composite.h carry ASC_DEPRECATED(9.2.0)."
---
# asc_pair_reduce_sum_sync (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T14:36:18.942Z pushedAt=2026-11-18T03:12:16.636Z nl=2 -->

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

**The `asc_pair_reduce_sum_sync` API is deprecated. Use the [corresponding non-synchronous API](../vector_compute/vector_reduce/asc_pair_reduce_sum.md) and the synchronous API [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Performs a reduction operation on the input data to obtain the total sum. Adjacent two (odd-even) elements are summed, for example, (a1, a2, a3, a4, a5, a6...), the sum of adjacent two data elements is (a1+a2, a3+a4, a5+a6...).

## Prototype

- Synchronous computation

    ```cpp
    __aicore__ inline void asc_pair_reduce_sum_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count)
    __aicore__ inline void asc_pair_reduce_sum_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
    ```

## Parameters

**Table 1** Parameter description

|Parameter|Input/Output|Description|
|------------|------------|-----------|
| dst     | Output     | Start address of the destination operand (vector). |
| src     | Input     | Start address of the source operand (vector). |
| count   | Input     | Number of elements involved in continuous computation. |

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).
- The start addresses of dst and src must be 32-byte aligned.

## Examples

```cpp
// Example: compute the adjacent element sum of 256 half values, completed in 2 iterations, each iteration processes 128 elements and outputs 64 results.
constexpr uint32_t src_length = 256;
constexpr uint32_t dst_length = 128;
__ubuf__ half src[src_length];
__ubuf__ half dst[dst_length];

// The synchronous API uses count to specify the total number of elements involved in the computation, without manually setting the mask.
asc_pair_reduce_sum_sync(dst, src, src_length);
```

Result example:

```
Input data src: [1 2 3 ... 16 17 ... 32 ... 225... 256]
Output data dst: [3 7 ... 511]
```
