---
okftype: reference.api
description: "Deprecated API page: under the 3510 architecture, asc_uint162uint8 and its _sat, _v2, and _sat_v2 forms convert a uint16_t vector to uint8_t. The suffix controls whether the write index is at even or odd positions and optionally enables saturation mode. It is recommended to use the explicit position parameter overloads instead."
tag:
  - roles: [compute]
  - operations: [conversion]
  - traits: [elementwise, saturation_mode]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: constrained_by
    condition: "The definitions of vector data registers and mask registers follow their data type definitions"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_convert/asc_uint162uint8.md
    kind: replaced_by
    condition: "Release Notes 9.2.0 requires replacing the suffix-controlled position prototypes with explicit position parameter overloads"
  - path: docs/zh/api/SIMD-API/c_api/spr/asc_set_ctrl.md
    kind: requires
    condition: "Enabling saturation or non-saturation mode requires configuring the ctrl register first"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "Release Notes 9.2.0 lists the overloads in Reg type conversion that control the position through suffixes or implicit rules as deprecated, replaced by explicit position parameter overloads; the four prototypes on this page remain available."
---
# asc_uint162uint8 (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T15:14:30.200Z pushedAt=2026-11-18T03:12:16.694Z nl=2 -->

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

The header file path is: `"c_api/reg_compute/reg_convert.h"`.

**The prototypes of this interface that control the destination operand write position through interface suffixes (including only the source operand, destination operand, and mask register parameters) are deprecated. Use the explicit position parameter overloads of [asc_uint162uint8](../reg_compute/reg_convert/asc_uint162uint8.md) instead.**

Converts uint16_t data to uint8_t type, with no rounding mode, and supports saturated/non-saturated modes.

For details about saturated/non-saturated modes, see [Rounding Mode and Saturation Mode](../reg_compute/reg_convert/rounding_mode.md).

Because the bit-width ratio between the source operand and the destination operand is 2:1, when writing data, one VL-sized data must be divided into two parts. Depending on the interface, data is written to positions with odd indices or positions with even indices.

## Prototype

```cpp
// Non-saturated mode, data is written to positions with even indices.
__simd_callee__ inline void asc_uint162uint8(vector_uint8_t& dst, vector_uint16_t src, vector_bool mask)
// Saturated mode, data is written to positions with even indices.
__simd_callee__ inline void asc_uint162uint8_sat(vector_uint8_t& dst, vector_uint16_t src, vector_bool mask)
// Non-saturated mode, data is written to positions with odd indices.
__simd_callee__ inline void asc_uint162uint8_v2(vector_uint8_t& dst, vector_uint16_t src, vector_bool mask)
// Saturated mode, data is written to positions with odd indices.
__simd_callee__ inline void asc_uint162uint8_sat_v2(vector_uint8_t& dst, vector_uint16_t src, vector_bool mask)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Destination operand (vector data register). |
| src | Input | Source operand (vector data register). |
| mask | Input | Source operand mask (mask register), used to indicate which elements participate in the computation. A value of 1 at the corresponding position means the element participates in the computation, and 0 means it does not. Elements not selected by mask are set to zero in the output. |

For detailed descriptions of vector data registers and mask registers, see [reg data type definition](../defs/type/data_type_definition.md).

## Return Value

None

## Constraints

- mask controls whether the source operand participates in the computation. Elements of the source operand that do not participate in the computation are set to zero at the corresponding positions in the output.
- When data is written to odd-indexed positions of the destination operand, the values at even-indexed positions are set to zero. The reverse is also true.
- When enabling saturation mode and non-saturation mode, the ctrl register must be configured. For detailed descriptions of the ctrl register, see [asc_set_ctrl.md](../spr/asc_set_ctrl.md).

## Examples

```cpp
vector_uint16_t src;
vector_uint8_t dst;
vector_bool mask = asc_create_mask_b16(PAT_ALL);
asc_loadalign(src, src_addr); // src_addr is the address of the external input Unified Buffer (UB) memory space.
asc_uint162uint8(dst, src, mask);
```
