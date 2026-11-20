---
okftype: reference.api
description: "Deprecated unaligned UB-to-GM transfer in high-dimensional slicing form, which uses the uint8_t l2_cache_mode to select Normal or Compact mode and pads to 32 bytes; use the asc_store_l2_cache_mode enum form instead."
tag:
  - roles: [data_movement]
  - operations: [store]
  - traits: [unaligned_transfer]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/enum/asc_store_l2_cache_mode.md
    kind: related
    condition: "Alternative L2 Cache management policy configuration type"
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "Storage unit space size and alignment requirements"
  - path: docs/zh/api/SIMD-API/c_api/vector_datamove/asc_copy_ub2gm_align/asc_copy_ub2gm_align_arch_3510.md
    kind: replaced_by
    condition: "The new interface form for high-dimensional slicing data movement declared in the body text"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "Starting from 9.2.0, the 7-parameter uint8_t l2_cache_mode form in include is marked ASC_DEPRECATED(9.2.0, \"2027/09/07\", asc_store_l2_cache_mode), and the body text simultaneously recommends using the asc_store_l2_cache_mode enumeration; in the same version, the header file adjusts the transfer parameter names from n_burst/len_burst to burst_count/burst_len, causing the prototype on this page to be unable to safely match the current declaration (doc_overrides reports no locatable declaration). This difference is only recorded in this document's annotation and the body text is not modified."
---
# asc_copy_ub2gm_align (NPU Architecture Version 3510, Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T12:34:10.336Z pushedAt=2026-11-18T03:12:16.444Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Not supported
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

**The high-dimensional slicing data movement interface is deprecated. Use the `asc_store_l2_cache_mode` type enumeration value to configure the L2 Cache management policy. For the new interface form, see [asc_copy_ub2gm_align high-dimensional slicing data movement interface](../vector_datamove/asc_copy_ub2gm_align/asc_copy_ub2gm_align_arch_3510.md#high-dimensional-slicing-data-movement).**

Provides non-aligned data movement, moving data from Unified Buffer (UB) to Global Memory (GM), and supports 8-bit/16-bit/32-bit data type movement.

This interface uses the high-dimensional slicing movement method and does not support setting a fill value. If the moved data length is not 32-byte aligned, the hardware supplements padding data when reading UB data, padding the moved data to 32-byte alignment. The supplementary padding data is discarded when writing to GM. You can select Normal mode or Compact mode by configuring the `src_stride` parameter:

- Compact mode: set `src_stride` to `len_burst`. Multiple data blocks are tightly arranged in UB and are treated as a whole in 32-byte padding processing; if the total length of all data blocks is not 32-byte aligned, padding data is supplemented only at the end of the whole to 32-byte alignment, and the supplementary padding data is discarded when writing to GM.
- Normal mode: set `src_stride` to a value that is not equal to `len_burst` and satisfies 32-byte alignment. Each data block is processed for 32-byte alignment separately.
    - If `len_burst` is 32-byte aligned, padding data is not read, and all data read from UB is written to GM.
    - If `len_burst` is not 32-byte aligned, padding data is supplemented to 32-byte alignment when reading UB data, and the supplementary padding data is discarded when writing to GM.

For example, when moving 3 data blocks each 48 bytes in length:

- In Compact mode, set `src_stride` to 48. The starting offsets of the data blocks in UB are 0, 48, and 96 in sequence, and 16 bytes of padding data are supplemented only at the end of the 144 bytes of valid data.
- In Normal mode, set `src_stride` to 64. The starting offsets of the data blocks in UB are 0, 64, and 128 in sequence, and 16 bytes of dummy padding data are appended to each data block.

In both modes, the dummy padding data is not written to GM. When only one data block is transferred, or when `len_burst` is already 32-byte aligned, the transfer results of the two modes are the same.

## Prototype

- High-dimensional slicing transfer

    ```cpp
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ int8_t* dst, __ubuf__ int8_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t l2_cache_mode, uint64_t dst_stride, uint32_t src_stride)
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ uint8_t* dst, __ubuf__ uint8_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t l2_cache_mode, uint64_t dst_stride, uint32_t src_stride)
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ hifloat8_t* dst, __ubuf__ hifloat8_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t l2_cache_mode, uint64_t dst_stride, uint32_t src_stride)
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ fp8_e5m2_t* dst, __ubuf__ fp8_e5m2_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t l2_cache_mode, uint64_t dst_stride, uint32_t src_stride)
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ fp8_e4m3fn_t* dst, __ubuf__ fp8_e4m3fn_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t l2_cache_mode, uint64_t dst_stride, uint32_t src_stride)
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ int16_t* dst, __ubuf__ int16_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t l2_cache_mode, uint64_t dst_stride, uint32_t src_stride)
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ uint16_t* dst, __ubuf__ uint16_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t l2_cache_mode, uint64_t dst_stride, uint32_t src_stride)
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ half* dst, __ubuf__ half* src, uint16_t n_burst, uint32_t len_burst, uint8_t l2_cache_mode, uint64_t dst_stride, uint32_t src_stride)
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ bfloat16_t* dst, __ubuf__ bfloat16_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t l2_cache_mode, uint64_t dst_stride, uint32_t src_stride)
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ int32_t* dst, __ubuf__ int32_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t l2_cache_mode, uint64_t dst_stride, uint32_t src_stride)
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ uint32_t* dst, __ubuf__ uint32_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t l2_cache_mode, uint64_t dst_stride, uint32_t src_stride)
    __aicore__ inline void asc_copy_ub2gm_align(__gm__ float* dst, __ubuf__ float* src, uint16_t n_burst, uint32_t len_burst, uint8_t l2_cache_mode, uint64_t dst_stride, uint32_t src_stride)
    ```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| dst | Output | Start address of the destination GM. 1-byte alignment is required. |
| src | Input | Start address of the source UB. 32-byte alignment is required. |
| n_burst | Input | Number of consecutive data blocks to be transferred. Value range: [0, 4095]. |
| len_burst | Input | Length of each consecutive data block to be transferred, in bytes. Value range: [0, 2097151]. |
| l2_cache_mode | Input | L2 Cache management policy configuration. The type of this parameter is `uint8_t`. It is recommended to use an enumeration value of the [asc_store_l2_cache_mode](../defs/enum/asc_store_l2_cache_mode.md) type instead. |
| dst_stride | Input | Distance between adjacent consecutive data blocks of the destination operand (the interval between the head of the preceding data block and the head of the following data block), in bytes.<br>When only one data block is transferred, that is, when `n_burst` is set to 1, this parameter can be set to 0. |
| src_stride | Input | Distance between adjacent consecutive data blocks of the source operand (the interval between the head of the preceding data block and the head of the following data block), in bytes. It is used to select the data transfer mode.<br>&bull; Equal to `len_burst`: Compact mode. The source data blocks are tightly arranged in UB, and `src_stride` supports byte alignment.<br>&bull; Not equal to `len_burst`: Normal mode. `src_stride` must meet the 32-byte alignment requirement. |

## Return Value

None

## Pipeline Type

PIPE_MTE3

## Constraints

- For the space size and alignment requirements of each storage unit, see [Storage Unit Description](../general_description_and_constraints.md#storage-unit-description).
- When either `n_burst` or `len_burst` is 0, this interface is treated as a NOP (no operation).
- If multiple `asc_copy_ub2gm_align` instructions need to be executed and the destination addresses of the `asc_copy_ub2gm_align` instructions overlap, synchronization instructions ([asc_sync_notify](../sync/intra_core_sync/asc_sync_notify.md) and [asc_sync_wait](../sync/intra_core_sync/asc_sync_wait.md)) must be inserted to ensure the serialization of multiple `asc_copy_ub2gm_align` instructions and prevent abnormal data.
- When `src_stride` is not equal to `len_burst`, `src_stride` must be 32-byte aligned.
