---
okftype: reference.api
description: "Obtains the number of Region Proposals already processed by each of the four queues after an asc_mrgsort4 operation. The output array length is defined as 4 by ASC_C_API_MRGSORT_ELEMENT_LEN. Call this API only after setting if_exhausted_suspension of mrgsort4 to true and synchronizing."
tag:
  - roles: [query]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_sort/asc_mrgsort4.md
    kind: requires
    condition: "Must be used together with the merge API, and its if_exhausted_suspension must be true."
version: ">=8.5.0"
version_changes: []
---
# asc_get_vms4_sr

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:04.039248+00:00 pushedAt=2026-11-16T02:10:59.352Z nl=2 -->

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

The header file path is `"c_api/vector_compute/compute/vector_sort.h"`.

This API obtains the number of Region Proposals already processed by each of the 4 queues after the [asc_mrgsort4](asc_mrgsort4.md) operation. When using this API, set the if_exhausted_suspension parameter in asc_mrgsort4 to true. In this configuration mode, the MrgSort4 instruction stops after a queue is exhausted.

## Prototype

```cpp
__aicore__ inline void asc_get_vms4_sr(uint16_t sorted_num[ASC_C_API_MRGSORT_ELEMENT_LEN])
```

## Parameters

**Table 1** Parameters

|Parameter|Input/Output|Description|
| ------------ | ------------ | ------------ |
| sorted_num | Output | Number of Region Proposals processed in each queue. The value of the queue count ASC_C_API_MRGSORT_ELEMENT_LEN is 4. |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- This API must be used together with the [asc_mrgsort4](asc_mrgsort4.md) operation, and the if_exhausted_suspension parameter in asc_mrgsort4 must be set to true.
- Use a synchronization operation to ensure that [asc_mrgsort4](asc_mrgsort4.md) has completed before calling this API to obtain the result.

## Examples

```cpp
// Merge-sort 4 sorted Region Proposal queues, each containing 32 elements.
constexpr uint16_t element_length = 32;
constexpr uint16_t array_length = element_length * 2;
// Each element contains a float32 score and a 4-byte index, occupying 8 bytes, equivalent to 2 sizeof(float).
__ubuf__ float src_workspace[array_length * 4];

__ubuf__ float* src[ASC_C_API_MRGSORT_ELEMENT_LEN];
__ubuf__ float dst[array_length * 4];

uint8_t repeat = 1;
uint8_t valid_bit = 0b1111;
bool if_exhausted_suspension = true;
src[0] = &src_workspace[0];
src[1] = &src_workspace[1 * array_length];
src[2] = &src_workspace[2 * array_length];
src[3] = &src_workspace[3 * array_length];

asc_mrgsort4(dst, src, repeat, element_length, element_length, element_length, element_length, if_exhausted_suspension, valid_bit);
asc_sync();

uint16_t sorted_num[ASC_C_API_MRGSORT_ELEMENT_LEN];
asc_get_vms4_sr(sorted_num);
```