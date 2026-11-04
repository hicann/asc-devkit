# TRACE\_START

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-23T07:53:25.007Z pushedAt=2026-10-26T11:12:09.564Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/Utils-API/tuning_interface/TRACE_START_res.md#id1 -->

## Description

In SIMD scenarios, when simulating operator performance through the simulator, you can insert trace points at any execution stage of the operator to analyze the pipeline graph of different instructions for further performance tuning.

Used to mark the start position, generally used together with [TRACE\_STOP](TRACE_STOP.md).

> [!CAUTION] Note
> This function is mainly used for **debugging and performance analysis**. Enabling it affects operator performance to some extent. It is typically used during the debugging phase, and **it is recommended to disable it in production environments**.
> By default, this function is disabled. You can enable the trace point function as needed in the following way.
> Modify the npu\_lib.cmake file in the cmake directory of the kernel direct-call project, and add the -DASCENDC\_TRACE\_ON compilation option to the ascendc\_compile\_definitions command to enable the trace point function. Example:
> ```
> // Enable the trace point function of the operator.
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
| pipe | Input | Specifies the pipeline type where the trace point is located. For values, see [Hardware Pipeline Type](../../SIMD-API/basic_api/sync_control/intra_core_sync/intra_core_sync_overview.md#zh-cn_topic_0000002542725361_section1272612276459).<!-- npu="950" id7 --><br>This takes effect only on Ascend 950PR&950DT products and does not take effect on other products.<!-- end id7 --> |

## Return Value

None

## Constraints

-   TRACE\_START/TRACE\_STOP must be used together. If the trace point is not displayed on the Trace graph, the two are not paired.
-   Cross-core usage is not supported. For example, if TRACE\_START inserts a trace point on AI Cube, the TRACE\_STOP trace point must also be on AI Cube, not on AI Vector.
-   The macro supports all product models, but when actually called, it must be consistent with the models supported by the debugging tool.
-   Only the kernel direct-call project is supported. Enabling the trace point function in a custom operator project is not supported.

## Examples

Insert TRACE\_START/TRACE\_STOP at a specific instruction position in the kernel code:

```
TRACE_START(0x2);
Add(zLocal, xLocal, yLocal, dataSize);
TRACE_STOP(0x2);
```
