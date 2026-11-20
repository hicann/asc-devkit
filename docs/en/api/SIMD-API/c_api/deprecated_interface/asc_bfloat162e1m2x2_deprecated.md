---
okftype: reference.api
description: "Deprecated overload without an explicit position parameter. Converts a bfloat16_t vector to fp4x2_e1m2_t and writes to indexes 0 through 3 using the five rounding modes rn, rna, rd, ru, and rz. Use the overload of the same name with an explicit position parameter instead."
tag:
  - roles: [compute]
  - operations: [conversion]
  - traits: [rounding_mode]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_convert/asc_bfloat162e1m2x2.md
    kind: replaced_by
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_convert/rounding_mode.md
    kind: related
version: ">=9.0.0"
version_changes:
  - since: "9.1.0"
    kind: parameter_changed
    scope: "param:dst"
    scope_basis: resolved
    note: "The destination operand type is corrected from vector_f4e1m2x2 to vector_fp4x2_e1m2_t (the two are aliases of the same underlying type, see include/c_api/defs/type.h:48); the source bfloat16_t and mask remain unchanged. The CANN_9_1_0 Bug Fix section lists the entire rn/rna/rd/ru/rz family."
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The overload that implicitly controls the destination position through the interface suffix enters the deprecation period. Use the current overload of the same name that explicitly passes asc_position_quarter_mode instead. All 20 declarations in include carry ASC_DEPRECATED(9.2.0), and the CANN_9_2_0 deprecation table shares the same source."
---
# asc_bfloat162e1m2x2 (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T11:16:03.947Z pushedAt=2026-11-18T03:12:16.362Z nl=2 -->

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

**The prototype of this interface without an explicit position parameter is deprecated. Use the overload with an explicit position parameter of [asc_bfloat162e1m2x2](../reg_compute/reg_convert/asc_bfloat162e1m2x2.md) instead.**

Converts the bfloat16_t type to the fp4x2_e1m2_t type and supports multiple rounding modes.

For details about rounding modes, see [Rounding Modes](../reg_compute/reg_convert/rounding_mode.md).

Because the bit-width ratio between the source operand and the destination operand is 4:1, when writing data, the data of one VL size must be divided into four parts, and index 0, index 1, index 2, or index 3 is selected according to the interface.

## Prototype

```cpp
// RINT rounding mode, in which data is written to the position of index 0
__simd_callee__ inline void asc_bfloat162e1m2x2_rn(vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask)
// RINT rounding mode, in which data is written to the position of index 1
__simd_callee__ inline void asc_bfloat162e1m2x2_rn_v2(vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask)
// RINT rounding mode, in which data is written to the position of index 2
__simd_callee__ inline void asc_bfloat162e1m2x2_rn_v3(vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask)
// RINT rounding mode, in which data is written to the position of index 3
__simd_callee__ inline void asc_bfloat162e1m2x2_rn_v4(vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask)
// ROUND rounding mode, in which data is written to index 0.
__simd_callee__ inline void asc_bfloat162e1m2x2_rna(vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask)
// ROUND rounding mode, in which data is written to index 1.
__simd_callee__ inline void asc_bfloat162e1m2x2_rna_v2(vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask)
// ROUND rounding mode, in which data is written to index 2.
__simd_callee__ inline void asc_bfloat162e1m2x2_rna_v3(vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask)
// ROUND rounding mode, in which data is written to index 3.
__simd_callee__ inline void asc_bfloat162e1m2x2_rna_v4(vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask)
// FLOOR rounding mode, in which data is written to index 0.
__simd_callee__ inline void asc_bfloat162e1m2x2_rd(vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask)
// FLOOR rounding mode, in which data is written to index 1.
__simd_callee__ inline void asc_bfloat162e1m2x2_rd_v2(vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask)
// FLOOR rounding mode, in which data is written to index 2.
__simd_callee__ inline void asc_bfloat162e1m2x2_rd_v3(vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask)
// FLOOR rounding mode, in which data is written to index 3.
__simd_callee__ inline void asc_bfloat162e1m2x2_rd_v4(vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask)
// CEIL rounding mode, in which data is written to index 0.
__simd_callee__ inline void asc_bfloat162e1m2x2_ru(vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask)
// CEIL rounding mode, in which data is written to index 1.
__simd_callee__ inline void asc_bfloat162e1m2x2_ru_v2(vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask)
// CEIL rounding mode, in which data is written to index 2
__simd_callee__ inline void asc_bfloat162e1m2x2_ru_v3(vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask)
// CEIL rounding mode, in which data is written to index 3
__simd_callee__ inline void asc_bfloat162e1m2x2_ru_v4(vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask)
// TRUNC rounding mode, in which data is written to index 0
__simd_callee__ inline void asc_bfloat162e1m2x2_rz(vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask)
// TRUNC rounding mode, in which data is written to index 1
__simd_callee__ inline void asc_bfloat162e1m2x2_rz_v2(vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask)
// TRUNC rounding mode, in which data is written to index 2
__simd_callee__ inline void asc_bfloat162e1m2x2_rz_v3(vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask)
// TRUNC rounding mode, in which data is written to index 3
__simd_callee__ inline void asc_bfloat162e1m2x2_rz_v4(vector_fp4x2_e1m2_t& dst, vector_bfloat16_t src, vector_bool mask)
```

## Parameters

**Table 1** Parameter description

| Parameter    | Input/Output | Description                |
| :------ | :----- | :----------------- |
| dst    | Output    | Destination operand (vector data register).            |
| src    | Input    | Source operand (vector data register).             |
| mask | Input | Source operand mask (mask register), used to indicate which elements participate in the computation. A value of 1 at the corresponding position indicates participation in the computation, and 0 indicates non-participation. Elements not selected by mask are set to zero in the output. |

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
vector_fp4x2_e1m2_t dst;
vector_bool mask = asc_create_mask_b16(PAT_ALL);
asc_loadalign(src, src_addr); // src_addr is the externally input Unified Buffer (UB) memory space address.
asc_bfloat162e1m2x2_rn(dst, src, mask);
```
