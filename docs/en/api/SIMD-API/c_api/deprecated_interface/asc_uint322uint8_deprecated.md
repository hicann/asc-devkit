---
okftype: reference.api
description: "Deprecated interface page: under the 3510 architecture, asc_uint322uint8 and its _sat and _v2 to _v4 forms narrow a uint32_t vector to uint8_t, using the interface suffix to control the write index position with optional saturation mode. It is recommended to use the explicit position parameter overload instead."
tag:
  - roles: [compute]
  - operations: [conversion]
  - traits: [elementwise, saturation_mode]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: constrained_by
    condition: "The definitions of the vector data register and mask register follow their data type definitions"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_convert/asc_uint322uint8.md
    kind: replaced_by
    condition: "Release Notes 9.2.0 requires using the explicit position parameter overload to replace the prototype without explicit position parameters"
  - path: docs/zh/api/SIMD-API/c_api/spr/asc_set_ctrl.md
    kind: requires
    condition: "To enable saturation or non-saturation mode, configure the ctrl register first"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "Release Notes 9.2.0 lists the overloads in Reg type conversion that control position through suffixes or implicit rules as deprecated, replaced by explicit position parameter overloads; the eight prototypes on this page remain available."
---
# asc_uint322uint8 (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T15:17:49.357Z pushedAt=2026-11-18T03:12:16.697Z nl=2 -->

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

The header file path is `"c_api/reg_compute/reg_convert.h"`.

**The prototypes of this interface without explicit position parameters are deprecated. Use the explicit position parameter overloads of [asc_uint322uint8](../reg_compute/reg_convert/asc_uint322uint8.md) instead.**

Converts uint32_t data to uint8_t, supporting saturation/non-saturation mode.

For details about rounding mode and saturation/non-saturation mode, see [Rounding Mode and Saturation Mode](../reg_compute/reg_convert/rounding_mode.md).

Because the bit width ratio of the source operand to the destination operand is 4:1, when writing data, one VL-sized data needs to be divided into four parts, and index 0, index 1, index 2, or index 3 is selected according to different interfaces.

## Prototype

```cpp
// Non-saturation mode, data is written to the position of index 0.
__simd_callee__ inline void asc_uint322uint8(vector_uint8_t& dst, vector_uint32_t src, vector_bool mask)
// Saturation mode, data is written to the position of index 0.
__simd_callee__ inline void asc_uint322uint8_sat(vector_uint8_t& dst, vector_uint32_t src, vector_bool mask)
// Non-saturation mode, data is written to the position of index 1.
__simd_callee__ inline void asc_uint322uint8_v2(vector_uint8_t& dst, vector_uint32_t src, vector_bool mask)
// Saturation mode, data is written to the position of index 1.
__simd_callee__ inline void asc_uint322uint8_sat_v2(vector_uint8_t& dst, vector_uint32_t src, vector_bool mask)
// Non-saturation mode, data is written to the position with index 2.
__simd_callee__ inline void asc_uint322uint8_v3(vector_uint8_t& dst, vector_uint32_t src, vector_bool mask)
// Saturation mode, data is written to the position with index 2.
__simd_callee__ inline void asc_uint322uint8_sat_v3(vector_uint8_t& dst, vector_uint32_t src, vector_bool mask)
// Non-saturation mode, data is written to the position with index 3.
__simd_callee__ inline void asc_uint322uint8_v4(vector_uint8_t& dst, vector_uint32_t src, vector_bool mask)
// Saturation mode, data is written to the position with index 3.
__simd_callee__ inline void asc_uint322uint8_sat_v4(vector_uint8_t& dst, vector_uint32_t src, vector_bool mask)
```

## Parameters

**Table 1** Parameter description

|Parameter|Input/Output|Description|
| ------------ | ------------ | ------------ |
| dst        | Output    | Destination operand (vector data register). |
| src        | Input    | Source operand (vector data register). |
| mask       | Input    | Source operand mask (mask register), used to indicate which elements participate in the calculation. When the corresponding position is 1, the element participates in the calculation; when it is 0, the element does not participate. Elements not filtered by `mask` are set to zero in the output. |

For details about vector data registers and mask registers, see [reg data type definition](../defs/type/data_type_definition.md).

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- When saturation mode and non-saturation mode are enabled, the ctrl register must be configured. For details about the ctrl register, see [asc_set_ctrl.md](../spr/asc_set_ctrl.md).

## Examples

```cpp
vector_uint32_t src;
vector_uint8_t dst;
vector_bool mask = asc_create_mask_b32(PAT_ALL);
asc_loadalign(src, src_addr); // src_addr is the address of the Unified Buffer (UB) memory space input externally.
asc_uint322uint8(dst, src, mask);
```
