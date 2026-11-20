---
okftype: reference.api
description: "On the 2201 architecture, moves data from the UB to GM, providing two forms: contiguous movement by size and high-dimensional slicing movement by burst_count/burst_len with source and destination strides."
tag:
  - roles: [data_movement]
  - operations: [copy]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: related
    condition: "The main text references storage units accordingly"
  - path: docs/zh/api/SIMD-API/c_api/sync/system_sync_overview.md
    kind: related
    condition: "Insert synchronization instructions according to this page when data dependencies exist"
version: ">=8.5.0"
version_changes: []
---
# asc_copy_ub2gm (NPU architecture version 2201)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:04.076456+00:00 pushedAt=2026-11-16T02:10:59.458Z nl=2 -->

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

The header file path is `"c_api/vector_datamove/vector_datamove.h"`.

Copies data from Unified Buffer (UB) to Global Memory (GM).

## Prototype

- Copying the first n data items

```cpp
__aicore__ inline void asc_copy_ub2gm(__gm__ void* dst, __ubuf__ void* src, uint32_t size)
```

- High-dimensional slicing data movement

```cpp
__aicore__ inline void asc_copy_ub2gm(__gm__ void* dst, __ubuf__ void* src, uint16_t burst_count, uint16_t burst_len, uint16_t src_gap, uint16_t dst_gap)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| dst | Output | Target GM address. |
| src | Input | Source UB address. |
| size | Input | Size of the data to be copied (in bytes). |
| burst_count | Input | Number of consecutive data blocks to be copied. Value range: [1, 4095]. |
| burst_len | Input | Length of each consecutive data block to be copied. The unit is DataBlock (32 bytes). Value range: [1, 65535]. |
| src_gap | Input | Gap between adjacent consecutive data blocks of the source operand (the gap between the tail of the preceding data block and the head of the following data block).<br>The unit is DataBlock (32 bytes). |
| dst_gap | Input | Gap between adjacent consecutive data blocks of the destination operand (the gap between the tail of the preceding data block and the head of the following data block).<br>The unit is DataBlock (32 bytes). |

## Return Value

None

## Pipeline Type

PIPE_MTE3

## Constraints

- For the space size and alignment requirements of each storage unit, see [Storage Unit Description](../../general_description_and_constraints.md#storage-unit-description).
- If there is a data dependency between this instruction and other instructions, you must insert a [synchronization instruction](../../sync/overview.md#synchronization-control-overview) to ensure that the instructions execute serially and prevent abnormal data from occurring.
- When the first n data items are copied, the size of the copied data must be 32-byte aligned.

## Examples

```cpp
// total_length refers to the total length of the data involved in the movement. dst is the externally provided half-type GM buffer.
constexpr uint64_t total_length = 128;
__ubuf__ half src[total_length];
asc_copy_ub2gm(dst, src, total_length * sizeof(half));
```
