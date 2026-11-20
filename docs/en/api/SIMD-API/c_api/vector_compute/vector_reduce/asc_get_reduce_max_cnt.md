---
okftype: reference.api
description: "Query API that obtains the maximum value and the index of the first maximum value from the asc_repeat_reduce_max computation result. It provides half and float overloads, must be called after synchronization, and has pipeline type PIPE_S."
tag:
  - roles: [query]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_reduce/asc_repeat_reduce_max.md
    kind: related
    condition: "Must be used with this verification reduction API and read its result"
version: ">=9.0.0"
version_changes: []
---
# asc_get_reduce_max_cnt

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.968531+00:00 pushedAt=2026-11-16T02:10:59.205Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Not supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Supported
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

The header file path is `"c_api/vector_compute/compute/vector_reduce.h"`.

This API is used to obtain the maximum value after the [asc_repeat_reduce_max](asc_repeat_reduce_max.md) operation, as well as the index of the first occurrence of the maximum value.

## Prototype

```cpp
__aicore__ inline void asc_get_reduce_max_cnt(half& val, uint32_t& index)
__aicore__ inline void asc_get_reduce_max_cnt(float& val, uint32_t& index)
```

## Parameters

**Table 1** Parameters

| Parameter  | Input/Output | Description |
| :----- | :------- | :------- |
| val | Output | Maximum value. |
| index | Output | Index of the first occurrence of the maximum value. |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- Must be used together with the [asc_repeat_reduce_max](asc_repeat_reduce_max.md) operation.
- A synchronization operation is required to ensure that [asc_repeat_reduce_max](asc_repeat_reduce_max.md) has completed before this API is called to obtain the result.

## Examples

```cpp
// Example: Compute the maximum value and index of 256 half values in 2 iterations, producing 2 groups of value_index.
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