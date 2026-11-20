---
okftype: reference.api
description: "Deprecated API page: the synchronous API asc_copy_gm2ub_align_sync on the 3510 architecture copies GM data to UB unaligned by byte count. The non-32-byte-aligned portion is padded with a manually filled value or a hardware dummy value. It supports 8-bit, 16-bit, 32-bit, hifloat8, and fp8 types. It is recommended to use the non-synchronous API plus asc_sync instead."
tag:
  - roles: [data_movement]
  - operations: [copy]
  - traits: [padding, unaligned_transfer]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "The space size and alignment requirements of each storage unit follow its storage unit description"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: related
    condition: "Handles the synchronization wait in the alternative solution"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync_notify.md
    kind: requires
    condition: "When the UB address overlaps with other instructions, a synchronization instruction must be inserted for serialization"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync_wait.md
    kind: requires
    condition: "When the UB address overlaps with other instructions, a synchronization instruction must be inserted for serialization"
  - path: docs/zh/api/SIMD-API/c_api/vector_datamove/asc_copy_gm2ub_align/asc_copy_gm2ub_align_arch_3510.md
    kind: replaced_by
    condition: "It is recommended to use the contiguous data copy form and explicitly call asc_sync"
  - path: docs/zh/api/SIMD-API/c_api/vector_datamove/asc_set_copy_pad_val.md
    kind: requires
    condition: "When manual padding is used, the padding value must be configured first"
version: ">=9.1.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The include header marks this synchronization interface with ASC_DEPRECATED(9.2.0). The replacement path is the contiguous data copy of asc_copy_gm2ub_align on 3510 plus asc_sync. The interface has not been removed."
---
# asc_copy_gm2ub_align_sync (NPU Architecture Version 3510, Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T11:47:30.108Z pushedAt=2026-11-18T03:12:16.403Z nl=2 -->

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

The header file path is `"c_api/composite/vector_compute_composite.h"`.

**The `asc_copy_gm2ub_align_sync` interface is deprecated. Use [asc_copy_gm2ub_align contiguous data copy](../vector_datamove/asc_copy_gm2ub_align/asc_copy_gm2ub_align_arch_3510.md#contiguous-data-movement) and the synchronization interface [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Provides non-aligned data copy functionality, copying data from Global Memory (GM) to Unified Buffer (UB), and supports 8-bit/16-bit/32-bit data type copy.

This interface is a synchronous copy interface. If the data copy length is not 32-byte aligned, the copied data is padded to 32-byte alignment. The following two padding modes are supported:

- Manual padding: Call [asc_set_copy_pad_val](../vector_datamove/asc_set_copy_pad_val.md) to configure the padding value before the copy.
- Automatic padding: The hardware automatically pads dummy data, whose value is the value of the first element of the data block.

## Prototype

```cpp
__aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ int8_t* dst, __gm__ int8_t* src, uint32_t size)
__aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ uint8_t* dst, __gm__ uint8_t* src, uint32_t size)
__aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ hifloat8_t* dst, __gm__ hifloat8_t* src, uint32_t size)
__aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ fp8_e5m2_t* dst, __gm__ fp8_e5m2_t* src, uint32_t size)
__aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ fp8_e4m3fn_t* dst, __gm__ fp8_e4m3fn_t* src, uint32_t size)
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
| dst | Output | Start address of the destination UB. Requires 32-byte alignment. |
| src | Input | Start address of the source GM. Requires 1-byte alignment. |
| size | Input | Size of the data copy, in bytes. Value range: [0, 2097151]. |

## Return Value

None

## Pipeline Type

PIPE_MTE2

## Constraints

- For the space size and alignment requirements of each storage unit, see [Storage Unit Description](../general_description_and_constraints.md#storage-unit-description).
- When the value of `size` is 0, this interface is treated as a NOP (no operation).
- If this instruction has UB address overlap with other instructions, you must insert the synchronization instructions [asc_sync_notify](../sync/intra_core_sync/asc_sync_notify.md) and [asc_sync_wait](../sync/intra_core_sync/asc_sync_wait.md) to serialize multiple instructions and prevent abnormal data.
- Synchronous computation includes synchronous waiting.
- If `size` is not 32-byte aligned, the data copy is padded to 32-byte alignment, and the destination UB must reserve space for the padded size. For manual padding, call `asc_set_copy_pad_val` to configure the padding value. For automatic padding, the hardware fills dummy data, and the value of the dummy data is the value of the first element of the data block.
