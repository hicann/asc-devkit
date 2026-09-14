# asc_load

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-08-29T09:40:23.597Z pushedAt=2026-09-03T12:34:36.068Z -->

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

Loads data from a non-32-byte-aligned source address **src** in UB to a vector data register. The amount of data transferred is VL.

For continuous transfer, the user must manually update the **src** address.

## Prototype

```cpp
__simd_callee__ inline void asc_load(vector_int8_t& dst, __ubuf__ int8_t* src)
__simd_callee__ inline void asc_load(vector_uint8_t& dst, __ubuf__ uint8_t* src)
__simd_callee__ inline void asc_load(vector_int16_t& dst, __ubuf__ int16_t* src)
__simd_callee__ inline void asc_load(vector_uint16_t& dst, __ubuf__ uint16_t* src)
__simd_callee__ inline void asc_load(vector_int32_t& dst, __ubuf__ int32_t* src)
__simd_callee__ inline void asc_load(vector_uint32_t& dst, __ubuf__ uint32_t* src)
__simd_callee__ inline void asc_load(vector_half& dst, __ubuf__ half* src)
__simd_callee__ inline void asc_load(vector_float& dst, __ubuf__ float* src)
__simd_callee__ inline void asc_load(vector_int64_t& dst, __ubuf__ int64_t* src)
__simd_callee__ inline void asc_load(vector_bfloat16_t& dst, __ubuf__ bfloat16_t* src)
__simd_callee__ inline void asc_load(vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t* src)
__simd_callee__ inline void asc_load(vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t* src)
__simd_callee__ inline void asc_load(vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t* src)
__simd_callee__ inline void asc_load(vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t* src)
__simd_callee__ inline void asc_load(vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t* src)
__simd_callee__ inline void asc_load(vector_hifloat8_t& dst, __ubuf__ hifloat8_t* src)
__simd_callee__ inline void asc_load(vector_int4x2_t& dst, __ubuf__ int4b_t* src)
```

## Parameters

| Parameter | Input/Output | Description |
| :----- | :------- | :------- |
| dst | Output | Destination operand (vector data register). |
| src | Input | Start address of the source operand (vector). |

For details about the vector data register, see [reg Data Definition](../reg_data_definition.md).

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

None

## Example

```cpp
vector_half dst;
__ubuf__ half* src = (__ubuf__ half*)asc_get_phy_buf_addr(0);
asc_load(dst, src);
```
