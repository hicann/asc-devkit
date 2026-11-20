---
okftype: reference.api
description: "Deprecated overload without explicit position parameters, which converts a bfloat16_t vector to fp4x2_e2m1_t using five rounding modes (rn, rna, rd, ru, rz) and writes the result into the destination register in four parts by indexes 0 to 3."
tag:
  - roles: [compute]
  - operations: [conversion]
  - traits: [rounding_mode]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_convert/asc_bfloat162e2m1x2.md
    kind: replaced_by
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_convert/rounding_mode.md
    kind: related
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The overload that implicitly controls the destination write position through the interface suffix is deprecated. Use the current overload of the same name that explicitly passes asc_position_quarter_mode instead. All 20 declarations in include/c_api/reg_compute/reg_convert.h carry ASC_DEPRECATED(9.2.0), and the CANN_9_2_0 deprecation table has the same source."
---
# asc_bfloat162e2m1x2 (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T11:16:08.507Z pushedAt=2026-11-18T03:12:16.363Z nl=2 -->

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

**The prototype of this interface without an explicit position parameter is deprecated. Use the explicit position parameter overload of [asc_bfloat162e2m1x2](../reg_compute/reg_convert/asc_bfloat162e2m1x2.md) instead.**

Converts the bfloat16_t type to the fp4x2_e2m1_t type and supports multiple rounding modes.

For details about rounding modes, see [Rounding Mode](../reg_compute/reg_convert/rounding_mode.md).

Because the bit-width ratio of the source operand to the destination operand is 4:1, when writing data, one VL-sized data block must be divided into four parts, and index 0, index 1, index 2, or index 3 is selected according to the interface.

## Prototype

```cpp
// RINT rounding mode, in which data is written to the position with index 0
__simd_callee__ inline void asc_bfloat162e2m1x2_rn(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
// RINT rounding mode, in which data is written to the position with index 1
__simd_callee__ inline void asc_bfloat162e2m1x2_rn_v2(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
// RINT rounding mode, in which data is written to the position with index 2
__simd_callee__ inline void asc_bfloat162e2m1x2_rn_v3(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
// RINT rounding mode, in whichdata is written to the position with index 3
__simd_callee__ inline void asc_bfloat162e2m1x2_rn_v4(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
// ROUND rounding mode, in which data written to index 0.
__simd_callee__ inline void asc_bfloat162e2m1x2_rna(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
// ROUND rounding mode, in which data written to index 1.
__simd_callee__ inline void asc_bfloat162e2m1x2_rna_v2(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
// ROUND rounding mode, in which data written to index 2.
__simd_callee__ inline void asc_bfloat162e2m1x2_rna_v3(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
// ROUND rounding mode, in which data written to index 3.
__simd_callee__ inline void asc_bfloat162e2m1x2_rna_v4(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
// FLOOR rounding mode, in which data written to index 0.
__simd_callee__ inline void asc_bfloat162e2m1x2_rd(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
// FLOOR rounding mode, in which data written to index 1.
__simd_callee__ inline void asc_bfloat162e2m1x2_rd_v2(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
// FLOOR rounding mode, in which data written to index 2.
__simd_callee__ inline void asc_bfloat162e2m1x2_rd_v3(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
// FLOOR rounding mode, in which data written to index 3.
__simd_callee__ inline void asc_bfloat162e2m1x2_rd_v4(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
// CEIL rounding mode, in which data written to index 0.
__simd_callee__ inline void asc_bfloat162e2m1x2_ru(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
// CEIL rounding mode, in which data written to index 1.
__simd_callee__ inline void asc_bfloat162e2m1x2_ru_v2(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
// CEIL rounding mode, in which data written to index 2.
__simd_callee__ inline void asc_bfloat162e2m1x2_ru_v3(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
// CEIL rounding mode, in which data written to index 3.
__simd_callee__ inline void asc_bfloat162e2m1x2_ru_v4(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
// TRUNC rounding mode, in which data written to index 0.
__simd_callee__ inline void asc_bfloat162e2m1x2_rz(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
// TRUNC rounding mode, in which data written to index 1.
__simd_callee__ inline void asc_bfloat162e2m1x2_rz_v2(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
// TRUNC rounding mode, in which data written to index 2.
__simd_callee__ inline void asc_bfloat162e2m1x2_rz_v3(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
// TRUNC rounding mode, in which data written to index 3.
__simd_callee__ inline void asc_bfloat162e2m1x2_rz_v4(vector_fp4x2_e2m1_t& dst, vector_bfloat16_t src, vector_bool mask)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| :----- | :------- | :------- |
| dst | Output | Destination operand (vector data register). |
| src | Input | Source operand (vector data register). |
| mask | Input | Source operand mask (mask register), used to indicate which elements participate in the computation. A value of 1 at the corresponding position means the element participates in the computation, and 0 means it does not. Elements not selected by mask are set to zero in the output. |

For details about vector data registers and mask registers, see [reg data type definition](../defs/type/data_type_definition.md).

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

None

## Examples

```cpp
vector_bfloat16_t src;
vector_fp4x2_e2m1_t dst;
vector_bool mask;
asc_bfloat162e2m1x2_rn(dst, src, mask);
```
