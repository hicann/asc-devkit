---
okftype: reference.api
description: "Sets the boundary value of the 3D format transfer interfaces of asc_copy_l12l0a and asc_copy_l12l0b on the L1 Buffer, in bytes. 0 means no boundary, and the entire L1 Buffer can be used."
tag:
  - roles: [data_movement_configuration]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_load/asc_copy_l12l0a_arch_2201.md
    kind: configures
    condition: "The source start address of this 3D format transfer must fall within the set boundary; if out of bounds, read from the L1 start address"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_load/asc_copy_l12l0b_arch_2201.md
    kind: configures
    condition: "The source start address of this 3D format transfer must fall within the set boundary; if out of bounds, read from the L1 start address"
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "Operand address alignment must refer to the storage unit description"
version: ">=9.0.0"
version_changes: []
---
# asc_set_l13d_size

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T11:42:49.695Z pushedAt=2026-11-18T08:15:34.341Z nl=2 -->

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

Sets the boundary value of the 3D format transfer interface of [asc_copy_l12l0a](../cube_compute_load/asc_copy_l12l0a_arch_2201.md)/[asc_copy_l12l0b](../cube_compute_load/asc_copy_l12l0b_arch_2201.md) in the L1 Buffer.

## Prototype

```cpp
__aicore__ inline void asc_set_l13d_size(uint64_t value)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| :----- | :------- | :------- |
| value | Input | Used to set the boundary value of the 3D format transfer interface of asc_copy_l12l0a/asc_copy_l12l0b in the L1 Buffer, in bytes. In non-zero scenarios, the boundary value cannot be less than 1024.<br>If value is set to 0, it indicates no boundary, and the asc_copy_l12l0a/asc_copy_l12l0b interface can use the entire L1 Buffer. |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- Must be used together with the 3D format transfer interface of asc_copy_l12l0a/asc_copy_l12l0b.
- If the boundary value is set using the asc_set_l13d_size interface, the initial address of the L1 Buffer for the 3D format transfer interface of asc_copy_l12l0a/asc_copy_l12l0b must be within the set boundary (value).
- If the address of the source operand of the 3D format transfer interface of asc_copy_l12l0a/asc_copy_l12l0b in the L1 Buffer exceeds the set boundary value (value), data will be read starting from the start address of the L1 Buffer.
- For operand address alignment constraints, see [Storage Unit Description](../../general_description_and_constraints.md#storage-unit-description).

## Examples

```cpp
uint64_t value = 1024;
asc_set_l13d_size(value);    // Set the boundary value of the L1 Buffer to 1024 (in bytes).
```
