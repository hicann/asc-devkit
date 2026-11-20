---
okftype: reference.api
description: "Deprecated synchronous quantization conversion interface description for int16_t to int8_t or uint8_t, with the result written to the upper or lower half block of the DataBlock. The replacement is the non-synchronous interface combined with asc_sync."
tag:
  - roles: [compute]
  - operations: [conversion]
  - traits: [datablock_granularity, elementwise, quantization]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: replaced_by
    condition: "Synchronous semantics are now expressed explicitly using asc_sync"
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_fused/asc_vdeq_int162b8.md
    kind: replaced_by
    condition: "Use the non-synchronous interface asc_vdeq_int162b8 instead"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "vector_compute_composite.h adds ASC_DEPRECATED(9.2.0) to the four _h_sync/_l_sync overloads, replaced by the non-sync asc_vdeq_int162b8_h/_l plus asc_sync; deprecated only, not removed, with no upper availability limit set."
---
# asc_vdeq_int162b8_sync (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T15:22:38.958Z pushedAt=2026-11-18T03:12:16.705Z nl=2 -->

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

**The `asc_vdeq_int162b8_h_sync` and `asc_vdeq_int162b8_l_sync` interfaces are deprecated. Use the [corresponding non-sync interfaces](../vector_compute/vector_fused/asc_vdeq_int162b8.md) together with the sync interface [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Converts int16_t data to int8_t or uint8_t, and stores the data in the upper half or lower half of each DataBlock. Before using this interface, call asc_set_deq_scale to set the quantization parameters.

- asc_vdeq_int162b8_h: Stores the data in the upper half of each DataBlock.
- asc_vdeq_int162b8_l: Stores the data in the lower half of each DataBlock.
The calculation formula is as follows:

$$
dst_i = (src_i * scale_j) + offset_j
$$

## Prototype

- Synchronous computation

    ```cpp
    __aicore__ inline void asc_vdeq_int162b8_h_sync(__ubuf__ int8_t* dst, __ubuf__ int16_t* src, uint32_t count)
    __aicore__ inline void asc_vdeq_int162b8_h_sync(__ubuf__ uint8_t* dst, __ubuf__ int16_t* src, uint32_t count)
    __aicore__ inline void asc_vdeq_int162b8_l_sync(__ubuf__ int8_t* dst, __ubuf__ int16_t* src, uint32_t count)
    __aicore__ inline void asc_vdeq_int162b8_l_sync(__ubuf__ uint8_t* dst, __ubuf__ int16_t* src, uint32_t count)
    ```

## Parameters

**Table 1** Parameter description

| Parameter  | Input/Output | Description |
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
// total_length indicates the data length involved in the computation.
constexpr uint32_t total_length = 32;
__ubuf__ int8_t dst[total_length];
__ubuf__ int16_t src[total_length];
asc_vdeq_int162b8_h_sync(dst, src, total_length);
asc_vdeq_int162b8_l_sync(dst, src, total_length);
```
