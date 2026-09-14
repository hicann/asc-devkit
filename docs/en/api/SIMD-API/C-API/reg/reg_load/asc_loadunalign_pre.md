# asc_loadunalign_pre

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-08-29T09:48:49.884Z pushedAt=2026-09-03T12:34:51.491Z -->

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

Used to initialize before loading unaligned data, and must be used together with the [asc_loadunalign](./asc_loadunalign.md) API.

- **asc_loadunalign_pre** (without the **iter_reg** parameter): Used together with the **asc_loadunalign** (without the **iter_reg** parameter) API or the **asc_loadunalign_postupdate** (without the **iter_reg** parameter) API.

- **asc_loadunalign_pre** (with the **iter_reg** parameter): Used together with the **asc_loadunalign_postupdate** (with the **iter_reg** parameter) API.

## Prototype

```cpp
// Without the iter_reg parameter.
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ int8_t* src)
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ uint8_t* src)
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ fp4x2_e2m1_t* src)
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ fp4x2_e1m2_t* src)
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ fp8_e8m0_t* src)
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ fp8_e5m2_t* src)
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ fp8_e4m3fn_t* src)
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ hifloat8_t* src)
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ int16_t* src)
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ uint16_t* src)
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ half* src)
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ bfloat16_t* src)
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ int32_t* src)
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ uint32_t* src)
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ float* src)
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ int64_t* src)
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ int4b_t* src)
// With the iter_reg parameter.
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ int8_t* src, iter_reg offset)
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ uint8_t* src, iter_reg offset)
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ fp4x2_e2m1_t* src, iter_reg offset)
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ fp4x2_e1m2_t* src, iter_reg offset)
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ fp8_e8m0_t* src, iter_reg offset)
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ fp8_e5m2_t* src, iter_reg offset)
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ fp8_e4m3fn_t* src, iter_reg offset)
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ hifloat8_t* src, iter_reg offset)
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ int16_t* src, iter_reg offset)
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ uint16_t* src, iter_reg offset)
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ half* src, iter_reg offset)
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ bfloat16_t* src, iter_reg offset)
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ int32_t* src, iter_reg offset)
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ uint32_t* src, iter_reg offset)
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ float* src, iter_reg offset)
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ int64_t* src, iter_reg offset)
__simd_callee__ inline void asc_loadunalign_pre(vector_load_unalign& dst, __ubuf__ int4b_t* src, iter_reg offset)
```

## Parameters

| Parameter | Input/Output | Description |
| --------- | ----- | ---------------- |
| **dst** | Output | Unaligned register used to store unaligned data, with a length of 32B. |
| **src** | Input | Start address of the source operand (vector). |
| **offset** | Input | Address register that stores the address offset. |

For details about the unaligned register, see [reg Data Definition](../reg_data_definition.md).

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

None

## Example

```cpp
vector_load_unalign dst;
__ubuf__ int8_t* src;
asc_loadunalign_pre(dst, src);
```
