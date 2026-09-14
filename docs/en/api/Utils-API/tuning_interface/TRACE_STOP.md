# TRACE\_STOP

<!-- md-trans-meta sourceCommit=482b56785b936fd20f7354a81c8239038750acee translatedAt=2026-09-06T09:16:52.184Z pushedAt=2026-09-11T01:51:09.919Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: supported
- Atlas A3 training products/Atlas A3 inference products: supported
- Atlas A2 training products/Atlas A2 inference products: supported
- Atlas 200I/500 A2 inference products: supported
- Atlas inference products AI Core: supported
- Atlas inference products Vector Core: not supported
- Atlas training products: supported

## Description

In SIMD scenarios, when performing operator performance simulation through CAModel, you can log any execution stage of the operator to analyze the pipeline diagram of different instructions for further performance tuning.

It is used to indicate the logging point at the end position, and is generally used together with [TRACE\_START](TRACE_START.md).

> [!CAUTION]
> This function is mainly used for **debugging and performance analysis**. After it is enabled, it will have a certain impact on operator performance. It is usually used in the debugging phase, and **it is recommended to disable it in the production environment**.
> By default, this function is disabled. Developers can enable the logging function as needed in the following way.
> Modify the npu\_lib.cmake file in the cmake directory of the Kernel direct-call project, and add the -DASCENDC\_TRACE\_ON compilation option to the ascendc\_compile\_definitions command to enable the logging function. The example is as follows:
> ```
> // Enable the logging function of the operator.
> ascendc_compile_definitions(ascendc_kernels_${RUN_MODE} PRIVATE
>     -DASCENDC_TRACE_ON
> )
> ```

## Prototype

```
#define TRACE_STOP(TraceId apid)
#define TRACE_STOP(pipe_t pipe, TraceId apid)
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| apid | Input | The value must be consistent with the value of the **apid** parameter in [TRACE_START](TRACE_START.md); otherwise, the logging result is affected. |
| pipe | Input | The value must be consistent with the value of the **pipe** parameter in [TRACE_START](TRACE_START.md). For the value of **pipe**, see [Hardware Pipeline Type](../../SIMD-API/basic_api/sync_control/intra_core_sync/intra_core_synchronization_capability_overview.md#zh-cn_topic_0000002542725361_section1272612276459); otherwise, the logging result is affected. This parameter takes effect only on Ascend 950PR/Ascend 950DT and does not take effect on other products. |

## Return Value

None

## Constraints

-   TRACE\_START/TRACE\_STOP must be used in pairs. If no logging point is displayed on the Trace graph, the two are not paired.
-   Cross-core usage is not supported. For example, if TRACE\_START places a logging  point on the AI Cube, the TRACE\_STOP logging point must also be placed on the AI Cube, not on the AI Vector.
-   The macro supports all product models, but the model used in actual calls must be consistent with the models supported by the debugging tool.
-   Only Kernel direct-call projects are supported. Enabling the logging point feature is not supported in custom operator projects.

## Example

Place TRACE\_START/TRACE\_STOP at specific instruction positions in the Kernel code:

```
TRACE_START(0x1);
DataCopy(zGm, zLocal, this->totalLength);
TRACE_STOP(0x1);
```
