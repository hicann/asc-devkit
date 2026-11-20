---
okftype: reference.api
description: "Deprecated float-to-fp8_e4m3fn_t register interface that supports RINT rounding and saturation or non-saturation modes, writes the result to the destination register by index 0 to 3. The prototype with no explicit bit position parameter is deprecated, and the replacement is the explicit position parameter overload."
tag:
  - roles: [compute]
  - operations: [conversion]
  - traits: [elementwise, rounding_mode, saturation_mode]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_convert/asc_float2e4m3.md
    kind: replaced_by
    condition: "The explicit position parameter overload replaces the deprecated prototype without a position parameter"
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
    note: "The prototypes without explicit position parameters (including the _sat and _v2/_v3/_v4 index suffixes) entered the deprecation period starting from 9.2.0. The include header marks asc_float2e4m3_rn, _rn_sat, and their index variants individually with ASC_DEPRECATED(9.2.0, 2027/09/07, asc_float2e4m3_rn). The CANN_9_2_0 Release Notes deprecation table covers the Reg type conversion position suffix overloads. The interfaces are retained and not removed."
---
# asc_float2e4m3 (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T13:04:55.938Z pushedAt=2026-11-18T03:12:16.487Z nl=2 -->

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

**The prototype without explicit position parameters of this interface is deprecated. Use the explicit position parameter overload of [asc_float2e4m3](../reg_compute/reg_convert/asc_float2e4m3.md) instead.**

Converts the float type to the fp8_e4m3fn_t type, supporting the RINT rounding mode and saturation/non-saturation modes.

For details about the rounding mode and saturation/non-saturation modes, see [Rounding Mode](../reg_compute/reg_convert/rounding_mode.md).

Because the bit-width ratio between the source operand and the destination operand is 4:1, when writing data, one VL-sized data block must be divided into four parts, and index 0, index 1, index 2, or index 3 is selected according to the interface.

## Prototype

```cpp
// RINT rounding mode, non-saturation mode, write data to the position with index 0.
__simd_callee__ inline void asc_float2e4m3_rn(vector_fp8_e4m3fn_t& dst, vector_float src, vector_bool mask)
// RINT rounding mode, saturation mode, write data to the position with index 0.
__simd_callee__ inline void asc_float2e4m3_rn_sat(vector_fp8_e4m3fn_t& dst, vector_float src, vector_bool mask)
// RINT rounding mode, non-saturation mode, write data to the position with index 1.
__simd_callee__ inline void asc_float2e4m3_rn_v2(vector_fp8_e4m3fn_t& dst, vector_float src, vector_bool mask)
// RINT rounding mode, saturation mode, write data to the position with index 1.
__simd_callee__ inline void asc_float2e4m3_rn_sat_v2(vector_fp8_e4m3fn_t& dst, vector_float src, vector_bool mask)
// RINT rounding mode, non-saturation mode, data written to index 2
__simd_callee__ inline void asc_float2e4m3_rn_v3(vector_fp8_e4m3fn_t& dst, vector_float src, vector_bool mask)
// RINT rounding mode, saturation mode, data written to index 2
__simd_callee__ inline void asc_float2e4m3_rn_sat_v3(vector_fp8_e4m3fn_t& dst, vector_float src, vector_bool mask)
// RINT rounding mode, non-saturation mode, data written to index 3
__simd_callee__ inline void asc_float2e4m3_rn_v4(vector_fp8_e4m3fn_t& dst, vector_float src, vector_bool mask)
// RINT rounding mode, saturation mode, data written to index 3
__simd_callee__ inline void asc_float2e4m3_rn_sat_v4(vector_fp8_e4m3fn_t& dst, vector_float src, vector_bool mask)

```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| :----- | :------- | :------- |
| dst | Output | Destination operand (vector data register). |
| src | Input | Source operand (vector data register). |
| mask | Input | Source operand mask (mask register), used to indicate which elements participate in the computation. A value of 1 at the corresponding position indicates participation in the computation, and 0 indicates non-participation. Elements not filtered by mask are set to zero in the output. |

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
vector_fp8_e4m3fn_t dst;
vector_bool mask;
asc_float2e4m3_rn(dst, src, mask);
```
