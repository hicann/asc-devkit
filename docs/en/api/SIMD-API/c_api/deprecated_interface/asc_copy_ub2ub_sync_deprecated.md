---
okftype: reference.api
description: "Deprecated synchronous UB-to-UB data movement interface that moves contiguous data by byte count and waits synchronously before returning, targeting 950PR&950DT and Atlas A3/A2. The replacement is asc_copy_ub2ub plus asc_sync."
tag:
  - roles: [data_movement]
  - operations: [copy]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "Storage unit space size and alignment requirements"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: replaced_by
    condition: "The synchronization part of the replacement"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync_notify.md
    kind: requires
    condition: "The synchronization instruction to insert when UB address overlap exists with other instructions"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync_wait.md
    kind: requires
    condition: "Synchronization instruction to be inserted when there is UB address overlap with other instructions"
  - path: docs/zh/api/SIMD-API/c_api/vector_datamove/asc_copy_ub2ub.md
    kind: replaced_by
    condition: "The replacement non-synchronous interface declared in the body text"
version: ">=9.0.0"
version_changes:
  - since: "9.1.0"
    kind: arch_support_extended
    scope: "page"
    scope_basis: kind_default
    note: "The 9.0.0 implementation is only in instr_impl/npu_arch_2201. Starting from 9.1.0, the npu_arch_3510 implementation is added and listed in the 3510 newly added data movement list of Release Notes CANN_9_1_0. In the product support table on the page, support for Ascend 950PR&950DT starts from 9.1.0."
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "Starting from 9.2.0, the include is marked ASC_DEPRECATED(9.2.0, \"2027/09/07\", asc_copy_ub2ub and asc_sync); the declaration is retained, classified as deprecated rather than removed."
---
# asc_copy_ub2ub_sync (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T12:40:00.600Z pushedAt=2026-11-18T03:12:16.453Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
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

**The `asc_copy_ub2ub_sync` interface is deprecated. Use the [corresponding non-synchronous interface](../vector_datamove/asc_copy_ub2ub.md) and the synchronous interface [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Moves data from Unified Buffer (UB) to UB.

## Prototype

- Synchronous computation

```cpp
__aicore__ inline void asc_copy_ub2ub_sync(__ubuf__ void* dst, __ubuf__ void* src, uint32_t size)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| dst | Output | Destination UB address. |
| src | Input | Source UB address. |
| size | Input | Size of the data to move (in bytes). |

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- For the space size and alignment requirements of each storage unit, see [Storage Unit Description](../general_description_and_constraints.md#storage-unit-description).
- If this instruction has UB address overlap with other instructions, insert the synchronization instructions [asc_sync_notify](../sync/intra_core_sync/asc_sync_notify.md) and [asc_sync_wait](../sync/intra_core_sync/asc_sync_wait.md) to serialize multiple instructions and prevent abnormal data.
- Synchronous computation includes synchronous waiting.
- When the first n data movement interfaces are used, the size of the data to move must be 32-byte aligned.

## Examples

```cpp
// total_length indicates the total length of the data involved in the movement.
constexpr uint64_t total_length = 128;
__ubuf__ half src[total_length];
__ubuf__ half dst[total_length];
asc_copy_ub2ub_sync(dst, src, total_length * sizeof(half));
```
