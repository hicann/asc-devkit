---
okftype: reference.api
description: "Deprecated merge sort synchronous interface that merges up to 4 sorted queues into 1 in descending order of score, supporting the 8-byte Region Proposal structure of half and float. The replacement is asc_mrgsort4 combined with asc_sync."
tag:
  - roles: [compute]
  - operations: [sort, synchronization]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: replaced_by
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_sort/asc_mrgsort4.md
    kind: replaced_by
version: ">=9.0.0"
version_changes:
  - since: "9.1.0"
    kind: arch_support_extended
    scope: "page"
    scope_basis: kind_default
    note: "Added 3510 (Ascend 950PR&950DT) implementation: history_probe shows that asc_mrgsort4_sync under npu_arch_3510 has appeared since 9.1.0, and CANN_9_1_0 \"New Features\" lists asc_mrgsort4 and asc_mrgsort4_sync for 3510; in 9.0.0 only 2201 (Atlas A2/A3) was available."
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The synchronous composite interface is deprecated; use asc_mrgsort4 and asc_sync instead. Both declarations in include/c_api/composite/vector_compute_composite.h carry ASC_DEPRECATED(9.2.0)."
---
# asc_mrgsort4_sync (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T14:20:40.026Z pushedAt=2026-11-18T03:12:16.612Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
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

The header file path is `"c_api/composite/vector_compute_composite.h"`.

**The `asc_mrgsort4_sync` interface is deprecated. Use the [corresponding non-synchronous interface](../vector_compute/vector_sort/asc_mrgsort4.md) and the synchronous interface [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Merges up to four already sorted queues into one queue, with the results sorted by the score field in descending order.

The data processed by asc_mrgsort4 is generally the data processed by the asc_bitsort instruction, that is, the output of the asc_bitsort instruction. The queue structure Region Proposal is as follows:
- The data type is float, and each structure occupies 8 bytes.

![ ](../../../figures/mrgsort4_01.png)

- The data type is half, and each structure also occupies 8 bytes, with 2 bytes reserved in the middle.

![ ](../../../figures/mrgsort4_02.png)

## Prototype

- Synchronous computation

    ```cpp
    __aicore__ inline void asc_mrgsort4_sync(__ubuf__ half* dst, __ubuf__ half* src[ASC_C_API_MRGSORT_ELEMENT_LEN], uint8_t repeat, uint16_t element_length_0, uint16_t element_length_1, uint16_t element_length_2, uint16_t element_length_3, bool if_exhausted_suspension, uint8_t valid_bit)
    __aicore__ inline void asc_mrgsort4_sync(__ubuf__ float* dst, __ubuf__ float* src[ASC_C_API_MRGSORT_ELEMENT_LEN], uint8_t repeat, uint16_t element_length_0, uint16_t element_length_1, uint16_t element_length_2, uint16_t element_length_3, bool if_exhausted_suspension, uint8_t valid_bit)
    ```

## Parameters

**Table 1** Parameter description

|Parameter|Input/Output|Description|
| ------------ | ------------ | ------------ |
| dst | Output | Start address of the destination operand (vector). |
| repeat | Input | Number of iterations. Each time, the source operand and destination operand are calculated and the total length of four queues is skipped. |
| element_length_0 | Input | Length of source queue 0. |
| element_length_1 | Input | Length of source queue 1. |
| element_length_2 | Input | Length of source queue 2. |
| element_length_3 | Input | Length of source queue 3. |
| if_exhausted_suspension | Input | Whether the instruction needs to stop after a queue is exhausted. |
| valid_bit | Input | Number of valid queues. Valid values are as follows:<br>3: The first two queues are valid.<br>7: The first three queues are valid.<br>15: All four queues are valid.  |

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- When repeat=1, it indicates one iteration, and the element_length_..., if_exhausted_suspension, and valid_bit parameters can be set freely.
- When repeat>1, it indicates multiple iterations, and the following conditions must be met: four source queues are included, the four source queues have the same length and are stored contiguously in memory, the value of valid_bit is 15, and the value of if_exhausted_suspension is false.
- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).
- The start addresses of dst and src must be 32-byte aligned.

## Examples

```cpp
// Perform merge sort on 8 sorted Region Proposal queues, each containing 32 elements.
// Step 1: sort four 32-element queues src at a time, repeating twice, to obtain two sorted 128-element new queues tmp.
// Step 2: perform merge sort on the two sorted 128-element new queues tmp again to obtain the final 256-element queue dst.

constexpr uint16_t element_length = 32;
// Each element contains a float32 score and a 4-byte index, occupying 8 bytes, equivalent to 2 sizeof(float).
constexpr uint16_t array_length = element_length * 2;
__ubuf__ float src_workspace[array_length * 8];
__ubuf__ float tmp_workspace[array_length * 8];
__ubuf__ float dst_workspace[array_length * 8];

__ubuf__ float* src[ASC_C_API_MRGSORT_ELEMENT_LEN];
__ubuf__ float* dst;
uint8_t valid_bit;

// Each iteration can process 4 queues, for a total of 2 iterations.
src[0] = &src_workspace[0];
src[1] = &src_workspace[1 * array_length];
src[2] = &src_workspace[2 * array_length];
src[3] = &src_workspace[3 * array_length];
dst = tmp_workspace;
valid_bit = 15;
asc_mrgsort4_sync(dst, src, 2, element_length, element_length, element_length, element_length, false, valid_bit);

// Iterate once to process 2 queues.
src[0] = &tmp_workspace[0];
src[1] = &tmp_workspace[1 * 4 * array_length];
dst = dst_workspace;
valid_bit = 3;
asc_mrgsort4_sync(dst, src, 1, 4 * element_length, 4 * element_length, 0, 0, false, valid_bit);
```
