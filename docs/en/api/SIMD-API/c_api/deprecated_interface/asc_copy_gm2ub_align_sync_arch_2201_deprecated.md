---
okftype: reference.api
description: "Deprecated API page: asc_copy_gm2ub_align_sync under the 2201 architecture performs synchronous unaligned copy from GM to UB using dst, src, and byte count. The non-32-byte-aligned portion is padded with a manually filled value or a hardware dummy value. It is officially recommended to use the asynchronous asc_copy_gm2ub_align plus asc_sync instead."
tag:
  - roles: [data_movement]
  - operations: [copy]
  - traits: [padding, unaligned_transfer]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync_notify.md
    kind: requires
    condition: "When UB addresses overlap with other instructions, synchronization instructions must be inserted for serialization"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync_wait.md
    kind: requires
    condition: "When UB addresses overlap with other instructions, synchronization instructions must be inserted for serialization"
  - path: docs/zh/api/SIMD-API/c_api/vector_datamove/asc_copy_gm2ub_align/asc_copy_gm2ub_align_arch_2201.md
    kind: replaced_by
    condition: "It is recommended to use the corresponding non-synchronous interface and explicitly call asc_sync"
  - path: docs/zh/api/SIMD-API/c_api/vector_datamove/asc_set_copy_pad_val.md
    kind: requires
    condition: "When using manual padding, the padding value must be configured first"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The include header marks this synchronous interface with ASC_DEPRECATED(9.2.0). The replacement path is the non-synchronous asc_copy_gm2ub_align plus asc_sync; the interface has not been removed."
---
# asc_copy_gm2ub_align_sync (NPU Architecture Version 2201, Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T11:46:05.602Z pushedAt=2026-11-18T03:12:16.402Z nl=2 -->

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

**The `asc_copy_gm2ub_align_sync` interface is deprecated. Use the [corresponding non-synchronous interface](../vector_datamove/asc_copy_gm2ub_align/asc_copy_gm2ub_align_arch_2201.md) together with the synchronization interface [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Provides non-aligned data movement, moving data from Global Memory (GM) to Unified Buffer (UB), and supports 8-bit/16-bit/32-bit data type movement.

This interface supports the following two data movement modes:

- First n data movement

    If the length of the moved data is not 32-byte aligned, the moved data is padded to 32-byte alignment. The following two padding modes are supported:

    - Manual padding: Call [asc_set_copy_pad_val](../vector_datamove/asc_set_copy_pad_val.md) to configure the padding value before the movement.
    - Automatic padding: When `asc_set_copy_pad_val` is not called, the hardware automatically pads dummy data, and the value of the dummy data is the value of the first element of the data block.

- High-dimensional split movement

    High-dimensional split movement uses Normal mode. If the length of a single data block specified by `burst_len` is not 32-byte aligned, each data block specified by `burst_count` is padded to 32-byte alignment separately. The following two padding modes are supported for non-32-byte-aligned scenarios:

    - Manual padding: call `asc_set_copy_pad_val` to set the padding value before the copy.
    - Automatic padding: when `asc_set_copy_pad_val` is not called, the hardware automatically pads dummy data, and the value of the dummy data is the value of the first element of the data block.

## Prototype

- Synchronous computation

    ```cpp
    __aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ int8_t* dst, __gm__ int8_t* src, uint32_t size)
    __aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ uint8_t* dst, __gm__ uint8_t* src, uint32_t size)
    __aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ int16_t* dst, __gm__ int16_t* src, uint32_t size)
    __aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ uint16_t* dst, __gm__ uint16_t* src, uint32_t size)
    __aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ half* dst, __gm__ half* src, uint32_t size)
    __aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint32_t size)
    __aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ int32_t* dst, __gm__ int32_t* src, uint32_t size)
    __aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ uint32_t* dst, __gm__ uint32_t* src, uint32_t size)
    __aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ float* dst, __gm__ float* src, uint32_t size)
    ```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| dst | Output | Start address of the destination UB. 32-byte alignment is required. |
| src | Input | Start address of the source GM. 1-byte alignment is required. |
| size | Input | Size of the data to be copied, in bytes. Value range: [0, 2097151]. |

## Return Value

None

## Pipeline Type

PIPE_MTE2

## Constraints

- For the space size and alignment requirements of each storage unit, see [Storage Unit Description](../general_description_and_constraints.md#storage-unit-description).
- When either `burst_count` or `burst_len` is 0, this interface is treated as a NOP (no operation).
- When the value of `size` is 0, this interface is treated as a NOP (no operation).
- If this instruction overlaps with other instructions in the UB address, you must insert the synchronization instructions [asc_sync_notify](../sync/intra_core_sync/asc_sync_notify.md) and [asc_sync_wait](../sync/intra_core_sync/asc_sync_wait.md) to serialize multiple instructions and prevent abnormal data.
- Synchronous computation includes synchronous wait.
- The padding data size corresponding to `left_padding_num` and `right_padding_num` must not exceed 32 bytes.

## Examples

```cpp
// total_length refers to the total length of data involved in the copy. src is the GM memory input from outside.
constexpr uint64_t total_length = 128;
__ubuf__ uint8_t dst[total_length];
asc_copy_gm2ub_align_sync(dst, src, total_length * sizeof(uint8_t));
```
