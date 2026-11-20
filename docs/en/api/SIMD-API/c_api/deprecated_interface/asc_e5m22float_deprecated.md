---
okftype: reference.api
description: "Deprecated FP8 conversion API: divides the vector_fp8_e5m2_t source operand into four parts by 256B, reads one part at a time and converts it to vector_float written to the destination operand. The replacement is the explicit positional parameter overload."
tag:
  - roles: [compute]
  - operations: [conversion]
  - traits: [elementwise]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: related
    condition: "vector data register and mask register definitions"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_convert/asc_e5m22float.md
    kind: replaced_by
    condition: "explicit positional parameter overloads replace the deprecated prototype without positional parameters"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The prototype without explicit position parameters and the _v2/_v3/_v4 position suffix forms entered the deprecation period starting from 9.2.0. The include header marks all 4 overloads with ASC_DEPRECATED(9.2.0, 2027/09/07, asc_e5m22float). The CANN_9_2_0 Release Notes deprecation table covers the Reg type conversion position suffix overloads. The interface is retained and not removed."
---
# asc_e5m22float (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T12:51:35.991Z pushedAt=2026-11-18T03:12:16.472Z nl=2 -->

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

**The prototype of this interface without explicit position parameters is deprecated. Use the explicit position parameter overload of [asc_e5m22float](../reg_compute/reg_convert/asc_e5m22float.md) instead.**

Divides the source operand of type vector_fp8_e5m2_t into four parts in units of 256B, reads the elements of one part, converts them to the vector_float type, and writes them to the destination operand.

- asc_e5m22float: reads the first part.

- asc_e5m22float_v2: reads the second part.

- asc_e5m22float_v3: reads the third part.

- asc_e5m22float_v4: reads the fourth part.

## Prototype

```cpp
__simd_callee__ inline void asc_e5m22float(vector_float& dst, vector_fp8_e5m2_t src, vector_bool mask)
__simd_callee__ inline void asc_e5m22float_v2(vector_float& dst, vector_fp8_e5m2_t src, vector_bool mask)
__simd_callee__ inline void asc_e5m22float_v3(vector_float& dst, vector_fp8_e5m2_t src, vector_bool mask)
__simd_callee__ inline void asc_e5m22float_v4(vector_float& dst, vector_fp8_e5m2_t src, vector_bool mask)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| --------- | ----- | ---------------- |
| dst       | Output    | Destination operand (vector data register).            |
| src | Input    | Source operand (vector data register).            |
| mask     | Input    | Source operand mask (mask register), used to indicate which elements participate in the computation. When the corresponding position is 1, the element participates in the computation; when it is 0, the element does not participate. Elements not selected by mask are set to zero in the output.        |

For details about vector data registers and mask registers, see [reg data type definition](../defs/type/data_type_definition.md).

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

None

## Examples

```cpp
vector_float dst;
vector_fp8_e5m2_t src;
vector_bool mask;
asc_e5m22float(dst, src, mask);    // Convert the first part of src to vector_float type and write it to dst.
asc_e5m22float_v2(dst, src, mask);    // Convert the second part of src to vector_float type and write it to dst.
asc_e5m22float_v3(dst, src, mask);    // Convert the third part of src to vector_float type and write it to dst.
asc_e5m22float_v4(dst, src, mask);    // Convert the fourth part of src to vector_float type and write it to dst.
```
