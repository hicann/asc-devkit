# MetricsProfStop<a name="ZH-CN_TOPIC_0000002044684574"></a>

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->

## 功能说明<a name="zh-cn_topic_0000002000280001_zh-cn_topic_0000001960477980_section259105813316"></a>

头文件路径为：`"basic_api/kernel_prof_trace_intf.h"`。

推荐使用[asc_prof_stop](../../../../Utils-API/调测接口/asc_prof_stop.md)接口进行性能数据采集信号启动的设置，该接口同时适用于C语言和C++语言编程。

设置性能数据采集信号停止，和[MetricsProfStart](MetricsProfStart.md)配合使用。使用msOpProf工具进行算子上板调优时，可在kernel侧代码段前后分别调用MetricsProfStart和MetricsProfStop来指定需要调优的代码段范围。

## 函数原型<a name="zh-cn_topic_0000002000280001_zh-cn_topic_0000001960477980_section2067518173415"></a>

```cpp
__aicore__ inline void MetricsProfStop()
```

## 参数说明<a name="zh-cn_topic_0000002000280001_zh-cn_topic_0000001960477980_section158061867342"></a>

无

## 返回值说明<a name="zh-cn_topic_0000002000280001_zh-cn_topic_0000001960477980_section640mcpsimp"></a>

无

## 约束说明<a name="zh-cn_topic_0000002000280001_zh-cn_topic_0000001960477980_section43265506459"></a>

无

## 调用示例<a name="zh-cn_topic_0000002000280001_zh-cn_topic_0000001960477980_section82241477610"></a>

```cpp
MetricsProfStart();
... // 需要调优的kernel侧代码段。
MetricsProfStop();
```
