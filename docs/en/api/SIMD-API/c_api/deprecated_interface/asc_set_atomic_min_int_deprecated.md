---
okftype: reference.api
description: "Deprecated atomic compare-and-minimum configuration interface that enables the int32_t atomic minimum operation for subsequent data movement, performing an element-wise minimum between the data to be transferred and the existing GM data. The replacement is asc_set_atomic_min_int32."
tag:
  - roles: [data_movement_configuration]
  - operations: [minimum]
  - traits: [atomic]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/atomic/datamove_atomic/asc_disable_dma_atomic.md
    kind: related
  - path: docs/zh/api/SIMD-API/c_api/atomic/datamove_atomic/asc_set_atomic_min.md
    kind: replaced_by
version: ">=9.0.0"
version_changes:
  - since: "9.1.0"
    kind: arch_support_extended
    scope: "page"
    scope_basis: kind_default
    note: "New 3510 (Ascend 950PR&950DT) implementation: history_probe shows that asc_set_atomic_min_int in npu_arch_3510/atomic_impl.h has appeared since 9.1.0; in 9.0.0 it was only available on 2201 (Atlas A2/A3)."
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The interface is deprecated. Use asc_set_atomic_min_int32, which carries an explicit data type suffix, instead; the declaration in include/c_api/atomic/datamove_atomic.h carries ASC_DEPRECATED(9.2.0, asc_set_atomic_min_int32), and the CANN_9_2_0 deprecation table has the same source."
---
# asc_set_atomic_min_int (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T14:46:12.303Z pushedAt=2026-11-18T03:12:16.655Z nl=2 -->

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

**This API is deprecated. Use [asc_set_atomic_min_int32](../atomic/datamove_atomic/asc_set_atomic_min.md) instead.**

Sets the computation result to be transferred to GM in an atomic comparison manner. Before copying, the int32_t data to be transferred is compared element-wise with the existing data in GM, and the minimum value is written to GM.

## Prototype

```c
[[deprecated("NOTICE: asc_set_atomic_min_int is deprecated. Please use asc_set_atomic_min_int32 instead.")]]
__aicore__ inline void asc_set_atomic_min_int()
```

## Parameters

None

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

After use, call [asc_disable_dma_atomic](../atomic/datamove_atomic/asc_disable_dma_atomic.md) to disable the atomic minimum operation, so as not to affect the subsequent related instructions.

## Examples

Refer to the calling example of [asc_set_atomic_min](../atomic/datamove_atomic/asc_set_atomic_min.md).
