---
okftype: reference.api
description: "Deprecated implicit position type conversion interface that converts int8_t data to half and reads source data by even or odd index. The two prototypes determine the read position by the function name suffix. The replacement is the explicit position parameter overload."
tag:
  - roles: [compute]
  - operations: [conversion]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: related
    condition: "Type definitions of vector_int8_t and vector_half"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_convert/asc_int82half.md
    kind: replaced_by
    condition: "The page containing the explicit position parameter overload that replaces the implicit position prototypes on this page"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The two implicit position prototypes are marked in include/c_api/reg_compute/reg_convert.h in the form ASC_DEPRECATED(9.2.0, 2027/09/07, asc_int82half(...ASC_POSITION_EVEN)) (_v2 corresponds to ASC_POSITION_ODD). The replacement is the explicit position parameter overload. Line 129 of the CANN_9_2_0 Release Notes states that the position suffix overload enters the deprecation period, and deprecation does not impose an upper availability limit."
---
# asc_int82half (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T14:00:02.005Z pushedAt=2026-11-18T03:12:16.578Z nl=2 -->

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

**The prototypes that control the destination operand write position through the interface suffix (including only the three parameters: source operand, destination operand, and mask register) are deprecated. Use the explicit position parameter overload of [asc_int82half](../reg_compute/reg_convert/asc_int82half.md) instead.**

Converts int8_t data to half type.

Because the bit-width ratio between the source operand and the destination operand is 1:2, when reading data, one VL-sized data block must be divided into two parts. Depending on the interface, the input data at odd-indexed positions or even-indexed positions is selected.

## Prototype

```cpp
// Select even-indexed positions
__simd_callee__ inline void asc_int82half(vector_half& dst, vector_int8_t src, vector_bool mask)
// Select odd-indexed positions
__simd_callee__ inline void asc_int82half_v2(vector_half& dst, vector_int8_t src, vector_bool mask)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Destination operand (vector data register). |
| src | Input | Source operand (vector data register). |
| mask | Input | Source operand mask (mask register), used to indicate which elements participate in the computation. When the corresponding position is 1, the element participates in the computation; when it is 0, it does not. Elements not selected by mask are set to zero in the output. |

For details about vector data registers and mask registers, see [reg data type definition](../defs/type/data_type_definition.md).

## Return Value

None

## Constraints

mask controls whether the source operand participates in the computation. Elements of the source operand that do not participate in the computation are set to zero at the corresponding positions in the output.

## Examples

```cpp
vector_int8_t src;
vector_half dst;
vector_bool mask = asc_create_mask_b8(PAT_ALL);
asc_loadalign(src, src_addr); // src_addr is the externally input Unified Buffer (UB) memory space address.
asc_int82half(dst, src, mask);
```
