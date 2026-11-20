---
okftype: reference.api
description: "Deprecated implicit position type conversion interface that converts int8_t data to int32_t and reads source data from a one-of-four index. The four prototypes determine the read position by the function name suffix. The replacement is the explicit position parameter overload."
tag:
  - roles: [compute]
  - operations: [conversion]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: related
    condition: "Type definitions of vector_int8_t and vector_int32_t"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_convert/asc_int82int32.md
    kind: replaced_by
    condition: "Page containing the explicit position parameter overload that replaces the implicit position prototypes on this page"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The four implicit position prototypes are marked in include/c_api/reg_compute/reg_convert.h in the form ASC_DEPRECATED(9.2.0, 2027/09/07, asc_int82int32(...ASC_DISPERSE_FIRST_QUARTER)) (_v2, _v3, and _v4 correspond to the second through fourth quarter positions, respectively). The replacement is the overload that explicitly selects the position using asc_position_quarter_mode. Line 129 of the CANN_9_2_0 Release Notes states that the position suffix overloads enter the deprecation period, and deprecation does not cap the availability upper limit."
---
# asc_int82int32 (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T14:01:57.139Z pushedAt=2026-11-18T03:12:16.583Z nl=2 -->

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

**The prototypes of this interface without explicit position parameters are deprecated. Use the explicit position parameter overloads of [asc_int82int32](../reg_compute/reg_convert/asc_int82int32.md) instead.**

Converts int8_t type to int32_t type.

Because the bit-width ratio between the source operand and the destination operand is 1:4, when reading data, one VL-sized data needs to be divided into four parts, and index 0, index 1, index 2, or index 3 is selected according to different interfaces.

## Prototype

```cpp
// Read the data at index 0.
__simd_callee__ inline void asc_int82int32(vector_int32_t& dst, vector_int8_t src, vector_bool mask)
// Read the data at index 1.
__simd_callee__ inline void asc_int82int32_v2(vector_int32_t& dst, vector_int8_t src, vector_bool mask)
// Read the data at index 2.
__simd_callee__ inline void asc_int82int32_v3(vector_int32_t& dst, vector_int8_t src, vector_bool mask)
// Read the data at index 3.
__simd_callee__ inline void asc_int82int32_v4(vector_int32_t& dst, vector_int8_t src, vector_bool mask)
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

## Pipeline Type

PIPE_V

## Constraints

None

## Examples

```cpp
vector_int8_t src;
vector_int32_t dst;
vector_bool mask = asc_create_mask_b8(PAT_ALL);
asc_loadalign(src, src_addr); // src_addr is the Unified Buffer (UB) memory space address from external input.
asc_int82int32(dst, src, mask);
```
