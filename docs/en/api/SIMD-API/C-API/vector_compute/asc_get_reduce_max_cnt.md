# asc_get_reduce_max_cnt

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T14:10:36.932Z pushedAt=2026-09-08T03:45:05.491Z -->

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

This API is used to obtain the maximum value after the [asc_repeat_reduce_max](./asc_repeat_reduce_max.md) operation, as well as the index of the first occurrence of the maximum value.

## Prototype

```cpp
__aicore__ inline void asc_get_reduce_max_cnt(half& val, uint32_t& index)
__aicore__ inline void asc_get_reduce_max_cnt(float& val, uint32_t& index)
```

## Parameters

| Parameter  | Input/Output | Description |
| :----- | :------- | :------- |
| val | Output | Maximum value. |
| index | Output | Index of the first occurrence of the maximum value. |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- Must be used together with the [asc_repeat_reduce_max](./asc_repeat_reduce_max.md) operation.
- A synchronization operation is required to ensure that [asc_repeat_reduce_max](./asc_repeat_reduce_max.md) has completed before this API is called to obtain the result.

## Example

```cpp
// Calculate the maximum value and index of 256 half values in two iterations, producing two groups of value_index.
constexpr uint32_t total_length = 256;
__ubuf__ half src[total_length];
constexpr uint32_t dst_length = 4;
__ubuf__ half dst[dst_length];

asc_repeat_reduce_max_value_index(dst, src, total_length);
asc_sync();

half maxValue;
uint32_t maxIndex;
asc_get_reduce_max_cnt(maxValue, maxIndex);
```

Result example:

```
Input data src: [1, 2, 3, ..., 128, 129, ..., 256]
maxValue: 256.0
maxIndex: 255
```