---
okftype: reference.api
description: "Deprecated atomic operation disable interface. Closes the atomic state after completing the atomic data movement configured by asc_set_atomic_add, asc_set_atomic_max, and asc_set_atomic_min. The replacement is asc_disable_dma_atomic."
tag:
  - roles: [data_movement_configuration]
  - traits: [atomic]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/atomic/datamove_atomic/asc_disable_dma_atomic.md
    kind: replaced_by
  - path: docs/zh/api/SIMD-API/c_api/atomic/datamove_atomic/asc_set_atomic_add.md
    kind: related
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The interface is deprecated. Use asc_disable_dma_atomic, which has clearer semantics. The declaration starting from include/c_api/atomic/datamove_atomic.h:184 carries ASC_DEPRECATED(9.2.0, asc_disable_dma_atomic), and the CANN_9_2_0 deprecation table has the same source."
---
# asc_set_atomic_none (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T14:46:17.185Z pushedAt=2026-11-18T03:12:16.654Z nl=2 -->

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

**This API is deprecated. Use [asc_disable_dma_atomic](../atomic/datamove_atomic/asc_disable_dma_atomic.md) instead.**

Used together with [asc_set_atomic_add](../atomic/datamove_atomic/asc_set_atomic_add.md), [asc_set_atomic_max](../atomic/datamove_atomic/asc_set_atomic_max.md), and [asc_set_atomic_min](../atomic/datamove_atomic/asc_set_atomic_min.md) to disable the atomic operation after the atomic operation is complete, preventing it from affecting subsequent data movement.

## Prototype

```c
[[deprecated("NOTICE: asc_set_atomic_none is deprecated. Please use asc_disable_dma_atomic instead.")]]
__aicore__ inline void asc_set_atomic_none()
```

## Parameters

None

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

None

## Examples

See [the call example of asc_disable_dma_atomic](../atomic/datamove_atomic/asc_disable_dma_atomic.md#examples).
