# Mc2SetCcReduceType

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR&950DT系列产品：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2系列产品：不支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas推理系列产品AI Core：不支持
- Atlas推理系列产品Vector Core：不支持
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas训练系列产品：不支持
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/HCCL_communication/HCCL_MC2/Mc2SetCcReduceType_res.md#id1 -->

## 功能说明

设置MC2通信任务的Reduce操作类型，仅对AllReduce、ReduceScatter等归约类通信任务生效。

## 函数原型

```cpp
extern Mc2Result __attribute__((visibility("default"))) Mc2SetCcReduceType(void* ccArgs, uint8_t reduceType)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| ccArgs | 输入 | [Mc2GetCcArgs](Mc2GetCcArgs.md)返回的MC2通信参数对象指针。 |
| reduceType | 输入 | Reduce操作类型，取值为HcclReduceOp枚举值，详细可参考[表2](../HCCL_Kernel/HCCL_usage.md#table2469980529)。 |

## 返回值说明

-   0表示设置成功。
-   非0表示设置失败，例如参数为空指针或Reduce操作类型不支持。

## 约束说明

-   仅归约类通信任务需要配置本接口。
-   未调用本接口时，默认Reduce操作类型为HCCL_REDUCE_SUM。

## 调用示例

```cpp
Mc2Result ret = Mc2SetCcReduceType(ccArgs, static_cast<uint8_t>(HCCL_REDUCE_SUM));
```
