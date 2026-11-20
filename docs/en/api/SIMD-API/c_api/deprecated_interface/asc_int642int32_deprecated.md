---
okftype: reference.api
description: "Deprecated implicit position type conversion interface that converts an int64_t source operand to int32_t and writes it to the destination register by even or odd index, supporting four prototypes in total for saturation and non-saturation modes. The replacement is the explicit position parameter overload."
tag:
  - roles: [compute]
  - operations: [conversion]
  - traits: [saturation_mode]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: related
    condition: "Type definitions of vector data registers and mask registers"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_convert/asc_int642int32.md
    kind: replaced_by
    condition: "Page containing the explicit position parameter overload that replaces the implicit position prototypes on this page"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_convert/rounding_mode.md
    kind: related
    condition: "Rounding mode and saturation/non-saturation mode description"
  - path: docs/zh/api/SIMD-API/c_api/spr/asc_set_ctrl.md
    kind: constrained_by
    condition: "To enable saturation or non-saturation mode, the ctrl register must be configured first"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The four implicit position prototypes are marked in include/c_api/reg_compute/reg_convert.h with multi-line ASC_DEPRECATED(9.2.0, 2027/09/07, asc_int642int32(..., ASC_POSITION_EVEN)) form (the saturation form corresponds to asc_int642int32_sat, and the _v2 form corresponds to ASC_POSITION_ODD), with the replacement being the explicit position parameter overload; line 129 of the CANN_9_2_0 Release Notes states that the position suffix overload enters the deprecation period, and deprecation does not impose an upper limit on usability."
---
# asc_int642int32 (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T13:58:11.449Z pushedAt=2026-11-18T03:12:16.574Z nl=2 -->

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

**The prototypes of this interface that control the write position of the destination operand through the interface suffix (including only the source operand, destination operand, and mask register parameters) are deprecated. Use the explicit position parameter overload of [asc_int642int32](../reg_compute/reg_convert/asc_int642int32.md) instead.**

Converts the int64_t source operand to int32_t and supports different saturation modes.

For details about rounding modes and saturation/non-saturation modes, see [Rounding Mode](../reg_compute/reg_convert/rounding_mode.md).

Because the bit-width ratio of the source operand to the destination operand is 2:1, data of one VL size must be divided into two parts when writing. Depending on the interface, data is written to positions with even indexes or odd indexes.

## Prototype

```cpp
// Non-saturation mode. Data is written to positions with even indexes.
__simd_callee__ inline void asc_int642int32(vector_int32_t& dst, vector_int64_t src, vector_bool mask)
// Saturation mode. Data is written to positions with even indexes.
__simd_callee__ inline void asc_int642int32_sat(vector_int32_t& dst, vector_int64_t src, vector_bool mask)
// Non-saturation mode. Data is written to positions with odd indexes.
__simd_callee__ inline void asc_int642int32_v2(vector_int32_t& dst, vector_int64_t src, vector_bool mask)
// Saturation mode. Data is written to positions with odd indexes.
__simd_callee__ inline void asc_int642int32_sat_v2(vector_int32_t& dst, vector_int64_t src, vector_bool mask)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Destination operand (vector data register). |
| src | Input | Source operand (vector data register). |
| mask | Input | Source operand mask (mask register), used to indicate which elements participate in the calculation. A value of 1 at the corresponding position means the element participates in the calculation, and 0 means it does not. Elements not selected by mask are set to zero in the output. |

For details about vector data registers and mask registers, see [reg data type definition](../defs/type/data_type_definition.md).

## Return Value

None

## Constraints

- mask controls whether the source operand participates in the calculation. Elements of the source operand that do not participate in the calculation are set to zero at the corresponding positions in the output.
- When data is written to positions with odd indices in the destination operand, the values at even-indexed positions are set to zero. The reverse is also true.
- When enabling saturation mode and non-saturation mode, the ctrl register must be configured. For details about the ctrl register, see [asc_set_ctrl.md](../spr/asc_set_ctrl.md).

## Examples

```cpp
vector_int32_t dst;
vector_int64_t src;
vector_bool mask;
mask = asc_create_mask_b32(PAT_ALL); // The current mask supports only b8, b16, and b32 modes. A low-bit-width fullmask can be used as a fullmask for a high-bit-width data type.
asc_int642int32(dst, src, mask);
asc_int642int32_sat(dst, src, mask);
asc_int642int32_v2(dst, src, mask);
asc_int642int32_sat_v2(dst, src, mask);
```