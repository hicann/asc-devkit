---
okftype: reference.api
description: "Deprecated interface page: asc_storealign_pack_v2, in its no-offset form, compresses the lower 8 bits of the 32-bit elements selected by mask and moves them out to a 32-byte aligned UB address. The destination address must be updated by the user. It is recommended to use asc_storealign_pack_quarter instead."
tag:
  - roles: [data_movement]
  - operations: [compression, store]
  - traits: [aligned_transfer]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: constrained_by
    condition: "The vector data register and the mask register are defined according to their data type definitions"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/store/asc_storealign_pack_quarter.md
    kind: replaced_by
    condition: "Release Notes 9.2.0 requires using this interface to express the quarter-width pack mode"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The Release Notes 9.2.0 deprecation table lists asc_storealign_pack_v2 as deprecated, replaced by asc_storealign_pack_quarter and asc_storealign_pack_quarter_postupdate; the interface has not been removed."
---
# asc_storealign_pack_v2 (No Offset, Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T15:01:17.430Z pushedAt=2026-11-18T03:12:16.678Z nl=2 -->

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

The header file path is `"c_api/reg_compute/store/storealign.h"`.

**This API is deprecated. Use [asc_storealign_pack_quarter](../reg_compute/store/asc_storealign_pack_quarter.md) to implement this function instead.**

Compresses and moves the low 8 bits of the valid 32-bit elements indicated by `mask` in the vector data register to the Unified Buffer (UB). This API does not pass in an offset, and the user needs to update the destination operand address.

## Prototype

```cpp
__simd_callee__ inline void asc_storealign_pack_v2(__ubuf__ int32_t* dst_align32b, vector_int32_t src, vector_bool mask)
__simd_callee__ inline void asc_storealign_pack_v2(__ubuf__ uint32_t* dst_align32b, vector_uint32_t src, vector_bool mask)
__simd_callee__ inline void asc_storealign_pack_v2(__ubuf__ float* dst_align32b, vector_float src, vector_bool mask)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst_align32b | Output | Start address of the destination operand, which must be 32-byte aligned. |
| src | Input | Source operand (vector data register). Supported data types are `int32_t`, `uint32_t`, and `float`. |
| mask | Input | Source operand mask (mask register), used to indicate the elements that participate in the move. When the corresponding bit is 1, the element participates in the move; when it is 0, the element does not participate. |

For details about the vector data register and mask register, see [reg data type definition](../defs/type/data_type_definition.md).

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- The start address of `dst_align32b` must be 32-byte aligned.
- `mask` must be pre-assigned via the mask setting interface before being passed in.

## Examples

```cpp
__ubuf__ int32_t* dst_align32b = (__ubuf__ int32_t*)asc_get_phy_buf_addr(0);
vector_int32_t src;
vector_bool mask = asc_create_mask_b32(PAT_ALL);
asc_storealign_pack_v2(dst_align32b, src, mask);
```
