# asc_set_deq_scale

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T15:16:19.474Z pushedAt=2026-09-08T03:45:05.579Z -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Not supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
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

Sets the value of the DEQSCALE (quantization parameter) register.

## Prototype

```cpp
__aicore__ inline void asc_set_deq_scale(__ubuf__ uint64_t* tmp, float scale_arr[ASC_VDEQ_SIZE], int16_t offset_arr[ASC_VDEQ_SIZE], bool sign_mode_arr[ASC_VDEQ_SIZE])
__aicore__ inline void asc_set_deq_scale(float scale, int16_t offset, bool sign_mode)
__aicore__ inline void asc_set_deq_scale(half scale)
```

## Parameters

|Parameter|Input/Output|Description|
| ------------ | ------------ | ------------ |
|tmp|Input|A temporary memory area used to store a set of 16 quantization parameters.|
|scale_arr|Input|**float** array type with a size of 16, used to store a set of quantization parameters.|
|offset_arr|Input|**int16_t** array type with a size of 16, used to store a set of quantization parameters.|
|sign_mode_arr|Input|**bool** array type with a size of 16, used to store a set of quantization parameters.|
|scale (half)|Input|Quantization parameter of the **half** type. Applies to scenarios where **int32_t** is converted to **half** in APIs such as **cast_deq**.|
|scale (float)|Input|Quantization parameter of the **float** type. Sets the value of the **DEQSCALE** register.|
|offset|Input|Quantization parameter of the **int16_t** type, where only the first 9 bits are valid.|
|sign_mode|Input|**bool** type, indicating whether the quantization result is signed.|

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

None

## Example

```cpp
constexpr uint32_t src_size = 256;
__ubuf__ half src[src_size];
__ubuf__ half dst[src_size];
float scale = 1.0;
int16_t offset = 2;
bool sign_mode = false;
asc_set_deq_scale(scale, offset, sign_mode);
```
