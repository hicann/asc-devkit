# asc\_time\_stamp

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-23T07:49:10.259Z pushedAt=2026-10-26T11:12:09.549Z nl=2 -->

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
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/Utils-API/tuning_interface/asc_time_stamp_res.md#id1 -->

## Description

In SIMD scenarios, a timestamping feature is provided to mark key execution points in the operator kernel function (Kernel) code. After the call, the following information is printed:

-   descId: user-defined identifier used to distinguish different timestamping positions.
-   rsv: reserved value, defaulting to 0, which requires no attention.
-   timeStamp: current system cycle count, used to calculate the time difference. For the time conversion rule, see [GetSystemCycle\(ISASI\)](../../SIMD-API/basic_api/tool_interface/system_resources_and_variables/GetSystemCycle_ISASI.md).
-   pcPtr: PC pointer value. If there is no special requirement, you do not need to pay attention to it.
-   entry: cycle count at which the operator starts execution. If there is no special requirement, you do not need to pay attention to it.

The print example is as follows:

```
descId is 11, rsv is 0, timeStamp is 815603975350485, pcPtr is 19792358553124, entry is 815603975328116.
```

> [!CAUTION]Note
>This feature is mainly used for **debugging and performance analysis**. Enabling it affects operator performance to a certain extent, so **it is recommended to disable it in production environments**.
>By default, this feature is disabled. You can enable timestamping as needed by adding the -DASCENDC\_TIME\_STAMP\_ON compilation option.

## Prototype

```
__aicore__ static __attribute__((noinline)) void asc_time_stamp(uint32_t desc_id)
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| desc_id | Input | user-defined identifier (a custom number) used to distinguish different timestamping positions.<br> Note: <br>[0, 0xffff] is reserved for internal modules of Ascend C. It is recommended that the user-defined desc_id be a value greater than 0xffff. |

## Return Value

None

## Constraints

-   This feature is intended only for on-board NPU debugging.
-   Printing in operator graph compilation scenarios is not supported yet.

-   The total amount of data printed by a single call to this API must not exceed 1MB (this also includes a small amount of header and trailer information required by the framework, which can usually be ignored). Note that if this limit is exceeded, the data will not be printed. When developing operators as part of a custom operator project, the total amount of data dumped by all APIs that use the Dump feature on each core must not exceed 1MB. You are responsible for controlling the amount of content to be printed; if it exceeds the limit, it will not be printed.

## Examples

```
asc_time_stamp(11);
```

The print result is as follows (the Dump information header and similar content are printed only when a custom operator project is used):

```
opType=AddCustom, DumpHead: AIV-0, CoreType=AIV, block dim=8, total_block_num=8, block_remain_len=1047136, block_initial_space=1048576, rsv=0, magic=5aa5bccd
...// Some internal framework timestamp information.
descId is 11, rsv is 0, timeStamp is 815603975350485, pcPtr is 19792358553124, entry is 815603975328116.
```
