# PrintTimeStamp<a name="ZH-CN_TOPIC_0000002122196581"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T15:14:51.375Z pushedAt=2026-10-26T07:41:44.388Z nl=2 -->

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
- Atlas 200I/500 A2 inference product: supported
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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/debug_interface/onboard_print/PrintTimeStamp_res.md#id1 -->

## Description<a name="section259105813316"></a>

The header file path is `"basic_api/kernel_operator_dump_tensor_intf.h"`.

Provides the timestamp marking capability for marking key execution points in the operator kernel function (Kernel) code. After the call, the following information is printed:

- descId: User-defined identifier used to distinguish different marking positions.
- rsv: Reserved value, which defaults to 0 and does not need attention.
- timeStamp: Current system cycle count, used to calculate the time difference. For the time conversion rule, see [GetSystemCycle(ISASI)](../../tool_interface/system_resources_and_variables/GetSystemCycle_ISASI.md).
- pcPtr: PC pointer value. Users do not need to pay attention to it unless there is a special requirement.
- entry: Cycle count at which the operator starts execution. Users do not need to pay attention to it unless there is a special requirement.

The print example is as follows:

```plain
descId is 11, rsv is 0, timeStamp is 815603975350485, pcPtr is 19792358553124, entry is 815603975328116.
```

> [!CAUTION]Note
> This API is mainly used for debugging and analysis. Enabling it has a certain impact on operator performance. It is usually used in the debugging phase, and it is recommended to disable it in the production environment.<br>
> By default, this function is disabled. Developers can enable the marking function by modifying the CMakeLists.txt file or the xxx.cmake file and adding -DASCENDC_TIME_STAMP_ON to the target_compile_definitions command. The example is as follows:<br>
> ```
> target_compile_definitions({kernel_target_name} PRIVATE
>   -DASCENDC_TIME_STAMP_ON
> )
> ```

## Prototype<a name="section2067518173415"></a>

```cpp
__aicore__ inline void PrintTimeStamp(uint32_t descId)
```

## Parameters<a name="section158061867342"></a>

| Parameter | Input/Output | Description |
| ------ | ------ | ------ |
| descId | Input | User-defined identifier (custom number) used to distinguish different timestamp positions.<br>•**Note**: [0, 0xffff] is reserved for internal use by Ascend C modules. It is recommended that user-defined **descId** values be greater than 0xffff. |

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section794123819592"></a>

Refer to [asc_time_stamp](../../../../Utils-API/tuning_interface/asc_time_stamp.md) under the SIMD constraints in the utils directory for details.

## Example<a name="section82241477610"></a>

```cpp
AscendC::PrintTimeStamp(65577);
```

The print result is as follows:

```plain
// Some timestamp information inside the framework.
descId is 65577, rsv is 0, timeStamp is 13806084506158, pcPtr is 20619064414544, entry is 13806084502126.
```
