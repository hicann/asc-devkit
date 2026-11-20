---
okftype: reference.api
description: "Deprecated interface page: asc_storealign_pack_postupdate_v2 compresses and moves out the lower 8 bits of 32-bit elements with int32_t offset and automatically updates the destination pointer after moving out. The pointer is passed by reference and the new address must still be 32-byte aligned. It is recommended to use asc_storealign_pack_quarter_postupdate instead."
tag:
  - roles: [data_movement]
  - operations: [compression, store]
  - traits: [aligned_transfer, post_update]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: constrained_by
    condition: "The definitions of the vector data register and mask register follow their data type definitions"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/store/asc_storealign_pack_quarter_postupdate.md
    kind: replaced_by
    condition: "Release Notes 9.2.0 requires using this interface to express the quarter-width pack mode"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The Release Notes 9.2.0 deprecation table lists asc_storealign_pack_postupdate_v2 as deprecated, replaced by asc_storealign_pack_quarter_postupdate; the interface has not been removed."
---
# asc_storealign_pack_postupdate_v2 (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T15:04:05.616Z pushedAt=2026-11-18T03:12:16.680Z nl=2 -->

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

**This interface is deprecated. Please use [asc_storealign_pack_quarter_postupdate](../reg_compute/store/asc_storealign_pack_quarter_postupdate.md) to implement this functionality.**

Compresses the low 8-bit data of the valid 32-bit elements indicated by `mask` in the vector data register and moves them out to the Unified Buffer (UB). This interface passes in the offset through `offset` of type `int32_t`, and enables Post Update mode to automatically update the destination operand address.

## Prototype

```cpp
__simd_callee__ inline void asc_storealign_pack_postupdate_v2(__ubuf__ int32_t*& dst_align32b, vector_int32_t src, int32_t offset, vector_bool mask)
__simd_callee__ inline void asc_storealign_pack_postupdate_v2(__ubuf__ uint32_t*& dst_align32b, vector_uint32_t src, int32_t offset, vector_bool mask)
__simd_callee__ inline void asc_storealign_pack_postupdate_v2(__ubuf__ float*& dst_align32b, vector_float src, int32_t offset, vector_bool mask)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst_align32b | Input/Output | The start address of the destination operand, passed in by pointer reference. The start address must be 32-byte aligned. After the move-out is complete, this pointer is automatically updated by the hardware. |
| src | Input | The source operand (vector data register). The supported data types are `int32_t`, `uint32_t`, and `float`. |
| offset | Input | The destination address update amount, of type `int32_t`, in units of elements. After the interface is executed, the destination address is incremented by `offset × sizeof(dtype)` bytes. |
| mask | Input | The source operand mask (mask register), used to indicate the elements that participate in the move-out. When the corresponding position is 1, the element participates in the move-out; when it is 0, the element does not participate. |

For detailed descriptions of the vector data register and the mask register, see [reg data type definition](../defs/type/data_type_definition.md).

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- The start address of `dst_align32b` must be 32-byte aligned.
- The new address of `dst_align32b` after Post Update must still be 32-byte aligned.
- `mask` must be assigned in advance through the mask setting interface before being passed in.

## Examples

```cpp
__ubuf__ int32_t* dst_align32b = (__ubuf__ int32_t*)asc_get_phy_buf_addr(0);
vector_int32_t src;
int32_t offset = 64;
vector_bool mask = asc_create_mask_b32(PAT_ALL);
asc_storealign_pack_postupdate_v2(dst_align32b, src, offset, mask);
```
