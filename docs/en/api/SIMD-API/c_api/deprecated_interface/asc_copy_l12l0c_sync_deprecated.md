---
okftype: reference.api
description: "Deprecated synchronous L1 Buffer to L0C Buffer matrix copy interface, supporting bfloat16_t, half, float, and 32-bit integer combinations. The replacement is asc_copy_l12l0c plus asc_sync."
tag:
  - roles: [data_movement]
  - operations: [load]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_load/asc_copy_l12l0c.md
    kind: replaced_by
    condition: "The replacement non-synchronous interface declared in the body text"
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "Storage unit space size and alignment requirements"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: replaced_by
    condition: "The synchronization part of the replacement solution"
version: ">=9.1.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "Marked as deprecated since 9.2.0. The replacement given in the body and include is asc_copy_l12l0c plus asc_sync; the declaration is retained, and it is deprecated rather than removed."
---
# asc_copy_l12l0c_sync (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T12:30:10.161Z pushedAt=2026-11-18T03:12:16.441Z nl=2 -->

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

**The `asc_copy_l12l0c_sync` interface is deprecated. Use the [corresponding non-synchronous interface](../cube_datamove/cube_compute_load/asc_copy_l12l0c.md) and the synchronous interface [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Moves a matrix from L1 Buffer to L0C Buffer. The data types supported by this move are as follows:
- bfloat16_t->bfloat16_t
- half->half
- float->half
- float->bfloat16_t
- float->float
- int32_t->int32_t
- uint32_t->uint32_t

## Prototype

- Synchronous copy

    ```cpp
    __aicore__ inline void asc_copy_l12l0c_sync(__cc__ bfloat16_t* dst, __cbuf__ bfloat16_t* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
    __aicore__ inline void asc_copy_l12l0c_sync(__cc__ half* dst, __cbuf__ half* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
    __aicore__ inline void asc_copy_l12l0c_sync(__cc__ half* dst, __cbuf__ float* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
    __aicore__ inline void asc_copy_l12l0c_sync(__cc__ bfloat16_t* dst, __cbuf__ float* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
    __aicore__ inline void asc_copy_l12l0c_sync(__cc__ float* dst, __cbuf__ float* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
    __aicore__ inline void asc_copy_l12l0c_sync(__cc__ int32_t* dst, __cbuf__ int32_t* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
    __aicore__ inline void asc_copy_l12l0c_sync(__cc__ uint32_t* dst, __cbuf__ uint32_t* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
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
| dst_gap | Input    | Gap between the end address of the first fractal of the previous iteration and the start address of the first fractal of the next iteration of the destination operand. |

## Return Value

None

## Pipeline Type

PIPE_MTE1

## Constraints

- For the space size and alignment requirements of each storage unit, see [Storage Unit Description](../general_description_and_constraints.md#storage-unit-description).
- If multiple asc_copy_l12l0c instructions need to be executed and the destination addresses of the asc_copy_l12l0c instructions overlap, insert a synchronization instruction to ensure that the multiple asc_copy_l12l0c instructions are serialized and prevent abnormal data.

## Examples

```cpp
// Copy one consecutive data block.
constexpr uint16_t n_burst = 1;
// The length of each consecutive data block to be copied is 32 bytes.
constexpr uint16_t len_burst = 1;
// The gap between the start addresses of the previous and next fractals of the source operand in adjacent iterations is 64 bytes.
constexpr uint16_t src_gap = 1;
// The gap between the start addresses of the previous and next fractals of the destination operand in adjacent iterations is 32 bytes.
constexpr uint16_t dst_gap = 0;
__cbuf__ half src[256];
__cc__ half dst[256];
asc_copy_l12l0c_sync(dst, src, n_burst, len_burst, src_gap, dst_gap);
```
