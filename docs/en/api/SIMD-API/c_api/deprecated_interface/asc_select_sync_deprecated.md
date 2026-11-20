---
okftype: reference.api
description: "Deprecated vector selection synchronization interface. It selects elements bit by bit from two source vectors according to the sel_mask set by asc_set_cmp_mask and writes them to the destination vector. It supports half and float. The replacement is asc_select combined with asc_sync."
tag:
  - roles: [compute]
  - operations: [selection, synchronization]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: replaced_by
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_compare/asc_set_cmp_mask.md
    kind: constrained_by
    condition: "The selection basis sel_mask must first be set through this interface"
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_permute_sel/asc_select.md
    kind: replaced_by
version: ">=8.5.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The synchronous composite interface is deprecated; use asc_select and asc_sync instead. Both declarations starting from include/c_api/composite/vector_compute_composite.h:2262 carry ASC_DEPRECATED(9.2.0)."
---
# asc_select_sync (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T14:43:29.387Z pushedAt=2026-11-18T03:12:16.650Z nl=2 -->

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

**The `asc_select_sync` interface is deprecated. Use the [corresponding non-synchronous interface](../vector_compute/vector_permute_sel/asc_select.md) and the synchronous interface [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Performs a vector selection operation. Given two source operands src0 and src1, it selects elements based on conditions to obtain the destination operand dst. This interface is used together with [asc_set_cmp_mask(sel_mask)](../vector_compute/vector_compare/asc_set_cmp_mask.md). The rule is: when a bit of sel_mask is 1, the element is selected from src0; when the bit is 0, the element is selected from src1.

## Prototype

- Synchronous computation

    ```cpp
    __aicore__ inline void asc_select_sync(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count)
    __aicore__ inline void asc_select_sync(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count)
    ```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| dst  | Output  | Start address of the destination operand (vector). |
| src0 | Input  | Start address of source operand 0 (vector). |
| src1 | Input  | Start address of source operand 1 (vector). |
| count | Input    | Number of elements involved in the computation. |

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- Must be used together with the [asc_set_cmp_mask](../vector_compute/vector_compare/asc_set_cmp_mask.md) interface.
- The start addresses of dst, src0, and src1 must be 32-byte aligned.
- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).

## Examples

```cpp
constexpr uint32_t total_length = 256;
constexpr uint32_t sel_length = 8;
__ubuf__ uint8_t sel_mask[sel_length];
__ubuf__ float dst[total_length];
__ubuf__ float src0[total_length];
__ubuf__ float src1[total_length];
asc_set_cmp_mask(sel_mask);
asc_sync_pipe(PIPE_V);
asc_select_sync(dst, src0, src1, total_length);
```
