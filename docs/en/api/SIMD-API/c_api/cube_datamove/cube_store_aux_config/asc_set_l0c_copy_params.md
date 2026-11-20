---
okftype: reference.api
description: "Configures the NZ-to-ND conversion parameters for Atlas A3 and Atlas A2 products when moving data from the L0C Buffer to GM or the L1 Buffer, including the number of ND cubes, the offset between adjacent source NZ cubes, and the offset between adjacent destination ND cubes."
tag:
  - roles: [data_movement_configuration]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_store/asc_copy_l0c2gm_arch_2201.md
    kind: configures
    condition: "Used together with the L0C-to-GM move-out interface corresponding to Atlas A3/A2"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_store/asc_copy_l0c2l1_arch_2201.md
    kind: configures
    condition: "Used together with the L0C-to-L1 move-out interface corresponding to Atlas A3/A2"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_aux_config/asc_set_l0c_copy_nz_para.md
    kind: related
    condition: "Ascend 950PR&950DT uses this interface instead of the current one, and the parameter units are subject to its description"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_key_features/NZ2ND.md
    kind: related
version: ">=9.0.0"
version_changes: []
---
# asc_set_l0c_copy_params

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T11:51:28.322Z pushedAt=2026-11-18T08:15:34.355Z nl=2 -->

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

When DataCopy performs on-the-fly format conversion (NZ format to ND format) during the copy from L0C Buffer to Global Memory or L1 Buffer, call this API to set the configuration related to format conversion.

<!-- npu="A3,910b" id8 -->
This API is used for [asc_copy_l0c2gm](../cube_compute_store/asc_copy_l0c2gm_arch_2201.md) and [asc_copy_l0c2l1](../cube_compute_store/asc_copy_l0c2l1_arch_2201.md) corresponding to Atlas A3 products and Atlas A2 products. For the illustration of the cube count and intervals, see [NZ2ND](../cube_store_key_features/NZ2ND.md).
<!-- end id8 -->
<!-- npu="950" id9 -->
Ascend 950PR&950DT products use [asc_set_l0c_copy_nz_para](asc_set_l0c_copy_nz_para.md), and the parameter units are subject to the description of that API.
<!-- end id9 -->

## Prototype

```cpp
__aicore__ inline void asc_set_l0c_copy_params(uint16_t nd_num, uint16_t src_nd_stride, uint16_t dst_nd_stride)
```

## Parameters

**Table 1** Parameter description

| Parameter       | Input/Output | Description               |
| --------- | ----- | ---------------- |
| nd_num       | Input    | 	Number of ND cubes. Value range: nd_num∈[1, 65535].            |
| src_nd_stride | Input    | Source stride in units of fractal size, which is the offset between the start addresses of adjacent source nz cubes.<br> src_nd_stride∈[1, 512], unit: fractal_size 1024B.|
| dst_nd_stride | Input    | 	Offset between the start addresses of adjacent destination ND cubes. The unit is element. <br> dst_nd_stride∈[1, 65535].      |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

None

## Examples

```cpp
uint16_t nd_num = 2;
uint16_t src_nd_stride = 2;
uint16_t dst_nd_stride = 1;
asc_set_l0c_copy_params(nd_num, src_nd_stride, dst_nd_stride);
```
