---
okftype: reference.api
description: "Deprecated Reg prototype description without explicit position parameters, converting half to int4x2, supporting rd, rn, rna, ru, rz rounding and optional sat saturation mode, and selecting the write index by the _v2 to _v4 suffix. Use the explicit position parameter overload instead."
tag:
  - roles: [compute]
  - operations: [conversion]
  - traits: [rounding_mode, saturation_mode]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: related
    condition: "The body text explains the vector data register and mask register accordingly"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_convert/asc_half2int4x2.md
    kind: replaced_by
    condition: "Use the rounding and saturation overload with explicit position parameters to replace the position suffix prototype"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_convert/rounding_mode.md
    kind: related
    condition: "The body text explains the rounding mode and saturation mode accordingly"
  - path: docs/zh/api/SIMD-API/c_api/spr/asc_set_ctrl.md
    kind: constrained_by
    condition: "The ctrl register must be configured when enabling saturation or non-saturation mode"
version: ">=9.0.0"
version_changes:
  - since: "9.1.0"
    kind: capability_extended
    scope: "section:function prototype"
    scope_basis: resolved
    note: "Rounding variants other than FLOOR enter the public header: asc_half2int4x2_rn, _rna, _ru, _rz and their _sat/_v2 to _v4 forms only have public declarations in include/c_api/reg_compute/reg_convert.h of upstream/9.1.0, while _rd has been public since 9.0.0-beta.1; the function prototype section of this page lists these rounding forms, so this extension is registered."
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "In include/c_api/reg_compute/reg_convert.h, asc_half2int4x2_rd/_rd_sat (starting at lines 20611-20650) and the ru/rn/rna/rz declarations without explicit position parameters are marked ASC_DEPRECATED(9.2.0, \"2027/09/07\", asc_half2int4x2_rd / asc_half2int4x2_ru, etc.), replaced by overloads with position parameters; the declarations are still retained in 9.3.0-beta.1, so this is deprecation rather than removal."
---
# asc_half2int4x2 (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T13:32:04.919Z pushedAt=2026-11-18T03:12:16.530Z nl=2 -->

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

**The prototype of this interface without explicit position parameters is deprecated. Use the overload with explicit position parameters of [asc_half2int4x2](../reg_compute/reg_convert/asc_half2int4x2.md) instead.**

Converts half type data to int4x2 type, and supports multiple rounding modes and saturation/non-saturation modes.

For details about rounding modes and saturation/non-saturation modes, see [Rounding Mode](../reg_compute/reg_convert/rounding_mode.md).

## Prototype

```cpp
// FLOOR rounding mode, non-saturation mode, select the position at index 0.
__simd_callee__ inline void asc_half2int4x2_rd(vector_int4x2_t& dst, vector_half src, vector_bool mask)
// FLOOR rounding mode, non-saturation mode, select the position at index 1.
__simd_callee__ inline void asc_half2int4x2_rd_v2(vector_int4x2_t& dst, vector_half src, vector_bool mask)
// FLOOR rounding mode, saturation mode, select the position at index 0.
__simd_callee__ inline void asc_half2int4x2_rd_sat(vector_int4x2_t& dst, vector_half src, vector_bool mask)
// FLOOR rounding mode, saturation mode, select the position at index 1.
__simd_callee__ inline void asc_half2int4x2_rd_sat_v2(vector_int4x2_t& dst, vector_half src, vector_bool mask)
// FLOOR rounding mode, non-saturation mode, select the position at index 2.
__simd_callee__ inline void asc_half2int4x2_rd_v3(vector_int4x2_t& dst, vector_half src, vector_bool mask)
// FLOOR rounding mode, saturation mode, select the position at index 2.
__simd_callee__ inline void asc_half2int4x2_rd_sat_v3(vector_int4x2_t& dst, vector_half src, vector_bool mask)
// FLOOR rounding mode, non-saturating mode, select the position of index 3.
__simd_callee__ inline void asc_half2int4x2_rd_v4(vector_int4x2_t& dst, vector_half src, vector_bool mask)
// FLOOR rounding mode, saturating mode, select the position of index 3.
__simd_callee__ inline void asc_half2int4x2_rd_sat_v4(vector_int4x2_t& dst, vector_half src, vector_bool mask)
// RINT rounding mode, non-saturating mode, select the position of index 0.
__simd_callee__ inline void asc_half2int4x2_rn(vector_int4x2_t& dst, vector_half src, vector_bool mask)
// RINT rounding mode, non-saturating mode, select the position of index 1.
__simd_callee__ inline void asc_half2int4x2_rn_v2(vector_int4x2_t& dst, vector_half src, vector_bool mask)
// RINT rounding mode, saturating mode, select the position of index 0.
__simd_callee__ inline void asc_half2int4x2_rn_sat(vector_int4x2_t& dst, vector_half src, vector_bool mask)
// RINT rounding mode, saturating mode, select the position of index 1.
__simd_callee__ inline void asc_half2int4x2_rn_sat_v2(vector_int4x2_t& dst, vector_half src, vector_bool mask)
// RINT rounding mode, non-saturating mode, select the position of index 2.
__simd_callee__ inline void asc_half2int4x2_rn_v3(vector_int4x2_t& dst, vector_half src, vector_bool mask)
// RINT rounding mode, saturating mode, select the position of index 2.
__simd_callee__ inline void asc_half2int4x2_rn_sat_v3(vector_int4x2_t& dst, vector_half src, vector_bool mask)
// RINT rounding mode, non-saturating mode, select the position of index 3.
__simd_callee__ inline void asc_half2int4x2_rn_v4(vector_int4x2_t& dst, vector_half src, vector_bool mask)
// RINT rounding mode, saturating mode, select the position of index 3.
__simd_callee__ inline void asc_half2int4x2_rn_sat_v4(vector_int4x2_t& dst, vector_half src, vector_bool mask)
// ROUND rounding mode, non-saturating mode, select the position with index 0.
__simd_callee__ inline void asc_half2int4x2_rna(vector_int4x2_t& dst, vector_half src, vector_bool mask)
// ROUND rounding mode, non-saturating mode, select the position with index 1.
__simd_callee__ inline void asc_half2int4x2_rna_v2(vector_int4x2_t& dst, vector_half src, vector_bool mask)
// ROUND rounding mode, saturating mode, select the position with index 0.
__simd_callee__ inline void asc_half2int4x2_rna_sat(vector_int4x2_t& dst, vector_half src, vector_bool mask)
// ROUND rounding mode, saturating mode, select the position with index 1.
__simd_callee__ inline void asc_half2int4x2_rna_sat_v2(vector_int4x2_t& dst, vector_half src, vector_bool mask)
// ROUND rounding mode, non-saturating mode, select the position with index 2.
__simd_callee__ inline void asc_half2int4x2_rna_v3(vector_int4x2_t& dst, vector_half src, vector_bool mask)
// ROUND rounding mode, saturating mode, select the position with index 2.
__simd_callee__ inline void asc_half2int4x2_rna_sat_v3(vector_int4x2_t& dst, vector_half src, vector_bool mask)
// ROUND rounding mode, non-saturating mode, select the position with index 3.
__simd_callee__ inline void asc_half2int4x2_rna_v4(vector_int4x2_t& dst, vector_half src, vector_bool mask)
// ROUND rounding mode, saturating mode, select the position with index 3.
__simd_callee__ inline void asc_half2int4x2_rna_sat_v4(vector_int4x2_t& dst, vector_half src, vector_bool mask)
// CEIL rounding mode, non-saturating mode, select the position with index 0.
__simd_callee__ inline void asc_half2int4x2_ru(vector_int4x2_t& dst, vector_half src, vector_bool mask)
// CEIL rounding mode, non-saturating mode, select the position with index 1.
__simd_callee__ inline void asc_half2int4x2_ru_v2(vector_int4x2_t& dst, vector_half src, vector_bool mask)
// CEIL rounding mode, saturation mode, select the position of index 0
__simd_callee__ inline void asc_half2int4x2_ru_sat(vector_int4x2_t& dst, vector_half src, vector_bool mask)
// CEIL rounding mode, saturation mode, select the position of index 1
__simd_callee__ inline void asc_half2int4x2_ru_sat_v2(vector_int4x2_t& dst, vector_half src, vector_bool mask)
// CEIL rounding mode, non-saturation mode, select the position of index 2
__simd_callee__ inline void asc_half2int4x2_ru_v3(vector_int4x2_t& dst, vector_half src, vector_bool mask)
// CEIL rounding mode, saturation mode, select the position of index 2
__simd_callee__ inline void asc_half2int4x2_ru_sat_v3(vector_int4x2_t& dst, vector_half src, vector_bool mask)
// CEIL rounding mode, non-saturation mode, select the position of index 3
__simd_callee__ inline void asc_half2int4x2_ru_v4(vector_int4x2_t& dst, vector_half src, vector_bool mask)
// CEIL rounding mode, saturation mode, select the position of index 3  
__simd_callee__ inline void asc_half2int4x2_ru_sat_v4(vector_int4x2_t& dst, vector_half src, vector_bool mask)
// TRUNC rounding mode, non-saturation mode, select the position of index 0
__simd_callee__ inline void asc_half2int4x2_rz(vector_int4x2_t& dst, vector_half src, vector_bool mask)
// TRUNC rounding mode, non-saturation mode, select the position of index 1
__simd_callee__ inline void asc_half2int4x2_rz_v2(vector_int4x2_t& dst, vector_half src, vector_bool mask)
// TRUNC rounding mode, saturation mode, select the position of index 0
__simd_callee__ inline void asc_half2int4x2_rz_sat(vector_int4x2_t& dst, vector_half src, vector_bool mask)
// TRUNC rounding mode, saturation mode, select the position of index 1
__simd_callee__ inline void asc_half2int4x2_rz_sat_v2(vector_int4x2_t& dst, vector_half src, vector_bool mask)
// TRUNC rounding mode, non-saturating mode, select the position with index 2
__simd_callee__ inline void asc_half2int4x2_rz_v3(vector_int4x2_t& dst, vector_half src, vector_bool mask)
// TRUNC rounding mode, saturating mode, select the position with index 2
__simd_callee__ inline void asc_half2int4x2_rz_sat_v3(vector_int4x2_t& dst, vector_half src, vector_bool mask)
// TRUNC rounding mode, non-saturating mode, select the position with index 3
__simd_callee__ inline void asc_half2int4x2_rz_v4(vector_int4x2_t& dst, vector_half src, vector_bool mask)
// TRUNC rounding mode, saturating mode, select the position with index 3 
__simd_callee__ inline void asc_half2int4x2_rz_sat_v4(vector_int4x2_t& dst, vector_half src, vector_bool mask)
```

## Parameters

**Table 1** Parameter description

|Parameter|Input/Output|Description|
| ------------ | ------------ | ------------ |
|dst|Output|Destination operand (vector data register).|
|src|Input|Source operand (vector data register).|
|mask|Input|Source operand mask (mask register), used to indicate which elements participate in the computation. When the corresponding position is 1, the element participates in the computation; when it is 0, the element does not participate. Elements not selected by mask are set to zero in the output.|

For details about vector data registers and mask registers, see [reg data type definition](../defs/type/data_type_definition.md).

## Return Value

None

## Constraints

- When enabling the saturating mode and non-saturating mode, you need to configure the ctrl register. For details about the ctrl register, see [asc_set_ctrl.md](../spr/asc_set_ctrl.md).

## Examples

```cpp
vector_half src;
vector_int4x2_t dst;
vector_bool mask = asc_create_mask_b16(PAT_ALL);
asc_loadalign(src, src_addr); // src_addr is the externally input Unified Buffer (UB) memory space address.
asc_half2int4x2_rna(dst, src, mask);
```
