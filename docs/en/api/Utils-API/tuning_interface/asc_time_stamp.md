# asc\_time\_stamp

<!-- md-trans-meta sourceCommit=482b56785b936fd20f7354a81c8239038750acee translatedAt=2026-09-06T09:05:23.503Z pushedAt=2026-09-11T01:46:28.689Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: supported
- Atlas A3 training products/Atlas A3 inference products: supported
- Atlas A2 training products/Atlas A2 inference products: supported
- Atlas 200I/500 A2 inference products: not supported
- Atlas inference products AI Core: not supported
- Atlas inference products Vector Core: not supported
- Atlas training products: not supported

## Description

In SIMD scenarios, this API provides a timestamp logging feature for marking key execution points in operator Kernel code. After being called, it prints the following information:

-   **descId**: User-defined identifier used to distinguish different marking positions.
-   **rsv**: Reserved value, which defaults to 0 and requires no attention.
-   **timeStamp**: Current system cycle count, used to calculate the time difference. For the time conversion rule, see [GetSystemCycle\(ISASI\)](../../SIMD-API/basic_api/tool_interface/system_resources_and_variables/GetSystemCycle_ISASI.md).
-   **pcPtr**: PC pointer value. Users do not need to pay attention to it unless there is a special requirement.
-   **entry**: Cycle count at which the operator starts execution. Users do not need to pay attention to it unless there is a special requirement.

The following is a print example:

```
descId is 11, rsv is 0, timeStamp is 815603975350485, pcPtr is 19792358553124, entry is 815603975328116.
```

> [!CAUTION]
>This feature is mainly used for **debugging and performance analysis**. Enabling it will have a certain impact on operator performance, so **it is recommended to disable it in production environments**.
>By default, this feature is disabled. Developers can enable the logging feature as needed by adding the -DASCENDC\_TIME\_STAMP\_ON compilation option.

## Prototype

```
__aicore__ inline void asc_time_stamp(uint32_t desc_id)
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **desc_id** | Input | User-defined identifier (custom number) used to distinguish different logging positions.<br> Note: <br>[0, 0xffff] is reserved for use by internal Ascend C modules. For user-defined **desc_id**, it is recommended to use a value greater than 0xffff. |

## Return Value

None

## Constraints

-   This function is used only for on-board debugging on the NPU.
-   Printing in operator graph scenarios is not supported.

-   The total amount of data printed by one call to this API must not exceed 1 MB (this also includes a small amount of header and footer information required by the framework, which is usually negligible). Note that if this limit is exceeded, the data will not be printed. When developing operators in a custom operator project for production use, the total amount of data dumped by all Dump APIs in a single operator on each core must not exceed 1 MB. Developers are responsible for controlling the amount of data to be printed; any data exceeding this limit will not be printed.

## Example

```
asc_time_stamp(11);
```

The print result is as follows (the Dump information header and the like are printed only when a custom operator project is used):

```
opType=AddCustom, DumpHead: AIV-0, CoreType=AIV, block dim=8, total_block_num=8, block_remain_len=1047136, block_initial_space=1048576, rsv=0, magic=5aa5bccd
// Some logging information inside the framework.
descId is 11, rsv is 0, timeStamp is 815603975350485, pcPtr is 19792358553124, entry is 815603975328116.
```
