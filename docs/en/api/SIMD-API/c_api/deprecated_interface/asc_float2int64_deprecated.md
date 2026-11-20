---
okftype: reference.api
description: "Deprecated float-to-int64_t register interface that combines RINT, ROUND, FLOOR, CEIL, or TRUNC rounding with saturation mode. Because the source and destination operand types have a bit-width ratio of 1:2, input data is read by even index or _v2 odd index. The replacement is the explicit position parameter overload."
tag:
  - roles: [compute]
  - operations: [conversion]
  - traits: [elementwise, rounding_mode, saturation_mode]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_convert/asc_float2int64.md
    kind: replaced_by
    condition: "The explicit position parameter overload replaces the deprecated prototypes with position suffixes"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_convert/rounding_mode.md
    kind: related
    condition: "Rounding and saturation mode description"
  - path: docs/zh/api/SIMD-API/c_api/spr/asc_set_ctrl.md
    kind: requires
    condition: "Configure the ctrl register before enabling saturation or non-saturation mode"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The entire family of prototypes that use only the source, destination, and mask parameters with a position suffix to control the read position entered the deprecation period starting from 9.2.0. The include header annotates the rn/rna/rd/ru/rz overloads and their _sat and _v2 overloads one by one with ASC_DEPRECATED(9.2.0, 2027/09/07, asc_float2int64_<mode>). The CANN_9_2_0 Release Notes deprecation table lists the Reg type conversion position suffix overloads. The interface is retained and not removed."
---
# asc_float2int64 (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T13:13:16.641Z pushedAt=2026-11-18T03:12:16.503Z nl=2 -->

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

**The prototypes of this interface that control the destination operand write position through the interface suffix (including only the source operand, destination operand, and mask register parameters) are deprecated. Use the explicit position parameter overload of [asc_float2int64](../reg_compute/reg_convert/asc_float2int64.md) instead.**

Converts float type data to int64_t type, and supports multiple rounding modes and saturation/non-saturation modes.

For details about rounding modes and saturation/non-saturation modes, see [Rounding Modes and Saturation Modes](../reg_compute/reg_convert/rounding_mode.md).

Because the source and destination operand types have a bit-width ratio of 1:2, when reading data, one VL-sized data block must be divided into two parts. Depending on the interface, the input data is read at odd-indexed positions or even-indexed positions.

## Prototype

```cpp
// RINT rounding mode, non-saturation mode, data read at even-indexed positions.
__simd_callee__ inline void asc_float2int64_rn(vector_int64_t& dst, vector_float src, vector_bool mask)
// RINT rounding mode, saturation mode, data read at even-indexed positions.
__simd_callee__ inline void asc_float2int64_rn_sat(vector_int64_t& dst, vector_float src, vector_bool mask)
// RINT rounding mode, non-saturation mode, data read at odd-indexed positions.
__simd_callee__ inline void asc_float2int64_rn_v2(vector_int64_t& dst, vector_float src, vector_bool mask)
// RINT rounding mode, saturation mode, data read at odd-indexed positions.
__simd_callee__ inline void asc_float2int64_rn_sat_v2(vector_int64_t& dst, vector_float src, vector_bool mask)
// ROUND rounding mode, non-saturation mode, data read at even index positions
__simd_callee__ inline void asc_float2int64_rna(vector_int64_t& dst, vector_float src, vector_bool mask)
// ROUND rounding mode, saturation mode, data read at even index positions
__simd_callee__ inline void asc_float2int64_rna_sat(vector_int64_t& dst, vector_float src, vector_bool mask)
// ROUND rounding mode, non-saturation mode, data read at odd index positions
__simd_callee__ inline void asc_float2int64_rna_v2(vector_int64_t& dst, vector_float src, vector_bool mask)
// ROUND rounding mode, saturation mode, data read at odd index positions
__simd_callee__ inline void asc_float2int64_rna_sat_v2(vector_int64_t& dst, vector_float src, vector_bool mask)
// FLOOR rounding mode, non-saturation mode, data read at even index positions
__simd_callee__ inline void asc_float2int64_rd(vector_int64_t& dst, vector_float src, vector_bool mask)
// FLOOR rounding mode, saturation mode, data read at even index positions
__simd_callee__ inline void asc_float2int64_rd_sat(vector_int64_t& dst, vector_float src, vector_bool mask)
// FLOOR rounding mode, non-saturation mode, data read at odd index positions
__simd_callee__ inline void asc_float2int64_rd_v2(vector_int64_t& dst, vector_float src, vector_bool mask)
// FLOOR rounding mode, saturation mode, data read at odd index positions
__simd_callee__ inline void asc_float2int64_rd_sat_v2(vector_int64_t& dst, vector_float src, vector_bool mask)
// CEIL rounding mode, non-saturation mode, data read at even index positions
__simd_callee__ inline void asc_float2int64_ru(vector_int64_t& dst, vector_float src, vector_bool mask)
// CEIL rounding mode, saturation mode, data read at even index positions
__simd_callee__ inline void asc_float2int64_ru_sat(vector_int64_t& dst, vector_float src, vector_bool mask)
// CEIL rounding mode, non-saturation mode, data read at odd indexes.
__simd_callee__ inline void asc_float2int64_ru_v2(vector_int64_t& dst, vector_float src, vector_bool mask)
// CEIL rounding mode, saturation mode, data read at odd indexes.
__simd_callee__ inline void asc_float2int64_ru_sat_v2(vector_int64_t& dst, vector_float src, vector_bool mask)
// TRUNC rounding mode, non-saturation mode, data read at even indexes.
__simd_callee__ inline void asc_float2int64_rz(vector_int64_t& dst, vector_float src, vector_bool mask)
// TRUNC rounding mode, saturation mode, data read at even indexes.
__simd_callee__ inline void asc_float2int64_rz_sat(vector_int64_t& dst, vector_float src, vector_bool mask)
// TRUNC rounding mode, non-saturation mode, data read at odd indexes.
__simd_callee__ inline void asc_float2int64_rz_v2(vector_int64_t& dst, vector_float src, vector_bool mask)
// TRUNC rounding mode, saturation mode, data read at odd indexes.
__simd_callee__ inline void asc_float2int64_rz_sat_v2(vector_int64_t& dst, vector_float src, vector_bool mask)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Destination operand (vector data register). |
| src | Input | Source operand (vector data register). |
| mask | Input | Source operand mask (mask register), used to indicate which elements participate in the computation. A value of 1 at the corresponding position means the element participates in the computation, and 0 means it does not. Elements not selected by mask are set to zero in the output. |

For details about vector data registers and mask registers, see [reg Data Type Definition](../defs/type/data_type_definition.md).

## Return Value

None

## Constraints

- mask controls whether the source operand participates in the computation. Elements of the source operand that do not participate in the computation are set to zero at the corresponding positions in the output.
- When saturation mode and non-saturation mode are enabled, the ctrl register must be configured. For details about the ctrl register, see [asc_set_ctrl.md](../spr/asc_set_ctrl.md).

## Examples

```cpp
vector_float src;
vector_int64_t dst;
vector_bool mask;
mask = asc_create_mask_b32(PAT_ALL);
asc_float2int64_rn(dst, src, mask);
```
