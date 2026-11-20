---
okftype: reference.api
description: "Deprecated API page: the integer l2_cache_mode version of asc_copy_gm2ub_align under the 3510 architecture provides unaligned transfer, pads GM data to 32-byte alignment before moving it into UB, selects Compact or Normal mode based on dst_stride, and manually or automatically pads the unaligned tail. It is officially recommended to use the new overload that configures the L2 policy via an enum."
tag:
  - roles: [data_movement]
  - operations: [copy]
  - traits: [padding, unaligned_transfer]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/enum/asc_load_l2_cache_mode.md
    kind: related
    condition: "The new overload uses this enum to express the L2 Cache management policy"
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "The space size and alignment requirements of each storage unit follow its storage unit description"
  - path: docs/zh/api/SIMD-API/c_api/vector_datamove/asc_copy_gm2ub_align/asc_copy_gm2ub_align_arch_3510.md
    kind: replaced_by
    condition: "The new interface form uses the asc_load_l2_cache_mode enum to configure the L2 Cache policy"
  - path: docs/zh/api/SIMD-API/c_api/vector_datamove/asc_set_copy_pad_val.md
    kind: requires
    condition: "When manual padding is used or the left/right padding is non-zero, the padding value must be set first"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: parameter_changed
    scope: "param:n_burst"
    scope_basis: fallback
    note: "Release Notes 9.2.0 records the fix for insufficient bit width of n_burst in the 3510 aligned copy interface and expands the related overloads to uint32_t. The deprecated prototype on this page still uses uint16_t; the exact effective overload range is yet to be verified, so it is marked as fallback."
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "Release Notes 9.2.0 deprecates the old overloads that express the L2 Cache policy with integer parameters. The replacement path is the overload of the same name that uses the asc_load_l2_cache_mode enum; the interface has not been removed."
---
# asc_copy_gm2ub_align (NPU Architecture Version 3510, Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T11:45:13.752Z pushedAt=2026-11-18T03:12:16.400Z nl=2 -->

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

**The high-dimensional slicing data movement API is deprecated. Use the `asc_load_l2_cache_mode` type enumeration value to configure the L2 Cache management policy. For the new API form, see [asc_copy_gm2ub_align high-dimensional slicing data movement API](../vector_datamove/asc_copy_gm2ub_align/asc_copy_gm2ub_align_arch_3510.md#high-dimensional-slicing-data-movement).**

Provides non-aligned data movement, moving data from Global Memory (GM) to Unified Buffer (UB), and supports 8-bit/16-bit/32-bit data type movement.

This API uses the high-dimensional slicing movement mode. If the moved data length is not 32-byte aligned, the moved data is padded to 32-byte alignment. You can select Normal mode or Compact mode by configuring the `dst_stride` parameter. The following two padding modes are supported in non-32-byte-aligned scenarios:

- Manual padding: Call `asc_set_copy_pad_val` to set the padding value before the movement, and set `enable_constant_pad` to `true`.
- Automatic padding: Set `enable_constant_pad` to `false`, and the hardware automatically pads dummy data, whose value is the value of the first element of the data block.

The data movement modes are described as follows:

- Compact mode: Set `dst_stride` to `len_burst`, and set `left_padding_num` and `right_padding_num` to 0. Multiple data blocks are tightly arranged in UB and are treated as a whole in 32-byte padding; if the total length of all data blocks is not 32-byte aligned, padding is performed only at the end of the whole to 32-byte alignment using the manual or automatic padding mode described above.
- Normal mode: Set `dst_stride` to a value that is not equal to `len_burst` and satisfies 32-byte alignment. The address interval between adjacent destination data blocks is specified by `dst_stride`, and intervals can be reserved between data blocks without tight arrangement.
    - When both `left_padding_num` and `right_padding_num` are 0, each data block is padded to 32-byte alignment separately using the manual or automatic padding mode described above.
    - When `left_padding_num` or `right_padding_num` is non-zero, `enable_constant_pad` does not take effect, and you must call `asc_set_copy_pad_val` to configure the padding value before the movement. If the data block length plus the total bytes of left and right padding is not 32-byte aligned, the hardware continues to pad dummy data on the right side to 32-byte alignment, and the value of the dummy data is the value configured by `asc_set_copy_pad_val`.

For example, when moving 3 data blocks of 48 bytes each without left or right padding:

- In Compact mode, set `dst_stride` to 48. The starting offsets of each data block in UB are 0, 48, and 96 in sequence, and only 16 bytes are padded at the end of the 144 bytes of valid data.
- In Normal mode, set `dst_stride` to 64. The starting offsets of each data block in UB are 0, 64, and 128 in sequence, and each data block is padded with 16 bytes separately.

When only 1 data block is moved, or when `len_burst` is already 32-byte aligned and there is no left or right padding, the movement results of the two modes are the same.

## Prototype

- High-dimensional slicing movement

    ```cpp
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ int8_t* dst, __gm__ int8_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, uint8_t l2_cache_mode, uint64_t src_stride, uint32_t dst_stride)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ uint8_t* dst, __gm__ uint8_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, uint8_t l2_cache_mode, uint64_t src_stride, uint32_t dst_stride)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ hifloat8_t* dst, __gm__ hifloat8_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, uint8_t l2_cache_mode, uint64_t src_stride, uint32_t dst_stride)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ fp8_e5m2_t* dst, __gm__ fp8_e5m2_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, uint8_t l2_cache_mode, uint64_t src_stride, uint32_t dst_stride)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ fp8_e4m3fn_t* dst, __gm__ fp8_e4m3fn_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, uint8_t l2_cache_mode, uint64_t src_stride, uint32_t dst_stride)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ int16_t* dst, __gm__ int16_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, uint8_t l2_cache_mode, uint64_t src_stride, uint32_t dst_stride)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ uint16_t* dst, __gm__ uint16_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, uint8_t l2_cache_mode, uint64_t src_stride, uint32_t dst_stride)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ half* dst, __gm__ half* src, uint16_t n_burst, uint32_t len_burst, uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, uint8_t l2_cache_mode, uint64_t src_stride, uint32_t dst_stride)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, uint8_t l2_cache_mode, uint64_t src_stride, uint32_t dst_stride)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ int32_t* dst, __gm__ int32_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, uint8_t l2_cache_mode, uint64_t src_stride, uint32_t dst_stride)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ uint32_t* dst, __gm__ uint32_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, uint8_t l2_cache_mode, uint64_t src_stride, uint32_t dst_stride)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ float* dst, __gm__ float* src, uint16_t n_burst, uint32_t len_burst, uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, uint8_t l2_cache_mode, uint64_t src_stride, uint32_t dst_stride)
    ```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| dst | Output | Starting address of the destination UB. Requires 32-byte alignment. |
| src | Input | Starting address of the source GM. Requires 1-byte alignment. |
| n_burst | Input | Number of consecutive data blocks to be moved. Value range: [0, 4095]. |
| len_burst | Input | Length of each consecutive data block to be moved, unit: byte. Value range: [0, 2097151]. |
| left_padding_num | Input | Number of elements to be padded on the left side of the consecutive data blocks. The padding data size corresponding to this parameter cannot exceed 32 bytes. Must be set to 0 in Compact mode. |
| right_padding_num | Input | Number of elements to be padded on the right side of the consecutive data blocks. The padding data size corresponding to this parameter cannot exceed 32 bytes. Must be set to 0 in Compact mode. |
| enable_constant_pad | Input | When both `left_padding_num` and `right_padding_num` are 0, configures the padding mode for non-aligned scenarios. Value description:  <br>&bull; `true`: manual padding, and the padding value is the value set by the `asc_set_copy_pad_val` interface. <br>&bull; `false`: automatic padding, where the hardware pads dummy data, and the value of the dummy data is the value of the first element of the data block.<br>When `left_padding_num` or `right_padding_num` is non-zero, this parameter does not take effect. |
| l2_cache_mode | Input | L2 Cache management policy configuration. The type of this parameter is `uint8_t`. It is recommended to use the enumeration value of the [asc_load_l2_cache_mode](../defs/enum/asc_load_l2_cache_mode.md) type instead. |
| src_stride | Input | Distance between adjacent consecutive data blocks of the source operand (the interval between the head of the preceding data block and the head of the following data block), unit: byte.<br>When only 1 data block is moved, that is, when `n_burst` is set to 1, this parameter can be set to 0. |
| dst_stride | Input | Distance between adjacent consecutive data blocks of the destination operand (the interval between the head of the preceding data block and the head of the following data block), unit: byte, used to select the data movement mode.<br>&bull; Equal to `len_burst`: Compact mode, where the destination data blocks are tightly arranged in UB, and `dst_stride` supports byte alignment.<br>&bull; Not equal to `len_burst`: Normal mode, where `dst_stride` must meet the 32-byte alignment requirement.<br>When only 1 data block is moved, that is, when `n_burst` is set to 1, this parameter can be set to 0. |

## Return Value

None

## Pipeline Type

PIPE_MTE2

## Constraints

- For the space size and alignment requirements of each storage unit, see [Storage Unit Description](../general_description_and_constraints.md#storage-unit-description).
- When either `n_burst` or `len_burst` is 0, this interface is treated as a NOP (no operation).
- If multiple `asc_copy_gm2ub_align` instructions need to be executed and the destination addresses of the `asc_copy_gm2ub_align` instructions overlap, synchronization instructions ([asc_sync_notify](../sync/intra_core_sync/asc_sync_notify.md) and [asc_sync_wait](../sync/intra_core_sync/asc_sync_wait.md)) must be inserted to ensure serialization of multiple `asc_copy_gm2ub_align` instructions and prevent abnormal data.
- When `left_padding_num` or `right_padding_num` is non-zero, `enable_constant_pad` does not take effect, and `asc_set_copy_pad_val` must be called to configure the padding value before the movement. The padding data size corresponding to `left_padding_num` and `right_padding_num` must not exceed 32 bytes.
- When `dst_stride` is not equal to `len_burst`, `dst_stride` must be 32-byte aligned.
