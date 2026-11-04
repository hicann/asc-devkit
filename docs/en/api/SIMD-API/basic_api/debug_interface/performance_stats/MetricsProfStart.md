# MetricsProfStart<a name="ZH-CN_TOPIC_0000002080803573"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T15:17:10.250Z pushedAt=2026-10-26T07:41:44.392Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/debug_interface/performance_stats/MetricsProfStart_res.md#id1 -->

## Description<a name="zh-cn_topic_0000001963799138_zh-cn_topic_0000001997078721_section259105813316"></a>

The header file path is `"basic_api/kernel_prof_trace_intf.h"`.

It is recommended to use the [asc_prof_start](../../../../Utils-API/tuning_interface/asc_prof_start.md) API to set the start signal for performance data collection. This API applies to both C and C++ programming.

This API is used to set the start signal for performance data collection and is used together with [MetricsProfStop](MetricsProfStop.md). When using the msOpProf tool for operator on-board tuning, you can call **MetricsProfStart** and **MetricsProfStop** before and after the kernel-side code segment to specify the range of the code segment to be tuned.

## Prototype<a name="zh-cn_topic_0000001963799138_zh-cn_topic_0000001997078721_section2067518173415"></a>

```cpp
__aicore__ inline void MetricsProfStart()
```

## Parameters<a name="zh-cn_topic_0000001963799138_zh-cn_topic_0000001997078721_section158061867342"></a>

None

## Return Value<a name="zh-cn_topic_0000001963799138_zh-cn_topic_0000001997078721_section640mcpsimp"></a>

None

## Constraints<a name="zh-cn_topic_0000001963799138_zh-cn_topic_0000001997078721_section43265506459"></a>

None

## Example<a name="zh-cn_topic_0000001963799138_zh-cn_topic_0000001997078721_section82241477610"></a>

```cpp
MetricsProfStart();
... // Kernel-side code segment to be tuned.
MetricsProfStop();
```
