---
okftype: reference.api
description: "Deprecated implicit position type conversion interface that converts int64_t data to float according to five rounding modes: RINT, ROUND, FLOOR, CEIL, or TRUNC. Each mode includes two prototypes for even and odd positions. The replacement is the explicit position parameter overload."
tag:
  - roles: [compute]
  - operations: [conversion]
  - traits: [rounding_mode]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: related
    condition: "Type definitions of vector data registers and mask registers"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_convert/asc_int642float.md
    kind: replaced_by
    condition: "The page containing the explicit position parameter overload that replaces the implicit position prototypes on this page"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_convert/rounding_mode.md
    kind: related
    condition: "Detailed description of the five rounding modes"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The ten implicit-position prototypes (asc_int642float_rn/_rna/_rd/_ru/_rz and their respective _v2) are marked as deprecated in include/c_api/reg_compute/reg_convert.h by rounding mode with ASC_DEPRECATED(9.2.0, 2027/09/07, asc_int642float_<mode>). The replacement is the same-name explicit-position parameter overload (pass ASC_POSITION_EVEN or ASC_POSITION_ODD as the fourth parameter). Line 129 of the CANN_9_2_0 Release Notes states that the position-suffix overloads have entered the deprecation period; deprecation does not impose an upper limit on availability."
---
# asc_int642float (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T13:56:35.951Z pushedAt=2026-11-18T03:12:16.573Z nl=2 -->

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

**The prototypes that control the destination operand write position through the interface suffix (including only the three parameters: source operand, destination operand, and mask register) are deprecated. Use the explicit position parameter overloads of [asc_int642float](../reg_compute/reg_convert/asc_int642float.md) instead.**

Converts int64_t data to float and supports multiple rounding modes.

For details about rounding modes, see [Rounding Mode](../reg_compute/reg_convert/rounding_mode.md).

Because the bit-width ratio between the source operand and the destination operand is 2:1, when writing data, one VL-sized data must be divided into two parts. Depending on the interface, the data is written to either odd-indexed or even-indexed positions.

## Prototype

```cpp
// RINT rounding mode, write data to even-indexed positions.
__simd_callee__ inline void asc_int642float_rn(vector_float& dst, vector_int64_t src, vector_bool mask)
// RINT rounding mode, write data to odd-indexed positions.
__simd_callee__ inline void asc_int642float_rn_v2(vector_float& dst, vector_int64_t src, vector_bool mask)
// ROUND rounding mode, write data to even-indexed positions.
__simd_callee__ inline void asc_int642float_rna(vector_float& dst, vector_int64_t src, vector_bool mask)
// ROUND rounding mode, write data to odd-indexed positions.
__simd_callee__ inline void asc_int642float_rna_v2(vector_float& dst, vector_int64_t src, vector_bool mask)
// FLOOR rounding mode. Write data to even-indexed positions.
__simd_callee__ inline void asc_int642float_rd(vector_float& dst, vector_int64_t src, vector_bool mask)
// FLOOR rounding mode. Write data to odd-indexed positions.
__simd_callee__ inline void asc_int642float_rd_v2(vector_float& dst, vector_int64_t src, vector_bool mask)
// CEIL rounding mode. Write data to even-indexed positions.
__simd_callee__ inline void asc_int642float_ru(vector_float& dst, vector_int64_t src, vector_bool mask)
// CEIL rounding mode. Write data to odd-indexed positions.
__simd_callee__ inline void asc_int642float_ru_v2(vector_float& dst, vector_int64_t src, vector_bool mask)
// TRUNC rounding mode. Write data to even-indexed positions.
__simd_callee__ inline void asc_int642float_rz(vector_float& dst, vector_int64_t src, vector_bool mask)
// TRUNC rounding mode. Write data to odd-indexed positions.
__simd_callee__ inline void asc_int642float_rz_v2(vector_float& dst, vector_int64_t src, vector_bool mask)
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
- When data is written to odd-indexed positions of the destination operand, the values at even-indexed positions are set to zero. The reverse is also true.

## Examples

```cpp
vector_int64_t src;
vector_float dst;
vector_bool mask = asc_create_mask_b32(PAT_ALL);
asc_loadalign(src, src_addr); // src_addr is the externally input Unified Buffer (UB) memory space address.
asc_int642float_rn(dst, src, mask);
```
