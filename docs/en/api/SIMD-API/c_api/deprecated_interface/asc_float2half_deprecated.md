---
okftype: reference.api
description: "Deprecated float-to-half register interface that combines RINT, ROUND, FLOOR, CEIL, TRUNC, or ODD rounding with saturation modes, writing the result to the destination register by even index or _v2 odd index. The replacement is the explicit position parameter overload."
tag:
  - roles: [compute]
  - operations: [conversion]
  - traits: [elementwise, rounding_mode, saturation_mode]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_convert/asc_float2half.md
    kind: replaced_by
    condition: "Explicit position parameter overload replaces the deprecated prototype with position suffix"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_convert/rounding_mode.md
    kind: related
    condition: "Rounding and saturation mode description"
  - path: docs/zh/api/SIMD-API/c_api/spr/asc_set_ctrl.md
    kind: requires
    condition: "The ctrl register must be configured before enabling saturation or non-saturation mode"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The entire family of prototypes that use only the source, destination, and mask three parameters with suffixes controlling the write position and saturation state has entered the deprecation period since 9.2.0. The include header marks the rn/rna/rd/ru/rz/ro overloads and their _sat and _v2 overloads one by one with ASC_DEPRECATED(9.2.0, 2027/09/07, asc_float2half_<mode>); the CANN_9_2_0 Release Notes deprecation table lists the Reg type conversion position suffix overloads; the interfaces are retained and not removed."
---
# asc_float2half (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T13:07:45.424Z pushedAt=2026-11-18T03:12:16.492Z nl=2 -->

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

**The prototypes of this interface that control the destination operand write position through interface suffixes (including only the three parameters: source operand, destination operand, and mask register) are deprecated. Use the explicit position parameter overloads of [asc_float2half](../reg_compute/reg_convert/asc_float2half.md) instead.**

Converts float type data to half type, and supports multiple rounding modes and saturation modes.

For detailed descriptions of rounding modes and saturation/non-saturation modes, see [Rounding Modes and Saturation Modes](../reg_compute/reg_convert/rounding_mode.md).

Because the bit-width ratio of the source operand to the destination operand is 2:1, when writing data, one VL-sized data block must be divided into two parts, and data is written to either odd-indexed positions or even-indexed positions depending on the interface.

## Prototype

```cpp
// RINT rounding mode, non-saturation mode, data written to even-indexed positions.
__simd_callee__ inline void asc_float2half_rn(vector_half& dst, vector_float src, vector_bool mask)
// RINT rounding mode, saturation mode, data written to even-indexed positions.
__simd_callee__ inline void asc_float2half_rn_sat(vector_half& dst, vector_float src, vector_bool mask)
// RINT rounding mode, non-saturation mode, data written to odd-indexed positions.
__simd_callee__ inline void asc_float2half_rn_v2(vector_half& dst, vector_float src, vector_bool mask)
// RINT rounding mode, saturation mode, data written to odd-indexed positions.
__simd_callee__ inline void asc_float2half_rn_sat_v2(vector_half& dst, vector_float src, vector_bool mask)
// ROUND rounding mode, non-saturating mode, write data to even-indexed positions.
__simd_callee__ inline void asc_float2half_rna(vector_half& dst, vector_float src, vector_bool mask)
// ROUND rounding mode, saturating mode, write data to even-indexed positions.
__simd_callee__ inline void asc_float2half_rna_sat(vector_half& dst, vector_float src, vector_bool mask)
// ROUND rounding mode, non-saturating mode, write data to odd-indexed positions.
__simd_callee__ inline void asc_float2half_rna_v2(vector_half& dst, vector_float src, vector_bool mask)
// ROUND rounding mode, saturating mode, write data to odd-indexed positions.
__simd_callee__ inline void asc_float2half_rna_sat_v2(vector_half& dst, vector_float src, vector_bool mask)
// FLOOR rounding mode, non-saturating mode, write data to even-indexed positions.
__simd_callee__ inline void asc_float2half_rd(vector_half& dst, vector_float src, vector_bool mask)
// FLOOR rounding mode, saturating mode, write data to even-indexed positions.
__simd_callee__ inline void asc_float2half_rd_sat(vector_half& dst, vector_float src, vector_bool mask)
// FLOOR rounding mode, non-saturating mode, write data to odd-indexed positions.
__simd_callee__ inline void asc_float2half_rd_v2(vector_half& dst, vector_float src, vector_bool mask)
// FLOOR rounding mode, saturating mode, write data to odd-indexed positions.
__simd_callee__ inline void asc_float2half_rd_sat_v2(vector_half& dst, vector_float src, vector_bool mask)
// CEIL rounding mode, non-saturating mode, write data to even-indexed positions.
__simd_callee__ inline void asc_float2half_ru(vector_half& dst, vector_float src, vector_bool mask)
// CEIL rounding mode, saturating mode, write data to even-indexed positions.
__simd_callee__ inline void asc_float2half_ru_sat(vector_half& dst, vector_float src, vector_bool mask)
// CEIL rounding mode, non-saturating mode, data written to odd-indexed positions.
__simd_callee__ inline void asc_float2half_ru_v2(vector_half& dst, vector_float src, vector_bool mask)
// CEIL rounding mode, saturating mode, data written to odd-indexed positions.
__simd_callee__ inline void asc_float2half_ru_sat_v2(vector_half& dst, vector_float src, vector_bool mask)
// TRUNC rounding mode, non-saturating mode, data written to even-indexed positions.
__simd_callee__ inline void asc_float2half_rz(vector_half& dst, vector_float src, vector_bool mask)
// TRUNC rounding mode, saturating mode, data written to even-indexed positions.
__simd_callee__ inline void asc_float2half_rz_sat(vector_half& dst, vector_float src, vector_bool mask)
// TRUNC rounding mode, non-saturating mode, data written to odd-indexed positions.
__simd_callee__ inline void asc_float2half_rz_v2(vector_half& dst, vector_float src, vector_bool mask)
// TRUNC rounding mode, saturating mode, data written to odd-indexed positions.
__simd_callee__ inline void asc_float2half_rz_sat_v2(vector_half& dst, vector_float src, vector_bool mask)
// ODD rounding mode, non-saturating mode, data written to even-indexed positions.
__simd_callee__ inline void asc_float2half_ro(vector_half& dst, vector_float src, vector_bool mask)
// ODD rounding mode, saturating mode, data written to even-indexed positions.
__simd_callee__ inline void asc_float2half_ro_sat(vector_half& dst, vector_float src, vector_bool mask)
// ODD rounding mode, non-saturating mode, data written to odd-indexed positions.
__simd_callee__ inline void asc_float2half_ro_v2(vector_half& dst, vector_float src, vector_bool mask)
// ODD rounding mode, saturating mode, data written to odd-indexed positions.
__simd_callee__ inline void asc_float2half_ro_sat_v2(vector_half& dst, vector_float src, vector_bool mask)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Destination operand (vector data register). |
| src | Input | Source operand (vector data register). |
| mask | Input | Source operand mask (mask register), used to indicate which elements participate in the computation. A value of 1 at the corresponding position means the element participates in the computation, and 0 means it does not. Elements not selected by mask are set to zero in the output. |

For details about vector data registers and mask registers, see [reg data type definition](../defs/type/data_type_definition.md).

## Return Value

None

## Constraints

- mask controls whether the source operand participates in the computation. Elements of the source operand that do not participate in the computation are set to zero at the corresponding positions in the output.
- When data is written to odd-indexed positions of the destination operand, the values at even-indexed positions are set to zero. The reverse also applies.
- When saturation mode and non-saturation mode are enabled, the ctrl register must be configured. For details about the ctrl register, see [asc_set_ctrl.md](../spr/asc_set_ctrl.md).

## Examples

```cpp
vector_float src;
vector_half dst;
vector_bool mask = asc_create_mask_b32(PAT_ALL);
asc_loadalign(src, src_addr); // src_addr is the address of the Unified Buffer (UB) memory space input externally.
asc_float2half_rn(dst, src, mask);
```
