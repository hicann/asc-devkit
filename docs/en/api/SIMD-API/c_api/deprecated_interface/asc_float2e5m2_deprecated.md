---
okftype: reference.api
description: "Deprecated float-to-fp8_e5m2_t register interface, supporting RINT rounding and saturation or non-saturation modes, writing results to the destination register by index 0 to 3. The prototype without explicit position parameters is deprecated, and the replacement is the explicit position parameter overload."
tag:
  - roles: [compute]
  - operations: [conversion]
  - traits: [elementwise, rounding_mode, saturation_mode]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_convert/asc_float2e5m2.md
    kind: replaced_by
    condition: "The explicit position parameter overload replaces the deprecated prototype without position parameters"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_convert/rounding_mode.md
    kind: related
    condition: "Rounding and saturation mode description"
  - path: docs/zh/api/SIMD-API/c_api/spr/asc_set_ctrl.md
    kind: requires
    condition: "Before enabling saturation or non-saturation mode, the ctrl register must be configured"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The prototypes without explicit positional parameters (including the _sat and _v2/_v3/_v4 index suffixes) have entered the deprecation period since 9.2.0. The include header marks asc_float2e5m2_rn, _rn_sat, and their index variants with ASC_DEPRECATED(9.2.0, 2027/09/07, asc_float2e5m2_rn); the CANN_9_2_0 Release Notes deprecation table covers the Reg type conversion positional suffix overloads; the interfaces are retained and not removed."
---
# asc_float2e5m2 (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T13:05:55.041Z pushedAt=2026-11-18T03:12:16.488Z nl=2 -->

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

**The prototype without explicit positional parameters of this interface has been deprecated. Use the explicit positional parameter overload of [asc_float2e5m2](../reg_compute/reg_convert/asc_float2e5m2.md) instead.**

Converts float type data to fp8_e5m2 type, supporting the RINT rounding mode and saturation/non-saturation modes.

For details about the rounding mode and saturation/non-saturation modes, see [Rounding Mode](../reg_compute/reg_convert/rounding_mode.md).

## Prototype

```cpp
// RINT rounding mode, non-saturation mode, write data to index 0
__simd_callee__ inline void asc_float2e5m2_rn(vector_fp8_e5m2_t& dst, vector_float src, vector_bool mask)
// RINT rounding mode, non-saturation mode, write data to index 1
__simd_callee__ inline void asc_float2e5m2_rn_v2(vector_fp8_e5m2_t& dst, vector_float src, vector_bool mask)
// RINT rounding mode, non-saturation mode, write data to index 2
__simd_callee__ inline void asc_float2e5m2_rn_v3(vector_fp8_e5m2_t& dst, vector_float src, vector_bool mask)
// RINT rounding mode, non-saturation mode, write data to index 3
__simd_callee__ inline void asc_float2e5m2_rn_v4(vector_fp8_e5m2_t& dst, vector_float src, vector_bool mask)
// RINT rounding mode, saturation mode, write data to index 0
__simd_callee__ inline void asc_float2e5m2_rn_sat(vector_fp8_e5m2_t& dst, vector_float src, vector_bool mask)
// RINT rounding mode, saturation mode, write the data to index 1.
__simd_callee__ inline void asc_float2e5m2_rn_sat_v2(vector_fp8_e5m2_t& dst, vector_float src, vector_bool mask)
// RINT rounding mode, saturation mode, write the data to index 2.
__simd_callee__ inline void asc_float2e5m2_rn_sat_v3(vector_fp8_e5m2_t& dst, vector_float src, vector_bool mask)
// RINT rounding mode, saturation mode, write the data to index 3.
__simd_callee__ inline void asc_float2e5m2_rn_sat_v4(vector_fp8_e5m2_t& dst, vector_float src, vector_bool mask)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description                                                                   |
|:------| :--- |:---------------------------------------------------------------------|
| dst   | Output | Destination operand (vector data register).                                                      |
| src   | Input | Source operand (vector data register).                                                            |
| mask  | Input | Source operand mask (mask register), used to indicate which elements participate in the computation. When the corresponding position is 1, it participates in the computation; when it is 0, it does not. Elements not selected by mask are set to zero in the output. |

For details about vector data registers and mask registers, see [reg data type definition](../defs/type/data_type_definition.md).

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- When enabling saturation mode and non-saturation mode, configure the ctrl register. For details about the ctrl register, see [asc_set_ctrl.md](../spr/asc_set_ctrl.md).

## Examples

```cpp
vector_fp8_e5m2_t dst;
vector_float src;
vector_bool mask = asc_create_mask_b32(PAT_ALL);
asc_loadalign(src, src_addr); // src_addr is the Unified Buffer (UB) memory space address input from outside.
asc_float2e5m2_rn(dst, src, mask);
```
