# PrintTimeStamp<a name="ZH-CN_TOPIC_0000002122196581"></a>

<!-- md-trans-meta sourceCommit=1d5c1445642448fe60bf90bf4c65624f94887316 translatedAt=2026-08-27T11:23:04.777Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not Supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not Supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not Supported
<!-- end id7 -->


## Description<a name="section259105813316"></a>

The header file path is `"basic_api/kernel_operator_dump_tensor_intf_impl.h"`.

Provides the timestamping feature for marking key execution points in the operator kernel code. After the call, the following information is printed:

- **descId**: User-defined identifier used to distinguish different timestamping positions.
- **rsv**: Reserved value, defaulting to 0, which requires no attention.
- **timeStamp**: Current system cycle count, used to calculate the time difference. For the time conversion rule, see [GetSystemCycle(ISASI)](../../Tool APIs/System Resources and Variables/GetSystemCycle_ISASI.md).
- **pcPtr**: PC pointer value. Users do not need to pay attention to it unless there is a special requirement.
- **entry**: Cycle count at which the operator starts execution. Users do not need to pay attention to it unless there is a special requirement.

The print example is as follows:

```plain
descId is 11, rsv is 0, timeStamp is 815603975350485, pcPtr is 19792358553124, entry is 815603975328116.
```

> [!CAUTION]Note
> This API is mainly used for debugging and analysis. Enabling it has a certain impact on operator performance. It is usually used in the debugging phase, and it is recommended to disable it in the production environment.<br>
> By default, this feature is disabled. Developers can enable the timestamping feature by modifying the CMakeList.txt file or the xxx.cmake file and adding -DASCENDC_TIME_STAMP_ON to the target_compile_definitions command. The example is as follows:<br>
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
| descId | Input | User-defined identifier (a custom number) used to distinguish different timestamping positions.<br>•**Note**: [0, 0xffff] is reserved for internal modules of Ascend C. For user-defined **descId**, a value greater than 0xffff is recommended. |

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section794123819592"></a>

None

## Example<a name="section82241477610"></a>

```cpp
AscendC::PrintTimeStamp(65577);
```

The print result is as follows:

```plain
// Some framework-internal timestamping information.
descId is 65577, rsv is 0, timeStamp is 13806084506158, pcPtr is 20619064414544, entry is 13806084502126.
```
