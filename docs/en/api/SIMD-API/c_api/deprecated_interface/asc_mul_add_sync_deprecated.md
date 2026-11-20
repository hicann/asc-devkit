---
okftype: reference.api
description: "Deprecated vector multiply-add synchronous interface that computes element by element using the formula src0 multiplied by dst plus src1 and writes the result back to dst. It supports half and float. The replacement is asc_mul_add combined with asc_sync."
tag:
  - roles: [compute]
  - operations: [fused_multiply_add, synchronization]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: replaced_by
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_fused/asc_mul_add.md
    kind: replaced_by
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The synchronous composite interface is deprecated; use asc_mul_add and asc_sync instead. Both declarations in include/c_api/composite/vector_compute_composite.h carry ASC_DEPRECATED(9.2.0)."
---
# asc_mul_add_sync (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T14:23:54.708Z pushedAt=2026-11-18T03:12:16.615Z nl=2 -->

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

**The `asc_mul_add_sync` interface is deprecated. Use the [corresponding non-synchronous interface](../vector_compute/vector_fused/asc_mul_add.md) and the synchronous interface [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Performs vector multiply-add operation. The calculation formula is as follows:

$$
dst_i = (src0_i * dst_i) + src1_i
$$

## Prototype

- Synchronous computation

    ```cpp
    __aicore__ inline void asc_mul_add_sync(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count)
    __aicore__ inline void asc_mul_add_sync(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count)
    ```

## Parameters

**Table 1** Parameter description

| Parameter  | Input/Output | Description |
| :----- | :------- | :------- |
| dst | Output | Start address of the destination operand (vector). |
| src0 | Input | Start address of the source operand (vector). |
| src1 | Input | Start address of the source operand (vector). |
| count | Input | Number of elements involved in the computation. |

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- The start addresses of dst, src0, and src1 must be 32-byte aligned.
- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).

## Examples

```cpp
constexpr uint64_t total_length = 128;    // total_length refers to the data length involved in the calculation.
__ubuf__ half src0[total_length];
__ubuf__ half src1[total_length];
__ubuf__ half dst[total_length];
asc_mul_add_sync(dst, src0, src1, total_length);    // dst = (src0 * dst) + src1
// src0 [1, 2, 3, ..., 128]
// src1 [1, 1, 1, ..., 1]
// Before calculation, dst [2, 2, 2, ..., 2]
// After calculation, dst [3, 5, 7, ..., 257]
```
