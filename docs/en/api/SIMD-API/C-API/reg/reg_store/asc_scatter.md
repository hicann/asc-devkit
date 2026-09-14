# asc_scatter

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-04T11:40:09.330Z pushedAt=2026-09-05T12:14:28.967Z -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Not supported
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

Header file path: `"c_api/reg_compute/reg_store.h"`.

This instruction scatters the elements in the source operand **src** into the destination operand **dst** according to the index value **index**. The scattering process is shown in the following figure:

![scatter description](../../figures/reg_scatter.png)

## Prototype

```cpp
__simd_callee__ inline void asc_scatter(__ubuf__ int8_t* dst, vector_int8_t src, vector_uint16_t index, vector_bool mask)
__simd_callee__ inline void asc_scatter(__ubuf__ uint8_t* dst, vector_uint8_t src, vector_uint16_t index, vector_bool mask)
__simd_callee__ inline void asc_scatter(__ubuf__ int16_t* dst, vector_int16_t src, vector_uint16_t index, vector_bool mask)
__simd_callee__ inline void asc_scatter(__ubuf__ uint16_t* dst, vector_uint16_t src, vector_uint16_t index, vector_bool mask)
__simd_callee__ inline void asc_scatter(__ubuf__ int32_t* dst, vector_int32_t src, vector_uint32_t index, vector_bool mask)
__simd_callee__ inline void asc_scatter(__ubuf__ uint32_t* dst, vector_uint32_t src, vector_uint32_t index, vector_bool mask)
__simd_callee__ inline void asc_scatter(__ubuf__ bfloat16_t* dst, vector_bfloat16_t src, vector_uint16_t index, vector_bool mask)
__simd_callee__ inline void asc_scatter(__ubuf__ half* dst, vector_half src, vector_uint16_t index, vector_bool mask)
__simd_callee__ inline void asc_scatter(__ubuf__ float* dst, vector_float src, vector_uint32_t index, vector_bool mask)
```

## Parameters

| Parameter   | Input/Output | Description                                                                      |
|:------| :--- |:------------------------------------------------------------------------|
| src   | Input | Source operand (vector data register).                                                           |
| dst   | Output | Starting address of the destination operand (vector).                                                          |
| index | Input | Data index (vector data register).                                                          |
| mask  | Input | Source operand mask (mask register), used to indicate which elements participate in the computation. Elements at positions with a value of 1 participate in the computation, and those with a value of 0 do not.|

For details about the mask register, see [reg Data Definition](../reg_data_definition.md).

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- The start address of **dst** must be 32-byte aligned.
- When **dst** is of the **int8_t** or **uint8_t** data type, only the even-position elements in the source operand are valid. That is, the data at even positions [0, 2, 4, ..., 252, 254] in **src** is scattered and stored into the destination operand.
- The values in **index** must be unique. If duplicate **index** values exist, the system retains only one of the corresponding data entries and ignores the rest. Since it is impossible to determine which one is retained, ensure that the **index** values are not duplicated.

## Example

```cpp
// dst is an externally input pointer of the half type pointing to the UB memory space.
vector_half src;
vector_uint16_t index;
vector_bool mask = asc_create_mask_b16(PAT_ALL);
asc_scatter(dst, src, index, mask);
```
