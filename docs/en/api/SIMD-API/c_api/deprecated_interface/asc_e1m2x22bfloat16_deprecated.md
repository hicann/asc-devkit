---
okftype: reference.api
description: "Deprecated FP4 conversion interface: reads the fp4x2_e1m2_t source operand in four parts by index 0 to 3 and converts them to bfloat16_t. The prototype without explicit position parameter is deprecated, and the replacement is the explicit position parameter overload."
tag:
  - roles: [compute]
  - operations: [conversion]
  - traits: [elementwise]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: related
    condition: "vector data register and mask register definition"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_convert/asc_e1m2x22bfloat16.md
    kind: replaced_by
    condition: "explicit position parameter overload replaces the deprecated prototype without position parameter"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The prototype without explicit position parameters and the _v2/_v3/_v4 position suffix forms entered the deprecation period starting from 9.2.0. The include header marks all 4 overloads with ASC_DEPRECATED(9.2.0, 2027/09/07, asc_e1m2x22bfloat16); the CANN_9_2_0 Release Notes deprecation table describes the Reg type conversion position suffix overloads in the same entry; the interface is retained and not removed."
---
# asc_e1m2x22bfloat16 (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T12:48:24.119Z pushedAt=2026-11-18T03:12:16.466Z nl=2 -->

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

**The prototype without explicit position parameters of this API is deprecated. Use the explicit position parameter overload of [asc_e1m2x22bfloat16](../reg_compute/reg_convert/asc_e1m2x22bfloat16.md) instead.**

Converts the fp4x2_e1m2_t type to the bfloat16_t type.

Because the bit-width ratio between the source operand and the destination operand is 1:4, when reading data, one VL-sized data must be divided into four parts, and index 0, index 1, index 2, or index 3 is selected according to the specific API.

## Prototype

```cpp
// Position for reading data at index 0
__simd_callee__ inline void asc_e1m2x22bfloat16(vector_bfloat16_t& dst, vector_fp4x2_e1m2_t src, vector_bool mask)
// Position for reading data at index 1
__simd_callee__ inline void asc_e1m2x22bfloat16_v2(vector_bfloat16_t& dst, vector_fp4x2_e1m2_t src, vector_bool mask)
// Position for reading data at index 2
__simd_callee__ inline void asc_e1m2x22bfloat16_v3(vector_bfloat16_t& dst, vector_fp4x2_e1m2_t src, vector_bool mask)
// Position for reading data at index 3
__simd_callee__ inline void asc_e1m2x22bfloat16_v4(vector_bfloat16_t& dst, vector_fp4x2_e1m2_t src, vector_bool mask)
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
vector_fp4x2_e1m2_t src;
vector_bfloat16_t dst;
vector_bool mask = asc_create_mask_b8(PAT_ALL);
asc_loadalign(src, src_addr); // src_addr is the externally input Unified Buffer (UB) memory space address.
asc_e1m2x22bfloat16(dst, src, mask);
```
