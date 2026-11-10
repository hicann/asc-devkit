---
okftype: reference.enum
description: "互斥锁阻塞或非阻塞执行模式的枚举，作为asc_lock与asc_unlock的mode参数类型，ASC_LOCK_BLOCK为默认阻塞模式，兼容类型名ascMutexExecuteMode是该类型的别名。"
tag:
  - roles: [configuration]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_lock.md
    kind: related
    condition: "该接口以本枚举作为mode参数选择阻塞或非阻塞"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_unlock.md
    kind: related
    condition: "该接口使用同一mode类型"
version: ">=9.2.0"
version_changes: []
---
# asc_mutex_execute_mode

`asc_mutex_execute_mode`用于选择互斥锁的阻塞或非阻塞执行模式，作为[asc_lock](../../sync/intra_core_sync/asc_lock.md)和[asc_unlock](../../sync/intra_core_sync/asc_unlock.md)接口的`mode`参数类型。兼容类型名`ascMutexExecuteMode`是该类型的别名。

头文件路径为：`"c_api/defs/enum.h"`。

## 枚举具体定义

```cpp
enum asc_mutex_execute_mode {
    ASC_LOCK_BLOCK = 0,
    ASC_LOCK_NON_BLOCK = 1
};

using ascMutexExecuteMode = asc_mutex_execute_mode;
```

## 枚举值详解

**表1** 枚举值说明

| 枚举值 | 取值 | 说明 |
| --- | --- | --- |
| ASC_LOCK_BLOCK | 0 | 阻塞`pipe`指定流水的执行，为默认模式。 |
| ASC_LOCK_NON_BLOCK | 1 | 不阻塞流水的执行。 |
