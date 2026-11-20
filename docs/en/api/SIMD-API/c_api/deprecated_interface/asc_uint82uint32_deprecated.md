---
okftype: reference.api
description: "Deprecated uint8_t to uint32_t vector conversion prototype description, divided into four forms by reading indexes 0 to 3: no suffix and v2, v3, v4, with a source-to-destination bit width ratio of 1 to 4."
tag:
  - roles: [compute]
  - operations: [conversion]
  - traits: [elementwise]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: related
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_convert/asc_uint82uint32.md
    kind: replaced_by
    condition: "Migrate to explicit positional parameter overloads"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "overload:asc_uint82uint32__Rvu32_vu8_vb"
    scope_basis: resolved
    note: "Since 9.2.0, ASC_DEPRECATED marks this position-suffix prototype; migrate to the explicit positional parameter overload. Deprecated but not removed, with no upper availability limit."
  - since: "9.2.0"
    kind: deprecated
    scope: "overload:asc_uint82uint32_v2__Rvu32_vu8_vb"
    scope_basis: resolved
    note: "Since 9.2.0, ASC_DEPRECATED marks this position-suffix prototype; migrate to the explicit positional parameter overload. Deprecated but not removed, with no upper availability limit."
  - since: "9.2.0"
    kind: deprecated
    scope: "overload:asc_uint82uint32_v3__Rvu32_vu8_vb"
    scope_basis: resolved
    note: "Since 9.2.0, ASC_DEPRECATED marks this position-suffix prototype; migrate to the explicit positional parameter overload. Deprecated but not removed, with no upper availability limit."
  - since: "9.2.0"
    kind: deprecated
    scope: "overload:asc_uint82uint32_v4__Rvu32_vu8_vb"
    scope_basis: resolved
    note: "Since 9.2.0, ASC_DEPRECATED marks this position-suffix prototype; migrate to the explicit positional parameter overload. Deprecated but not removed, with no upper availability limit."
---
# asc_uint82uint32 (deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T15:21:36.240Z pushedAt=2026-11-18T03:12:16.703Z nl=2 -->

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

**The prototype of this interface without explicit position parameters is deprecated. Use the explicit position parameter overload of [asc_uint82uint32](../reg_compute/reg_convert/asc_uint82uint32.md) instead.**

Converts the uint8_t type to the uint32_t type.

Because the bit-width ratio of the source operand to the destination operand is 1:4, when reading data, one VL-sized data block must be divided into four parts, and index 0, index 1, index 2, or index 3 is selected according to the specific interface.

## Prototype

```cpp
// Read the position at index 0.
__simd_callee__ inline void asc_uint82uint32(vector_uint32_t& dst, vector_uint8_t src, vector_bool mask)
// Read the position at index 1.
__simd_callee__ inline void asc_uint82uint32_v2(vector_uint32_t& dst, vector_uint8_t src, vector_bool mask)
// Read the position at index 2.
__simd_callee__ inline void asc_uint82uint32_v3(vector_uint32_t& dst, vector_uint8_t src, vector_bool mask)
// Read the position at index 3.
__simd_callee__ inline void asc_uint82uint32_v4(vector_uint32_t& dst, vector_uint8_t src, vector_bool mask)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| :----- | :------- | :------- |
| dst | Output | Destination operand (vector data register). |
| src | Input | Source operand (vector data register). |
| mask | Input | Source operand mask (mask register), used to indicate which elements participate in the computation. When the corresponding position is 1, the element participates in the computation; when it is 0, the element does not participate. Elements not selected by mask are set to zero in the output. |

For details about vector data registers and mask registers, see [reg data type definition](../defs/type/data_type_definition.md).

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

None

## Examples

```cpp
vector_uint8_t src;
vector_uint32_t dst;
vector_bool mask;
asc_uint82uint32(dst, src, mask);
```
