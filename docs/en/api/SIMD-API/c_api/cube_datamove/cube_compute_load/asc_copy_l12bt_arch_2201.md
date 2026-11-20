---
okftype: reference.api
description: "Moves Bias data from L1 Buffer to BiasTable Buffer for the cube computation unit to read as Bias. Provides two parameter lists: continuous movement by size, and high-dimensional splitting by block count, block length, and source/destination gap."
tag:
  - roles: [data_movement]
  - operations: [copy]
  - traits: [aligned_transfer, datablock_granularity]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_compute/mmad_compute/asc_mmad.md
    kind: related
    condition: "Read by Mmad when Bias is used as the initial value of the C matrix"
version: ">=9.0.0"
version_changes: []
---
# asc_copy_l12bt (NPU Architecture Version 2201)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T10:37:52.020Z pushedAt=2026-11-18T08:15:34.261Z nl=2 -->

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

Moves data from the L1 Buffer to the BiasTable Buffer. The BiasTable Buffer is used to store the Bias in cube computation.

For the format of the Bias cube, see [Auxiliary Cube Fractal Format Details](../../cube_compute/cube_compute_fractal_intro/aux_cube_fractal_format_details.md). In [asc_mmad](../../cube_compute/mmad_compute/asc_mmad.md), select Bias as the initial value of the C cube according to the overload for the corresponding architecture.

## Prototype

- First n data movement

    ```cpp
    __aicore__ inline void asc_copy_l12bt(uint64_t dst, __cbuf__ void* src, uint32_t size)
    ```

- High-dimensional split movement

    ```cpp
    __aicore__ inline void asc_copy_l12bt(uint64_t dst, __cbuf__ void* src, uint16_t conv_control, uint16_t n_burst, uint16_t len_burst, uint16_t source_gap, uint16_t dst_gap)
    ```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| dst | Output | Start address of the destination operand. |
| src | Input | Start address of the source operand. |
| size | Input | Size of the data to be moved (in bytes).|
| conv_control | Input | Whether to enable on-the-fly conversion.|
| n_burst | Input | Number of consecutive data blocks to be moved. Value range: [1, 4095]. |
| len_burst | Input | Length of each consecutive data block to be moved, unit: DataBlock (32 bytes). Value range: [1, 65535]. |
| source_gap | Input | Gap between adjacent consecutive data blocks of the source operand (the gap between the tail of the preceding data block and the head of the following data block).<br>Unit: DataBlock (32 bytes). |
| dst_gap | Input | Gap between adjacent consecutive data blocks of the destination operand (the gap between the tail of the preceding data block and the head of the following data block).<br>Unit: DataBlock (32 bytes). |

## Return Value

None

## Pipeline Type

PIPE_MTE1

## Constraints

- For the space size and alignment requirements of each storage unit, see [Storage Unit Description](../../general_description_and_constraints.md#storage-unit-description).
- For operand address overlap constraints, see [General Address Overlap Constraints](../../general_description_and_constraints.md#general-address-overlap-constraints).
- When using the first n data transfer interfaces, the transferred data size must be 32-byte aligned.

## Examples

```cpp
constexpr uint16_t n_burst = 1;
constexpr uint16_t len_burst = 1;
constexpr uint16_t source_gap = 0;
constexpr uint16_t dst_gap = 1;
constexpr uint16_t conv_control = 0;
__cbuf__ half src[256];
__biasbuf__ half dst[256];
uint64_t dst_addr = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dst)) / 16;
asc_copy_l12bt(dst_addr, src, conv_control, n_burst, len_burst, source_gap, dst_gap);
```
