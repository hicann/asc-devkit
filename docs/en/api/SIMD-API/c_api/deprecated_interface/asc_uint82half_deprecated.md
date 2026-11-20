---
okftype: reference.api
description: "Deprecated interface page: On the 3510 architecture, asc_uint82half and asc_uint82half_v2 widen a uint8_t vector to half. Because the bit-width ratio is 1:2, the read position with an odd or even index must be selected by suffix. It is recommended to use the explicit position parameter overload instead."
tag:
  - roles: [compute]
  - operations: [conversion]
  - traits: [elementwise, widening_result]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: constrained_by
    condition: "The definitions of vector data registers and mask registers follow their data type definitions"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_convert/asc_uint82half.md
    kind: replaced_by
    condition: "Release Notes 9.2.0 requires replacing the prototypes that control the position via suffix with the explicit position parameter overload"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "Release Notes 9.2.0 lists the overloads in Reg type conversion that control the destination position through interface suffixes or implicit rules as deprecated, replaced by the explicit position parameter overloads of asc_uint82half; the two prototypes on this page remain available."
---
# asc_uint82half (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T15:19:32.682Z pushedAt=2026-11-18T03:12:16.698Z nl=2 -->

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

**The prototype of this interface that controls the destination operand write position through the interface suffix (including only the three parameters: source operand, destination operand, and mask register) is deprecated. Use the explicit position parameter overload of [asc_uint82half](../reg_compute/reg_convert/asc_uint82half.md) instead.**

Converts uint8_t data to half type.

Because the bit-width ratio of the source operand to the destination operand is 1:2, when reading data, one VL-sized data block must be split into two parts. Depending on the interface, select the positions where the input data index is odd or even.

## Prototype

```cpp
// Read data at even index positions
__simd_callee__ inline void asc_uint82half(vector_half& dst, vector_uint8_t src, vector_bool mask)    
// Read data at odd index positions
__simd_callee__ inline void asc_uint82half_v2(vector_half& dst, vector_uint8_t src, vector_bool mask)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Destination operand (vector data register). |
| src | Input | Source operand (vector data register). |
| mask | Input | Source operand mask (mask register), used to indicate which elements participate in the computation. When the corresponding position is 1, the element participates in the computation; when it is 0, it does not. Elements not selected by the mask are set to zero in the output. |

For details about vector data registers and mask registers, see [reg data type definition](../defs/type/data_type_definition.md).

## Return Value

None

## Constraints

- mask controls whether the source operand participates in the computation. Elements of the source operand that do not participate in the computation are set to zero at the corresponding positions in the output.

## Examples

```cpp
vector_uint8_t src;
vector_half dst;
vector_bool mask = asc_create_mask_b8(PAT_ALL);
asc_loadalign(src, src_addr); // src_addr is the address of the externally input Unified Buffer (UB) memory space.
asc_uint82half(dst, src, mask);
```