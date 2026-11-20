---
okftype: reference.api
description: "Deprecated synchronous UB-to-GM unaligned transfer interface variant for 950PR&950DT, supporting data types including hifloat8_t and fp8 with 32-byte padding. The replacement is asc_copy_ub2gm_align plus asc_sync."
tag:
  - roles: [data_movement]
  - operations: [store]
  - traits: [unaligned_transfer]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "Storage unit space size and alignment requirements"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: replaced_by
    condition: "The synchronization part of the replacement"
  - path: docs/zh/api/SIMD-API/c_api/vector_datamove/asc_copy_ub2gm_align/asc_copy_ub2gm_align_arch_3510.md
    kind: replaced_by
    condition: "the replacement non-synchronous interface declared in the body"
version: ">=9.1.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "Starting from 9.2.0, the _sync declarations in the 3510 group in include are likewise marked ASC_DEPRECATED(9.2.0, \"2027/09/07\", asc_copy_ub2gm_align and asc_sync); the declarations are retained, and are deprecated rather than removed."
---
# asc_copy_ub2gm_align_sync (NPU Architecture Version 3510, Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T12:35:50.097Z pushedAt=2026-11-18T03:12:16.446Z nl=2 -->

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

**The `asc_copy_ub2gm_align_sync` interface is deprecated. Use [asc_copy_ub2gm_align contiguous data movement](../vector_datamove/asc_copy_ub2gm_align/asc_copy_ub2gm_align_arch_3510.md#contiguous-data-movement) and the synchronization interface [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Provides the unaligned data move function, moving data from Unified Buffer (UB) to Global Memory (GM), and supports 8-bit/16-bit/32-bit data type moves.

This interface is a synchronous move interface and does not support setting a fill value. If the moved data length is not 32-byte aligned, the hardware automatically pads dummy data when reading UB data to align the moved data to 32 bytes. The padded dummy data is discarded when writing to GM.

## Prototype

```cpp
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ int8_t* dst, __ubuf__ int8_t* src, uint32_t size)
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ uint8_t* dst, __ubuf__ uint8_t* src, uint32_t size)
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ hifloat8_t* dst, __ubuf__ hifloat8_t* src, uint32_t size)
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ fp8_e5m2_t* dst, __ubuf__ fp8_e5m2_t* src, uint32_t size)
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ fp8_e4m3fn_t* dst, __ubuf__ fp8_e4m3fn_t* src, uint32_t size)
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ int16_t* dst, __ubuf__ int16_t* src, uint32_t size)
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ uint16_t* dst, __ubuf__ uint16_t* src, uint32_t size)
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ half* dst, __ubuf__ half* src, uint32_t size)
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ bfloat16_t* dst, __ubuf__ bfloat16_t* src, uint32_t size)
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ int32_t* dst, __ubuf__ int32_t* src, uint32_t size)
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ uint32_t* dst, __ubuf__ uint32_t* src, uint32_t size)
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ float* dst, __ubuf__ float* src, uint32_t size)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| dst | Output | Start address of the destination GM. Requires 1-byte alignment. |
| src | Input | Start address of the source UB. Requires 32-byte alignment. |
| size | Input | Size of the moved data, in bytes. Value range: [0, 2097151]. |

## Return Value

None

## Pipeline Type

PIPE_MTE3

## Constraints

- For the space size and alignment requirements of each storage unit, see [storage unit description](../general_description_and_constraints.md#storage-unit-description).
- When the value of `size` is 0, this interface is treated as a NOP (no operation).
- If this instruction overlaps with other instructions in UB addresses, you must insert the synchronization instructions [asc_sync_notify](../sync/intra_core_sync/asc_sync_notify.md) and [asc_sync_wait](../sync/intra_core_sync/asc_sync_wait.md) to ensure that multiple instructions are serialized and prevent abnormal data.
- Synchronous computation includes synchronous waiting.
- `size` does not need to be 32-byte aligned. If `size` is not 32-byte aligned, the hardware automatically pads dummy data to 32-byte alignment. The value of the dummy data is the value of the first element of the data block. The padded dummy data is discarded when writing to GM.
