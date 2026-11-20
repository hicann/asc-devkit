---
okftype: reference.api
description: "Disables the atomic operation state after completing the data movement atomic operation, preventing the enabled atomic operation from affecting subsequent movements. The interface has no parameters and no return value, and its pipeline type is PIPE_S. It must be used together with asc_set_atomic_add, asc_set_atomic_max, and asc_set_atomic_min, and supports the Ascend 950PR&950DT, Atlas A3, and Atlas A2 products."
tag:
  - roles: [data_movement_configuration]
  - traits: [atomic]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/atomic/datamove_atomic/asc_set_atomic_add.md
    kind: related
  - path: docs/zh/api/SIMD-API/c_api/atomic/datamove_atomic/asc_set_atomic_max.md
    kind: related
  - path: docs/zh/api/SIMD-API/c_api/atomic/datamove_atomic/asc_set_atomic_min.md
    kind: related
version: ">=9.2.0"
version_changes: []
---
# asc_disable_dma_atomic

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T09:18:59.153Z pushedAt=2026-11-18T08:15:34.164Z nl=2 -->

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

Used together with [asc_set_atomic_add](asc_set_atomic_add.md), [asc_set_atomic_max](asc_set_atomic_max.md), and [asc_set_atomic_min](asc_set_atomic_min.md) to disable the atomic operation after it completes, preventing it from affecting subsequent data movement.

## Prototype

```c
__aicore__ inline void asc_disable_dma_atomic()
```

## Parameters

None

## Return Value

None

## Pipeline Type

`PIPE_S`

## Constraints

None

## Examples

**asc_disable_dma_atomic** is used together with atomic operation APIs such as [**asc_set_atomic_add**](asc_set_atomic_add.md). For an example, see [Examples of asc_set_atomic_add](asc_set_atomic_add.md#examples).
