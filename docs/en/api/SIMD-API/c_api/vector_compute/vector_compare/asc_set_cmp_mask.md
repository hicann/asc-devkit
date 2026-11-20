---
okftype: reference.api
description: "Sets the Mask used for the asc_select operation. Reads the mask content from the Unified Buffer address. Must be used with asc_select. Supported only on the 2201 architecture."
tag:
  - roles: [configuration]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_permute_sel/asc_select.md
    kind: related
    condition: "The description states that this mask is used for the asc_select operation."
version: ">=8.5.0"
version_changes: []
---
# asc_set_cmp_mask

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.763848+00:00 pushedAt=2026-11-16T02:10:58.460Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Not supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Supported
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

The header file path is `"c_api/vector_compute/compute/vector_compare.h"`.

Sets the Mask used for selection in the [asc_select](../vector_permute_sel/asc_select.md) operation.

## Prototype

```cpp
__aicore__ inline void asc_set_cmp_mask(__ubuf__ void* sel_mask)
```

## Parameters

**Table 1** Parameters

|Parameter|Input/Output|Description|
| ------------ | ------------ | ------------ |
|sel_mask|Input|Start address of the Mask used for selection.|

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

Must be used together with the [asc_select](../vector_permute_sel/asc_select.md) operation.

## Examples

```cpp
constexpr uint32_t total_length = 256;
constexpr uint32_t sel_length = 8;
__ubuf__ uint8_t sel_mask[sel_length];
__ubuf__ float dst[total_length];
__ubuf__ float src0[total_length];
__ubuf__ float src1[total_length];
asc_set_cmp_mask(sel_mask);
asc_sync_pipe(PIPE_V);
asc_select(dst, src0, src1, total_length);
```
