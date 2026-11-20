---
okftype: reference.api
description: "Deprecated fused multiply-add synchronous interface that computes the product of src0 and src1 element-wise, adds it to dst, and waits for PIPE_V. It supports half and float combinations. The replacement is asc_fma used together with asc_sync."
tag:
  - roles: [compute]
  - operations: [fused_multiply_add]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: requires
    condition: "After switching to the non-sync interface, you must call asc_sync to complete synchronization."
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_fused/asc_fma.md
    kind: replaced_by
    condition: "The non-sync fused multiply-add interface replaces the synchronous interface on this page."
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The include header uniformly marks the three overloads (half, float×half, and float) with ASC_DEPRECATED(9.2.0, 2027/09/07, asc_fma and asc_sync); the CANN_9_2_0 Release Notes deprecation table covers this class of *_sync composite interfaces; the interface is retained and available only in 2201."
---
# asc_fma_sync (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T13:15:23.272Z pushedAt=2026-11-18T03:12:16.506Z nl=2 -->

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

**The `asc_fma_sync` interface is deprecated. Use the [corresponding non-synchronous interface](../vector_compute/vector_fused/asc_fma.md) and the synchronous interface [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Multiplies src0 and src1 element-wise and adds the result to dst, then stores the final result into dst. The calculation formula is as follows:
$$
dst_i = (src0_i * src1_i) + dst_i
$$

## Prototype

- Synchronous computation

  ```cpp
  __aicore__ inline void asc_fma_sync(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count)
  __aicore__ inline void asc_fma_sync(__ubuf__ float* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count)
  __aicore__ inline void asc_fma_sync(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count)
  ```

## Parameters

**Table 1** Parameter description

| Parameter       | Input/Output | Description                |
| --------- | ----- | ----------------- |
| dst       | Output    | Start address of the destination operand (vector). |
| src0      | Input    | Start address of the source operand (vector). |
| src1      | Input    | Start address of the source operand (vector). |
| count     | Input    | Number of elements involved in the computation.        |

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).
- The start addresses of dst, src0, and src1 must be 32-byte aligned.

## Examples

```cpp
__ubuf__ half dst[256];
__ubuf__ half src0[256];
__ubuf__ half src1[256];
asc_fma_sync(dst, src0, src1, 256);
```
