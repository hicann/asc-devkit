---
okftype: reference.api
description: "Deprecated API page: asc_storealign_pack_v2 passes the offset in the form of int32_t offset to compress and move the low 8 bits of the 32-bit elements selected by mask to a 32-byte-aligned UB address. The address after the offset must still be aligned. It is recommended to use asc_storealign_pack_quarter instead."
tag:
  - roles: [data_movement]
  - operations: [compression, store]
  - traits: [aligned_transfer]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: constrained_by
    condition: "The definitions of the vector data register and mask register follow their data type definitions"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/store/asc_storealign_pack_quarter.md
    kind: replaced_by
    condition: "Release Notes 9.2.0 requires using this API to express the quarter-width pack mode"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The Release Notes 9.2.0 deprecation table lists asc_storealign_pack_v2 as deprecated, replaced by asc_storealign_pack_quarter and asc_storealign_pack_quarter_postupdate; the int32_t offset form on this page remains available."
---
# asc_storealign_pack_v2 (Immediate Offset, Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T15:03:06.286Z pushedAt=2026-11-18T03:12:16.681Z nl=2 -->

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

The header file path is: `"c_api/reg_compute/store/storealign.h"`.

**This API is deprecated. Use [asc_storealign_pack_quarter](../reg_compute/store/asc_storealign_pack_quarter.md) to implement this function.**

Compresses the low 8 bits of the valid 32-bit elements indicated by `mask` in the vector data register and moves them out to the Unified Buffer (UB). This API passes the offset through `offset` of the `int32_t` type. You can choose to update the offset or the destination operand address.

## Prototype

```cpp
__simd_callee__ inline void asc_storealign_pack_v2(__ubuf__ int32_t* dst_align32b, vector_int32_t src, int32_t offset, vector_bool mask)
__simd_callee__ inline void asc_storealign_pack_v2(__ubuf__ uint32_t* dst_align32b, vector_uint32_t src, int32_t offset, vector_bool mask)
__simd_callee__ inline void asc_storealign_pack_v2(__ubuf__ float* dst_align32b, vector_float src, int32_t offset, vector_bool mask)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst_align32b | Output | Start address of the destination operand, which must be 32-byte aligned. |
| src | Input | Source operand (vector data register). Supported data types are `int32_t`, `uint32_t`, and `float`. |
| offset | Input | Offset relative to the start address of `dst_align32b`, of the `int32_t` type, in elements. |
| mask | Input | Source operand mask (mask register), used to indicate the elements that participate in the move-out. When the corresponding position is 1, the element participates in the move-out; when it is 0, the element does not participate. |

For details about the vector data register and the mask register, see [reg Data Type Definition](../defs/type/data_type_definition.md).

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- The start address of `dst_align32b` must be 32-byte aligned.
- The actual access address after the `offset` is applied must fall within the UB address range and must still be 32-byte aligned.
- `mask` must be assigned in advance through the mask setting API before being passed in.

## Examples

```cpp
__ubuf__ int32_t* dst_align32b = (__ubuf__ int32_t*)asc_get_phy_buf_addr(0);
vector_int32_t src;
int32_t offset = 0;
vector_bool mask = asc_create_mask_b32(PAT_ALL);
asc_storealign_pack_v2(dst_align32b, src, offset, mask);
```
