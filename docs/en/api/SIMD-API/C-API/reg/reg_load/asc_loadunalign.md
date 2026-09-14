# asc_loadunalign

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-03T12:36:42.138Z pushedAt=2026-09-03T12:54:25.365Z -->

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

A reg computation data transfer API, applicable to scenarios where data is continuously loaded into vector data registers from a non-32B-aligned start address in UB.

This API does not change the address of the source operand in UB. For each loop iteration, the user must manually update the address of the source operand in UB.

## Prototype

```cpp
__simd_callee__ inline void asc_loadunalign(vector_int8_t& dst, vector_load_unalign& src0, __ubuf__ int8_t *src1)
__simd_callee__ inline void asc_loadunalign(vector_uint8_t& dst, vector_load_unalign& src0, __ubuf__ uint8_t *src1)
__simd_callee__ inline void asc_loadunalign(vector_int16_t& dst, vector_load_unalign& src0, __ubuf__ int16_t *src1)
__simd_callee__ inline void asc_loadunalign(vector_uint16_t& dst, vector_load_unalign& src0, __ubuf__ uint16_t *src1)
__simd_callee__ inline void asc_loadunalign(vector_int32_t& dst, vector_load_unalign& src0, __ubuf__ int32_t *src1)
__simd_callee__ inline void asc_loadunalign(vector_uint32_t& dst, vector_load_unalign& src0, __ubuf__ uint32_t *src1)
__simd_callee__ inline void asc_loadunalign(vector_half& dst, vector_load_unalign& src0, __ubuf__ half *src1)
__simd_callee__ inline void asc_loadunalign(vector_float& dst, vector_load_unalign& src0, __ubuf__ float *src1)
__simd_callee__ inline void asc_loadunalign(vector_int64_t& dst, vector_load_unalign& src0, __ubuf__ int64_t *src1)
__simd_callee__ inline void asc_loadunalign(vector_bfloat16_t& dst, vector_load_unalign& src0, __ubuf__ bfloat16_t *src1)
__simd_callee__ inline void asc_loadunalign(vector_fp8_e4m3fn_t& dst, vector_load_unalign& src0, __ubuf__ fp8_e4m3fn_t *src1)
__simd_callee__ inline void asc_loadunalign(vector_fp8_e5m2_t& dst, vector_load_unalign& src0, __ubuf__ fp8_e5m2_t *src1)
__simd_callee__ inline void asc_loadunalign(vector_fp8_e8m0_t& dst, vector_load_unalign& src0, __ubuf__ fp8_e8m0_t *src1)
__simd_callee__ inline void asc_loadunalign(vector_fp4x2_e2m1_t& dst, vector_load_unalign& src0, __ubuf__ fp4x2_e2m1_t *src1)
__simd_callee__ inline void asc_loadunalign(vector_fp4x2_e1m2_t& dst, vector_load_unalign& src0, __ubuf__ fp4x2_e1m2_t *src1)
__simd_callee__ inline void asc_loadunalign(vector_hifloat8_t& dst, vector_load_unalign& src0, __ubuf__ hifloat8_t *src1)
__simd_callee__ inline void asc_loadunalign(vector_int4x2_t& dst, vector_load_unalign& src0, __ubuf__ int4b_t *src1)
```

## Parameters

| Parameter  | Input/Output | Description |
| :----- | :------- | :------- |
| dst | Output | Destination operand (vector data register). |
| src0 | Input/Output | Unaligned register used to store unaligned data, with a length of 32B. |
| src1 | Input | Start address of the source operand (vector). |

For details about vector data registers and non-aligned registers, see [reg Data Definition](../reg_data_definition.md).

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- **src1** in this API does not need to be 32B aligned.
- Before calling this API, call [asc_loadunalign_pre](./asc_loadunalign_pre.md) to perform initialization before non-aligned loading.

## Example

```cpp
vector_load_unalign ureg;
__ubuf__ half* src = (__ubuf__ half*)asc_get_phy_buf_addr(0);
uint32_t offset = 5;
asc_loadunalign_pre(ureg, src + offset);
vector_half dst;
asc_loadunalign(dst, ureg, src + offset);
```
