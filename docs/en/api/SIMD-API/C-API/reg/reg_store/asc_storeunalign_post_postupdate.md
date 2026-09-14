# asc_storeunalign_post_postupdate

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-04T11:48:09.493Z pushedAt=2026-09-05T12:14:28.976Z -->

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

A reg data transfer API, applicable to the tail block scenario where data is continuously moved from a non-32B-aligned start address of a vector data register to the UB.

Each call to this API updates the address of the destination operand in the UB.

Call the following API before calling this API.
- [asc_storeunalign_postupdate](./asc_storeunalign_postupdate.md), an API that uses uint32_t as the storage offset.

The movement principle is as follows:
Let the start address of the destination operand be dst_start, the end address be dst_end, and the number of tail block elements be unalign_count = (dst_end - dst_end / 32 * 32) / sizeof(T).
Then the moved data is divided into two parts:
- The 32B-aligned main block part [dst_start, dst_end - unalign_count * sizeof(T)].
- The non-32B-aligned tail block part [dst_end - unalign_count * sizeof(T), dst_end].
When the store_unalign or store_unalign_postupdate API is executed, the main block is moved out to the UB, and the tail block is temporarily stored in the unaligned register [0, unalign_count].
When this API is executed, the tail block is moved out from the unaligned register [0, unalign_count] to the UB.


## Prototype

```cpp
__simd_callee__ inline void asc_storeunalign_post_postupdate(__ubuf__  int8_t*& dst, vector_store_unalign src, int32_t offset)
__simd_callee__ inline void asc_storeunalign_post_postupdate(__ubuf__  uint8_t*& dst, vector_store_unalign src, int32_t offset)
__simd_callee__ inline void asc_storeunalign_post_postupdate(__ubuf__  fp4x2_e2m1_t*& dst, vector_store_unalign src, int32_t offset)
__simd_callee__ inline void asc_storeunalign_post_postupdate(__ubuf__  fp4x2_e1m2_t*& dst, vector_store_unalign src, int32_t offset)
__simd_callee__ inline void asc_storeunalign_post_postupdate(__ubuf__  int4b_t*& dst, vector_store_unalign src, int32_t offset)
__simd_callee__ inline void asc_storeunalign_post_postupdate(__ubuf__  fp8_e8m0_t*& dst, vector_store_unalign src, int32_t offset)
__simd_callee__ inline void asc_storeunalign_post_postupdate(__ubuf__  fp8_e5m2_t*& dst, vector_store_unalign src, int32_t offset)
__simd_callee__ inline void asc_storeunalign_post_postupdate(__ubuf__  fp8_e4m3fn_t*& dst, vector_store_unalign src, int32_t offset)
__simd_callee__ inline void asc_storeunalign_post_postupdate(__ubuf__  hifloat8_t*& dst, vector_store_unalign src, int32_t offset)
__simd_callee__ inline void asc_storeunalign_post_postupdate(__ubuf__  int16_t*& dst, vector_store_unalign src, int32_t offset)
__simd_callee__ inline void asc_storeunalign_post_postupdate(__ubuf__  uint16_t*& dst, vector_store_unalign src, int32_t offset)
__simd_callee__ inline void asc_storeunalign_post_postupdate(__ubuf__  half*& dst, vector_store_unalign src, int32_t offset)
__simd_callee__ inline void asc_storeunalign_post_postupdate(__ubuf__  bfloat16_t*& dst, vector_store_unalign src, int32_t offset)
__simd_callee__ inline void asc_storeunalign_post_postupdate(__ubuf__  int32_t*& dst, vector_store_unalign src, int32_t offset)
__simd_callee__ inline void asc_storeunalign_post_postupdate(__ubuf__  uint32_t*& dst, vector_store_unalign src, int32_t offset)
__simd_callee__ inline void asc_storeunalign_post_postupdate(__ubuf__  float*& dst, vector_store_unalign src, int32_t offset)
__simd_callee__ inline void asc_storeunalign_post_postupdate(__ubuf__  int64_t*& dst, vector_store_unalign src, int32_t offset)
```

## Parameters

| Parameter  | Input/Output | Description |
| :----- | :------- | :------- |
| dst | Output | Start address of the destination operand (vector). |
| src | Input | Unaligned register used to store unaligned data, with a length of 32B. |
| offset | Input | Offset of the end address of the destination operand, in elements. |

For details about the unaligned register, see [reg Data Definition](../reg_data_definition.md).

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- The dst in this API does not need to be 32B-aligned, but dst of data type T needs to be sizeof(T) Byte-aligned.
- Before calling this API, call the [asc_storeunalign_postupdate](./asc_storeunalign_postupdate.md) API that uses uint32_t as the storage offset, and the unaligned register of this API must be consistent with that of the preceding API.

## Example

```cpp
// The dst address is 8, which is not 32B-aligned, and the occupied address is UB[8:520].
__ubuf__ uint32_t* dst = (__ubuf__ uint32_t*)asc_get_phy_buf_addr(8);
vector_store_unalign ureg;
vector_uint32_t src;
uint32_t count = 64;
uint32_t repeat = 2;
for (uint32_t i = 0; i < repeat; i++) {
    // Other reg operations.

    // First time: src[0:62] is written to dst[0:62] (that is, UB[8:256]); at the same time, src[62:64] is written to ureg[0:2]; at the same time, dst increments to dst[64] (that is, UB[264]).
    // Second time: ureg[0:2] + src[0:62] is written to dst[-2:62] (that is, UB[256:512]); at the same time, src[62:64] is written to ureg[0:2]; at the same time, dst increments to dst[128] (that is, UB[520]).
    asc_storeunalign_postupdate(dst + i*count, ureg, src, count);
}
// The dst end address is 520, but dst has already incremented to 520, so offset must be configured as 0.
int32_t offset = repeat * count;
// ureg[0:2] is written to dst[126:128] (that is, UB[512:520]).
asc_storeunalign_post_postupdate(dst, ureg, offset);
```
