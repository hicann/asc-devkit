---
okftype: reference.api
description: "Deprecated synchronous L1 Buffer to Unified Buffer data movement interface that moves data between Cube and Vector cores with an optional destination Vector Core. The replacement is asc_copy_l12ub plus asc_sync, supported only on 950PR&950DT."
tag:
  - roles: [data_movement]
  - operations: [load]
  - traits: [datablock_granularity, inter_core_synchronization]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_store/asc_copy_l12ub.md
    kind: replaced_by
    condition: "The non-synchronous replacement interface declared in the body text"
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "Storage unit space size, alignment requirements, and general address overlap constraints"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: replaced_by
    condition: "the synchronization part in the replacement solution"
version: ">=9.1.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "Starting from 9.2.0, the include marks ASC_DEPRECATED(9.2.0, \"2027/09/07\", asc_copy_l12ub and asc_sync). The replacement relationship table in Release Notes CANN_9_2_0 also records that asc_copy_l12ub_sync is replaced by asc_copy_l12ub plus asc_sync. The declaration is retained, so it is deprecated rather than removed."
---
# asc_copy_l12ub_sync (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T12:30:28.380Z pushedAt=2026-11-18T03:12:16.442Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Not supported
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

The header file path is `"c_api/composite/cube_datamove_composite.h"`.

**The `asc_copy_l12ub_sync` API is deprecated. Use the [corresponding non-synchronous API](../cube_datamove/cube_compute_store/asc_copy_l12ub.md) and the synchronous API [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Copies data from L1 Buffer to Unified Buffer (UB).

## Prototype

- Synchronous copy

    ```cpp
    __aicore__ inline void asc_copy_l12ub_sync(__ubuf__ void* dst_addr, __cbuf__ void* src_addr, bool sub_blockid, uint16_t burst_count, uint16_t burst_len, uint16_t src_gap, uint16_t dst_gap)
    ```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| dst_addr | Output | Start address of the destination operand. |
| src_addr | Input | Start address of the source operand. |
| sub_blockid | Input | Sub-block ID to use, of bool type. false indicates 0, and true indicates 1. |
| burst_count | Input | Number of consecutive data blocks to be copied. |
| burst_len | Input | Length of each consecutive data block to be copied. The unit is DataBlock (32 bytes). |
| src_gap | Input | Interval between adjacent consecutive data blocks of the source operand (the interval between the end of the preceding data block and the start of the following data block).<br>The unit is DataBlock (32 bytes). |
| dst_gap | Input | Interval between adjacent consecutive data blocks of the destination operand (the interval between the end of the preceding data block and the start of the following data block).<br>The unit is DataBlock (32 bytes). |

## Return Value

None

## Pipeline Type

PIPE_MTE1

## Constraints

- For the space size and alignment requirements of each storage unit, see [Storage Unit Description](../general_description_and_constraints.md#storage-unit-description).
- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).
- Because this API implements data copy between the Cube core and the Vector core, if a kernel calls this API, the `__cube__` or `__vector__` execution space modifier cannot be used. Otherwise, execution fails.

## Examples

```cpp
constexpr uint16_t burst_count = 1;
constexpr uint16_t burst_len = 1;
constexpr uint16_t src_gap = 0;
constexpr uint16_t dst_gap = 1;
__cbuf__ half src[256];
__ubuf__ half dst[256];
asc_copy_l12ub_sync(dst, src, false, burst_count, burst_len, src_gap, dst_gap);
```
