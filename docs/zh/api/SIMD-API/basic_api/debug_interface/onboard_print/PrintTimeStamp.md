# PrintTimeStamp<a name="ZH-CN_TOPIC_0000002122196581"></a>

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
- Atlas 推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/debug_interface/onboard_print/PrintTimeStamp_res.md#id1 -->

## 功能说明<a name="section259105813316"></a>

头文件路径为：`"basic_api/kernel_operator_dump_tensor_intf.h"`。

提供时间戳打点功能，用于在算子核函数（Kernel）代码中标记关键执行点。调用后会打印如下信息：

- descId：用户自定义标识符，用于区分不同打点位置；
- rsv：保留值，默认为0，无需关注；
- timeStamp：当前系统cycle数，用于计算时间差，时间换算规则可参考[GetSystemCycle(ISASI)](../../tool_interface/system_resources_and_variables/GetSystemCycle_ISASI.md)。
- pcPtr：pc指针数值，若无特殊需求，用户无需关注。
- entry：算子开始执行的cycle数，若无特殊需求，用户无需关注。

打印示例如下：

```plain
descId is 11, rsv is 0, timeStamp is 815603975350485, pcPtr is 19792358553124, entry is 815603975328116.
```

> [!CAUTION]注意
> 该接口主要用于调试分析，开启后会对算子性能产生一定影响，通常在调试阶段使用，生产环境建议关闭。<br>
> 默认情况下，该功能关闭，开发者可以通过修改CMakeList.txt文件或xxx.cmake文件，在target_compile_definitions命令中增加-DASCENDC_TIME_STAMP_ON来开启打点功能。示例如下：<br>
> ```
> target_compile_definitions({kernel_target_name} PRIVATE
>   -DASCENDC_TIME_STAMP_ON
> )
> ```

## 函数原型<a name="section2067518173415"></a>

```cpp
__aicore__ inline void PrintTimeStamp(uint32_t descId)
```

## 参数说明<a name="section158061867342"></a>

| 参数名称 | 输入/输出 | 描述 |
| ------ | ------ | ------ |
| descId | 输入 | 用户自定义标识符（自定义数字），用于区分不同打点位置。<br>•**注意**：[0，0xffff]是预留给Ascend C内部各个模块使用的id值，用户自定义的descId建议使用大于0xffff的数值。 |

## 返回值说明<a name="section640mcpsimp"></a>

无

## 约束说明<a name="section794123819592"></a>

可以参考utils目录SIMD约束下的[asc_time_stamp](../../../../Utils-API/tuning_interface/asc_time_stamp.md)说明。

## 调用示例<a name="section82241477610"></a>

```cpp
AscendC::PrintTimeStamp(65577);
```

打印结果如下：

```plain
// 一些框架内部的打点信息
descId is 65577, rsv is 0, timeStamp is 13806084506158, pcPtr is 20619064414544, entry is 13806084502126.
```
