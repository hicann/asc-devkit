---
okftype: reference.api
description: "On the 2201 architecture, moves UB data out to GM at a length that is not an integer multiple of 32 bytes, pads the UB read and discards the dummy data to write only valid data, supporting the first n data and high-dimensional slicing in Normal mode."
tag:
  - roles: [data_movement]
  - operations: [copy]
  - traits: [unaligned_transfer]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: related
    condition: "The main text references storage units accordingly"
  - path: docs/zh/api/SIMD-API/c_api/sync/system_sync_overview.md
    kind: related
    condition: "Insert synchronization instructions according to this page when data dependencies exist"
version: ">=9.0.0"
version_changes:
  - since: "9.1.0"
    kind: overload_added
    scope: "section:Prototype"
    scope_basis: resolved
    note: "Added a simplified overload that takes only dst, src, and size: CANN 9.1.0 Release Notes record that asc_copy_ub2gm_align (including _sync) added a simplified overload, corresponding to the three-parameter form for moving the first n data in the body."
---
# asc_copy_ub2gm_align (NPU architecture version 2201)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:04.085982+00:00 pushedAt=2026-11-16T02:10:59.491Z nl=2 -->

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

The header file path is `"c_api/vector_datamove/vector_datamove.h"`.

Copies data from the Unified Buffer (UB) to Global Memory (GM) and supports data lengths that are not integer multiples of 32 bytes. During the copy, the data is padded to an integer multiple of 32 bytes. When writing to GM, the padding data is discarded and only valid data of the specified length is written. This API supports copying 8-bit/16-bit/32-bit data types. The copied data length must still meet the byte alignment requirement of the corresponding data type. For the alignment requirements of the source and destination start addresses, see [Parameters](#parameters).

This API supports the following two data movement methods, neither of which supports setting a padding value:

- Copying the first n data items

    If the length of the copied data is not 32-byte aligned, the copied data is automatically padded with dummy data to 32-byte alignment when reading UB data during the copy. The value of the dummy data is the value of the first element of the data block. The padding data is discarded when writing to GM.

- High-dimensional slicing data movement

    High-dimensional slicing data movement uses Normal mode. If the length of a single data block specified by `burst_len` is not 32-byte aligned, each data block specified by `burst_count` is individually padded with dummy data to 32-byte alignment when reading UB data. The value of the dummy data is the value of the first element of the data block. The padding data is discarded when writing to GM.

## Prototype

- Copying the first n data items

    ```cpp
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ uint8_t* dst, __ubuf__ uint8_t* src, uint32_t size)
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ int8_t* dst, __ubuf__ int8_t* src, uint32_t size)
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ half* dst, __ubuf__ half* src, uint32_t size)
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ uint16_t* dst, __ubuf__ uint16_t* src, uint32_t size)
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ int16_t* dst, __ubuf__ int16_t* src, uint32_t size)
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ bfloat16_t* dst, __ubuf__ bfloat16_t* src, uint32_t size)
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ uint32_t* dst, __ubuf__ uint32_t* src, uint32_t size)
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ float* dst, __ubuf__ float* src, uint32_t size)
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ int32_t* dst, __ubuf__ int32_t* src, uint32_t size)
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ double* dst, __ubuf__ double* src, uint32_t size)
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ int64_t* dst, __ubuf__ int64_t* src, uint32_t size)
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ uint64_t* dst, __ubuf__ uint64_t* src, uint32_t size)
    ```

- High-dimensional slicing data movement

    ```cpp
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ uint8_t* dst, __ubuf__ uint8_t* src, uint16_t burst_count, uint32_t burst_len,uint8_t left_padding_num, uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap)
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ int8_t* dst, __ubuf__ int8_t* src, uint16_t burst_count, uint32_t burst_len,uint8_t left_padding_num, uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap)
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ half* dst, __ubuf__ half* src, uint16_t burst_count, uint32_t burst_len,uint8_t left_padding_num, uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap)
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ uint16_t* dst, __ubuf__ uint16_t* src, uint16_t burst_count, uint32_t burst_len,uint8_t left_padding_num, uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap)
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ int16_t* dst, __ubuf__ int16_t* src, uint16_t burst_count, uint32_t burst_len,uint8_t left_padding_num, uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap)
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ bfloat16_t* dst, __ubuf__ bfloat16_t* src, uint16_t burst_count, uint32_t burst_len,uint8_t left_padding_num, uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap)
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ uint32_t* dst, __ubuf__ uint32_t* src, uint16_t burst_count, uint32_t burst_len,uint8_t left_padding_num, uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap)
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ float* dst, __ubuf__ float* src, uint16_t burst_count, uint32_t burst_len,uint8_t left_padding_num, uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap)
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ int32_t* dst, __ubuf__ int32_t* src, uint16_t burst_count, uint32_t burst_len,uint8_t left_padding_num, uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap)
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ double* dst, __ubuf__ double* src, uint16_t burst_count, uint32_t burst_len,uint8_t left_padding_num, uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap)
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ int64_t* dst, __ubuf__ int64_t* src, uint16_t burst_count, uint32_t burst_len,uint8_t left_padding_num, uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap)
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ uint64_t* dst, __ubuf__ uint64_t* src, uint16_t burst_count, uint32_t burst_len,uint8_t left_padding_num, uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap)
    ```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| dst | Output | Start address of the destination GM. Must be 1-byte aligned. |
| src | Input | Start address of the source UB. Must be 32-byte aligned. |
| size | Input | Size of the copied data, in bytes. Value range: [0, 2097151]. |
| burst_count | Input | Number of consecutive data blocks to be copied. Value range: [0, 4095]. |
| burst_len | Input | Length of each consecutive data block to be copied, in bytes. Value range: [0, 2097151]. |
| left_padding_num | Input | Reserved parameter, which must be set to 0. |
| right_padding_num | Input | Reserved parameter, which must be set to 0. |
| src_gap | Input | Gap between adjacent consecutive data blocks of the source operand (the gap between the tail of the preceding data block and the head of the following data block). The unit is DataBlock (32 bytes).|
| dst_gap | Input | Gap between adjacent consecutive data blocks of the destination operand (the gap between the tail of the preceding data block and the head of the following data block). The unit is bytes. |

## Return Value

None

## Pipeline Type

PIPE_MTE3

## Constraints

- For the space size and alignment requirements of each storage unit, see [Storage Unit Description](../../general_description_and_constraints.md#storage-unit-description).
- When either `burst_count` or `burst_len` is 0, this API is treated as a NOP (no operation).
- When `size` is 0, this API is treated as a NOP (no operation).
- If there is a data dependency between this instruction and other instructions, you must insert a [synchronization instruction](../../sync/overview.md#synchronization-control-overview) to ensure that the instructions execute serially and prevent abnormal data from occurring.

## Examples

```cpp
//total_length refers to the total length of the data involved in the movement. dst is the externally input GM buffer of type uint8_t.
constexpr uint64_t total_length = 128;
__ubuf__ uint8_t src[total_length];
asc_copy_ub2gm_align(dst, src, total_length * sizeof(uint8_t));
```
