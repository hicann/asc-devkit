---
okftype: reference.api
description: "Deprecated synchronous gather interface that gathers count elements from src into dst at the byte offset given by src_offset. It runs on PIPE_V and is supported only on the 2201 architecture. Use the non-synchronous asc_gather together with asc_sync instead."
tag:
  - roles: [compute]
  - operations: [gather]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: replaced_by
    condition: "Replaces the original synchronous wait semantics of this interface"
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_gather/asc_gather.md
    kind: replaced_by
    condition: "Replaces the offset-based gather capability of this interface; synchronization must be inserted separately"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "All overloads of this interface in include/c_api/composite/vector_compute_composite.h are marked ASC_DEPRECATED(9.2.0, \"2027/09/07\", asc_gather and asc_sync), and the body explicitly states to use asc_gather plus asc_sync instead; the declaration is still retained in 9.3.0-beta.1, so only deprecation is registered, not removal, and no upper availability limit is set."
---
# asc_gather_sync (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T13:17:23.348Z pushedAt=2026-11-18T03:12:16.508Z nl=2 -->

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

**The `asc_gather_sync` interface is deprecated. Use the [corresponding non-synchronous interface](../vector_compute/vector_gather/asc_gather.md) and the synchronous interface [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Gathers elements from src into dst according to the offsets given by src_offset.

## Prototype

- Synchronous computation

    ```cpp
    __aicore__ inline void asc_gather_sync(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, __ubuf__ uint32_t* src_offset, uint32_t count)
    __aicore__ inline void asc_gather_sync(__ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, __ubuf__ uint32_t* src_offset, uint32_t count)
    __aicore__ inline void asc_gather_sync(__ubuf__ half* dst, __ubuf__ half* src, __ubuf__ uint32_t* src_offset, uint32_t count)
    __aicore__ inline void asc_gather_sync(__ubuf__ bfloat16_t* dst, __ubuf__ bfloat16_t* src, __ubuf__ uint32_t* src_offset, uint32_t count)
    __aicore__ inline void asc_gather_sync(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, __ubuf__ uint32_t* src_offset, uint32_t count)
    __aicore__ inline void asc_gather_sync(__ubuf__ uint32_t* dst, __ubuf__ uint32_t* src, __ubuf__ uint32_t* src_offset, uint32_t count)
    __aicore__ inline void asc_gather_sync(__ubuf__ float* dst, __ubuf__ float* src, __ubuf__ uint32_t* src_offset, uint32_t count)
    ```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| :----- | :------- | :------- |
| dst | Output | Start address of the destination operand (vector). |
| src | Input | Start address of the source operand (vector). |
| src_offset | Input | Address offset corresponding to each element of the source operand. The offset is relative to the start base address of the source operand, in bytes. |
| count | Input | Number of elements involved in the computation. |

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- The offset values in src_offset must ensure that the element type bit width of src is aligned.
- The address obtained by adding src_offset to src must not exceed the range of the Unified Buffer (UB) data size.
- The start addresses of dst and src must be 32-byte aligned.
- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).

## Examples

```cpp
constexpr uint64_t total_length = 128;    // total_length refers to the data length involved in the computation.
__ubuf__ half src[total_length];
__ubuf__ uint32_t src_offset[total_length];
__ubuf__ half dst[total_length];
asc_gather_sync(dst, src, src_offset, total_length);
// src [1, 2, 3, ..., 126, 127, 128], 128 half-type data elements.
// src_offset [254, 252, 250, ..., 4, 2, 0], the offset of each element in src, based on the start base address of src, in bytes.
// dst [128, 127, 126, ..., 3, 2, 1]
```
