# asc_get_icache_preload_status

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
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->

## 功能说明

头文件路径为：`"c_api/cache_ctrl/cache_ctrl.h"`。

该接口为调试接口，在[asc_icache_preload](./asc_icache_preload.md)后调用，用于获取ICache的PreLoad的状态：

- 当返回值为0时，说明ICache的PreLoad已完成。
- 当返回值为1时，说明ICache的PreLoad未完成。

## 函数原型

```c
__aicore__ inline int64_t asc_get_icache_preload_status()
```

## 参数说明

无

## 返回值说明

返回`int64_t`类型的ICache预取状态值，仅有效位取值如下：

- 返回`0`：ICache预取空闲，上一次预取已完成。
- 返回`1`：ICache预取正在进行中，尚未完成。

## 流水类型

PIPE_S

## 约束说明

本接口仅用于调试与性能分析场景下的状态查询，不影响预取本身的执行流程。

## 调用示例

本接口需在调用[asc_icache_preload](./asc_icache_preload.md)后使用，完整调用示例请参考[asc_icache_preload的调用示例](./asc_icache_preload.md#调用示例)。
