---
okftype: reference.api
description: "Deprecated API page: Under the 2201 architecture, asc_copy_l12gm_sync synchronously moves L1 Buffer data to GM by data block count, block length, and source/destination stride. The block length is in units of 32 bytes. It is recommended to use the non-synchronous asc_copy_l12gm together with asc_sync instead."
tag:
  - roles: [data_movement]
  - operations: [store]
  - traits: [datablock_granularity]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_store/asc_copy_l12gm.md
    kind: replaced_by
    condition: "It is recommended to use the corresponding non-synchronous API and explicitly call asc_sync"
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "Space and alignment requirements follow its storage unit description"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: related
    condition: "Serves as the synchronization wait in the alternative solution"
version: ">=9.1.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The include header marks this synchronous form with ASC_DEPRECATED(9.2.0); the alternative path is the non-synchronous asc_copy_l12gm plus asc_sync. The interface has not been removed."
---
# asc_copy_l12gm_sync (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T12:08:39.305Z pushedAt=2026-11-18T03:12:16.426Z nl=2 -->

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

The header file path is: `"c_api/composite/cube_datamove_composite.h"`.

**The `asc_copy_l12gm_sync` interface is deprecated. Use the [corresponding asynchronous interface](../cube_datamove/cube_compute_store/asc_copy_l12gm.md) together with the synchronization interface [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Moves data from L1 Buffer to Global Memory.

## Prototype

- Synchronous data movement

    ```cpp
    __aicore__ inline void asc_copy_l12gm_sync(__gm__ void* dst, __cbuf__ void* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
    ```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description                                                              |
| :--- |:------|:----------------------------------------------------------------|
| dst | Output    | Start address of the destination operand.|
| src | Input    | Start address of the source operand.|
| n_burst | Input    | Number of consecutive data blocks to be moved. Value range: [1, 4095]. |
| len_burst | Input    | Length of each consecutive data block to be moved, in units of 32 bytes. Value range: [1, 65535]. |
| src_gap | Input    | Gap between the end address of the first fractal of the previous iteration and the start address of the first fractal of the next iteration of the source operand. |
| dst_gap | Input    | Gap between the end address of the first fractal of the previous iteration and the start address of the first fractal of the next iteration of the destination operand. Unit: 32 bytes. |

## Return Value

None

## Pipeline Type

PIPE_MTE3

## Constraints

- For the space size and alignment requirements of each storage unit, see [Storage Unit Description](../general_description_and_constraints.md#storage-unit-description).
- If multiple asc_copy_l12gm instructions need to be executed and their destination addresses overlap, insert synchronization instructions to ensure serialization of the multiple asc_copy_l12gm instructions and prevent abnormal data.

## Examples

```cpp
//The number of consecutive data blocks to be moved is 1
constexpr uint16_t n_burst = 1;
//The length of each continuous data burst to be copied is 32 bytes.
constexpr uint16_t len_burst = 1;
//Between adjacent iterations, the interval between the start addresses of the previous and next fractals of the source operand is 32 bytes.
constexpr uint16_t src_gap = 0;
//Between adjacent iterations, the interval between the start addresses of the previous and next fractals of the destination operand is 64 bytes.
constexpr uint16_t dst_gap = 1;
__cbuf__ half src[256];
//dst indicates the start address of the destination operand.
asc_copy_l12gm_sync(dst, src, n_burst, len_burst, src_gap, dst_gap);
```
