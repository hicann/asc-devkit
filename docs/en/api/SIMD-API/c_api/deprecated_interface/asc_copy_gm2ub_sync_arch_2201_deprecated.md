---
okftype: reference.api
description: "Deprecated interface page: Under the 2201 architecture, asc_copy_gm2ub_sync synchronously moves GM data into UB using dst, src, and the byte count, with implicit sync wait. When moving the first n data items, the move size must be 32-byte aligned. It is recommended to use the non-synchronous asc_copy_gm2ub together with asc_sync instead."
tag:
  - roles: [data_movement]
  - operations: [copy]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "The space size and alignment requirements of each storage unit follow its storage unit description"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: related
    condition: "Handles the sync wait in the alternative approach"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync_notify.md
    kind: requires
    condition: "When the UB address overlaps with other instructions, a sync instruction must be inserted for serialization"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync_wait.md
    kind: requires
    condition: "When the UB address overlaps with other instructions, insert a sync instruction to serialize them"
  - path: docs/zh/api/SIMD-API/c_api/vector_datamove/asc_copy_gm2ub/asc_copy_gm2ub_arch_2201.md
    kind: replaced_by
    condition: "It is recommended to use the corresponding non-sync interface and explicitly call asc_sync"
version: ">=8.5.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The include header marks this sync interface with ASC_DEPRECATED(9.2.0). The replacement path is the non-sync asc_copy_gm2ub plus asc_sync; the interface has not been removed."
---
# asc_copy_gm2ub_sync (NPU Architecture Version 2201, Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T11:49:00.838Z pushedAt=2026-11-18T03:12:16.405Z nl=2 -->

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

**The `asc_copy_gm2ub_sync` API is deprecated. Use the [corresponding asynchronous API](../vector_datamove/asc_copy_gm2ub/asc_copy_gm2ub_arch_2201.md) and the synchronization API [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Moves data from Global Memory (GM) to Unified Buffer (UB).

## Prototype

- Synchronous computation

```cpp
__aicore__ inline void asc_copy_gm2ub_sync(__ubuf__ void* dst, __gm__ void* src, uint32_t size)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| dst | Output | Destination UB address. |
| src | Input | Source GM address. |
| size | Input | Size of the data to move (in bytes). |

## Return Value

None

## Pipeline Type

PIPE_MTE2

## Constraints

- For the space size and alignment requirements of each storage unit, see [Storage Unit Description](../general_description_and_constraints.md#storage-unit-description).
- If this instruction overlaps with other instructions in UB addresses, you must insert the synchronization instructions [asc_sync_notify](../sync/intra_core_sync/asc_sync_notify.md) and [asc_sync_wait](../sync/intra_core_sync/asc_sync_wait.md) to serialize multiple instructions and prevent abnormal data.
- Synchronous computation includes synchronous waiting.
- When the first-n data move API is used, the data size to move must be 32-byte aligned.

## Examples

```cpp
// total_length indicates the total length of the data to move. src is the externally input GM memory of the half type.
constexpr uint64_t total_length = 128;
__ubuf__ half dst[total_length];
asc_copy_gm2ub_sync(dst, src, total_length * sizeof(half));
```
