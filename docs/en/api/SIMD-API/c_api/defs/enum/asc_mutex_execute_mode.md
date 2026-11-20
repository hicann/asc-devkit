---
okftype: reference.enum
description: "Enumeration of blocking or non-blocking execution modes for mutexes, used as the mode parameter type of asc_lock and asc_unlock. ASC_LOCK_BLOCK is the default blocking mode. The compatible type name ascMutexExecuteMode is an alias of this type."
tag:
  - roles: [configuration]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_lock.md
    kind: related
    condition: "This interface uses this enumeration as the mode parameter to select blocking or non-blocking"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_unlock.md
    kind: related
    condition: "This interface uses the same mode type"
version: ">=9.2.0"
version_changes: []
---
# asc_mutex_execute_mode

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T12:10:48.456Z pushedAt=2026-11-18T08:15:34.390Z nl=2 -->

`asc_mutex_execute_mode` is used to select the blocking or non-blocking execution mode of a mutex, and serves as the `mode` parameter type of the [asc_lock](../../sync/intra_core_sync/asc_lock.md) and [asc_unlock](../../sync/intra_core_sync/asc_unlock.md) interfaces. The compatible type name `ascMutexExecuteMode` is an alias of this type.

The header file path is: `"c_api/defs/enum.h"`.

## Specific Definition of the Enum

```cpp
enum asc_mutex_execute_mode {
    ASC_LOCK_BLOCK = 0,
    ASC_LOCK_NON_BLOCK = 1
};

using ascMutexExecuteMode = asc_mutex_execute_mode;
```

## Enum Value Details

**Table 1** Enum value description

| Enum Value | Value | Description |
| --- | --- | --- |
| ASC_LOCK_BLOCK | 0 | Blocks the execution of the `pipe` specified pipeline. This is the default mode. |
| ASC_LOCK_NON_BLOCK | 1 | Does not block the execution of the pipeline. |
