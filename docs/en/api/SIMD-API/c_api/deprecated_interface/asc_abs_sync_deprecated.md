---
okftype: reference.api
description: "Deprecated vector absolute value synchronization interface that computes the absolute value of count half or float elements in the Unified Buffer element by element, where the destination element equals the absolute value of the source element. It is supported only on Atlas A2 and A3, and the replacement is asc_abs combined with asc_sync."
tag:
  - roles: [compute]
  - operations: [absolute, synchronization]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: replaced_by
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_arith/asc_abs.md
    kind: replaced_by
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The entire synchronous composite interface family has entered the deprecation period, and the replacement is asc_abs combined with asc_sync; both declarations at include/c_api/composite/vector_compute_composite.h:1593,1606 carry ASC_DEPRECATED(9.2.0)."
---
# asc_abs_sync (deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T09:00:40.391Z pushedAt=2026-11-16T09:17:34.116Z nl=2 -->

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

This API is deprecated. Use [asc_abs](../vector_compute/vector_arith/asc_abs.md) together with the synchronization API [asc_sync](../sync/intra_core_sync/asc_sync.md) for fine-grained tuning to achieve optimal performance.

Computes the absolute value of each element. The calculation formula is as follows:
$$
dst_i = |src_i|
$$

## Prototype

- Synchronous computation

    ```cpp
    __aicore__ inline void asc_abs_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count)
    __aicore__ inline void asc_abs_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
    ```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| :----- | :------- | :------- |
| dst | Output | Start address of the destination operand (vector). |
| src | Input | Start address of the source operand (vector). |
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
//total_length indicates the length of the data involved in the computation.
constexpr uint32_t total_length = 128;
__ubuf__ half src[total_length];
__ubuf__ half dst[total_length];
asc_abs_sync(dst, src, total_length);
```
