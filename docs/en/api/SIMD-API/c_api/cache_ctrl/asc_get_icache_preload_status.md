---
okftype: reference.api
description: "Debug status query interface. Call it after asc_icache_preload to return the ICache preload status: 0 indicates the previous preload has completed, and 1 indicates the preload is still in progress. It is used for debugging and performance analysis to determine whether instruction preload is ready."
tag:
  - roles: [query]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cache_ctrl/asc_icache_preload.md
    kind: requires
    condition: "It must be called after this interface so that the return value represents the status of that preload."
version: ">=9.0.0"
version_changes:
  - since: "9.1.0"
    kind: behavior_fixed
    scope: "page"
    scope_basis: kind_default
    note: "Since 9.0.0, cache_ctrl.h reuses the protection macro of the atomic operation header file. When the include order is incorrect, cache control declarations such as asc_get_icache_preload_status are skipped. The CANN 9.1.0 Release Notes record that this issue has been fixed, and the fix takes effect from 9.1.0."
---
# asc_get_icache_preload_status

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T09:49:45.391Z pushedAt=2026-11-18T08:15:34.194Z nl=2 -->

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

The header file path is: `"c_api/cache_ctrl/cache_ctrl.h"`.

This interface is a debug interface. It is called after [asc_icache_preload](./asc_icache_preload.md) to obtain the PreLoad status of the ICache:

- When the return value is 0, it indicates that the ICache PreLoad has completed.
- When the return value is 1, it indicates that the ICache PreLoad has not completed.

## Prototype

```c
__aicore__ inline int64_t asc_get_icache_preload_status()
```

## Parameters

None

## Return Value

Returns the ICache preload status value of the `int64_t` type. Only the valid bits take the following values:

- Returns `0`: The ICache preload is idle, and the previous preload has completed.
- Returns `1`: The ICache preload is in progress and has not yet completed.

## Pipeline Type

PIPE_S

## Constraints

This interface is used only for status query in debugging and performance analysis scenarios, and does not affect the execution flow of the preload itself.

## Examples

This API must be used after calling [asc_icache_preload](./asc_icache_preload.md). For a complete call example, see [examples of asc_icache_preload](./asc_icache_preload.md#examples).
