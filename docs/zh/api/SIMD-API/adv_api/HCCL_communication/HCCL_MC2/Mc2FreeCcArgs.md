# Mc2FreeCcArgs

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/HCCL_communication/HCCL_MC2/Mc2FreeCcArgs_res.md#id1 -->

## 功能说明

释放[Mc2GetCcArgs](Mc2GetCcArgs.md)创建的MC2通信参数对象。

## 函数原型

```cpp
extern Mc2Result __attribute__((visibility("default"))) Mc2FreeCcArgs(void* ccArgs)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| ccArgs | 输入 | [Mc2GetCcArgs](Mc2GetCcArgs.md)返回的MC2通信参数对象指针。 |

## 返回值说明

-   0表示释放成功。
-   非0表示释放失败，例如参数为空指针。

## 约束说明

-   本接口仅释放ccArgs，不释放[Mc2AcquireCcResCtx](Mc2AcquireCcResCtx.md)返回的ccResCtx。
-   ccResCtx由通信域生命周期管理，不需要用户单独释放。
-   不支持重复释放同一个ccArgs。

## 调用示例

本接口的调用示例请见[HCCL MC2使用说明](HCCL_MC2_usage.md)。
