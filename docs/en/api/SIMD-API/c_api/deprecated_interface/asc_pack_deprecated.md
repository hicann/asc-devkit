---
okftype: reference.api
description: "Deprecated Reg pack interface that takes the low 8, 16, or 32 bits of source operand elements and writes them to the lower half or upper half of the destination register, covering 5 groups of types such as vector_uint8_t, with asc_pack_to_low and asc_pack_to_high as replacements."
tag:
  - roles: [compute]
  - operations: [bit_manipulation]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_permute_sel/asc_pack_to_high.md
    kind: replaced_by
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_permute_sel/asc_pack_to_low.md
    kind: replaced_by
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "asc_pack and asc_pack_v2 are deprecated; use asc_pack_to_low and asc_pack_to_high instead. The 10 declarations starting from include/c_api/reg_compute/compute/reg_permute_sel.h:2709 all carry ASC_DEPRECATED(9.2.0), same source as line 124 of the CANN_9_2_0 deprecation table."
---
# asc_pack (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T14:34:33.254Z pushedAt=2026-11-18T03:12:16.634Z nl=2 -->

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

The header file path is: `"c_api/reg_compute/compute/reg_permute_sel.h"`.

**This API is deprecated. Use [asc_pack_to_low](../reg_compute/reg_permute_sel/asc_pack_to_low.md) and [asc_pack_to_high](../reg_compute/reg_permute_sel/asc_pack_to_high.md) instead.**

Selects the low 8 bits (b16), low 16 bits (b32), or low 32 bits (b64) of the elements in the source operand and writes them to the lower half or upper half of the destination operand.

- asc_pack: Writes the source operand to the lower half of the destination operand.

- asc_pack_v2: Writes the source operand to the upper half of the destination operand.

## Prototype

```cpp
__simd_callee__ inline void asc_pack(vector_uint8_t& dst, vector_uint16_t src)
__simd_callee__ inline void asc_pack(vector_uint8_t& dst, vector_int16_t src)
__simd_callee__ inline void asc_pack(vector_uint16_t& dst, vector_uint32_t src)
__simd_callee__ inline void asc_pack(vector_uint16_t& dst, vector_int32_t src)
__simd_callee__ inline void asc_pack(vector_bool& dst, vector_bool src)
__simd_callee__ inline void asc_pack_v2(vector_uint8_t& dst, vector_uint16_t src)
__simd_callee__ inline void asc_pack_v2(vector_uint8_t& dst, vector_int16_t src)
__simd_callee__ inline void asc_pack_v2(vector_uint16_t& dst, vector_uint32_t src)
__simd_callee__ inline void asc_pack_v2(vector_uint16_t& dst, vector_int32_t src)
__simd_callee__ inline void asc_pack_v2(vector_bool& dst, vector_bool src)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| --------- | ----- | ---------------- |
| dst       | Output    | Destination operand (vector data register/mask register).            |
| src | Input    | Source operand (vector data register/mask register).            |

For details about vector data registers and mask registers, see [reg data type definition](../defs/type/data_type_definition.md).

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

None

## Examples

```cpp
vector_uint8_t dst;
vector_uint16_t src;
asc_pack(dst, src);    // Write the low 8 bits of src to the lower half of dst.
asc_pack_v2(dst, src);    // Write the low 8 bits of src to the upper half of dst.
```