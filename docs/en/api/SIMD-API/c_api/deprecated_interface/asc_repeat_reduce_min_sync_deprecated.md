---
okftype: reference.api
description: "Deprecated family of synchronous interfaces for computing the minimum per iteration, including four result layouts: index_value, value_index, only_value, and only_index. Returns the minimum and its index, supports half and float. The replacement is asc_repeat_reduce_min combined with asc_sync."
tag:
  - roles: [compute]
  - operations: [minimum, reduction, synchronization]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: replaced_by
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_reduce/asc_get_reduce_min_cnt.md
    kind: related
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_reduce/asc_repeat_reduce_min.md
    kind: replaced_by
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The four min reduce sync interfaces (index_value, value_index, only_value, only_index) are deprecated. Use asc_repeat_reduce_min and asc_sync instead. The 8 declarations in include/c_api/composite/vector_compute_composite.h all carry ASC_DEPRECATED(9.2.0)."
---
# asc_repeat_reduce_min_sync (deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T14:40:08.547Z pushedAt=2026-11-18T03:12:16.643Z nl=2 -->

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

The header file path is `"c_api/composite/vector_compute_composite.h"`.

**The `asc_repeat_reduce_min_index_value_sync`, `asc_repeat_reduce_min_value_index_sync`, `asc_repeat_reduce_min_only_value_sync`, and `asc_repeat_reduce_min_only_index_sync` interfaces are deprecated. Use [the corresponding non-sync interface](../vector_compute/vector_reduce/asc_repeat_reduce_min.md) and the sync interface [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Computes the minimum of all elements in each Repeat, and the index of the first minimum element within the current Repeat. One Repeat computes 256 bytes.

index_value interface: indicates that index is in the lower half, and the return result storage order is [index, value]. When the input data type is half, the index data type is uint16; when the input data type is float, the index data type is uint32.
value_index interface: indicates that value is in the lower half, and the return result storage order is [value, index]. When the input data type is half, the index data type is uint16; when the input data type is float, the index data type is uint32.
only_value interface: indicates that only the extreme value is returned, and the return result storage order is [value].
only_index interface: indicates that only the extreme value index is returned, and the return result storage order is [index]. The index data type is fixed to uint32

After executing this interface, you can continue to execute [asc_get_reduce_min_cnt](../vector_compute/vector_reduce/asc_get_reduce_min_cnt.md) to obtain the minimum value across all repeats, and the index of the first minimum value.

## Prototype

- Synchronous computation

    ```cpp
    __aicore__ inline void asc_repeat_reduce_min_index_value_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count)
    __aicore__ inline void asc_repeat_reduce_min_index_value_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
    __aicore__ inline void asc_repeat_reduce_min_value_index_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count)
    __aicore__ inline void asc_repeat_reduce_min_value_index_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
    __aicore__ inline void asc_repeat_reduce_min_only_value_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count)
    __aicore__ inline void asc_repeat_reduce_min_only_value_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
    __aicore__ inline void asc_repeat_reduce_min_only_index_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count)
    __aicore__ inline void asc_repeat_reduce_min_only_index_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
    ```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
|----|-----|-----|
| dst | Output | Start address of the destination operand (vector). |
| src  | Input | Start address of the source operand (vector). |
| count | Input | Number of elements involved in the computation. |

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- The index data in the output is stored according to the data type of dst. For example, the index data of a float output should be uint32, so when reading, float must be reinterpreted as uint32.
- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).
- The start address of dst must be 4-byte aligned when the input type is half, and 8-byte aligned when the input type is float.
- The start address of src must be 32-byte aligned.

## Examples

```cpp
// Compute the minimum and index of 256 half values in 2 iterations, producing 2 groups of value_index results.
constexpr uint32_t total_length = 256;
__ubuf__ half src[total_length];
constexpr uint32_t dst_length = 4;
__ubuf__ half dst[dst_length];

// The sync interface uses count to specify the total number of elements involved in the computation, without the need to manually set the mask.
asc_repeat_reduce_min_value_index_sync(dst, src, total_length);
```

Result example:

```
Input data src: [256, 255, 254, ..., 129, 128, ..., 2, 1]
Output data dst: [129, 7.6e-6, 1, 7.6e-6]
Output description: The binary representation of the half type 7.6e-6 is 0x007F, which, when reinterpreted as uint32, is the minimum index 127.
```
