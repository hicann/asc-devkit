---
okftype: reference.api
description: "Deprecated GM-to-UB non-aligned move padding value configuration interface. Sets the data values to be padded on the left and right sides of a continuously moved data block. The replacement is the general-purpose asc_set_copy_pad_val."
tag:
  - roles: [data_movement_configuration]
  - operations: [fill]
  - traits: [padding]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/vector_datamove/asc_copy_gm2ub_align/asc_copy_gm2ub_align.md
    kind: related
  - path: docs/zh/api/SIMD-API/c_api/vector_datamove/asc_set_copy_pad_val.md
    kind: replaced_by
version: ">=9.0.0"
version_changes:
  - since: "9.1.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "Interface deprecated. Use the general move pad value configuration interface asc_set_copy_pad_val instead. Declarations starting from include/c_api/vector_datamove/vector_datamove.h:2513 carry ASC_DEPRECATED(9.1.0, asc_set_copy_pad_val), same source as line 139 of the CANN_9_1_0 deprecation table."
---
# asc_set_gm2ub_pad (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T14:47:50.708Z pushedAt=2026-11-18T03:12:16.657Z nl=2 -->

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

The header file path is: `"c_api/vector_datamove/vector_datamove.h"`.

**This API is deprecated. To set the padding data value when source data is moved from GM to Unified Buffer (UB) in a non-aligned manner, use [asc_set_copy_pad_val](../vector_datamove/asc_set_copy_pad_val.md).**

When moving data from Global Memory (GM) to UB in a non-aligned manner, call this API to set the padding data values required on the left and right sides of the contiguous data block being moved.

## Prototype

```cpp 
__aicore__ inline void asc_set_gm2ub_pad(uint32_t pad_val)
``` 

## Parameters

**Table 1** Parameter description

|Parameter|Input/Output|Description| 
|------------|------------|-----------| 
| pad_val    | Input       | Padding data value required on the left and right sides.|

## Return Value

None 

## Pipeline Type

PIPE_S 

## Constraints

None 

## Examples

```cpp 
asc_set_gm2ub_loop_size(2, 2); 
asc_set_gm2ub_loop1_stride(96, 128); 
asc_set_gm2ub_loop2_stride(192, 288); 
// The source operand is non-aligned and requires padding data. 
asc_set_gm2ub_pad(0); 
asc_copy_gm2ub_align(dst, src, 2, 46 * sizeof(int8_t), 1, 1, true, asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, 48, 48); 
asc_set_gm2ub_loop_size(1, 1); 
```
