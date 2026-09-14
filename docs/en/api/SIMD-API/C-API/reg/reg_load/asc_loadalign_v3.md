# asc_loadalign

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-08-29T09:45:10.131Z pushedAt=2026-09-03T12:34:45.498Z -->

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

Loads data from the UB into the destination operand in a continuous and aligned manner. It supports multiple load modes. The API passes the offset as **int32_t**, and you can choose to update the offset or update the address of the source operand.

- **NORM** mode: Normal mode, which loads VL data.
- **BRC** mode: Loads one b8/b16/b32 data element and broadcasts it to all element positions.
- **UP2X** mode: 2x upsampling, which loads VL/2 data elements and repeats each input element twice. The data type is b8/b16.
- **DOWN2X** mode: 2x downsampling, which loads 2x VL data and retains every other data element. The data type is b8/b16.
- **UNPACK** mode: Unpack mode, which loads VL/2 length data as unsigned integers u8/u16/u32 and unpacks them to u16/u32/u64 of VL length, padding zeros in the middle positions.
- **UNPACKV2** mode: Unpack mode, which loads VL/4 length data as unsigned integer u8 and unpacks them to u32 of VL length, padding zeros in the middle positions.
- **BRCV2** mode: Reads one DataBlock (32B) and broadcasts it to VL.
- **BRCV3** mode: Loads (VL/DataBlock) B of data and broadcasts each element (16bit/32bit) into one DataBlock (32B).
- **DEINTLV** mode: A dual load mode that performs element-based interleaved transfer. It reads 2*VL length data from **src**, stores elements at even indices into **dst0**, and stores elements at odd indices into **dst1**. The data type is b8/b16/b32.

## Prototype

```cpp
// NORM load mode
__simd_callee__ inline void asc_loadalign(vector_int8_t& dst, __ubuf__ int8_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign(vector_uint8_t& dst, __ubuf__ uint8_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign(vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign(vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign(vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign(vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign(vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign(vector_hifloat8_t& dst, __ubuf__ hifloat8_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign(vector_int16_t& dst, __ubuf__ int16_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign(vector_uint16_t& dst, __ubuf__ uint16_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign(vector_half& dst, __ubuf__ half* src, int32_t offset)
__simd_callee__ inline void asc_loadalign(vector_bfloat16_t& dst, __ubuf__ bfloat16_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign(vector_int32_t& dst, __ubuf__ int32_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign(vector_uint32_t& dst, __ubuf__ uint32_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign(vector_float& dst, __ubuf__ float* src, int32_t offset)
__simd_callee__ inline void asc_loadalign(vector_int64_t& dst, __ubuf__ int64_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign(vector_uint64_t& dst, __ubuf__ uint64_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign(vector_int4x2_t& dst, __ubuf__ int4b_t* src, int32_t offset)
// BRC load mode
__simd_callee__ inline void asc_loadalign_brc(vector_int8_t& dst, __ubuf__ int8_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc(vector_uint8_t& dst, __ubuf__ uint8_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc(vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc(vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc(vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc(vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc(vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc(vector_hifloat8_t& dst, __ubuf__ hifloat8_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc(vector_int16_t& dst, __ubuf__ int16_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc(vector_uint16_t& dst, __ubuf__ uint16_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc(vector_half& dst, __ubuf__ half* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc(vector_bfloat16_t& dst, __ubuf__ bfloat16_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc(vector_int32_t& dst, __ubuf__ int32_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc(vector_uint32_t& dst, __ubuf__ uint32_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc(vector_float& dst, __ubuf__ float* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc(vector_int4x2_t& dst, __ubuf__ int4b_t* src, int32_t offset)
// UP2X load mode
__simd_callee__ inline void asc_loadalign_upsample(vector_int8_t& dst, __ubuf__ int8_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_upsample(vector_uint8_t& dst, __ubuf__ uint8_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_upsample(vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_upsample(vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_upsample(vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_upsample(vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_upsample(vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_upsample(vector_hifloat8_t& dst, __ubuf__ hifloat8_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_upsample(vector_int16_t& dst, __ubuf__ int16_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_upsample(vector_uint16_t& dst, __ubuf__ uint16_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_upsample(vector_half& dst, __ubuf__ half* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_upsample(vector_bfloat16_t& dst, __ubuf__ bfloat16_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_upsample(vector_int4x2_t& dst, __ubuf__ int4b_t* src, int32_t offset)
// DOWN2X load mode
__simd_callee__ inline void asc_loadalign_downsample(vector_int8_t& dst, __ubuf__ int8_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_downsample(vector_uint8_t& dst, __ubuf__ uint8_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_downsample(vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_downsample(vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_downsample(vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_downsample(vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_downsample(vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_downsample(vector_hifloat8_t& dst, __ubuf__ hifloat8_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_downsample(vector_int16_t& dst, __ubuf__ int16_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_downsample(vector_uint16_t& dst, __ubuf__ uint16_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_downsample(vector_half& dst, __ubuf__ half* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_downsample(vector_bfloat16_t& dst, __ubuf__ bfloat16_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_downsample(vector_int4x2_t& dst, __ubuf__ int4b_t* src, int32_t offset)
// UNPACK load mode
__simd_callee__ inline void asc_loadalign_unpack(vector_int8_t& dst, __ubuf__ int8_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_unpack(vector_uint8_t& dst, __ubuf__ uint8_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_unpack(vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_unpack(vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_unpack(vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_unpack(vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_unpack(vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_unpack(vector_hifloat8_t& dst, __ubuf__ hifloat8_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_unpack(vector_int16_t& dst, __ubuf__ int16_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_unpack(vector_uint16_t& dst, __ubuf__ uint16_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_unpack(vector_half& dst, __ubuf__ half* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_unpack(vector_bfloat16_t& dst, __ubuf__ bfloat16_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_unpack(vector_int32_t& dst, __ubuf__ int32_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_unpack(vector_uint32_t& dst, __ubuf__ uint32_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_unpack(vector_float& dst, __ubuf__ float* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_unpack(vector_int4x2_t& dst, __ubuf__ int4b_t* src, int32_t offset)
// UNPACKV2 load mode
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_int8_t& dst, __ubuf__ int8_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_uint8_t& dst, __ubuf__ uint8_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_hifloat8_t& dst, __ubuf__ hifloat8_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_int4x2_t& dst, __ubuf__ int4b_t* src, int32_t offset)
// BRCV2 load mode
__simd_callee__ inline void asc_loadalign_brc_v2(vector_int8_t& dst, __ubuf__ int8_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_uint8_t& dst, __ubuf__ uint8_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_hifloat8_t& dst, __ubuf__ hifloat8_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_int16_t& dst, __ubuf__ int16_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_uint16_t& dst, __ubuf__ uint16_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_half& dst, __ubuf__ half* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_bfloat16_t& dst, __ubuf__ bfloat16_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_int32_t& dst, __ubuf__ int32_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_uint32_t& dst, __ubuf__ uint32_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_float& dst, __ubuf__ float* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_v2(vector_int4x2_t& dst, __ubuf__ int4b_t* src, int32_t offset)
// BRCV3 load mode
__simd_callee__ inline void asc_loadalign_brc_v3(vector_int16_t& dst, __ubuf__ int16_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_v3(vector_uint16_t& dst, __ubuf__ uint16_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_v3(vector_half& dst, __ubuf__ half* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_v3(vector_bfloat16_t& dst, __ubuf__ bfloat16_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_v3(vector_int32_t& dst, __ubuf__ int32_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_v3(vector_uint32_t& dst, __ubuf__ uint32_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_brc_v3(vector_float& dst, __ubuf__ float* src, int32_t offset)
```

## Parameters

| Parameter       | Input/Output | Description               |
| --------- | ----- | ---------------- |
| dst       | Output    | Destination operand (vector data register).            |
| dst0       | Output    | Destination operand (vector data register).            |
| dst1       | Output    | Destination operand (vector data register).            |
| src | Input    | Start address of the source operand (vector).            |
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
asc_loadalign(dst, src, offset);
```
