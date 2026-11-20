---
okftype: reference.api
description: "Deprecated address register initialization interface family that creates address registers capable of storing 1 to 4 offsets by b8, b16, and b32 bit widths, for multi-dimensional iteration addressing within loops. The replacement is asc_update_addr_reg."
tag:
  - roles: [configuration]
  - traits: [high_dimensional]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: related
    condition: "Page containing the type definition and call examples of the address register"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_addr_reg/asc_update_addr_reg.md
    kind: replaced_by
    condition: "The replacement interface declared in the body, corresponding to b8/b16/b32 respectively"
version: ">=9.0.0"
version_changes:
  - since: "9.1.0"
    kind: capability_extended
    scope: "page"
    scope_basis: kind_default
    note: "Starting from 9.1.0, the address register initialization interfaces of the three bit-width versions are extended from a single offset to supporting 1 to 4 offset parameters (this extension is recorded in Release Notes CANN_9_1_0), to adapt to loop addressing of 1- to 4-dimensional Tensors; 9.0.0 only has the single-parameter form."
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "Starting from 9.2.0, include marks the three bit-width overloads one by one with ASC_DEPRECATED(9.2.0, \"2027/09/07\", asc_update_addr_reg_b32/b16/b8). The replacement relationship table in Release Notes CANN_9_2_0 states that the address register type and initialization interfaces are uniformly renamed to addr_reg and asc_update_addr_reg_b8/b16/b32; the declarations are retained, and this is deprecated rather than removed."
---
# asc_create_iter_reg (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T12:41:12.571Z pushedAt=2026-11-18T03:12:16.455Z nl=2 -->

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

The header file path is: `"c_api/reg_compute/compute/reg_addr_reg.h"`.

**This API is deprecated. Use [asc_update_addr_reg](../reg_compute/reg_addr_reg/asc_update_addr_reg.md) to implement this function instead.**

The address register is initialized through this API and then used to store address offsets within a loop. This overloaded API supports 1 to 4 offset parameters simultaneously, and supports initializing registers of b8/b16/b32 bit widths, adapting to 1- to 4-dimensional Tensors. At the underlying level, address offsets are automatically computed in multi-dimensional loops, eliminating the overhead of scalar address computation.

## Prototype

  ```cpp
  __simd_callee__ inline iter_reg asc_create_iter_reg_b32(uint32_t offset)
  __simd_callee__ inline iter_reg asc_create_iter_reg_b16(uint32_t offset)
  __simd_callee__ inline iter_reg asc_create_iter_reg_b8(uint32_t offset)
  __simd_callee__ inline iter_reg asc_create_iter_reg_b32(uint32_t offset0, uint32_t offset1)
  __simd_callee__ inline iter_reg asc_create_iter_reg_b16(uint32_t offset0, uint32_t offset1)
  __simd_callee__ inline iter_reg asc_create_iter_reg_b8(uint32_t offset0, uint32_t offset1)
  __simd_callee__ inline iter_reg asc_create_iter_reg_b32(uint32_t offset0, uint32_t offset1, uint32_t offset2)
  __simd_callee__ inline iter_reg asc_create_iter_reg_b16(uint32_t offset0, uint32_t offset1, uint32_t offset2)
  __simd_callee__ inline iter_reg asc_create_iter_reg_b8(uint32_t offset0, uint32_t offset1, uint32_t offset2)
  __simd_callee__ inline iter_reg asc_create_iter_reg_b32(uint32_t offset0, uint32_t offset1, uint32_t offset2, uint32_t offset3)
  __simd_callee__ inline iter_reg asc_create_iter_reg_b16(uint32_t offset0, uint32_t offset1, uint32_t offset2, uint32_t offset3)
  __simd_callee__ inline iter_reg asc_create_iter_reg_b8(uint32_t offset0, uint32_t offset1, uint32_t offset2, uint32_t offset3)
  ```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| --------- | ----- | ----------------- |
| offset       | Input | Address offset. |
| offset0      | Input | Address offset. |
| offset1     | Input | Address offset. |
| offset2     | Input | Address offset. |
| offset3     | Input | Address offset. |

## Return Value

Address register.

## Pipeline Type

PIPE_V

## Constraints

- Do not use if/else inside a loop.
- iter_reg must be assigned and used within the innermost loop body.
- For operands with different strides, it is recommended to use multiple iter_reg to avoid address overwriting.

## Examples

See [address register call example](../defs/type/data_type_definition.md#example4).
