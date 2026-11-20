---
okftype: reference.api
description: "Deprecated mask gather synchronous interface that selects elements from the source operand and writes them to the destination operand according to a built-in fixed pattern or a user-defined gather mask. Supports uint16_t and uint32_t. The replacement is asc_reduce combined with asc_sync."
tag:
  - roles: [compute]
  - operations: [gather, synchronization]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: replaced_by
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_reduce/asc_reduce.md
    kind: replaced_by
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The synchronous composite interface is deprecated; use asc_reduce and asc_sync instead. Both declarations in include/c_api/composite/vector_compute_composite.h carry ASC_DEPRECATED(9.2.0)."
---
# asc_reduce_sync (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T14:37:44.460Z pushedAt=2026-11-18T03:12:16.639Z nl=2 -->

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

**The `asc_reduce_sync` interface is deprecated. Use the [corresponding non-synchronous interface](../vector_compute/vector_reduce/asc_reduce.md) and the synchronous interface [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Selects elements from the source operand and writes them to the destination operand based on the binary value corresponding to a built-in fixed pattern or the gather mask (data collection mask) corresponding to a user-defined input value.

## Prototype

- Synchronous computation

  ```cpp
  __aicore__ inline void asc_reduce_sync(__ubuf__ uint16_t* dst, __ubuf__ uint16_t* src0, __ubuf__ uint16_t* src1, uint32_t count)
  __aicore__ inline void asc_reduce_sync(__ubuf__ uint32_t* dst, __ubuf__ uint32_t* src0, __ubuf__ uint32_t* src1, uint32_t count)
  ```

## Parameters

**Table 1** Parameter description

| Parameter                | Input/Output | Description                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   |
|:-------------------| :-----|:-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| dst                | Output | Start address of the destination operand (vector).                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      |
| src0               | Input | Start address of the source operand (vector).                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       |
| src1               | Input | In user-defined mode, the user-defined data collection mask.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            |
| count              | Input | Number of elements involved in the computation.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           |

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- The interface does not support mask configuration through [asc_set_vector_mask](../vector_compute/vector_mask_config/asc_set_vector_mask.md).
- The start addresses of dst, src0, and src1 must be 32-byte aligned.
- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).

## Examples

```cpp
//total_length refers to the total length of the data involved in the computation.
constexpr uint64_t total_length = 128;
__ubuf__ uint16_t src0[total_length];
__ubuf__ uint16_t src1[total_length/16];
__ubuf__ uint16_t dst[total_length];
asc_reduce_sync(dst, src0, src1, total_length);
```
