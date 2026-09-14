# asc_loadalign_deintlv

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-08-29T09:42:11.081Z pushedAt=2026-09-03T12:34:39.363Z -->

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

`asc_loadalign_deintlv` reads `2 x VL` consecutive data starting from a 32B-aligned address in the UB. The data is interpreted by element type width. After deinterleaving during the load process, the data is written to two destination vector data registers.

Taking the `float` data type on Ascend 950PR/Ascend 950DT as an example, if `VL = 256B`, a single operation covers 64 `float` elements. After calling `asc_loadalign_deintlv`, the data layout from UB to Reg is shown in the following figure.

![](../../figures/asc_loadalign_deintlv.png)

> Note:
>
> To use this API, include the header file `reg_load.h`.

## Prototype

`asc_loadalign_deintlv` is divided into the following three types of APIs based on the source address addressing mode: [base addressing API](#base-addressing-api), [scalar offset addressing API](#scalar-offset-addressing-api), and [address register offset addressing API](#address-register-offset-addressing-api). For supported data types, see [Data Type](#data-type).

### Base Addressing API<a name="base-addressing-api"></a>

```cpp
__simd_callee__ inline void asc_loadalign_deintlv(vector_int4x2_t& dst0, vector_int4x2_t& dst1, __ubuf__ int4b_t* src)
__simd_callee__ inline void asc_loadalign_deintlv(vector_int8_t& dst0, vector_int8_t& dst1, __ubuf__ int8_t* src)
__simd_callee__ inline void asc_loadalign_deintlv(vector_uint8_t& dst0, vector_uint8_t& dst1, __ubuf__ uint8_t* src)
__simd_callee__ inline void asc_loadalign_deintlv(vector_fp4x2_e2m1_t& dst0, vector_fp4x2_e2m1_t& dst1, __ubuf__ fp4x2_e2m1_t* src)
__simd_callee__ inline void asc_loadalign_deintlv(vector_fp4x2_e1m2_t& dst0, vector_fp4x2_e1m2_t& dst1, __ubuf__ fp4x2_e1m2_t* src)
__simd_callee__ inline void asc_loadalign_deintlv(vector_hifloat8_t& dst0, vector_hifloat8_t& dst1, __ubuf__ hifloat8_t* src)
__simd_callee__ inline void asc_loadalign_deintlv(vector_fp8_e8m0_t& dst0, vector_fp8_e8m0_t& dst1, __ubuf__ fp8_e8m0_t* src)
__simd_callee__ inline void asc_loadalign_deintlv(vector_fp8_e5m2_t& dst0, vector_fp8_e5m2_t& dst1, __ubuf__ fp8_e5m2_t* src)
__simd_callee__ inline void asc_loadalign_deintlv(vector_fp8_e4m3fn_t& dst0, vector_fp8_e4m3fn_t& dst1, __ubuf__ fp8_e4m3fn_t* src)
__simd_callee__ inline void asc_loadalign_deintlv(vector_int16_t& dst0, vector_int16_t& dst1, __ubuf__ int16_t* src)
__simd_callee__ inline void asc_loadalign_deintlv(vector_uint16_t& dst0, vector_uint16_t& dst1, __ubuf__ uint16_t* src)
__simd_callee__ inline void asc_loadalign_deintlv(vector_half& dst0, vector_half& dst1, __ubuf__ half* src)
__simd_callee__ inline void asc_loadalign_deintlv(vector_bfloat16_t& dst0, vector_bfloat16_t& dst1, __ubuf__ bfloat16_t* src)
__simd_callee__ inline void asc_loadalign_deintlv(vector_int32_t& dst0, vector_int32_t& dst1, __ubuf__ int32_t* src)
__simd_callee__ inline void asc_loadalign_deintlv(vector_uint32_t& dst0, vector_uint32_t& dst1, __ubuf__ uint32_t* src)
__simd_callee__ inline void asc_loadalign_deintlv(vector_float& dst0, vector_float& dst1, __ubuf__ float* src)
```

### Scalar Offset Addressing API<a name="base-scalar-offset-api"></a>

The source address is calculated as `src + offset`, and the result must meet the 32B alignment requirement. The unit of `offset` is the number of elements, and the actual address offset in bytes is `offset * sizeof(data_type)`.

```cpp
__simd_callee__ inline void asc_loadalign_deintlv(vector_int8_t& dst0, vector_int8_t& dst1, __ubuf__ int8_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_uint8_t& dst0, vector_uint8_t& dst1, __ubuf__ uint8_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_fp4x2_e2m1_t& dst0, vector_fp4x2_e2m1_t& dst1, __ubuf__ fp4x2_e2m1_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_fp4x2_e1m2_t& dst0, vector_fp4x2_e1m2_t& dst1, __ubuf__ fp4x2_e1m2_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_fp8_e8m0_t& dst0, vector_fp8_e8m0_t& dst1, __ubuf__ fp8_e8m0_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_fp8_e5m2_t& dst0, vector_fp8_e5m2_t& dst1, __ubuf__ fp8_e5m2_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_fp8_e4m3fn_t& dst0, vector_fp8_e4m3fn_t& dst1, __ubuf__ fp8_e4m3fn_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_hifloat8_t& dst0, vector_hifloat8_t& dst1, __ubuf__ hifloat8_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_int16_t& dst0, vector_int16_t& dst1, __ubuf__ int16_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_uint16_t& dst0, vector_uint16_t& dst1, __ubuf__ uint16_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_half& dst0, vector_half& dst1, __ubuf__ half* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_bfloat16_t& dst0, vector_bfloat16_t& dst1, __ubuf__ bfloat16_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_int32_t& dst0, vector_int32_t& dst1, __ubuf__ int32_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_uint32_t& dst0, vector_uint32_t& dst1, __ubuf__ uint32_t* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_float& dst0, vector_float& dst1, __ubuf__ float* src, int32_t offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_int4x2_t& dst0, vector_int4x2_t& dst1, __ubuf__ int4b_t* src, int32_t offset)
```

### Address Register Offset Addressing API<a name="base-register-indexed-api"></a>

This type of API generates the actual access address through the address register `iter_reg offset`. The address register is typically used in loop scenarios and must be initialized with [`asc_create_iter_reg`](../reg_vector/asc_create_iter_reg.md). The actual access address must meet the 32B alignment requirement.

```cpp
__simd_callee__ inline void asc_loadalign_deintlv(vector_fp4x2_e1m2_t& dst0, vector_fp4x2_e1m2_t& dst1, __ubuf__ fp4x2_e1m2_t* src, iter_reg offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_fp4x2_e2m1_t& dst0, vector_fp4x2_e2m1_t& dst1, __ubuf__ fp4x2_e2m1_t* src, iter_reg offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_uint8_t& dst0, vector_uint8_t& dst1, __ubuf__ uint8_t* src, iter_reg offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_int8_t& dst0, vector_int8_t& dst1, __ubuf__ int8_t* src, iter_reg offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_fp8_e4m3fn_t& dst0, vector_fp8_e4m3fn_t& dst1, __ubuf__ fp8_e4m3fn_t* src, iter_reg offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_fp8_e5m2_t& dst0, vector_fp8_e5m2_t& dst1, __ubuf__ fp8_e5m2_t* src, iter_reg offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_fp8_e8m0_t& dst0, vector_fp8_e8m0_t& dst1, __ubuf__ fp8_e8m0_t* src, iter_reg offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_hifloat8_t& dst0, vector_hifloat8_t& dst1, __ubuf__ hifloat8_t* src, iter_reg offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_uint16_t& dst0, vector_uint16_t& dst1, __ubuf__ uint16_t* src, iter_reg offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_int16_t& dst0, vector_int16_t& dst1, __ubuf__ int16_t* src, iter_reg offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_bfloat16_t& dst0, vector_bfloat16_t& dst1, __ubuf__ bfloat16_t* src, iter_reg offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_half& dst0, vector_half& dst1, __ubuf__ half* src, iter_reg offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_uint32_t& dst0, vector_uint32_t& dst1, __ubuf__ uint32_t* src, iter_reg offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_int32_t& dst0, vector_int32_t& dst1, __ubuf__ int32_t* src, iter_reg offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_float& dst0, vector_float& dst1, __ubuf__ float* src, iter_reg offset)
__simd_callee__ inline void asc_loadalign_deintlv(vector_int4x2_t& dst0, vector_int4x2_t& dst1, __ubuf__ int4b_t* src, iter_reg offset)
```

## Parameters

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| dst0 | Output | First destination vector register. |
| dst1 | Output | Second destination vector register. |
| src | Input | Start address of the source data in UB. For different addressing APIs, `src` represents the base address or the base address participating in address calculation. |
| offset | Input | Address offset. For the scalar offset addressing API, the unit is the number of elements; for the address register offset addressing API, it represents the address register. |

For details about register types, see [reg Data Definition](../reg_data_definition.md).

## Data Type<a name="data-type"></a>

The data types supported by `asc_loadalign_deintlv` are listed in the following table. All three addressing APIs support these data types.

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

- For the [base addressing API](#base-addressing-api), `src` must be a 32B-aligned address, and the access range must not exceed the upper bound of the UB valid address minus `2 x VL`.
- For the [scalar offset addressing API](#scalar-offset-addressing-api) and the [address register offset addressing API](#address-register-offset-addressing-api), the actual access address must be 32B-aligned, and the access range must not exceed the upper bound of the UB valid address minus `2 x VL`.
- When using the address register offset addressing API, initialize the address register through [`asc_create_iter_reg`](../reg_vector/asc_create_iter_reg.md) first.

## Example

Continuously reads `float` data from the UB starting at the 32B-aligned address `src`.

### Base Addressing API

```cpp
for (uint16_t i = 0; i < repeat_times; i++) {
    asc_loadalign_deintlv(reg0, reg1, src + i * one_repeat_size);
    ...
}
```

### Scalar Offset Addressing API

```cpp
// Method 1: Control the offset via offset.
for (uint16_t i = 0; i < repeat_times; i++) {
    asc_loadalign_deintlv(reg0, reg1, src, i * one_repeat_size);
    ...
}

// Method 2: Control the offset via src.
for (uint16_t i = 0; i < repeat_times; i++) {
    asc_loadalign_deintlv(reg0, reg1, src + i * one_repeat_size, 0);
    ...
}
```

### Address Register Offset Addressing API

```cpp
for (uint16_t i = 0; i < repeat_times; i++) {
    iter_reg addr_reg = asc_create_iter_reg_b32(one_repeat_size);
    asc_loadalign_deintlv(reg0, reg1, src, addr_reg);
    ...
}
```
