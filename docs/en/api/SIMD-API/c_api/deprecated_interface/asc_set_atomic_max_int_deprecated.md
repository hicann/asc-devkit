---
okftype: reference.api
description: "Deprecated atomic compare-and-max configuration interface. Enables the int32_t atomic maximum operation for subsequent data movement, performing element-wise maximum between the data to be transferred and the existing data in GM. The replacement is asc_set_atomic_max_int32."
tag:
  - roles: [data_movement_configuration]
  - operations: [maximum]
  - traits: [atomic]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/atomic/datamove_atomic/asc_disable_dma_atomic.md
    kind: related
  - path: docs/zh/api/SIMD-API/c_api/atomic/datamove_atomic/asc_set_atomic_max.md
    kind: replaced_by
version: ">=9.0.0"
version_changes:
  - since: "9.1.0"
    kind: arch_support_extended
    scope: "page"
    scope_basis: kind_default
    note: "Added 3510 (Ascend 950PR&950DT) implementation: history_probe shows that asc_set_atomic_max_int in npu_arch_3510/atomic_impl.h has appeared since 9.1.0; in 9.0.0 only 2201 (Atlas A2/A3) was available."
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The API is deprecated. Use asc_set_atomic_max_int32 with an explicit data type suffix instead; the declaration in include/c_api/atomic/datamove_atomic.h carries ASC_DEPRECATED(9.2.0, asc_set_atomic_max_int32), and the CANN_9_2_0 deprecation table has the same source."
---
# asc_set_atomic_max_int (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T14:44:52.075Z pushedAt=2026-11-18T03:12:16.653Z nl=2 -->

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

The header file path is: `"c_api/atomic/datamove_atomic.h"`.

**This API is deprecated. Use [asc_set_atomic_max_int32](../atomic/datamove_atomic/asc_set_atomic_max.md) instead.**

Sets the computation result to be transferred to GM in an atomic compare manner. Before copying, compares the int32_t data to be transferred with the existing data in GM element by element, and writes the maximum value to GM.

## Prototype

```c
[[deprecated("NOTICE: asc_set_atomic_max_int is deprecated. Please use asc_set_atomic_max_int32 instead.")]]
__aicore__ inline void asc_set_atomic_max_int()
```

## Parameters

None

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

After use, call [asc_disable_dma_atomic](../atomic/datamove_atomic/asc_disable_dma_atomic.md) to disable the atomic maximum operation, so as not to affect the subsequent related instructions.

## Examples

Refer to the calling example of [asc_set_atomic_max](../atomic/datamove_atomic/asc_set_atomic_max.md).
