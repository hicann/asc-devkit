---
okftype: reference.api
description: "Deprecated implicit position type conversion interface that converts int32_t elements to uint8_t according to the mask and writes them to the destination register at a one-of-four index, providing eight prototypes in total for saturating and non-saturating modes. The replacement is the explicit position parameter overload."
tag:
  - roles: [compute]
  - operations: [conversion]
  - traits: [saturation_mode]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: related
    condition: "Type definitions of vector data registers and mask registers"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_convert/asc_int322uint8.md
    kind: replaced_by
    condition: "Page containing the explicit position parameter overload that replaces the implicit position prototypes on this page"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_convert/rounding_mode.md
    kind: related
    condition: "Rounding mode and saturating/non-saturating mode description"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The eight implicit position prototypes (asc_int322uint8, _sat, and the _v2, _v3, _v4 position variants) are marked deprecated in include/c_api/reg_compute/reg_convert.h with ASC_DEPRECATED(9.2.0, 2027/09/07, asc_int322uint8). The replacement is the new overloads with explicit position parameters based on ASC_POSITION_*. Line 129 of the CANN_9_2_0 Release Notes states that the position-suffix overloads enter the deprecation period, and deprecation does not impose an upper limit on availability."
---
# asc_int322uint8 (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T13:52:12.777Z pushedAt=2026-11-18T03:12:16.562Z nl=2 -->

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

**The prototypes of this API without explicit position parameters are deprecated. Use the explicit position parameter overloads of [asc_int322uint8](../reg_compute/reg_convert/asc_int322uint8.md) instead.**

Converts the int32_t type to the uint8_t type, supporting saturating/non-saturating modes.

For details about rounding modes and saturating/non-saturating modes, see [Rounding Modes and Saturating Modes](../reg_compute/reg_convert/rounding_mode.md).

Because the bit-width ratio between the source operand and the destination operand is 4:1, when writing data, the data of one VL size must be divided into four parts, and index 0, index 1, index 2, or index 3 is selected according to different APIs.

## Prototype

```cpp
// Non-saturating mode, data is written to the position of index 0.
__simd_callee__ inline void asc_int322uint8(vector_uint8_t& dst, vector_int32_t src, vector_bool mask)
// Saturating mode, data is written to the position of index 0.
__simd_callee__ inline void asc_int322uint8_sat(vector_uint8_t& dst, vector_int32_t src, vector_bool mask)
// Non-saturating mode, data is written to the position of index 1.
__simd_callee__ inline void asc_int322uint8_v2(vector_uint8_t& dst, vector_int32_t src, vector_bool mask)
// Saturating mode, data is written to the position of index 1.
__simd_callee__ inline void asc_int322uint8_sat_v2(vector_uint8_t& dst, vector_int32_t src, vector_bool mask)
// Non-saturating mode, data is written to index 2.
__simd_callee__ inline void asc_int322uint8_v3(vector_uint8_t& dst, vector_int32_t src, vector_bool mask)
// Saturating mode, data is written to index 2.
__simd_callee__ inline void asc_int322uint8_sat_v3(vector_uint8_t& dst, vector_int32_t src, vector_bool mask)
// Non-saturating mode, data is written to index 3.
__simd_callee__ inline void asc_int322uint8_v4(vector_uint8_t& dst, vector_int32_t src, vector_bool mask)
// Saturating mode, data is written to index 3.
__simd_callee__ inline void asc_int322uint8_sat_v4(vector_uint8_t& dst, vector_int32_t src, vector_bool mask)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| :------ | :----- | :----------------- |
| dst    | Output    | Destination operand (vector data register). |
| src    | Input    | Source operand (vector data register). |
| mask | Input | Source operand mask (mask register), used to indicate which elements participate in the computation. When the corresponding position is 1, the element participates in the computation; when it is 0, the element does not participate. Elements not filtered by `mask` are set to zero in the output. |

For details about vector data registers and mask registers, see [reg data type definition](../defs/type/data_type_definition.md).

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

None

## Examples

```cpp
vector_int32_t src;
vector_uint8_t dst;
vector_bool mask = asc_create_mask_b32(PAT_ALL);
asc_loadalign(src, src_addr); // src_addr is the address of the external Unified Buffer (UB) memory space.
asc_int322uint8(dst, src, mask);
```
