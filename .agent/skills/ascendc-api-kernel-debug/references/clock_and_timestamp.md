# clock 和 asc_time_stamp

## `clock`

```cpp
#include "utils/debug/asc_time.h"

uint64_t start = clock();
RunTheCode();
uint64_t end = clock();
uint64_t cycles = end - start;
```

`clock()` 返回从程序开始到调用时刻经历的时钟周期数，返回类型为 `uint64_t`。它适合测量同一线程中一段局部代码的相对周期差；不要把不同线程或不同核的值直接当作一个全局同步时间线。

测量时：

- 只让一个代表线程打印结果，或将结果写回 GM 后由 Host 汇总。
- 在测量区域外放置 `printf`/`asc_dump`，否则调试输出本身会污染结果。
- 说明冷启动、编译优化、同步和线程调度对结果的影响。
- 需要跨阶段结构化记录时，使用 `asc_time_stamp`，不要自行把 `clock` 输出伪装成 timestamp dump。

## `asc_time_stamp`

```cpp
#include "utils/debug/asc_time.h"

asc_time_stamp(0x10000);
```

`asc_time_stamp` 在 SIMD Kernel 中按用户描述符输出结构化时间戳信息，包括当前 cycle、PC 和 Kernel 入口等字段。默认关闭，需要增加：

```text
-DASCENDC_TIME_STAMP_ON
```

用户自定义 ID 建议大于 `0xffff`，因为 `[0, 0xffff]` 预留给 AscendC 内部模块。该能力主要用于 NPU 上板调试，当前公开文档声明不支持算子入图场景；每核累计 dump 数据量也需要控制。

## “时间戳没有输出”排查

1. 检查调用是否位于 SIMD 支持的 Kernel 路径。
2. 确认重新编译时确实传入 `-DASCENDC_TIME_STAMP_ON`。
3. 确认不是 CPU 调试或算子入图场景。
4. 检查每核累计 dump 数据量和 `ASCENDC_DUMP` 设置。
5. 检查 Kernel 是否在执行到打点位置前就失败或提前返回。

## 与 profiling 的边界

本 skill 只覆盖 `clock` 和 `asc_time_stamp`。`asc_prof_start`、`asc_prof_stop`、`asc_mark_stamp`、`TRACE_START`、`TRACE_STOP` 不在本期范围，不要用本 reference 推导它们的签名或行为。
