---
okftype: reference.api
description: "Deprecated interface page: asc_squeeze_v2 copies the valid elements selected by mask sequentially and continuously to the destination vector data register and sets the remaining positions to zero. It can be used with asc_storeunalign_postupdate to pass the valid data size through the AR register. It is recommended to use asc_squeeze_and_storeunalign instead."
tag:
  - roles: [compute]
  - operations: [compact, selection]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: constrained_by
    condition: "The definitions of vector data registers and mask registers follow their data type definitions"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_permute_sel/asc_squeeze_and_storeunalign.md
    kind: replaced_by
    condition: "It is recommended to use this interface family to select and continuously move out valid elements"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/store/asc_storeunalign_postupdate.md
    kind: related
    condition: "When used together, save the valid data size to the AR register and use it as the store offset"
  - path: docs/zh/api/SIMD-API/c_api/spr/asc_get_squeeze_status.md
    kind: related
    condition: "The valid data size in the AR register can be read through this interface"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "Release Notes 9.2.0 states that asc_squeeze_v2 already existed in CANN 9.1.0, and this version adds a deprecation marker, replaced by the asc_squeeze_and_storeunalign interface family; the interface has not been removed."
---
# asc_squeeze_v2 (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T14:59:25.392Z pushedAt=2026-11-18T03:12:16.675Z nl=2 -->

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

**This API is deprecated. Use [asc_squeeze_and_storeunalign](../reg_compute/reg_permute_sel/asc_squeeze_and_storeunalign.md) to filter and continuously move out valid elements.**

Copies the valid elements selected by mask from src to dst in sequence. The valid elements are arranged contiguously in dst. The remaining positions in dst are set to zero.

When asc_squeeze_v2 is used together with [asc_storeunalign_postupdate](../reg_compute/store/asc_storeunalign_postupdate.md), the valid data size is saved to the AR register and used as the store offset when moving out. The value in the AR register can be obtained through the [asc_get_squeeze_status](../spr/asc_get_squeeze_status.md) API.

## Prototype

```cpp
__simd_callee__ inline void asc_squeeze_v2(vector_int8_t& dst, vector_int8_t src, vector_bool mask)
__simd_callee__ inline void asc_squeeze_v2(vector_uint8_t& dst, vector_uint8_t src, vector_bool mask)
__simd_callee__ inline void asc_squeeze_v2(vector_hifloat8_t& dst, vector_hifloat8_t src, vector_bool mask)
__simd_callee__ inline void asc_squeeze_v2(vector_fp8_e5m2_t& dst, vector_fp8_e5m2_t src, vector_bool mask)
__simd_callee__ inline void asc_squeeze_v2(vector_fp8_e4m3fn_t& dst, vector_fp8_e4m3fn_t src, vector_bool mask)
__simd_callee__ inline void asc_squeeze_v2(vector_fp8_e8m0_t& dst, vector_fp8_e8m0_t src, vector_bool mask)
__simd_callee__ inline void asc_squeeze_v2(vector_int16_t& dst, vector_int16_t src, vector_bool mask)
__simd_callee__ inline void asc_squeeze_v2(vector_uint16_t& dst, vector_uint16_t src, vector_bool mask)
__simd_callee__ inline void asc_squeeze_v2(vector_half& dst, vector_half src, vector_bool mask)
__simd_callee__ inline void asc_squeeze_v2(vector_bfloat16_t& dst, vector_bfloat16_t src, vector_bool mask)
__simd_callee__ inline void asc_squeeze_v2(vector_int32_t& dst, vector_int32_t src, vector_bool mask)
__simd_callee__ inline void asc_squeeze_v2(vector_uint32_t& dst, vector_uint32_t src, vector_bool mask)
__simd_callee__ inline void asc_squeeze_v2(vector_float& dst, vector_float src, vector_bool mask)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| --------- | ----- | ----------------- |
| dst | Output | Destination operand (vector data register). |
| src | Input | Source operand (vector data register). |
| mask | Input | Source operand mask (mask register), which indicates which elements participate in the computation. A value of 1 at the corresponding position means the element participates in the computation, and 0 means it does not. Elements not selected by mask are set to zero in the output. |

For details about vector data registers and mask registers, see [reg data type definition](../defs/type/data_type_definition.md).

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

None

## Examples

```cpp
vector_int8_t src;
vector_bool mask = asc_create_mask_b8(PAT_ALL);
asc_loadalign(src, src_addr); // src_addr is the externally input Unified Buffer (UB) memory space address.
vector_int8_t dst;
asc_squeeze_v2(dst, src, mask);
```
