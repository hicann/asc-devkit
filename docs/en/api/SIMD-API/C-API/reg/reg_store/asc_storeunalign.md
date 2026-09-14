# asc_storeunalign

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-04T11:51:23.961Z pushedAt=2026-09-05T12:14:28.983Z -->

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

A register-based data transfer API, applicable to scenarios where data is continuously moved out from a vector data register starting at a non-32B-aligned address to the UB.

This API uses uint32_t as the storage offset and does not change the address of the source operand in the UB. For each loop iteration, the user must manually update the address of the destination operand in the UB.

## Prototype

```cpp
__simd_callee__ inline void asc_storeunalign(__ubuf__ int8_t* dst, vector_store_unalign& src0, vector_int8_t src1, uint32_t count)
__simd_callee__ inline void asc_storeunalign(__ubuf__ uint8_t* dst, vector_store_unalign& src0, vector_uint8_t src1, uint32_t count)
__simd_callee__ inline void asc_storeunalign(__ubuf__ int16_t* dst, vector_store_unalign& src0, vector_int16_t src1, uint32_t count)
__simd_callee__ inline void asc_storeunalign(__ubuf__ uint16_t* dst, vector_store_unalign& src0, vector_uint16_t src1, uint32_t count)
__simd_callee__ inline void asc_storeunalign(__ubuf__ int32_t* dst, vector_store_unalign& src0, vector_int32_t src1, uint32_t count)
__simd_callee__ inline void asc_storeunalign(__ubuf__ int64_t* dst, vector_store_unalign& src0, vector_int64_t src1, uint32_t count)
__simd_callee__ inline void asc_storeunalign(__ubuf__ uint32_t* dst, vector_store_unalign& src0, vector_uint32_t src1, uint32_t count)
__simd_callee__ inline void asc_storeunalign(__ubuf__ half* dst, vector_store_unalign& src0, vector_half src1, uint32_t count)
__simd_callee__ inline void asc_storeunalign(__ubuf__ float* dst, vector_store_unalign& src0, vector_float src1, uint32_t count)
__simd_callee__ inline void asc_storeunalign(__ubuf__ bfloat16_t* dst, vector_store_unalign& src0, vector_bfloat16_t src1, uint32_t count)
__simd_callee__ inline void asc_storeunalign(__ubuf__ fp8_e4m3fn_t* dst, vector_store_unalign& src0, vector_fp8_e4m3fn_t src1, uint32_t count)
__simd_callee__ inline void asc_storeunalign(__ubuf__ hifloat8_t* dst, vector_store_unalign& src0, vector_hifloat8_t src1, uint32_t count)
__simd_callee__ inline void asc_storeunalign(__ubuf__ fp8_e5m2_t* dst, vector_store_unalign& src0, vector_fp8_e5m2_t src1, uint32_t count)
__simd_callee__ inline void asc_storeunalign(__ubuf__ fp8_e8m0_t* dst, vector_store_unalign& src0, vector_fp8_e8m0_t src1, uint32_t count)
__simd_callee__ inline void asc_storeunalign(__ubuf__ fp4x2_e2m1_t* dst, vector_store_unalign& src0, vector_fp4x2_e2m1_t src1, uint32_t count)
__simd_callee__ inline void asc_storeunalign(__ubuf__ fp4x2_e1m2_t* dst, vector_store_unalign& src0, vector_fp4x2_e1m2_t src1, uint32_t count)
__simd_callee__ inline void asc_storeunalign(__ubuf__ int4b_t* dst, vector_store_unalign& src0, vector_int4x2_t src1, uint32_t count)
```

## Parameters

| Parameter  | Input/Output | Description |
| :----- | :------- | :------- |
| dst | Output | Start address of the destination operand (vector). |
| src0 | Input/Output | Unaligned register used to store unaligned data, with a length of 32B. |
| src1 | Input | Source operand (vector data register). |
| count | Input | Number of elements to be transferred. |

For details about vector data registers and unaligned registers, see [reg Data Definition](../reg_data_definition.md).

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- **dst** in this API does not need to be 32B aligned.
- After calling this API, call [asc_storeunalign_post](./asc_storeunalign_post.md) to process the tail block of the unaligned move-out.

## Example

```cpp
// The dst address is 8, which is not 32B aligned, and the occupied address range is UB[8:520].
__ubuf__ uint32_t* dst = (__ubuf__ uint32_t*)asc_get_phy_buf_addr(8);
vector_store_unalign ureg;
vector_uint32_t src;
uint32_t count = 64;
uint32_t repeat = 2;
for (uint32_t i = 0; i < repeat; i++) {
    // Perform other register operations.

    // First time: write src[0:62] to dst[0:62] (that is, UB[8:256]); at the same time, write src[62:64] to ureg[0:2].
    // Second time: ureg[0:2] + src[0:62] is written to dst[62:126] (that is, UB[256:512]); meanwhile, src[62:64] are written to ureg[0:2].
    asc_storeunalign(dst + i*count, ureg, src, count);
}
// The end address of dst is 520, that is, dst + repeat * count * sizeof(uint32_t), so offset must be configured as repeat * count.
int32_t offset = repeat * count;
// ureg[0:2] is written to dst[126:128] (that is, UB[512:520]).
asc_storeunalign_post(dst, ureg, offset);
```
