# asc_loadunalign_postupdate

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-03T12:36:45.571Z pushedAt=2026-09-03T12:54:23.718Z -->

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

Data transfer API, suitable for continuous loading starting from a non-32B-aligned address in UB.

- **asc_loadunalign_postupdate** (without the **iter_reg** parameter): Used together with the **asc_loadunalign_pre** (without the **iter_reg** parameter) API.
- **asc_loadunalign_postupdate** (with the **iter_reg** parameter): Used together with the **asc_loadunalign_pre** (with the **iter_reg** parameter) API.

## Prototype

```cpp
__simd_callee__ inline void asc_loadunalign_postupdate(vector_int8_t& dst, vector_load_unalign& src0, __ubuf__ int8_t*& src1, uint32_t count)
__simd_callee__ inline void asc_loadunalign_postupdate(vector_uint8_t& dst, vector_load_unalign& src0, __ubuf__ uint8_t*& src1, uint32_t count)
__simd_callee__ inline void asc_loadunalign_postupdate(vector_fp4x2_e2m1_t& dst, vector_load_unalign& src0, __ubuf__ fp4x2_e2m1_t*& src1, uint32_t count)
__simd_callee__ inline void asc_loadunalign_postupdate(vector_fp4x2_e1m2_t& dst, vector_load_unalign& src0, __ubuf__ fp4x2_e1m2_t*& src1, uint32_t count)
__simd_callee__ inline void asc_loadunalign_postupdate(vector_fp8_e8m0_t& dst, vector_load_unalign& src0, __ubuf__ fp8_e8m0_t*& src1, uint32_t count)
__simd_callee__ inline void asc_loadunalign_postupdate(vector_fp8_e5m2_t& dst, vector_load_unalign& src0, __ubuf__ fp8_e5m2_t*& src1, uint32_t count)
__simd_callee__ inline void asc_loadunalign_postupdate(vector_fp8_e4m3fn_t& dst, vector_load_unalign& src0, __ubuf__ fp8_e4m3fn_t*& src1, uint32_t count)
__simd_callee__ inline void asc_loadunalign_postupdate(vector_hifloat8_t& dst, vector_load_unalign& src0, __ubuf__ hifloat8_t*& src1, uint32_t count)
__simd_callee__ inline void asc_loadunalign_postupdate(vector_half& dst, vector_load_unalign& src0, __ubuf__ half*& src1, uint32_t count)
__simd_callee__ inline void asc_loadunalign_postupdate(vector_int16_t& dst, vector_load_unalign& src0, __ubuf__ int16_t*& src1, uint32_t count)
__simd_callee__ inline void asc_loadunalign_postupdate(vector_uint16_t& dst, vector_load_unalign& src0, __ubuf__ uint16_t*& src1, uint32_t count)
__simd_callee__ inline void asc_loadunalign_postupdate(vector_bfloat16_t& dst, vector_load_unalign& src0, __ubuf__ bfloat16_t*& src1, uint32_t count)
__simd_callee__ inline void asc_loadunalign_postupdate(vector_int32_t& dst, vector_load_unalign& src0, __ubuf__ int32_t*& src1, uint32_t count)
__simd_callee__ inline void asc_loadunalign_postupdate(vector_uint32_t& dst, vector_load_unalign& src0, __ubuf__ uint32_t*& src1, uint32_t count)
__simd_callee__ inline void asc_loadunalign_postupdate(vector_float& dst, vector_load_unalign& src0, __ubuf__ float*& src1, uint32_t count)
__simd_callee__ inline void asc_loadunalign_postupdate(vector_int64_t& dst, vector_load_unalign& src0, __ubuf__ int64_t*& src1, uint32_t count)
__simd_callee__ inline void asc_loadunalign_postupdate(vector_int4x2_t& dst, vector_load_unalign& src0, __ubuf__ int4b_t*& src1, uint32_t count)
__simd_callee__ inline void asc_loadunalign_postupdate(vector_int8_t& dst, vector_load_unalign& src0, __ubuf__ int8_t *src1, iter_reg& offset, uint32_t inc)
__simd_callee__ inline void asc_loadunalign_postupdate(vector_uint8_t& dst, vector_load_unalign& src0, __ubuf__ uint8_t *src1, iter_reg& offset, uint32_t inc)
__simd_callee__ inline void asc_loadunalign_postupdate(vector_fp4x2_e2m1_t& dst, vector_load_unalign& src0, __ubuf__ fp4x2_e2m1_t *src1, iter_reg& offset, uint32_t inc)
__simd_callee__ inline void asc_loadunalign_postupdate(vector_fp4x2_e1m2_t& dst, vector_load_unalign& src0, __ubuf__ fp4x2_e1m2_t *src1, iter_reg& offset, uint32_t inc)
__simd_callee__ inline void asc_loadunalign_postupdate(vector_fp8_e8m0_t& dst, vector_load_unalign& src0, __ubuf__ fp8_e8m0_t *src1, iter_reg& offset, uint32_t inc)
__simd_callee__ inline void asc_loadunalign_postupdate(vector_fp8_e5m2_t& dst, vector_load_unalign& src0, __ubuf__ fp8_e5m2_t *src1, iter_reg& offset, uint32_t inc)
__simd_callee__ inline void asc_loadunalign_postupdate(vector_fp8_e4m3fn_t& dst, vector_load_unalign& src0, __ubuf__ fp8_e4m3fn_t *src1, iter_reg& offset, uint32_t inc)
__simd_callee__ inline void asc_loadunalign_postupdate(vector_hifloat8_t& dst, vector_load_unalign& src0, __ubuf__ hifloat8_t *src1, iter_reg& offset, uint32_t inc)
__simd_callee__ inline void asc_loadunalign_postupdate(vector_int16_t& dst, vector_load_unalign& src0, __ubuf__ int16_t *src1, iter_reg& offset, uint32_t inc)
__simd_callee__ inline void asc_loadunalign_postupdate(vector_uint16_t& dst, vector_load_unalign& src0, __ubuf__ uint16_t *src1, iter_reg& offset, uint32_t inc)
__simd_callee__ inline void asc_loadunalign_postupdate(vector_half& dst, vector_load_unalign& src0, __ubuf__ half *src1, iter_reg& offset, uint32_t inc)
__simd_callee__ inline void asc_loadunalign_postupdate(vector_bfloat16_t& dst, vector_load_unalign& src0, __ubuf__ bfloat16_t *src1, iter_reg& offset, uint32_t inc)
__simd_callee__ inline void asc_loadunalign_postupdate(vector_int32_t& dst, vector_load_unalign& src0, __ubuf__ int32_t *src1, iter_reg& offset, uint32_t inc)
__simd_callee__ inline void asc_loadunalign_postupdate(vector_uint32_t& dst, vector_load_unalign& src0, __ubuf__ uint32_t *src1, iter_reg& offset, uint32_t inc)
__simd_callee__ inline void asc_loadunalign_postupdate(vector_float& dst, vector_load_unalign& src0, __ubuf__ float *src1, iter_reg& offset, uint32_t inc)
__simd_callee__ inline void asc_loadunalign_postupdate(vector_int64_t& dst, vector_load_unalign& src0, __ubuf__ int64_t *src1, iter_reg& offset, uint32_t inc)
__simd_callee__ inline void asc_loadunalign_postupdate(vector_int4x2_t& dst, vector_load_unalign& src0, __ubuf__ int4b_t *src1, iter_reg& offset, uint32_t inc)
```

## Parameters

| Parameter  | Input/Output | Description |
| :----- | :------- | :------- |
| dst | Output | Destination operand (vector). |
| src0 | Input | Source operand (vector data register). |
| src1 | Input/Output | Start address of the source operand (vector). When the API without the **iter_reg** parameter is used, this address is automatically updated after the transfer is complete. |
| offset | Input/Output | Offset of the storage address. When the API with the **iter_reg** parameter is used, this offset is automatically updated after the transfer is complete. |
| count | Input | Number of elements to transfer. |
| inc | Input | In multiple loops, when this API is called multiple times, the offset for the next transfer is updated to offset + inc before self-increment after each transfer is complete. It is recommended to set this to 0.|

For details about vector data registers and mask registers, see [reg Data Definition](../reg_data_definition.md).

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- **src1** in this API does not need to be 32B aligned.

## Example

```cpp
vector_int8_t dst;
vector_load_unalign src0;
__ubuf__ int8_t* src_ptr = (__ubuf__ int8_t*)asc_get_phy_buf_addr(0);
uint32_t count = 16;
asc_loadunalign_postupdate(dst, src0, src_ptr, count);
```
