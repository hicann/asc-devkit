# TRACE\_STOP

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-23T07:53:43.743Z pushedAt=2026-10-26T11:12:09.568Z nl=2 -->

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
- Atlas 200I/500 A2 inference products: Supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Supported
- Atlas inference products Vector Core: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/Utils-API/tuning_interface/TRACE_STOP_res.md#id1 -->

## Description

In SIMD scenarios, when you use the simulator to perform operator performance simulation, you can insert trace points at any execution stage of the operator to analyze the pipeline diagram of different instructions for further performance tuning.

It is used to mark the end position for tracing and is generally used together with [TRACE\_START](TRACE_START.md).

> [!CAUTION]Note
>This feature is mainly used for **debugging and performance analysis**. Enabling it affects operator performance to a certain extent. It is usually used in the debugging phase, and **it is recommended that you disable it in the production environment**.
>By default, this feature is disabled. You can enable tracing as needed in the following way.
>Modify the npu\_lib.cmake file in the cmake directory of the kernel direct-call project, and add the -DASCENDC\_TRACE\_ON compilation option to the ascendc\_compile\_definitions command to enable tracing. The following is an example:
>```
>// Enable tracing for the operator.
>ascendc_compile_definitions(ascendc_kernels_${RUN_MODE} PRIVATE
>    -DASCENDC_TRACE_ON
>)
>```

## Prototype

```
#define TRACE_STOP(TraceId apid)
#define TRACE_STOP(pipe_t pipe, TraceId apid)
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| apid | Input | The value must be consistent with that of the [TRACE_START](TRACE_START.md) parameter; otherwise, the tracing result is affected. |
| pipe | Input | The value must be consistent with that of the pipe parameter of [TRACE_START](TRACE_START.md). For the value of pipe, see [Hardware Pipeline Type](../../SIMD-API/basic_api/sync_control/intra_core_sync/intra_core_sync_overview.md#zh-cn_topic_0000002542725361_section1272612276459); otherwise, the tracing result is affected.<!-- npu="950" id7 --><br>This parameter takes effect only on Ascend 950PR&950DT products and does not take effect on other products.<!-- end id7 --> |

## Return Value

None

## Constraints

-   TRACE\_START/TRACE\_STOP must be used together. If no trace point is displayed on the Trace diagram, the two are not paired.
-   Cross-core use is not supported. For example, if TRACE\_START inserts a trace point on the AI Cube, the trace point of TRACE\_STOP must also be on the AI Cube, not on the AI Vector.
-   The macro supports all product models, but the model used in actual calls must be consistent with the models supported by the debugging tool.
-   Tracing is supported only in kernel direct-call projects, not in custom operator projects.

## Examples

Insert TRACE\_START/TRACE\_STOP at the position of a specific instruction in the kernel code:

```
TRACE_START(0x1);
DataCopy(zGm, zLocal, this->totalLength);
TRACE_STOP(0x1);
```
