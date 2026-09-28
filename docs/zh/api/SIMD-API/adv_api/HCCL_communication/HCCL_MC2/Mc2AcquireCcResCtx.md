# Mc2AcquireCcResCtx

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/HCCL_communication/HCCL_MC2/Mc2AcquireCcResCtx_res.md#id1 -->

## 功能说明

根据通信域、通信任务类型和MC2通信参数，完成算法选择和通信资源申请，并返回设备侧通信资源上下文。该上下文既用于启动通信服务端，也用于AI Core侧HCCL客户端初始化。

## 函数原型

```cpp
extern Mc2Result __attribute__((visibility("default"))) Mc2AcquireCcResCtx(
    HcclComm comm, uint8_t ccType, void* ccArgs, void** ccResCtx, uint32_t* ccResCtxSize)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| comm | 输入 | 已初始化的通信域句柄。 |
| ccType | 输入 | 通信任务类型，取值为HcclCMDType枚举值。AI CPU和CCU路径均支持AllGather、AllReduce、ReduceScatter、AlltoAll和AlltoAllV。 |
| ccArgs | 输入 | [Mc2GetCcArgs](Mc2GetCcArgs.md)创建并通过Mc2SetCc*接口填充的MC2通信参数对象。 |
| ccResCtx | 输出 | 设备侧通信资源上下文地址。 |
| ccResCtxSize | 输出 | 设备侧通信资源上下文大小，单位为字节。 |

## 返回值说明

-   0表示通信资源申请成功。
-   非0表示通信资源申请失败，例如参数非法、通信任务类型不支持、算法不支持或资源不足。

## 约束说明

-   AI CPU路径和CCU路径均要求通信域内rank数大于1。
-   ccResCtx由通信域生命周期管理，用户不需要单独释放。
-   AI CPU路径使用AICPU_TS；CCU路径使用CCU_MS或CCU_SCHED。
-   返回的ccResCtx需要同时传给[Mc2CcKernelLaunch](Mc2CcKernelLaunch.md)和AI Core侧[InitV2](../HCCL_Kernel/InitV2.md)接口。

## 调用示例

```cpp
void* ccResCtx = nullptr;
uint32_t ccResCtxSize = 0U;
const uint8_t ccType = static_cast<uint8_t>(AscendC::HcclCMDType::HCCL_CMD_ALLTOALL);

Mc2Result ret = Mc2AcquireCcResCtx(comm, ccType, ccArgs, &ccResCtx, &ccResCtxSize);
if (ret != HCCL_SUCCESS) {
    // 错误处理
}
(void)Mc2FreeCcArgs(ccArgs);
```
