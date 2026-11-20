---
okftype: reference.api
description: "Deprecated synchronous version of the UB-to-GM unaligned copy interface, supporting 8/16/32/64-bit block copy and automatically discarding dummy padding data, targeting Atlas A3 and Atlas A2, with the replacement being asc_copy_ub2gm_align plus asc_sync."
tag:
  - roles: [data_movement]
  - operations: [store]
  - traits: [unaligned_transfer]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "storage unit space size and alignment requirements"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: replaced_by
    condition: "the synchronization part of the replacement"
  - path: docs/zh/api/SIMD-API/c_api/vector_datamove/asc_copy_ub2gm_align/asc_copy_ub2gm_align_arch_2201.md
    kind: replaced_by
    condition: "The replacement non-sync interface declared in the body text"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "Since 9.2.0, the include is marked ASC_DEPRECATED(9.2.0, \"2027/09/07\", asc_copy_ub2gm_align and asc_sync); the declaration is retained, so it is deprecated rather than removed, and the page has no upper availability limit set."
---
# asc_copy_ub2gm_align_sync (NPU architecture version 2201, deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T12:35:34.967Z pushedAt=2026-11-18T03:12:16.445Z nl=2 -->

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

**The `asc_copy_ub2gm_align_sync` API is deprecated. Use the [corresponding non-synchronous API](../vector_datamove/asc_copy_ub2gm_align/asc_copy_ub2gm_align_arch_2201.md) and the synchronous API [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Copies data from Unified Buffer (UB) to Global Memory (GM), supporting 8-bit/16-bit/32-bit block copy operations.

This API supports the following two data copy modes, neither of which supports setting a fill value:

- First n data copy

    If the data length to be copied is not 32-byte aligned, dummy data is automatically padded when reading UB data during the copy-out, padding the copied data to 32-byte alignment. The value of the dummy data is the value of the first element of the data block. The padded dummy data is discarded when writing to GM.

- High-dimensional split copy

    High-dimensional split copy uses Normal mode. If the length of a single data block specified by `burst_len` is not 32-byte aligned, each data block specified by `burst_count` is padded with dummy data to 32-byte alignment when reading UB data. The value of the dummy data is the value of the first element of the data block. The padded dummy data is discarded when writing to GM.

## Prototype

- Synchronous computation

    ```cpp
    __aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ uint8_t* dst, __ubuf__ uint8_t* src, uint32_t size)
    __aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ int8_t* dst, __ubuf__ int8_t* src, uint32_t size)
    __aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ half* dst, __ubuf__ half* src, uint32_t size)
    __aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ uint16_t* dst, __ubuf__ uint16_t* src, uint32_t size)
    __aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ int16_t* dst, __ubuf__ int16_t* src, uint32_t size)
    __aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ bfloat16_t* dst, __ubuf__ bfloat16_t* src, uint32_t size)
    __aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ uint32_t* dst, __ubuf__ uint32_t* src, uint32_t size)
    __aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ float* dst, __ubuf__ float* src, uint32_t size)
    __aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ int32_t* dst, __ubuf__ int32_t* src, uint32_t size)
    __aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ double* dst, __ubuf__ double* src, uint32_t size)
    __aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ int64_t* dst, __ubuf__ int64_t* src, uint32_t size)
    __aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ uint64_t* dst, __ubuf__ uint64_t* src, uint32_t size)
    ```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| dst | Output | Start address of the destination GM. Requires 1-byte alignment. |
| src | Input | Start address of the source UB. Requires 32-byte alignment. |
| size | Input | Size of the data to be copied, in bytes. Value range: [0, 2097151]. |

## Return Value

None

## Pipeline Type

PIPE_MTE3

## Constraints

- For the space size and alignment requirements of each storage unit, see [Storage Unit Description](../general_description_and_constraints.md#storage-unit-description).
- When either `burst_count` or `burst_len` is 0, this API is treated as a NOP (no operation).
- When `size` is 0, this API is treated as a NOP (no operation).
- If this instruction overlaps with other instructions in UB addresses, you must insert the synchronization instructions [asc_sync_notify](../sync/intra_core_sync/asc_sync_notify.md) and [asc_sync_wait](../sync/intra_core_sync/asc_sync_wait.md) to serialize multiple instructions and prevent abnormal data.

## Examples

```cpp
// total_length refers to the total length of the data involved in the copy. dst is the externally input GM memory of the uint8_t type.
constexpr uint64_t total_length = 128;
__ubuf__ uint8_t src[total_length];
asc_copy_ub2gm_align_sync(dst, src, total_length * sizeof(uint8_t));
```
