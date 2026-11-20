---
okftype: reference.api
description: "Deprecated vector broadcast synchronization interface. Each time, it takes 8 elements of the source vector and fills them into the 8 32-byte DataBlocks of the destination respectively. It supports uint16_t and uint32_t and requires passing the block stride, repeat stride, and repeat count item by item."
tag:
  - roles: [compute]
  - operations: [broadcast, synchronization]
  - traits: [datablock_granularity]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: replaced_by
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_broadcast/asc_brcb.md
    kind: replaced_by
version: ">=8.5.0"
version_changes:
  - since: "9.0.0"
    kind: parameter_changed
    scope: "page"
    scope_basis: kind_default
    note: "The parameter form changed from const asc_brcb_config& config in 8.5.0 to passing uint16_t dst_block_stride, uint16_t dst_repeat_stride, and uint8_t repeat item by item; history_probe shows that the asc_brcb_config& form no longer appears after 8.5.0, and the item-by-item parameter declarations have appeared in the public header since 9.0.0-beta.1."
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The synchronous composite interface is deprecated; use asc_brcb and asc_sync instead. Both item-by-item parameter declarations in include/c_api/composite/vector_compute_composite.h:143,160 carry ASC_DEPRECATED(9.2.0)."
---
# asc_brcb_sync (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T11:22:23.889Z pushedAt=2026-11-18T03:12:16.373Z nl=2 -->

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

**The `asc_brcb_sync` interface is deprecated. Use the [corresponding non-synchronous interface](../vector_compute/vector_broadcast/asc_brcb.md) and the synchronous interface [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Given an input vector, each time 8 numbers from the input vector are taken and filled into the 8 datablocks (32 bytes) of the result vector, with each number corresponding to one datablock.

## Prototype

- Synchronous high-dimensional split computation

    ```cpp
    __aicore__ inline void asc_brcb_sync(__ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, uint16_t dst_block_stride, uint16_t dst_repeat_stride, uint8_t repeat)
    __aicore__ inline void asc_brcb_sync(__ubuf__ uint32_t* dst, __ubuf__ uint32_t* src, uint16_t dst_block_stride, uint16_t dst_repeat_stride, uint8_t repeat)
    ```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| dst | Output | Start address of the destination operand (vector). |
| src | Input | Start address of the source operand (vector). |
| dst_block_stride | Input | Address stride between different DataBlocks within a single iteration of the target operand. |
| dst_repeat_stride | Input | Address stride of the same DataBlock between adjacent iterations of the target operand. |
| repeat | Input | Number of iterations. |

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- src and dst cannot be the same memory address.

## Examples

```cpp
constexpr uint32_t src_length = 16;
constexpr uint32_t dst_length = 256;
__ubuf__ uint16_t src[src_length];
__ubuf__ uint16_t dst[dst_length];
asc_brcb_sync(dst, src, 1, 8, 2);
```

Result example:

```
Input data src: [1 2 3 ... 16]
Output data dst: [1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 ... 15 15 15 15 15 15 15 15 15 15 15 15 15 15 15 15 16 16 16 16 16 16 16 16 16 16 16 16 16 16 16 16]
```
