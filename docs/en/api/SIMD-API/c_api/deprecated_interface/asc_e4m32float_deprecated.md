---
okftype: reference.api
description: "Deprecated FP8 conversion interface: reads the fp8_e4m3fn_t source operand in four parts by index 0 to 3 and converts them to float. The prototype without an explicit position parameter is deprecated; the replacement is the explicit position parameter overload."
tag:
  - roles: [compute]
  - operations: [conversion]
  - traits: [elementwise]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: related
    condition: "Vector data register and mask register definitions"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_convert/asc_e4m32float.md
    kind: replaced_by
    condition: "Explicit position parameter overload replaces the deprecated prototype without a position parameter"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The prototype without an explicit position parameter and the _v2/_v3/_v4 position suffix forms entered the deprecation period starting from 9.2.0. The include header marks all 4 overloads with ASC_DEPRECATED(9.2.0, 2027/09/07, asc_e4m32float); the CANN_9_2_0 Release Notes deprecation table covers the Reg type conversion position suffix overloads; the interface is retained, not removed."
---
# asc_e4m32float (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T12:50:09.412Z pushedAt=2026-11-18T03:12:16.469Z nl=2 -->

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

**The prototype of this interface without an explicit position parameter has been deprecated. Use the explicit position parameter overload of [asc_e4m32float](../reg_compute/reg_convert/asc_e4m32float.md) instead.**

Converts the fp8_e4m3fn_t type to the float type.

Because the bit width ratio between the source operand and the destination operand is 1:4, when reading data, one VL-sized data block must be divided into four parts, and index 0, index 1, index 2, or index 3 is selected according to the interface.

## Prototype

```cpp
// Data read at index 0 position
__simd_callee__ inline void asc_e4m32float(vector_float& dst, vector_fp8_e4m3fn_t src, vector_bool mask)
// Data read at index 1 position
__simd_callee__ inline void asc_e4m32float_v2(vector_float& dst, vector_fp8_e4m3fn_t src, vector_bool mask)
// Data read at index 2 position
__simd_callee__ inline void asc_e4m32float_v3(vector_float& dst, vector_fp8_e4m3fn_t src, vector_bool mask)
// Data read at index 3 position
__simd_callee__ inline void asc_e4m32float_v4(vector_float& dst, vector_fp8_e4m3fn_t src, vector_bool mask)
```

## Parameters

**Table 1** Parameter description

| Parameter    | Input/Output | Description                |
| :------ | :----- | :----------------- |
| dst    | Output    | Destination operand (vector data register).            |
| src    | Input    | Source operand (vector data register).             |
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
vector_fp8_e4m3fn_t src;
vector_float dst;
vector_bool mask = asc_create_mask_b8(PAT_ALL);
asc_loadalign(src, src_addr); // src_addr is the externally input Unified Buffer (UB) memory space address.
asc_e4m32float(dst, src, mask);
```
