---
okftype: reference.api
description: "Deprecated API page: the asc_storealign_pack_v2 form with addr_reg offset compresses the low 8 bits of the 32-bit elements selected by mask and moves them out to a 32-byte-aligned UB address. The offset must be pre-constructed with asc_update_addr_reg. It is recommended to use asc_storealign_pack_quarter instead."
tag:
  - roles: [data_movement]
  - operations: [compression, store]
  - traits: [aligned_transfer]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: constrained_by
    condition: "The vector data register, mask register, and address register are defined according to their data type definitions"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_addr_reg/asc_update_addr_reg.md
    kind: requires
    condition: "offset must be pre-constructed through this API"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/store/asc_storealign_pack_quarter.md
    kind: replaced_by
    condition: "Release Notes 9.2.0 requires using this interface to express the quarter-width pack mode"
version: ">=9.2.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "Release Notes 9.2.0 deprecation table lists asc_storealign_pack_v2 and its postupdate form as deprecated, replaced by the asc_storealign_pack_quarter series; the form with addr_reg offset on this page remains available."
---
# asc_storealign_pack_v2 (Address Register Offset, Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T15:02:16.501Z pushedAt=2026-11-18T03:12:16.679Z nl=2 -->

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

Compresses and moves the low 8-bit data of the valid 32-bit elements indicated by `mask` in the vector data register to the Unified Buffer (UB). This API passes the offset through the address register `offset`, and the user can choose to update the offset or the destination operand address.

## Prototype

```cpp
__simd_callee__ inline void asc_storealign_pack_v2(__ubuf__ int32_t* dst_align32b, vector_int32_t src, addr_reg offset, vector_bool mask)
__simd_callee__ inline void asc_storealign_pack_v2(__ubuf__ uint32_t* dst_align32b, vector_uint32_t src, addr_reg offset, vector_bool mask)
__simd_callee__ inline void asc_storealign_pack_v2(__ubuf__ float* dst_align32b, vector_float src, addr_reg offset, vector_bool mask)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst_align32b | Output | Start address of the destination operand, which must be 32-byte aligned. |
| src | Input | Source operand (vector data register). Supported data types are `int32_t`, `uint32_t`, and `float`. |
| offset | Input | Address register offset, of type `addr_reg`, pre-constructed by the corresponding address register construction API. |
| mask | Input | Source operand mask (mask register), used to indicate the elements that participate in the move. A value of 1 at the corresponding position indicates participation in the move, and 0 indicates no participation. |

For details about the vector data register, mask register, and address register, see [reg Data Type Definition](../defs/type/data_type_definition.md).

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- The start address of `dst_align32b` must be 32-byte aligned.
- `offset` must be pre-constructed through [asc_update_addr_reg](../reg_compute/reg_addr_reg/asc_update_addr_reg.md).
- `mask` must be assigned a value through the mask setting API before being passed in.

## Examples

```cpp
__ubuf__ int32_t* dst_align32b = (__ubuf__ int32_t*)asc_get_phy_buf_addr(0);
vector_int32_t src;
addr_reg offset = asc_update_addr_reg_b32(0);
vector_bool mask = asc_create_mask_b32(PAT_ALL);
asc_storealign_pack_v2(dst_align32b, src, offset, mask);
```
