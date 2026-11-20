---
okftype: reference.api
description: "Deprecated float to hifloat8_t register interface that supports ROUND and HYBRID rounding modes and saturation or non-saturation mode, writes the result to the destination register by index 0 to 3, and is replaced by the explicit positional parameter overload."
tag:
  - roles: [compute]
  - operations: [conversion]
  - traits: [elementwise, rounding_mode, saturation_mode]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_convert/asc_float2hif8.md
    kind: replaced_by
    condition: "The explicit positional parameter overload replaces the deprecated prototype without positional parameters"
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
    note: "Prototypes without explicit positional parameters (including _sat and _v2/_v3/_v4 index suffixes) entered the deprecation period starting from 9.2.0. The include header marks asc_float2hif8_rna, _rna_sat, _rh, _rh_sat and their index variants with ASC_DEPRECATED(9.2.0, 2027/09/07, asc_float2hif8_<mode>); the CANN_9_2_0 Release Notes deprecation table covers Reg type conversion positional suffix overloads; the interface is retained, not removed."
---
# asc_float2hif8 (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T13:09:40.748Z pushedAt=2026-11-18T03:12:16.495Z nl=2 -->

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

**The prototypes of this interface without explicit position parameters are deprecated. Use the explicit position parameter overloads of [asc_float2hif8](../reg_compute/reg_convert/asc_float2hif8.md) instead.**

Converts the float type to the hifloat8_t type, and supports multiple rounding modes as well as saturation/non-saturation modes.

For details about rounding modes and saturation/non-saturation modes, see [rounding mode](../reg_compute/reg_convert/rounding_mode.md).

Because the bit-width ratio between the source operand and the destination operand is 4:1, when writing data, the data of one VL size must be divided into four parts, and index 0, index 1, index 2, or index 3 is selected according to the interface.

## Prototype

```cpp
// ROUND rounding mode, non-saturation mode, data written to index 0.
__simd_callee__ inline void asc_float2hif8_rna(vector_hifloat8_t& dst, vector_float src, vector_bool mask)
// ROUND rounding mode, saturation mode, data written to index 0.
__simd_callee__ inline void asc_float2hif8_rna_sat(vector_hifloat8_t& dst, vector_float src, vector_bool mask)
// ROUND rounding mode, non-saturation mode, data written to index 1.
__simd_callee__ inline void asc_float2hif8_rna_v2(vector_hifloat8_t& dst, vector_float src, vector_bool mask)
// ROUND rounding mode, saturation mode, data written to index 1.
__simd_callee__ inline void asc_float2hif8_rna_sat_v2(vector_hifloat8_t& dst, vector_float src, vector_bool mask)
// ROUND rounding mode, non-saturation mode, data written to index 2
__simd_callee__ inline void asc_float2hif8_rna_v3(vector_hifloat8_t& dst, vector_float src, vector_bool mask)
// ROUND rounding mode, saturation mode, data written to index 2
__simd_callee__ inline void asc_float2hif8_rna_sat_v3(vector_hifloat8_t& dst, vector_float src, vector_bool mask)
// ROUND rounding mode, non-saturation mode, data written to index 3
__simd_callee__ inline void asc_float2hif8_rna_v4(vector_hifloat8_t& dst, vector_float src, vector_bool mask)
// ROUND rounding mode, saturation mode, data written to index 3
__simd_callee__ inline void asc_float2hif8_rna_sat_v4(vector_hifloat8_t& dst, vector_float src, vector_bool mask)
// HYBRID rounding mode, non-saturation mode, data written to index 0
__simd_callee__ inline void asc_float2hif8_rh(vector_hifloat8_t& dst, vector_float src, vector_bool mask)
// HYBRID rounding mode, saturation mode, data written to index 0
__simd_callee__ inline void asc_float2hif8_rh_sat(vector_hifloat8_t& dst, vector_float src, vector_bool mask)
// HYBRID rounding mode, non-saturation mode, data written to index 1
__simd_callee__ inline void asc_float2hif8_rh_v2(vector_hifloat8_t& dst, vector_float src, vector_bool mask)
// HYBRID rounding mode, saturation mode, data written to index 1
__simd_callee__ inline void asc_float2hif8_rh_sat_v2(vector_hifloat8_t& dst, vector_float src, vector_bool mask)
// HYBRID rounding mode, non-saturation mode, data written to index 2
__simd_callee__ inline void asc_float2hif8_rh_v3(vector_hifloat8_t& dst, vector_float src, vector_bool mask)
// HYBRID rounding mode, saturation mode, data written to index 2
__simd_callee__ inline void asc_float2hif8_rh_sat_v3(vector_hifloat8_t& dst, vector_float src, vector_bool mask)
// HYBRID rounding mode, non-saturation mode, data written to index 3
__simd_callee__ inline void asc_float2hif8_rh_v4(vector_hifloat8_t& dst, vector_float src, vector_bool mask)
// HYBRID rounding mode, saturation mode, data written to index 3
__simd_callee__ inline void asc_float2hif8_rh_sat_v4(vector_hifloat8_t& dst, vector_float src, vector_bool mask)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| :----- | :------- | :------- |
| dst | Output | Destination operand (vector data register). |
| src | Input | Source operand (vector data register). |
| mask | Input | Source operand mask (mask register), used to indicate which elements participate in the computation. A value of 1 at the corresponding position indicates participation in the computation, and 0 indicates non-participation. Elements not selected by mask are set to zero in the output. |

For details about vector data registers and mask registers, see [reg data type definition](../defs/type/data_type_definition.md).

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- When enabling saturation mode and non-saturation mode, the ctrl register must be configured. For details about the ctrl register, see [asc_set_ctrl.md](../spr/asc_set_ctrl.md).

## Examples

```cpp
vector_float src;
vector_hifloat8_t dst;
vector_bool mask;
asc_float2hif8_rna(dst, src, mask);
```
