# Mc2CcKernelLaunch

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/HCCL_communication/HCCL_MC2/Mc2CcKernelLaunch_res.md#id1 -->

## 功能说明

使用[Mc2AcquireCcResCtx](Mc2AcquireCcResCtx.md)返回的通信资源上下文启动通信服务端。AI CPU路径下，本接口在指定AI CPU任务流上启动AI CPU Server；CCU路径下，本接口启动CCU服务端。后续AI Core侧调用HCCL高阶API时，会作为客户端向服务端下发实际通信任务。

## 函数原型

```cpp
extern Mc2Result __attribute__((visibility("default"))) Mc2CcKernelLaunch(
    void* stream, void* ccResCtx, uint32_t ccResCtxSize)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| stream | 输入 | aclrtStream指针。AI CPU路径为必填，用于启动AI CPU Server；CCU路径不使用该参数，可传入nullptr。 |
| ccResCtx | 输入 | [Mc2AcquireCcResCtx](Mc2AcquireCcResCtx.md)返回的设备侧通信资源上下文。 |
| ccResCtxSize | 输入 | [Mc2AcquireCcResCtx](Mc2AcquireCcResCtx.md)返回的通信资源上下文大小，单位为字节。 |

## 返回值说明

-   0表示通信服务端启动成功。
-   非0表示通信服务端启动失败，例如参数非法、通信资源上下文非法或设备类型不支持。

## 约束说明

-   AI CPU路径的stream必须与AI Core任务所在stream不同。AI CPU Server会阻塞等待AI Core客户端消息，若共用stream会导致任务无法完成。
-   AI CPU路径建议先同步AI Core任务流、再同步AI CPU任务流。
-   CCU路径的启动stream由通信资源上下文内部携带，stream参数不参与CCU服务端启动。
-   本接口必须与[Mc2AcquireCcResCtx](Mc2AcquireCcResCtx.md)返回的同一个ccResCtx和ccResCtxSize配合使用。
-   AI Core侧调用Finalize后，AI CPU Server才能正常退出。

## 调用示例

CCU示例：

```cpp
Mc2Result ret = Mc2CcKernelLaunch(nullptr, ccResCtx, ccResCtxSize);
if (ret != HCCL_SUCCESS) {
    // 错误处理
}
```

AI CPU示例：

```cpp
Mc2Result ret = Mc2CcKernelLaunch(aicpuStream, ccResCtx, ccResCtxSize);
if (ret != HCCL_SUCCESS) {
    // 错误处理
}
```
