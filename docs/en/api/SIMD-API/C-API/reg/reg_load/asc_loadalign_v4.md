# asc_loadalign

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-08-29T09:45:26.112Z pushedAt=2026-09-03T12:34:46.949Z -->

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

Loads data from UB into the destination operand in a continuous and aligned manner. It supports multiple load modes. The API passes the offset through int32_t and enables hardware automatic Post Update, so users do not need to update the offset or the source operand address.

- NORM load mode: Normal mode, which loads VL data.
- BRC load mode: Loads one b8/b16/b32 data element and broadcasts it to all element positions.
- UP2X load mode: 2x data upsampling, which loads VL/2 data elements and repeats each input element twice. The data type is b8/b16.
- DOWN2X load mode: 2x data downsampling, which loads 2*VL data elements and keeps every other element. The data type is b8/b16.
- UNPACK load mode: Unpack mode, which loads VL/2 length data as unsigned integers u8/u16/u32 and unpacks them to VL-length u16/u32/u64 data, padding zeros in the middle positions.
- UNPACKV2 load mode: Unpack mode, which loads VL/4 length data as unsigned integers u8 and unpacks them to VL-length u32 data, padding zeros in the middle positions.
- BRCV2 load mode: Reads one DataBlock (32B) and broadcasts it to VL.
- BRCV3 load mode: Loads (VL/DataBlock) B of data and broadcasts each element (16bit/32bit) into one DataBlock (32B).
- DEINTLV load mode: A dual load mode that performs element-based interleaved transfer. It reads 2*VL length data from src, stores elements at even indices into dst0, and stores elements at odd indices into dst1. The data type is b8/b16/b32.

## Prototype

```cpp
// NORM load mode.
__simd_callee__ inline void asc_loadalign_postupdate(vector_int8_t& dst, __ubuf__ int8_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_postupdate(vector_uint8_t& dst, __ubuf__ uint8_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_postupdate(vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_postupdate(vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_postupdate(vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_postupdate(vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_postupdate(vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_postupdate(vector_hifloat8_t& dst, __ubuf__ hifloat8_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_postupdate(vector_int16_t& dst, __ubuf__ int16_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_postupdate(vector_uint16_t& dst, __ubuf__ uint16_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_postupdate(vector_half& dst, __ubuf__ half*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_postupdate(vector_bfloat16_t& dst, __ubuf__ bfloat16_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_postupdate(vector_int32_t& dst, __ubuf__ int32_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_postupdate(vector_uint32_t& dst, __ubuf__ uint32_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_postupdate(vector_float& dst, __ubuf__ float*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_postupdate(vector_int64_t& dst, __ubuf__ int64_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_postupdate(vector_uint64_t& dst, __ubuf__ uint64_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_postupdate(vector_int4x2_t& dst, __ubuf__ int4b_t*& src, int32_t offset)
// BRC load mode.
__simd_callee__ inline void asc_loadalign_brc_postupdate(vector_int8_t& dst, __ubuf__ int8_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_postupdate(vector_uint8_t& dst, __ubuf__ uint8_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_postupdate(vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_postupdate(vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_postupdate(vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_postupdate(vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_postupdate(vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_postupdate(vector_hifloat8_t& dst, __ubuf__ hifloat8_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_postupdate(vector_int16_t& dst, __ubuf__ int16_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_postupdate(vector_uint16_t& dst, __ubuf__ uint16_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_postupdate(vector_half& dst, __ubuf__ half*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_postupdate(vector_bfloat16_t& dst, __ubuf__ bfloat16_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_postupdate(vector_int32_t& dst, __ubuf__ int32_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_postupdate(vector_uint32_t& dst, __ubuf__ uint32_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_postupdate(vector_float& dst, __ubuf__ float*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_postupdate(vector_int4x2_t& dst, __ubuf__ int4b_t*& src, int32_t offset)
// UP2X load mode.
__simd_callee__ inline void asc_loadalign_upsample_postupdate(vector_int8_t& dst, __ubuf__ int8_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_upsample_postupdate(vector_uint8_t& dst, __ubuf__ uint8_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_upsample_postupdate(vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_upsample_postupdate(vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_upsample_postupdate(vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_upsample_postupdate(vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_upsample_postupdate(vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_upsample_postupdate(vector_hifloat8_t& dst, __ubuf__ hifloat8_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_upsample_postupdate(vector_int16_t& dst, __ubuf__ int16_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_upsample_postupdate(vector_uint16_t& dst, __ubuf__ uint16_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_upsample_postupdate(vector_half& dst, __ubuf__ half*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_upsample_postupdate(vector_bfloat16_t& dst, __ubuf__ bfloat16_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_upsample_postupdate(vector_int4x2_t& dst, __ubuf__ int4b_t*& src, int32_t offset)
// DOWN2X load mode.
__simd_callee__ inline void asc_loadalign_downsample_postupdate(vector_int8_t& dst, __ubuf__ int8_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_downsample_postupdate(vector_uint8_t& dst, __ubuf__ uint8_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_downsample_postupdate(vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_downsample_postupdate(vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_downsample_postupdate(vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_downsample_postupdate(vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_downsample_postupdate(vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_downsample_postupdate(vector_hifloat8_t& dst, __ubuf__ hifloat8_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_downsample_postupdate(vector_int16_t& dst, __ubuf__ int16_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_downsample_postupdate(vector_uint16_t& dst, __ubuf__ uint16_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_downsample_postupdate(vector_half& dst, __ubuf__ half*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_downsample_postupdate(vector_bfloat16_t& dst, __ubuf__ bfloat16_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_downsample_postupdate(vector_int4x2_t& dst, __ubuf__ int4b_t*& src, int32_t offset)
// UNPACK load mode.
__simd_callee__ inline void asc_loadalign_unpack_postupdate(vector_int8_t& dst, __ubuf__ int8_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_unpack_postupdate(vector_uint8_t& dst, __ubuf__ uint8_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_unpack_postupdate(vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_unpack_postupdate(vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_unpack_postupdate(vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_unpack_postupdate(vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_unpack_postupdate(vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_unpack_postupdate(vector_hifloat8_t& dst, __ubuf__ hifloat8_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_unpack_postupdate(vector_int16_t& dst, __ubuf__ int16_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_unpack_postupdate(vector_uint16_t& dst, __ubuf__ uint16_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_unpack_postupdate(vector_half& dst, __ubuf__ half*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_unpack_postupdate(vector_bfloat16_t& dst, __ubuf__ bfloat16_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_unpack_postupdate(vector_int32_t& dst, __ubuf__ int32_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_unpack_postupdate(vector_uint32_t& dst, __ubuf__ uint32_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_unpack_postupdate(vector_float& dst, __ubuf__ float*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_unpack_postupdate(vector_int4x2_t& dst, __ubuf__ int4b_t*& src, int32_t offset)
// UNPACKV2 load mode.
__simd_callee__ inline void asc_loadalign_unpack_postupdate_v2(vector_int8_t& dst, __ubuf__ int8_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_unpack_postupdate_v2(vector_uint8_t& dst, __ubuf__ uint8_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_unpack_postupdate_v2(vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_unpack_postupdate_v2(vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_unpack_postupdate_v2(vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_unpack_postupdate_v2(vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_unpack_postupdate_v2(vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_unpack_postupdate_v2(vector_hifloat8_t& dst, __ubuf__ hifloat8_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_unpack_postupdate_v2(vector_int4x2_t& dst, __ubuf__ int4b_t*& src, int32_t offset)
// BRCV2 load mode.
__simd_callee__ inline void asc_loadalign_brc_postupdate_v2(vector_int8_t& dst, __ubuf__ int8_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_postupdate_v2(vector_uint8_t& dst, __ubuf__ uint8_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_postupdate_v2(vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_postupdate_v2(vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_postupdate_v2(vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_postupdate_v2(vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_postupdate_v2(vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_postupdate_v2(vector_hifloat8_t& dst, __ubuf__ hifloat8_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_postupdate_v2(vector_int16_t& dst, __ubuf__ int16_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_postupdate_v2(vector_uint16_t& dst, __ubuf__ uint16_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_postupdate_v2(vector_half& dst, __ubuf__ half*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_postupdate_v2(vector_bfloat16_t& dst, __ubuf__ bfloat16_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_postupdate_v2(vector_int32_t& dst, __ubuf__ int32_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_postupdate_v2(vector_uint32_t& dst, __ubuf__ uint32_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_postupdate_v2(vector_float& dst, __ubuf__ float*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_postupdate_v2(vector_int4x2_t& dst, __ubuf__ int4b_t*& src, int32_t offset)
// BRCV3 load mode.
__simd_callee__ inline void asc_loadalign_brc_postupdate_v3(vector_int16_t& dst, __ubuf__ int16_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_postupdate_v3(vector_uint16_t& dst, __ubuf__ uint16_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_postupdate_v3(vector_half& dst, __ubuf__ half*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_postupdate_v3(vector_bfloat16_t& dst, __ubuf__ bfloat16_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_postupdate_v3(vector_int32_t& dst, __ubuf__ int32_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_postupdate_v3(vector_uint32_t& dst, __ubuf__ uint32_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_postupdate_v3(vector_float& dst, __ubuf__ float*& src, int32_t offset)
```

## Parameters

| Parameter       | Input/Output | Description               |
| --------- | ----- | ---------------- |
| dst       | Output    | Destination operand (vector data register).            |
| dst0       | Output    | Destination operand (vector data register).            |
| dst1       | Output    | Destination operand (vector data register).            |
| src | Input/Output    | Starting address of the source operand (vector).            |
| offset | Input    | Address offset.       |

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
__ubuf__ half* src;
int32_t offset;
asc_loadalign_postupdate(dst, src, offset);
```
