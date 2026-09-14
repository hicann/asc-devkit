# asc_loadalign_postupdate

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-08-29T09:42:05.769Z pushedAt=2026-09-03T12:34:40.815Z -->

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

Loads data from UB to MaskReg. When post mode is enabled, each call to this API updates the address of the source operand in UB.

## Prototype

- Normal load

    ```cpp
    __simd_callee__ inline void asc_loadalign_postupdate(vector_bool& dst, __ubuf__ uint32_t*& src, int32_t offset)
    ```

- Upsampling load

    ```cpp
    __simd_callee__ inline void asc_loadalign_upsample_postupdate(vector_bool& dst, __ubuf__ uint32_t*& src, int32_t offset)
    ```

- Downsampling load

    ```cpp
    __simd_callee__ inline void asc_loadalign_downsample_postupdate(vector_bool& dst, __ubuf__ uint32_t*& src, int32_t offset)
    ```

## Parameters

| Parameter  | Input/Output | Description |
| :----- | :------- | :------- |
| dst | Output | Destination operand (mask register). |
| src | Input | Start address of the source operand (vector). |
| offset | Input | Offset of the data transfer. |

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

None

## Example

```cpp
//Total data length is 256.
constexpr uint64_t total_length = 256;
vector_bool dst = asc_create_mask_b16(PAT_ALL);
__ubuf__ uint32_t src[total_length];
//Offset of the data transfer is 64.
int32_t offset = 64;
asc_loadalign_postupdate(dst, src, offset);
```