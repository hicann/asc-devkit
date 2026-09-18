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
