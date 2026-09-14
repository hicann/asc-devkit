# asc_storealign

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-09-04T11:44:42.111Z pushedAt=2026-09-05T12:14:28.972Z -->

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

A register-based data transfer API for moving out vector data continuously in an aligned manner from vector data registers to the UB. The API uses the repeat stride mode.

## Prototype

```cpp
// Move out in repeat stride mode.
__simd_callee__ inline void asc_storealign(__ubuf__ int8_t* dst_align32b, vector_int8_t src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask)
__simd_callee__ inline void asc_storealign(__ubuf__ uint8_t* dst_align32b, vector_uint8_t src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask)
__simd_callee__ inline void asc_storealign(__ubuf__ fp8_e5m2_t* dst_align32b, vector_fp8_e5m2_t src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask)
__simd_callee__ inline void asc_storealign(__ubuf__ fp8_e4m3fn_t* dst_align32b, vector_fp8_e4m3fn_t src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask)
__simd_callee__ inline void asc_storealign(__ubuf__ hifloat8_t* dst_align32b, vector_hifloat8_t src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask)
__simd_callee__ inline void asc_storealign(__ubuf__ fp8_e8m0_t* dst_align32b, vector_fp8_e8m0_t src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask)
__simd_callee__ inline void asc_storealign(__ubuf__ int16_t* dst_align32b, vector_int16_t src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask)
__simd_callee__ inline void asc_storealign(__ubuf__ uint16_t* dst_align32b, vector_uint16_t src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask)
__simd_callee__ inline void asc_storealign(__ubuf__ half* dst_align32b, vector_half src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask)
__simd_callee__ inline void asc_storealign(__ubuf__ bfloat16_t* dst_align32b, vector_bfloat16_t src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask)
__simd_callee__ inline void asc_storealign(__ubuf__ int32_t* dst_align32b, vector_int32_t src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask)
__simd_callee__ inline void asc_storealign(__ubuf__ uint32_t* dst_align32b, vector_uint32_t src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask)
__simd_callee__ inline void asc_storealign(__ubuf__ float* dst_align32b, vector_float src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask)

// Move out in repeat stride mode while enabling the hardware automatic Post Update mode.
__simd_callee__ inline void asc_storealign_postupdate(__ubuf__ int8_t* dst_align32b, vector_int8_t src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask)
__simd_callee__ inline void asc_storealign_postupdate(__ubuf__ uint8_t* dst_align32b, vector_uint8_t src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask)
__simd_callee__ inline void asc_storealign_postupdate(__ubuf__ fp8_e5m2_t* dst_align32b, vector_fp8_e5m2_t src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask)
__simd_callee__ inline void asc_storealign_postupdate(__ubuf__ fp8_e4m3fn_t* dst_align32b, vector_fp8_e4m3fn_t src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask)
__simd_callee__ inline void asc_storealign_postupdate(__ubuf__ hifloat8_t* dst_align32b, vector_hifloat8_t src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask)
__simd_callee__ inline void asc_storealign_postupdate(__ubuf__ fp8_e8m0_t* dst_align32b, vector_fp8_e8m0_t src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask)
__simd_callee__ inline void asc_storealign_postupdate(__ubuf__ int16_t* dst_align32b, vector_int16_t src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask)
__simd_callee__ inline void asc_storealign_postupdate(__ubuf__ uint16_t* dst_align32b, vector_uint16_t src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask)
__simd_callee__ inline void asc_storealign_postupdate(__ubuf__ half* dst_align32b, vector_half src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask)
__simd_callee__ inline void asc_storealign_postupdate(__ubuf__ bfloat16_t* dst_align32b, vector_bfloat16_t src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask)
__simd_callee__ inline void asc_storealign_postupdate(__ubuf__ int32_t* dst_align32b, vector_int32_t src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask)
__simd_callee__ inline void asc_storealign_postupdate(__ubuf__ uint32_t* dst_align32b, vector_uint32_t src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask)
__simd_callee__ inline void asc_storealign_postupdate(__ubuf__ float* dst_align32b, vector_float src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask)
```

## Parameters

| Parameter  | Input/Output | Description |
| :----- | :------- | :------- |
| dst_align32b | Output | Starting address of the destination operand (vector). |
| src | Input | Source operand (vector data register). |
| block_stride | Input | Address stride between different DataBlocks within a single iteration of the source operand. |
| repeat_stride | Input | Address stride of the same DataBlock between adjacent iterations of the source operand. |
| mask | Input | Source operand mask (mask register), used to indicate which elements participate in the computation. Elements at positions with a value of 1 participate in the computation, and those with a value of 0 do not. |

For details about vector data registers and mask registers, see [reg Data Definition](../reg_data_definition.md).

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

None

## Example

```cpp
// asc_storealign
__ubuf__ int8_t* dst_align32b;
vector_int8_t src;
uint16_t block_stride;
uint16_t repeat_stride;
vector_bool mask = asc_create_mask_b8(PAT_ALL);
asc_storealign(dst_align32b, src, block_stride, repeat_stride, mask);

// asc_storealign_postupdate
__ubuf__ int8_t* dst_align32b;
vector_int8_t src;
uint16_t block_stride;
uint16_t repeat_stride;
vector_bool mask = asc_create_mask_b8(PAT_ALL);
asc_storealign_postupdate(dst_align32b, src, block_stride, repeat_stride, mask);
```
