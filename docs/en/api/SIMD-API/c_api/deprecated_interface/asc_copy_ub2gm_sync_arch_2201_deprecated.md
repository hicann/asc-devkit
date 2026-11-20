---
okftype: reference.api
description: "Deprecated synchronous UB-to-GM copy interface that copies contiguous data by byte count and completes the synchronous wait before returning, for Atlas A3 and Atlas A2. The replacement is asc_copy_ub2gm plus asc_sync."
tag:
  - roles: [data_movement]
  - operations: [store]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "Storage unit space size and alignment requirements"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: replaced_by
    condition: "The synchronization part of the replacement"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync_notify.md
    kind: requires
    condition: "Synchronization instruction to insert when there is UB address overlap with other instructions"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync_wait.md
    kind: requires
    condition: "Synchronization instruction that must be inserted when there is UB address overlap with other instructions"
  - path: docs/zh/api/SIMD-API/c_api/vector_datamove/asc_copy_ub2gm/asc_copy_ub2gm_arch_2201.md
    kind: replaced_by
    condition: "The replacement non-synchronous interface declared in the body text"
version: ">=8.5.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "Starting from 9.2.0, the include marker ASC_DEPRECATED(9.2.0, \"2027/09/07\", asc_copy_ub2gm and asc_sync) is added, and the body text provides the replacement solution; the declaration is retained, so it is deprecated rather than removed, and the page does not set an upper availability limit."
---
# asc_copy_ub2gm_sync (NPU architecture version 2201, deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T12:37:17.393Z pushedAt=2026-11-18T03:12:16.448Z nl=2 -->

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

**The `asc_copy_ub2gm_sync` interface is deprecated. Use the [corresponding non-synchronous interface](../vector_datamove/asc_copy_ub2gm/asc_copy_ub2gm_arch_2201.md) and the synchronous interface [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Copies data from Unified Buffer (UB) to Global Memory (GM).

## Prototype

- Synchronous computation

```cpp
__aicore__ inline void asc_copy_ub2gm_sync(__gm__ void* dst, __ubuf__ void* src, uint32_t size)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| dst | Output | Destination GM address. |
| src | Input | Source UB address. |
| size | Input | Size of data to copy (in bytes). |

## Return Value

None

## Pipeline Type

PIPE_MTE3

## Constraints

- For the space size and alignment requirements of each storage unit, see [Storage Unit Description](../general_description_and_constraints.md#storage-unit-description).
- If this instruction has UB address overlap with other instructions, you must insert the synchronization instructions [asc_sync_notify](../sync/intra_core_sync/asc_sync_notify.md) and [asc_sync_wait](../sync/intra_core_sync/asc_sync_wait.md) to serialize multiple instructions and prevent abnormal data.
- Synchronous computation includes synchronous waiting.
- When the first n data copy interfaces are used, the size of data to copy must be 32-byte aligned.

## Examples

```cpp
// total_length indicates the total length of data involved in the copy. dst is the externally input GM memory of the half type.
constexpr uint64_t total_length = 128;
__ubuf__ half src[total_length];
asc_copy_ub2gm_sync(dst, src, total_length * sizeof(half));
```
