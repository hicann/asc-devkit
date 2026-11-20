---
okftype: reference.api
description: "Deprecated DataBlock-granularity gather synchronization interface. It gathers a uint16_t or uint32_t input vector into the target vector in 32-byte blocks according to the address offset given by src_offset and waits for PIPE_V. The alternative is asc_gather_datablock combined with asc_sync."
tag:
  - roles: [data_movement]
  - operations: [gather]
  - traits: [datablock_granularity]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: requires
    condition: "After switching to the non-synchronization interface, you must call asc_sync yourself to complete synchronization."
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_gather/asc_gather_datablock.md
    kind: replaced_by
    condition: "The non-synchronization gather interface replaces the synchronization interface on this page."
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The include header uniformly marks the two overloads for uint16_t and uint32_t as ASC_DEPRECATED(9.2.0, 2027/09/07, asc_gather_datablock and asc_sync); the CANN_9_2_0 Release Notes deprecation table covers this class of *_sync composite interfaces; the interface is retained and available only on 2201."
---
# asc_gather_datablock_sync (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T13:16:51.862Z pushedAt=2026-11-18T03:12:16.509Z nl=2 -->

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

**The `asc_gather_datablock_sync` API is deprecated. Use the [corresponding non-synchronous API](../vector_compute/vector_gather/asc_gather_datablock.md) and the synchronous API [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Given an input vector and an address offset vector, gathers the input vector into the destination vector at the DataBlock granularity according to the offset addresses.

## Prototype

- Synchronous computation

  ```cpp
  __aicore__ inline void asc_gather_datablock_sync(__ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, __ubuf__ uint32_t* src_offset, uint16_t dst_repeat_stride, uint8_t dst_block_stride, uint8_t repeat)
  __aicore__ inline void asc_gather_datablock_sync(__ubuf__ uint32_t* dst, __ubuf__ uint32_t* src, __ubuf__ uint32_t* src_offset, uint16_t dst_repeat_stride, uint8_t dst_block_stride, uint8_t repeat)
  ```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| :----- | :------- | :------- |
| dst | Output | Start address of the destination operand (vector). |
| src | Input | Start address of the source operand (vector). |
| src_offset | Input | Address offset of each DataBlock in the source operand. |
| dst_repeat_stride | Input | Address stride of the same DataBlock between adjacent iterations of the destination operand. |
| dst_block_stride | Input | Address stride between different DataBlocks within a single iteration of the destination operand. |
| repeat | Input | Number of iterations. |

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- The value of src_offset must ensure bit-width alignment of the src element type.
- The address after adding src_offset to src must not exceed the range of the Unified Buffer (UB) size.
- The start addresses of dst and src must be 32-byte aligned.
- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).

## Examples

```cpp
constexpr uint64_t total_length = 128;    // total_length is the length of data involved in the computation.
__ubuf__ uint32_t src_offset[total_length * sizeof(uint32_t) / 32];
__ubuf__ uint32_t dst[total_length];
__ubuf__ uint32_t src[total_length];
asc_gather_datablock_sync(dst, src, src_offset, 8, 1, 2);
// src [1, 2, 3, ..., 126, 127, 128] 128 uint32_t data
// src_offset [0, 32, 64, ..., 416, 448, 480]
// dst [1, 2, 3, ..., 126, 127, 128]
```
