# Mc2SetCcSrcDataType

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/HCCL_communication/HCCL_MC2/Mc2SetCcSrcDataType_res.md#id1 -->

## 功能说明

设置MC2通信任务输入数据的数据类型。

## 函数原型

```cpp
extern Mc2Result __attribute__((visibility("default"))) Mc2SetCcSrcDataType(void* ccArgs, uint8_t srcDataType)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| ccArgs | 输入 | [Mc2GetCcArgs](Mc2GetCcArgs.md)返回的MC2通信参数对象指针。 |
| srcDataType | 输入 | 输入数据类型，取值为HcclDataType枚举值，详细可参考[表1](../HCCL_Kernel/HCCL_usage.md#table116710585514)。 |

## 返回值说明

-   0表示设置成功。
-   非0表示设置失败，例如参数为空指针或数据类型不支持。

## 约束说明

-   不同通信任务支持的输入数据类型不同，具体约束请参考对应通信任务接口文档。
-   未调用本接口时，默认输入数据类型为HCCL_DATA_TYPE_FP16。

## 调用示例

```cpp
Mc2Result ret = Mc2SetCcSrcDataType(ccArgs, HCCL_DATA_TYPE_FP32);
```
