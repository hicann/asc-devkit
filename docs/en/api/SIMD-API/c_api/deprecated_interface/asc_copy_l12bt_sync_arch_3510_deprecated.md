---
okftype: reference.api
description: "Deprecated API page: Under the 3510 architecture, asc_copy_l12bt_sync synchronously moves the MMAD Bias data from the L1 Buffer to the BiasTable Buffer. It supports half, bfloat16_t, int32_t, and float. The first n data transfers require 32-byte alignment. It is recommended to use the non-synchronous API plus asc_sync instead."
tag:
  - roles: [data_movement]
  - operations: [copy]
  - traits: [aligned_transfer]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_load/asc_copy_l12bt_arch_3510.md
    kind: replaced_by
    condition: "It is recommended to use the corresponding non-synchronous API and explicitly call asc_sync"
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "Space, alignment, and address overlap constraints follow the storage unit description and the general address overlap constraints"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: related
    condition: "The alternative solution handles the synchronous wait"
version: ">=9.1.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The include header marks this 3510 synchronous form with ASC_DEPRECATED(9.2.0). The alternative path is the non-synchronous asc_copy_l12bt plus asc_sync; the interface has not been removed."
---
# asc_copy_l12bt_sync (NPU Architecture Version 3510, Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T12:02:33.446Z pushedAt=2026-11-18T03:12:16.418Z nl=2 -->

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

The header file path is: `"c_api/composite/cube_datamove_composite.h"`.

**The `asc_copy_l12bt_sync` interface is deprecated. Use the [corresponding non-synchronous interface](../cube_datamove/cube_compute_load/asc_copy_l12bt_arch_3510.md) and the synchronous interface [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Moves the Bias data of the MMAD instruction from the L1 Buffer to the BiasTable Buffer.

## Prototype

```c
// Placeholder form
__aicore__ inline void asc_copy_l12bt_sync(uint64_t dst,
                                           __cbuf__ <dtype>* src,
                                           uint32_t size)
```

### Data Types Supported by dtype

The value of `dtype` can be: `half`, `bfloat16_t`, `int32_t`, `float`.

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| dst | Output | Start address of the destination operand. |
| src | Input | Start address of the source operand. |
| size| Input | Size of the data to move (in bytes).|

## Return Value

None

## Pipeline Type

PIPE_MTE1

## Constraints

- For the space size and alignment requirements of each storage unit, see [Storage Unit Description](../general_description_and_constraints.md#storage-unit-description).
- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).
- When using the first n data move interfaces, the size of the data to move must be 32-byte aligned.

## Examples

```cpp
constexpr uint32_t size = 512;
__cbuf__ half src[256];
__biasbuf__ half dst[256];
uint64_t dst_addr = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dst)) / 16;
asc_copy_l12bt_sync(dst_addr, src, size);
```
