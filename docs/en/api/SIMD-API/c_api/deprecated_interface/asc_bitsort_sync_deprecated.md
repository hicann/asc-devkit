---
okftype: reference.api
description: "Deprecated bitonic sort synchronous interface. Each iteration sorts 32 elements, with scores stored in src0 and indexes stored in src1. It writes scores and indexes in pairs to dst in descending order of scores. The replacement is asc_bitsort combined with asc_sync."
tag:
  - roles: [compute]
  - operations: [sort, synchronization]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: replaced_by
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_sort/asc_bitsort.md
    kind: replaced_by
version: ">=9.0.0"
version_changes:
  - since: "9.1.0"
    kind: arch_support_extended
    scope: "page"
    scope_basis: kind_default
    note: "Added 3510 (Ascend 950PR&950DT) implementation: history_probe shows that asc_bitsort_sync in npu_arch_3510/vector_compute_impl.h has appeared since 9.1.0, and CANN_9_1_0 \"New Features\" lists this interface for 3510; in 9.0.0 only 2201 (Atlas A2/A3) was available."
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The synchronous composite interface is deprecated; use asc_bitsort and asc_sync instead. Both declarations in include/c_api/composite/vector_compute_composite.h:177,192 carry ASC_DEPRECATED(9.2.0)."
---
# asc_bitsort_sync (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T11:22:09.128Z pushedAt=2026-11-18T03:12:16.372Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
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

**The `asc_bitsort_sync` interface is deprecated. Use the [corresponding non-synchronous interface](../vector_compute/vector_sort/asc_bitsort.md) and the synchronous interface [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Bitonic sort function. One iteration can sort 32 elements, and the data is stored in the structure described as follows:

Score and Index are stored in src0 and src1 respectively, and sorted by Score (elements with larger Score come first). The sorted Score and its corresponding Index are stored together in dst in the (Score, Index) structure.

## Prototype

- Synchronous computation

  ```cpp
  __aicore__ inline void asc_bitsort_sync(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ uint32_t* src1, int32_t repeat)
  __aicore__ inline void asc_bitsort_sync(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ uint32_t* src1, int32_t repeat)
  ```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| --------- | ----- | ----------------- |
| dst | Output | Start address of the destination operand (vector). |
| src0 | Input | Start address of the source operand (vector). |
| src1 | Input | Start address of the source operand (vector). |
| repeat | Input | Number of repeated iterations. Each iteration sorts 32 elements. In the next iteration, src0 and src1 each skip 32 elements, and dst skips 256 Byte of space. Value range: [0, 255]. |

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).
- The start addresses of dst and src must be 32-byte aligned.

## Examples

```cpp
__ubuf__ half dst[256];
__ubuf__ half src0[256];
__ubuf__ uint32_t src1[256];
asc_bitsort_sync(dst, src0, src1, 8);
```
