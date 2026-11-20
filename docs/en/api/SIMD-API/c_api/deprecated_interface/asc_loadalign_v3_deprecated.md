---
okftype: reference.api
description: "Deprecated integer-offset aligned load interface that loads aligned data continuously from Unified Buffer and passes the offset via an int32_t offset parameter. Users can choose to update the offset or source address. It supports three load modes with 32 prototypes in total. The replacement is a new interface with semantic naming."
tag:
  - roles: [data_movement]
  - operations: [load]
  - traits: [aligned_transfer, datablock_granularity]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: related
    condition: "Type definitions for vector data registers and address registers"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/load/asc_loadalign_brc_datablock.md
    kind: replaced_by
    condition: "Replacement for BRCV2 load mode"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/load/asc_loadalign_brc_elem2datablock.md
    kind: replaced_by
    condition: "Replacement for BRCV3 load mode"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/load/asc_loadalign_unpack4.md
    kind: replaced_by
    condition: "Replacement for UNPACKV2 load mode"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The 32 integer offset prototypes on this page are marked by mode in include/c_api/reg_compute/load/loadalign.h with ASC_DEPRECATED(9.2.0, 2027/09/07, asc_loadalign_unpack4), asc_loadalign_brc_datablock, and asc_loadalign_brc_elem2datablock; line 121 of the CANN_9_2_0 Release Notes lists asc_loadalign_brc_v2, brc_v3, unpack_v2, and the Post Update overloads in the deprecation and replacement table; deprecation does not cap the availability upper limit."
---
# asc_loadalign (Immediate Offset, Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T14:09:26.709Z pushedAt=2026-11-18T03:12:16.593Z nl=2 -->

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

The header file path is: `"c_api/reg_compute/load/loadalign.h"`.

**This API is deprecated. For asc_loadalign_brc_v2, use [asc_loadalign_brc_datablock](../reg_compute/load/asc_loadalign_brc_datablock.md); for asc_loadalign_brc_v3, use [asc_loadalign_brc_elem2datablock](../reg_compute/load/asc_loadalign_brc_elem2datablock.md); for asc_loadalign_unpack_v2, use [asc_loadalign_unpack4](../reg_compute/load/asc_loadalign_unpack4.md) to implement this function.**

The aligned data load API loads data from the Unified Buffer (UB) into the destination operand in a continuous aligned manner. It supports multiple load modes. The API passes the offset through int32_t, and the user can choose to update the offset or update the address of the source operand.

- UNPACKV2 load mode: unpack mode. Loads VL/4-length data as unsigned integer u8, unpacks it to VL-length u32 type, and pads the middle positions with 0.
- BRCV2 load mode: reads one DataBlock (32B) and broadcasts it to VL.
- BRCV3 load mode: loads (VL/DataBlock) B of data and broadcasts each element (16bit/32bit) into one DataBlock (32B).

## Prototype

```cpp
// UNPACKV2 load mode
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_int8_t& dst, __ubuf__ int8_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_uint8_t& dst, __ubuf__ uint8_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_hifloat8_t& dst, __ubuf__ hifloat8_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_int4x2_t& dst, __ubuf__ int4b_t* src, int32_t offset)
// BRCV2 load mode
__simd_callee__ inline void asc_loadalign_brc_v2(vector_int8_t& dst, __ubuf__ int8_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_uint8_t& dst, __ubuf__ uint8_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_hifloat8_t& dst, __ubuf__ hifloat8_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_int16_t& dst, __ubuf__ int16_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_uint16_t& dst, __ubuf__ uint16_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_half& dst, __ubuf__ half* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_bfloat16_t& dst, __ubuf__ bfloat16_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_int32_t& dst, __ubuf__ int32_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_uint32_t& dst, __ubuf__ uint32_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_float& dst, __ubuf__ float* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_int4x2_t& dst, __ubuf__ int4b_t* src, int32_t offset)
// BRCV3 load mode
__simd_callee__ inline void asc_loadalign_brc_v3(vector_int16_t& dst, __ubuf__ int16_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_v3(vector_uint16_t& dst, __ubuf__ uint16_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_v3(vector_half& dst, __ubuf__ half* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_v3(vector_bfloat16_t& dst, __ubuf__ bfloat16_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_v3(vector_int32_t& dst, __ubuf__ int32_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_v3(vector_uint32_t& dst, __ubuf__ uint32_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_v3(vector_float& dst, __ubuf__ float* src, int32_t offset)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| --------- | ----- | ---------------- |
| dst | Output | Destination operand (vector data register). |
| src | Input | Start address of the source operand (vector). |
| offset | Input | Address offset. |

For details about the vector data register, see [reg Data Type Definition](../defs/type/data_type_definition.md).

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

None

## Examples

```cpp
vector_int8_t dst;
__ubuf__ int8_t* src;
int32_t offset = 0;
asc_loadalign_unpack_v2(dst, src, offset);
```
