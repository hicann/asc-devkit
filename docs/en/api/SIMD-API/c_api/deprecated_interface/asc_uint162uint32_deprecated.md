---
okftype: reference.api
description: "Deprecated interface page: Under the 3510 architecture, asc_uint162uint32 and asc_uint162uint32_v2 widen-convert a uint16_t vector to uint32_t and write it to the destination vector register, using the interface suffix to control the odd/even position of the write index. It is recommended to use the explicit position parameter overload of asc_uint162uint32 instead."
tag:
  - roles: [compute]
  - operations: [conversion]
  - traits: [elementwise, widening_result]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: constrained_by
    condition: "The definitions of the vector data register and the mask register follow their data type definitions"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_convert/asc_uint162uint32.md
    kind: replaced_by
    condition: "Release Notes 9.2.0 requires using the explicit position parameter overload to replace the prototype that controls the position via the suffix"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "Release Notes 9.2.0 lists the overloads in Reg type conversion that control source/destination position through interface suffixes or implicit rules as deprecated, replaced by explicit position parameter overloads of the interface of the same name; the two prototypes on this page remain available."
---
# asc_uint162uint32 (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T15:13:37.871Z pushedAt=2026-11-18T03:12:16.691Z nl=2 -->

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

**The prototype of this interface that controls the destination operand write position through the interface suffix (including only the source operand, destination operand, and mask register parameters) is deprecated. Use the explicit position parameter overload of [asc_uint162uint32](../reg_compute/reg_convert/asc_uint162uint32.md) instead.**

Converts uint16_t type data to uint32_t type.

Since the bit width ratio of the source operand to the destination operand is 1:2, when reading data, one VL-sized data needs to be divided into two parts. Depending on the interface, select the positions where the input data index is odd or even.

## Prototype

```cpp
// Read data at even index positions.
__simd_callee__ inline void asc_uint162uint32(vector_uint32_t& dst, vector_uint16_t src, vector_bool mask)    
// Read data at odd index positions.
__simd_callee__ inline void asc_uint162uint32_v2(vector_uint32_t& dst, vector_uint16_t src, vector_bool mask)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Destination operand (vector data register). |
| src | Input | Source operand (vector data register). |
| mask | Input | Source operand mask (mask register), used to indicate which elements participate in the computation. A value of 1 at the corresponding position means participation, and 0 means no participation. Elements not selected by mask are set to zero in the output. |

For details about vector data registers and mask registers, see [reg data type definition](../defs/type/data_type_definition.md).

## Return Value

None

## Constraints

- mask controls whether the source operand participates in the computation. Elements of the source operand that do not participate are set to zero at the corresponding positions in the output.

## Examples

```cpp
vector_uint16_t src;
vector_uint32_t dst;
vector_bool mask = asc_create_mask_b16(PAT_ALL);
asc_loadalign(src, src_addr); // src_addr is the external Unified Buffer (UB) memory space address.
asc_uint162uint32(dst, src, mask);
```