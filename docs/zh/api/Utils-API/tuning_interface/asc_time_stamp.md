# asc\_time\_stamp

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
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：不支持
- Atlas 推理系列产品Vector Core：不支持
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas 训练系列产品：不支持
<!-- end id6 -->

## 功能说明

SIMD场景下，提供时间戳打点功能，用于在算子核函数（Kernel）代码中标记关键执行点。调用后会打印如下信息：

-   descId：用户自定义标识符，用于区分不同打点位置。
-   rsv：保留值，默认为0，无需关注。
-   timeStamp：当前系统cycle数，用于计算时间差，时间换算规则可参考[GetSystemCycle\(ISASI\)](../../SIMD-API/basic_api/tool_interface/system_resources_and_variables/GetSystemCycle_ISASI.md)。
-   pcPtr：pc指针数值，若无特殊需求，用户无需关注。
-   entry：算子开始执行的cycle数，若无特殊需求，用户无需关注。

打印示例如下：

```
descId is 11, rsv is 0, timeStamp is 815603975350485, pcPtr is 19792358553124, entry is 815603975328116.
```

> [!CAUTION]注意
>该功能主要用于**调试和性能分析**，开启后会对算子性能产生一定影响，**生产环境建议关闭**。
>默认情况下，该功能关闭，开发者可以按需通过增加-DASCENDC\_TIME\_STAMP\_ON编译选项的方式，开启打点功能。

## 函数原型

```
__aicore__ static __attribute__((noinline)) void asc_time_stamp(uint32_t desc_id)
```

## 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| desc_id | 输入 | 用户自定义标识符（自定义数字），用于区分不同打点位置。<br> 注意： <br>[0, 0xffff]是预留给Ascend C内部各个模块使用的id值，用户自定义的desc_id建议使用大于0xffff的数值。 |

## 返回值说明

无

## 约束说明

-   该功能仅用于NPU上板调试。
-   暂不支持算子入图场景的打印。

-   单次调用本接口打印的数据总量不可超过1MB（还包括少量框架需要的头尾信息，通常可忽略）。使用时应注意，如果超出这个限制，则数据不会被打印。在使用自定义算子工程进行工程化算子开发时，一个算子所有使用Dump功能的接口在每个核上Dump的数据总量不可超过1MB。请开发者自行控制待打印的内容数据量，超出则不会打印。

## 调用示例

```
asc_time_stamp(11);
```

打印结果如下（Dump信息头等仅在使用自定义算子工程时才会打印）：

```
opType=AddCustom, DumpHead: AIV-0, CoreType=AIV, block dim=8, total_block_num=8, block_remain_len=1047136, block_initial_space=1048576, rsv=0, magic=5aa5bccd
...// 一些框架内部的打点信息
descId is 11, rsv is 0, timeStamp is 815603975350485, pcPtr is 19792358553124, entry is 815603975328116.
```
