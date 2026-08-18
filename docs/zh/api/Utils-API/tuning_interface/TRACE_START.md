# TRACE\_START

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
- Atlas 200I/500 A2 推理产品：支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：支持
- Atlas 推理系列产品Vector Core：不支持
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas 训练系列产品：支持
<!-- end id6 -->

## 功能说明

SIMD场景下，通过仿真器进行算子性能仿真时，可对算子任意运行阶段打点，从而分析不同指令的流水图，以便进一步性能调优。

用于表示起始位置打点，一般与[TRACE\_STOP](TRACE_STOP.md)配套使用。

> [!CAUTION]注意
>该功能主要用于**调试和性能分析**，开启后会对算子性能产生一定影响，通常在调测阶段使用，**生产环境建议关闭**。
>默认情况下，该功能关闭，开发者可以按需通过如下方式开启打点功能。
>修改核函数（Kernel）直调工程cmake目录下的npu\_lib.cmake文件，在ascendc\_compile\_definitions命令中增加-DASCENDC\_TRACE\_ON编译选项，来开启打点功能。示例如下：
>```
>// 打开算子的打点功能
>ascendc_compile_definitions(ascendc_kernels_${RUN_MODE} PRIVATE
>    -DASCENDC_TRACE_ON
>)
>```

## 函数原型

```
#define TRACE_START(TraceId apid)
#define TRACE_START(pipe_t pipe, TraceId apid)
```

## 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| apid | 输入 | 当前预留了十个用户自定义的类型：<br>0x0：USER_DEFINE_0<br>0x1：USER_DEFINE_1<br>0x2：USER_DEFINE_2<br>0x3：USER_DEFINE_3<br>0x4：USER_DEFINE_4<br>0x5：USER_DEFINE_5<br>0x6：USER_DEFINE_6<br>0x7：USER_DEFINE_7<br>0x8：USER_DEFINE_8<br>0x9：USER_DEFINE_9 |
| pipe | 输入 | 指定打点所在的pipeline类型，取值请参考[硬件流水类型](../../SIMD-API/basic_api/sync_control/intra_core_sync/intra_core_sync_overview.md#zh-cn_topic_0000002542725361_section1272612276459)。<!-- npu="950" id7 --><br>仅Ascend 950PR/Ascend 950DT生效，其他产品上不生效。<!-- end id7 --> |

## 返回值说明

无

## 约束说明

-   TRACE\_START/TRACE\_STOP需配套使用，若Trace图上未显示打点，则说明两者没有配对。
-   不支持跨核使用，例如TRACE\_START在AI Cube打点，则TRACE\_STOP打点也需要在AI Cube上，不能在AI Vector上。
-   宏支持所有的产品型号，但实际调用时需与调测工具支持的型号保持一致。
-   仅支持核函数（Kernel）直调工程，不支持自定义算子工程下开启打点功能。

## 调用示例

在核函数（Kernel）代码中特定指令位置打上TRACE\_START/TRACE\_STOP：

```
TRACE_START(0x2);
Add(zLocal, xLocal, yLocal, dataSize);
TRACE_STOP(0x2);
```
