# TRACE\_START

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-13T09:21:32.860Z pushedAt=2026-11-16T07:07:41.611Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT product: Supported
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
- Atlas inference product AI Core: Supported
- Atlas inference product Vector Core: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/Utils-API/tuning_interface/TRACE_START_res.md#id1 -->

## Description

In SIMD scenarios, when you use the simulator to perform operator performance simulation, you can insert trace points at any execution stage of the operator to analyze the pipeline diagram of different instructions for further performance tuning.

Used to start position logging, generally used together with [TRACE\_STOP](TRACE_STOP.md).

> [!CAUTION] Caution
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
#define TRACE_START(TraceId apid)
#define TRACE_START(pipe_t pipe, TraceId apid)
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| apid | Input | Ten user-defined types are currently reserved:<br>0x0: USER_DEFINE_0<br>0x1: USER_DEFINE_1<br>0x2: USER_DEFINE_2<br>0x3: USER_DEFINE_3<br>0x4: USER_DEFINE_4<br>0x5: USER_DEFINE_5<br>0x6: USER_DEFINE_6<br>0x7: USER_DEFINE_7<br>0x8: USER_DEFINE_8<br>0x9: USER_DEFINE_9 |
| pipe | Input | Specifies the pipeline type where the trace point is located. For values, see [Hardware Pipeline Type](../../SIMD-API/basic_api/sync_control/intra_core_sync/intra_core_sync_overview.md#zh-cn_topic_0000002542725361_section1272612276459).<!-- npu="950" id7 --><br>This takes effect only on Ascend 950PR&950DT products and does not take effect on other products.<!-- end id7 --> |

## Return Value

None

## Constraints

-   TRACE\_START/TRACE\_STOP must be used in conjunction. If no trace point is displayed on the Trace diagram, the two are not paired.
-   Cross-core use is not supported. For example, if TRACE\_START inserts a trace point on the AI Cube, the trace point of TRACE\_STOP must also be on the AI Cube, not on the AI Vector.
-   The macro supports all product models, but the model used in actual calls must be consistent with the models supported by the debugging tool.
-   Logging is supported only in kernel function direct call projects, not in custom operator projects.

## Examples

Insert TRACE\_START/TRACE\_STOP at the position of a specific instruction in the kernel code:

```
TRACE_START(0x2);
Add(zLocal, xLocal, yLocal, dataSize);
TRACE_STOP(0x2);
```
