# asc_loadalign

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-08-29T09:47:02.936Z pushedAt=2026-09-03T12:34:49.641Z -->

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

Loads data from the UB to the MaskReg.

## Prototype

- Normal load

    ```cpp
    __simd_callee__ inline void asc_loadalign(vector_bool& dst, __ubuf__ uint32_t* src)
    __simd_callee__ inline void asc_loadalign(vector_bool& dst, __ubuf__ uint32_t* src, int32_t offset)
    __simd_callee__ inline void asc_loadalign(vector_bool& dst, __ubuf__ uint32_t* src, iter_reg offset)
    ```

- Upsampling load

    ```cpp
    __simd_callee__ inline void asc_loadalign_upsample(vector_bool& dst, __ubuf__ uint32_t* src)
    __simd_callee__ inline void asc_loadalign_upsample(vector_bool& dst, __ubuf__ uint32_t* src, int32_t offset)
    __simd_callee__ inline void asc_loadalign_upsample(vector_bool& dst, __ubuf__ uint32_t* src, iter_reg offset)
    ```

- Downsampling load

    ```cpp
    __simd_callee__ inline void asc_loadalign_downsample(vector_bool& dst, __ubuf__ uint32_t* src)
    __simd_callee__ inline void asc_loadalign_downsample(vector_bool& dst, __ubuf__ uint32_t* src, int32_t offset)
    __simd_callee__ inline void asc_loadalign_downsample(vector_bool& dst, __ubuf__ uint32_t* src, iter_reg offset)
    ```

## Parameters

| Parameter  | Input/Output | Description |
| :----- | :------- | :------- |
| dst | Output | Destination operand (mask register). |
| src | Input | Start address of the source operand (vector). |
| offset | Input | When the input is of the **reg_iter** type, the user passes the offset through an address register; when the input is of the **int32_t** type, the user passes the offset directly as a value. |

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- When **offset** is omitted, the default offset value is **offset**=0.
- The user can pass the **offset** parameter by incrementing the base address or by using an offset.

## Example

```cpp
constexpr uint64_t total_length = 256;
vector_bool dst = asc_create_mask_b16(PAT_ALL);
__ubuf__ uint32_t src[total_length];
iter_reg offset = asc_create_iter_reg_b32(64);
asc_loadalign(dst, src, offset);
```