---
okftype: reference.api
description: "Deprecated implicit-position type conversion interface that converts the 4-bit signed integers packed in int4x2_t to int16_t according to the mask, and reads the source data from a one-of-four index. The replacement is the explicit quarter-position overload."
tag:
  - roles: [compute]
  - operations: [conversion]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: related
    condition: "Type definitions of vector_int4x2_t and vector_int16_t"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_convert/asc_int4x22int16.md
    kind: replaced_by
    condition: "The page containing the explicit quarter-position overload that replaces the implicit-position prototypes on this page"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The four implicit-position prototypes (asc_int4x22int16 and _v2, _v3, _v4) are marked deprecated in include/c_api/reg_compute/reg_convert.h with ASC_DEPRECATED(9.2.0, 2027/09/07, asc_int4x22int16). The replacement is the current same-name overload that explicitly selects the position using the asc_position_quarter_mode quarter-position value (CANN_9_2_0 Release Notes line 61); deprecation does not impose an upper limit on availability."
---
# asc_int4x22int16 (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T13:55:14.896Z pushedAt=2026-11-18T03:12:16.566Z nl=2 -->

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

**The prototype of this interface without an explicit position parameter is deprecated. Use the explicit position parameter overload of [asc_int4x22int16](../reg_compute/reg_convert/asc_int4x22int16.md) instead.**

`int4x2_t` packs two consecutive 4-bit signed integers into one 8-bit storage unit. This interface converts the 4-bit signed integers that participate in the computation into the `int16_t` type based on `mask`.

Because the bit width of each 4-bit signed integer is less than 1 byte, during actual processing, the two 4-bit signed integers in the same `int4x2_t` storage unit jointly correspond to one mask bit in `mask`. When this mask bit is 1, the two 4-bit signed integers participate in the conversion at the same time, and the conversion result is two consecutively stored `int16_t` type elements. When it is 0, the corresponding two `int16_t` type elements are both set to 0.

Because the type bit-width ratio of a single source data element to a destination data element is 1:4, when reading data, one VL-sized data block must be divided into four parts, and index 0, index 1, index 2, or index 3 is selected according to the different interfaces.

## Prototype

```cpp
// Read data at index 0.
__simd_callee__ inline void asc_int4x22int16(vector_int16_t& dst, vector_int4x2_t src, vector_bool mask)
// Read data at index 1.
__simd_callee__ inline void asc_int4x22int16_v2(vector_int16_t& dst, vector_int4x2_t src, vector_bool mask)
// Read data at index 2.
__simd_callee__ inline void asc_int4x22int16_v3(vector_int16_t& dst, vector_int4x2_t src, vector_bool mask)
// Read data at index 3.
__simd_callee__ inline void asc_int4x22int16_v4(vector_int16_t& dst, vector_int4x2_t src, vector_bool mask)
```

## Parameters

**Table 1** Parameter description

| Parameter    | Input/Output | Description                |
| :------ | :----- | :----------------- |
| dst    | Output    | Destination operand (vector data register).            |
| src    | Input    | Source operand (vector data register).             |
| mask | Input | Source operand mask (mask register), used to indicate which elements participate in the computation. When the corresponding position is 1, the element participates in the computation; when it is 0, it does not. Elements not selected by mask are set to zero in the output. |

For details about vector data registers and mask registers, see [reg data type definition](../defs/type/data_type_definition.md).

## Return Value

None

## Constraints

None

## Examples

```cpp
vector_int4x2_t src;
vector_int16_t dst;
vector_bool mask = asc_create_mask_b8(PAT_ALL);
asc_loadalign(src, src_addr); // src_addr is the Unified Buffer (UB) memory space address from external input.
asc_int4x22int16(dst, src, mask);
```
