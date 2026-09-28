# Mc2SetCcCommEngine

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/HCCL_communication/HCCL_MC2/Mc2SetCcCommEngine_res.md#id1 -->

## 功能说明

设置MC2通信任务使用的通信引擎，并自动设置与该通信引擎对应的通信服务端启动协议版本，用户无需单独配置该版本。

## 函数原型

```cpp
extern Mc2Result __attribute__((visibility("default"))) Mc2SetCcCommEngine(void* ccArgs, uint8_t commEngine)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| ccArgs | 输入 | [Mc2GetCcArgs](Mc2GetCcArgs.md)返回的MC2通信参数对象指针。 |
| commEngine | 输入 | 通信引擎。2（AICPU_TS）表示使用AI CPU通信服务端；5（CCU_MS）表示使用CCU MS模式通信服务端；6（CCU_SCHED）表示使用CCU调度模式通信服务端，建议CCU场景使用。当前AI CPU仅支持AICPU_TS通信引擎。 |

## 返回值说明

-   0表示设置成功。
-   非0表示设置失败，例如参数为空指针或通信引擎不支持。

## 约束说明

-   当前仅支持上述三个通信引擎取值，其他取值返回错误。

## 调用示例

CCU引擎使用示例：

```cpp
constexpr uint8_t kCcuSchedEngine = 6U; // CCU_SCHED
Mc2Result ret = Mc2SetCcCommEngine(ccArgs, kCcuSchedEngine);
```

AI CPU引擎使用示例：

```cpp
constexpr uint8_t kAicpuTsEngine = 2U; // AICPU_TS
Mc2Result ret = Mc2SetCcCommEngine(ccArgs, kAicpuTsEngine);
```
