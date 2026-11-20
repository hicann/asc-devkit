---
okftype: reference.api
description: "Deprecated atomic add configuration interface. Enables int32_t atomic add for subsequent data movement from UB, L0C, or L1 to Global Memory. The replacement is asc_set_atomic_add_int32, which has clear semantics and a data type suffix."
tag:
  - roles: [data_movement_configuration]
  - operations: [add]
  - traits: [atomic]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/atomic/datamove_atomic/asc_disable_dma_atomic.md
    kind: related
  - path: docs/zh/api/SIMD-API/c_api/atomic/datamove_atomic/asc_set_atomic_add.md
    kind: replaced_by
version: ">=9.0.0"
version_changes:
  - since: "9.1.0"
    kind: arch_support_extended
    scope: "page"
    scope_basis: kind_default
    note: "Added 3510 (Ascend 950PR&950DT) implementation: history_probe shows that asc_set_atomic_add_int in npu_arch_3510/atomic_impl.h has appeared since 9.1.0, and the CANN_9_1_0 enhancement section lists asc_set_atomic_add_int; in 9.0.0, only 2201 (Atlas A2/A3) was available."
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The interface is deprecated. Use asc_set_atomic_add_int32 with an explicit data type suffix instead; the declaration starting from include/c_api/atomic/datamove_atomic.h:194 carries ASC_DEPRECATED(9.2.0), and the CANN_9_2_0 deprecation table has the same source."
---
# asc_set_atomic_add_int (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T14:44:43.192Z pushedAt=2026-11-18T03:12:16.651Z nl=2 -->

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

**This API is deprecated. Use [asc_set_atomic_add_int32](../atomic/datamove_atomic/asc_set_atomic_add.md) instead.**

Enables atomic addition for subsequent data transfers from Unified Buffer (UB)/L0C Buffer/L1 Buffer to Global Memory. The data type for the addition is int32_t.

## Prototype

```c
[[deprecated("NOTICE: asc_set_atomic_add_int is deprecated. Please use asc_set_atomic_add_int32 instead.")]]
__aicore__ inline void asc_set_atomic_add_int()
```

## Parameters

None

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- After use, it is recommended to clear the atomic operation state (see [asc_disable_dma_atomic](../atomic/datamove_atomic/asc_disable_dma_atomic.md) for details) to avoid affecting the functionality of subsequent related instructions.
- This instruction does not clear the GM data before execution. You need to manually add a clearing operation when required.

## Examples

Refer to the calling example of [asc_set_atomic_add](../atomic/datamove_atomic/asc_set_atomic_add.md).
