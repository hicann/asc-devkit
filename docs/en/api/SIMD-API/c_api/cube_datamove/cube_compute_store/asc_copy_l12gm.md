---
okftype: reference.api
description: "Moves data from L1 Buffer to Global Memory by the number of consecutive data blocks, block length, and source/destination gap. Supports non-contiguous movement. Available only on Atlas A3 and Atlas A2 products."
tag:
  - roles: [data_movement]
  - operations: [store]
  - traits: [datablock_granularity]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "Storage unit space size and alignment requirements"
  - path: docs/zh/api/SIMD-API/c_api/sync/system_sync_overview.md
    kind: requires
    condition: "When there is a data dependency with other instructions, a synchronization instruction must be inserted for serialization"
version: ">=9.1.0"
version_changes: []
---
# asc_copy_l12gm

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T11:17:47.114Z pushedAt=2026-11-18T08:15:34.304Z nl=2 -->

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

The header file path is: `"c_api/cube_datamove/cube_datamove.h"`.

Copies data from L1 Buffer to Global Memory.

## Prototype

- Regular copy

    ```cpp
    __aicore__ inline void asc_copy_l12gm(__gm__ void* dst, __cbuf__ void* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
    ```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description                                                              |
| :--- |:------|:----------------------------------------------------------------|
| dst | Output    | Start address of the destination operand.|
| src | Input    | Start address of the source operand.|
| n_burst | Input    | Number of consecutive data blocks to be copied. Value range: [1, 4095]. |
| len_burst | Input    | Length of each consecutive data block to be copied, in units of 32 bytes. Value range: [1, 65535]. |
| src_gap | Input    | Gap between the end address of the first fractal of the previous iteration and the start address of the first fractal of the next iteration of the source operand. |
| dst_gap | Input    | Gap between the end address of the first fractal of the previous iteration and the start address of the first fractal of the next iteration of the destination operand. Unit: 32 bytes. |

## Return Value

None

## Pipeline Type

PIPE_MTE3

## Constraints

- For the space size and alignment requirements of each storage unit, see [Storage Unit Description](../../general_description_and_constraints.md#storage-unit-description).
- If this instruction has a data dependency with other instructions, insert a [synchronization instruction](../../sync/overview.md#synchronization-control-overview) to serialize multiple instructions and prevent abnormal data.

## Examples

```cpp
//The number of consecutive data blocks to be copied is 1
constexpr uint16_t n_burst = 1;
//The length of each consecutive data block to be copied is 32 bytes
constexpr uint16_t len_burst = 1;
// Between adjacent iterations, the gap between the start addresses of the previous and next fractals of the source operand is 32 bytes.
constexpr uint16_t src_gap = 0;
// Between adjacent iterations, the gap between the start addresses of the previous and next fractals of the destination operand is 64 bytes.
constexpr uint16_t dst_gap = 1;
__cbuf__ half src[256];
// dst indicates the start address of the destination operand.
asc_copy_l12gm(dst, src, n_burst, len_burst, src_gap, dst_gap);
```
