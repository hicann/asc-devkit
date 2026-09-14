# TRACE\_START

<!-- md-trans-meta sourceCommit=482b56785b936fd20f7354a81c8239038750acee translatedAt=2026-09-06T09:15:03.760Z pushedAt=2026-09-11T01:47:00.923Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: supported
- Atlas A3 training products/Atlas A3 inference products: supported
- Atlas A2 training products/Atlas A2 inference products: supported
- Atlas 200I/500 A2 inference products: supported
- Atlas inference products AI Core: supported
- Atlas inference products Vector Core: not supported
- Atlas training products: supported

## Description

In SIMD scenarios, when performing operator performance simulation through CAModel, you can insert logging points at any execution stage of the operator to analyze the instruction pipeline graphs of different instructions for further performance tuning.

Used to mark the logging point at the start position and is generally used together with [TRACE\_STOP](TRACE_STOP.md).

> [!CAUTION]
> This function is mainly used for **debugging and performance analysis**. Enabling it will have a certain impact on operator performance. It is usually used in the debugging phase, and **it is recommended to disable it in the production environment**.
> By default, this function is disabled. Developers can enable the logging function as needed in the following way.
> Modify the npu\_lib.cmake file in the cmake directory of the Kernel launch project, and add the -DASCENDC\_TRACE\_ON compilation option to the ascendc\_compile\_definitions command to enable the logging function. The example is as follows:
> ```
> // Enable the logging function of the operator.
> ascendc_compile_definitions(ascendc_kernels_${RUN_MODE} PRIVATE
>     -DASCENDC_TRACE_ON
> )
> ```

## Prototype

```
#define TRACE_START(TraceId apid)
#define TRACE_START(pipe_t pipe, TraceId apid)
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| apid | Input | Ten user-defined types are currently reserved:<br>0x0: USER_DEFINE_0<br>0x1: USER_DEFINE_1<br>0x2: USER_DEFINE_2<br>0x3: USER_DEFINE_3<br>0x4: USER_DEFINE_4<br>0x5: USER_DEFINE_5<br>0x6: USER_DEFINE_6<br>0x7: USER_DEFINE_7<br>0x8: USER_DEFINE_8<br>0x9: USER_DEFINE_9 |
| pipe | Input | Pipeline type where the logging point is located. For values, see [Hardware Pipeline Type](../../SIMD-API/basic_api/sync_control/intra_core_sync/intra_core_synchronization_capability_overview.md#zh-cn_topic_0000002542725361_section1272612276459). This parameter takes effect only on Ascend 950PR/Ascend 950DT and does not take effect on other products. |

## Return Value

None

## Constraints

-   TRACE\_START/TRACE\_STOP must be used together. If no logging point is displayed on the Trace graph, the two are not paired.
-   Cross-core usage is not supported. For example, if TRACE\_START places a logging point on the AI Cube, TRACE\_STOP must also place its logging point on the AI Cube, not on the AI Vector.
-   The macro supports all product models, but the model used in actual invocation must be consistent with the model supported by the debugging tool.
-   Only the Kernel launch project is supported. Enabling the logging feature in a custom operator project is not supported.

## Example

Insert TRACE\_START/TRACE\_STOP at specific instruction positions in the Kernel code:

```
TRACE_START(0x2);
Add(zLocal, xLocal, yLocal, dataSize);
TRACE_STOP(0x2);
```
