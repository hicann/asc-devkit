---
okftype: reference.api
description: "Deprecated implicit position type conversion interface that converts 4-bit signed integers packed in int4x2_t to half according to mask. Two elements in the same storage unit share one mask bit. The replacement is the explicit quarter position overload."
tag:
  - roles: [compute]
  - operations: [conversion]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: related
    condition: "Type definitions of vector_int4x2_t and vector_half"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_convert/asc_int4x22half.md
    kind: replaced_by
    condition: "The page containing the explicit quarter position overload that replaces the implicit position prototype on this page"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The four implicit position prototypes (asc_int4x22half and _v2, _v3, _v4) are marked as deprecated with ASC_DEPRECATED(9.2.0, 2027/09/07, asc_int4x22half) in include/c_api/reg_compute/reg_convert.h. The replacement is the current overload with the same name that explicitly selects the position using the asc_position_quarter_mode quarter position value (CANN_9_2_0 Release Notes line 61). Deprecation does not impose an upper limit on availability."
---
# asc_int4x22half (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T13:54:58.256Z pushedAt=2026-11-18T03:12:16.568Z nl=2 -->

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

**The prototype of this API without explicit position parameters is deprecated. Use the explicit position parameter overload of [asc_int4x22half](../reg_compute/reg_convert/asc_int4x22half.md) instead.**

`int4x2_t` packs two consecutive 4-bit signed integers into one 8-bit storage unit. This API converts the 4-bit signed integers that participate in the computation to the `half` type based on `mask`.

Because the bit width of each 4-bit signed integer is less than 1 byte, during actual processing, the two 4-bit signed integers in the same `int4x2_t` storage unit share one mask bit in `mask`. When this mask bit is 1, the two 4-bit signed integers participate in the conversion together, and the conversion result is two consecutively stored `half` type elements. When this mask bit is 0, the corresponding two `half` type elements are both set to 0.

## Prototype

```cpp
// Position with data read index 0.
__simd_callee__ inline void asc_int4x22half(vector_half& dst, vector_int4x2_t src, vector_bool mask)
// Position with data read index 1.
__simd_callee__ inline void asc_int4x22half_v2(vector_half& dst, vector_int4x2_t src, vector_bool mask)
// Position with data read index 2.
__simd_callee__ inline void asc_int4x22half_v3(vector_half& dst, vector_int4x2_t src, vector_bool mask)
// Position with data read index 3.
__simd_callee__ inline void asc_int4x22half_v4(vector_half& dst, vector_int4x2_t src, vector_bool mask)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description                                                                 |
|:------| :---|:---------------------------------------------------------------------|
| dst   | Output | Destination operand (vector data register).                                                      |
| src   | Input | Source operand (vector data register).                                                            |
| mask  | Input | Source operand mask (mask register), which indicates the elements that participate in the computation. When the corresponding position is 1, the element participates in the computation; when it is 0, the element does not participate. Elements not selected by mask are set to 0 in the output. |

For details about vector data registers and mask registers, see [reg data type definition](../defs/type/data_type_definition.md).

## Return Value

None

## Constraints

None

## Examples

```cpp
vector_half dst;
vector_int4x2_t src;
vector_bool mask = asc_create_mask_b8(PAT_ALL);
asc_loadalign(src, src_addr); // src_addr is the externally input Unified Buffer (UB) memory space address.
asc_int4x22half(dst, src, mask);
```
