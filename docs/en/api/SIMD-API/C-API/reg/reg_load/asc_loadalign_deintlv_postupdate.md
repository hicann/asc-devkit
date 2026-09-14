# asc_loadalign_deintlv_postupdate

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-08-29T09:40:44.053Z pushedAt=2026-09-03T12:34:37.712Z -->

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

`asc_loadalign_deintlv_postupdate` reads `2 x VL` consecutive data from the UB starting at the 32B-aligned address `src`. The data is interpreted by element type width, deinterleaved during the load process, and then written to two destination vector registers `dst0` and `dst1`.

Unlike [`asc_loadalign_deintlv`](./asc_loadalign_deintlv.md), this API performs a post-update operation on the source address parameter `src` after the data load is completed:

```cpp
src = src + offset
```

Here, the unit of `offset` is the number of elements, and the actual address offset in bytes is `offset * sizeof(data_type)`.

Taking the `float` data type and the Ascend 950PR/Ascend 950DT products as an example, if `VL = 256B`, `src = 32`, and `offset = 128`, after calling `asc_loadalign_deintlv_postupdate`, the data layout from the UB to the registers is shown in the following figure, and `src` is updated to `544`, which can be directly used for subsequent computation.

![](../../figures/asc_loadalign_deintlv_postupdate.png)

> Note:
>
> 1. To use this API, include the header file `reg_load.h`.
> 2. This API modifies the input UB address parameter `src`. To retain the original address, use [`asc_loadalign_deintlv`](./asc_loadalign_deintlv.md).

## Prototype

For supported data types, see [Data Type](#data-type).

```cpp
__simd_callee__ inline void asc_loadalign_deintlv_postupdate(vector_int4x2_t& dst0, vector_int4x2_t& dst1, __ubuf__ int4b_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv_postupdate(vector_fp4x2_e2m1_t& dst0, vector_fp4x2_e2m1_t& dst1, __ubuf__ fp4x2_e2m1_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv_postupdate(vector_fp4x2_e1m2_t& dst0, vector_fp4x2_e1m2_t& dst1, __ubuf__ fp4x2_e1m2_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv_postupdate(vector_int8_t& dst0, vector_int8_t& dst1, __ubuf__ int8_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv_postupdate(vector_uint8_t& dst0, vector_uint8_t& dst1, __ubuf__ uint8_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv_postupdate(vector_fp8_e8m0_t& dst0, vector_fp8_e8m0_t& dst1, __ubuf__ fp8_e8m0_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv_postupdate(vector_fp8_e5m2_t& dst0, vector_fp8_e5m2_t& dst1, __ubuf__ fp8_e5m2_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv_postupdate(vector_fp8_e4m3fn_t& dst0, vector_fp8_e4m3fn_t& dst1, __ubuf__ fp8_e4m3fn_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv_postupdate(vector_hifloat8_t& dst0, vector_hifloat8_t& dst1, __ubuf__ hifloat8_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv_postupdate(vector_int16_t& dst0, vector_int16_t& dst1, __ubuf__ int16_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv_postupdate(vector_uint16_t& dst0, vector_uint16_t& dst1, __ubuf__ uint16_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv_postupdate(vector_half& dst0, vector_half& dst1, __ubuf__ half*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv_postupdate(vector_bfloat16_t& dst0, vector_bfloat16_t& dst1, __ubuf__ bfloat16_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv_postupdate(vector_int32_t& dst0, vector_int32_t& dst1, __ubuf__ int32_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv_postupdate(vector_uint32_t& dst0, vector_uint32_t& dst1, __ubuf__ uint32_t*& src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv_postupdate(vector_float& dst0, vector_float& dst1, __ubuf__ float*& src, int32_t offset)
```

## Parameters

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| dst0 | Output | First destination vector register. |
| dst1 | Output | Second destination vector register. |
| src | Input/Output | Start address of the source data in the UB, which must be 32B-aligned. After the API execution is completed, `src` is updated to `src + offset`. |
| offset | Input | Address offset, in number of elements. |

For details about vector register types, see [reg Data Definition.md](../reg_data_definition.md).

## Data Type<a name="data-type"></a>

The data types supported by `asc_loadalign_deintlv_postupdate` are listed in the following table.

| Source Data Type `src` | Destination Register Type `dst0/dst1` |
| :--- | :--- |
| `int4b_t` | `vector_int4x2_t` |
| `fp4x2_e1m2_t` | `vector_fp4x2_e1m2_t` |
| `fp4x2_e2m1_t` | `vector_fp4x2_e2m1_t` |
| `int8_t` | `vector_int8_t` |
| `uint8_t` | `vector_uint8_t` |
| `fp8_e4m3fn_t` | `vector_fp8_e4m3fn_t` |
| `fp8_e5m2_t` | `vector_fp8_e5m2_t` |
| `fp8_e8m0_t` | `vector_fp8_e8m0_t` |
| `hifloat8_t` | `vector_hifloat8_t` |
| `int16_t` | `vector_int16_t` |
| `uint16_t` | `vector_uint16_t` |
| `half` | `vector_half` |
| `bfloat16_t` | `vector_bfloat16_t` |
| `int32_t` | `vector_int32_t` |
| `uint32_t` | `vector_uint32_t` |
| `float` | `vector_float` |

## Return Value

None

## Pipeline Type

`PIPE_V`

## Constraints

- `src` must be a 32B-aligned address, and the accessed range must not exceed the upper bound of the UB valid address minus `2 x VL`; otherwise, out-of-bounds UB access may occur.
- When this API is used in a loop, the value of `offset` must ensure that the updated `src` still meets the 32B alignment requirement.
- `offset` is in units of elements, and the actual address stride must be converted based on the width of the source data type.

## Example

Continuously reads `float` data from the UB starting at the 32B-aligned address `src`, and automatically updates the address after each call.

```cpp
for (uint16_t i = 0; i < repeat_times; i++) {
    asc_loadalign_deintlv_postupdate(reg0, reg1, src, one_repeat_size);
    ...
}
```
