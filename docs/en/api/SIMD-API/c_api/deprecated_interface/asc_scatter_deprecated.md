---
okftype: reference.api
description: "Deprecated Reg scatter overload with the old parameter order, which scatters source vector register elements into the Unified Buffer by index. It includes nine type groups from int8_t to float. The replacement is the overload with the same name that places the destination address first and the source register last."
tag:
  - roles: [compute]
  - operations: [scatter]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/scatter/asc_scatter.md
    kind: replaced_by
version: ">=9.0.0"
version_changes:
  - since: "9.1.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The nine overloads that place the source vector register first and the destination UB address last are deprecated. Use the overload with the same name that places the destination UB address first and the source vector register last instead (only the parameter order is adjusted; the supported data types remain unchanged). All nine declarations in include/c_api/reg_compute/scatter/ub_scatter.h carry ASC_DEPRECATED(9.1.0), and the CANN_9_1_0 deprecation table has the same source."
---
# asc_scatter (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T14:43:09.302Z pushedAt=2026-11-18T03:12:16.648Z nl=2 -->

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

The header file path is: `"c_api/reg_compute/scatter/ub_scatter.h"`.

**This API is deprecated. Use [asc_scatter](../reg_compute/scatter/asc_scatter.md) to implement this function.**

This instruction scatters the elements in the source operand src into the destination operand dst based on the index values. The scatter process is shown in the following figure:

![scatter function description](../../../figures/reg_scatter_c_api.png)

## Prototype

```cpp
__simd_callee__ inline void asc_scatter(vector_int8_t& src, __ubuf__ int8_t* dst, vector_uint16_t index, vector_bool mask)
__simd_callee__ inline void asc_scatter(vector_uint8_t& src, __ubuf__ uint8_t* dst, vector_uint16_t index, vector_bool mask)
__simd_callee__ inline void asc_scatter(vector_int16_t& src, __ubuf__ int16_t* dst, vector_uint16_t index, vector_bool mask)
__simd_callee__ inline void asc_scatter(vector_uint16_t& src, __ubuf__ uint16_t* dst, vector_uint16_t index, vector_bool mask)
__simd_callee__ inline void asc_scatter(vector_int32_t& src, __ubuf__ int32_t* dst, vector_uint32_t index, vector_bool mask)
__simd_callee__ inline void asc_scatter(vector_uint32_t& src, __ubuf__ uint32_t* dst, vector_uint32_t index, vector_bool mask)
__simd_callee__ inline void asc_scatter(vector_bfloat16_t& src, __ubuf__ bfloat16_t* dst, vector_uint16_t index, vector_bool mask)
__simd_callee__ inline void asc_scatter(vector_half& src, __ubuf__ half* dst, vector_uint16_t index, vector_bool mask)
__simd_callee__ inline void asc_scatter(vector_float& src, __ubuf__ float* dst, vector_uint32_t index, vector_bool mask)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description                                                                      |
|:------| :--- |:------------------------------------------------------------------------|
| src   | Input | Source operand (vector data register).                                                           |
| dst   | Output | Start address of the destination operand (vector).                                                          |
| index | Input | Data index (vector data register).                                                          |
| mask  | Input | Source operand mask (mask register), which indicates which elements participate in the computation. When the corresponding bit is 1, the element participates in the computation; when it is 0, the element does not participate. |

For details about the mask register, see [reg data type definition](../defs/type/data_type_definition.md).

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- The start address of dst must be 32B aligned.
- When dst is of the int8_t or uint8_t data type, only the even-indexed elements in the source operand are valid. That is, the data at even positions [0, 2, 4, ..., 252, 254] in src will be scattered and stored into the destination operand.
- The values in index must be unique. If duplicate index values exist, the system retains only one of the corresponding data entries and ignores the rest. It is impossible to determine which one is retained, so you must ensure that the index values are not duplicated.

## Examples

```cpp
// dst is an externally input pointer to the half type that points to the Unified Buffer (UB) memory space.
vector_half src;
vector_uint16_t index;
vector_bool mask = asc_create_mask_b16(PAT_ALL);
asc_scatter(src, dst, index, mask);
```
