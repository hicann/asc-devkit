# HCCL MC2使用说明

> [!NOTE]说明
>本章节描述MC2内置通信算子的Host侧接口。用户通过这些接口配置通信任务、申请通信资源上下文并启动通信服务端。接口同时支持AI CPU和CCU两种通信服务端，二者共用同一组参数配置和资源上下文接口。

HCCL MC2接口由Host侧参数配置、资源上下文申请和通信服务端启动三部分组成。用户在Host侧配置通信任务参数后，接口内部会完成算法选择、通信资源申请以及设备侧资源上下文的写入。

通信引擎用于选择通信服务端的执行方式：`AICPU_TS`表示AI CPU通信服务端，`CCU_MS`和`CCU_SCHED`表示CCU通信服务端。其中，`CCU_SCHED`表示CCU调度模式，建议CCU场景使用该模式。

正式申请资源前，通信引擎设置为`CCU_SCHED`时，可以调用[CheckOpResSufficient](CheckOpResSufficient.md)预检查通信资源是否充足。

典型流程如下：

1.  调用[Mc2GetCcArgs](Mc2GetCcArgs.md)创建通信参数对象。

2.  调用配置接口填充通信参数。

    -   [Mc2SetCcCommEngine](Mc2SetCcCommEngine.md)：设置通信引擎。
    -   [Mc2SetCcSrcDataType](Mc2SetCcSrcDataType.md)：设置输入数据类型。
    -   [Mc2SetCcDstDataType](Mc2SetCcDstDataType.md)：设置输出数据类型。
    -   [Mc2SetCcReduceType](Mc2SetCcReduceType.md)：设置Reduce操作类型，仅AllReduce、ReduceScatter等归约类任务需要配置。
    -   [Mc2SetCcAlgConfig](Mc2SetCcAlgConfig.md)：设置通信算法。

3.  调用[Mc2AcquireCcResCtx](Mc2AcquireCcResCtx.md)申请通信资源上下文，获得设备侧资源上下文地址和大小。

4.  调用[Mc2CcKernelLaunch](Mc2CcKernelLaunch.md)启动通信服务端。AI CPU路径传入独立于AI Core任务的AI CPU任务流；CCU路径可传入nullptr。

5.  在AI Core核函数中，将步骤3返回的资源上下文传入[InitV2](../HCCL_Kernel/InitV2.md)，再调用AllGather、AllReduce、AlltoAll等HCCL高阶API下发客户端通信任务。

6.  参数对象不再使用后，调用[Mc2FreeCcArgs](Mc2FreeCcArgs.md)释放。

上述流程中的第3步与第5步必须使用同一个资源上下文。ccResCtx是[Mc2AcquireCcResCtx](Mc2AcquireCcResCtx.md)返回的设备侧通信资源上下文地址，包含通信算法和通信资源相关信息；该资源由通信域生命周期管理，用户不需要调用单独的接口释放。

[Mc2GetCcArgs](Mc2GetCcArgs.md)创建的参数对象默认通信引擎为`AICPU_TS`，即AI CPU通信服务端。若使用CCU路径，必须调用[Mc2SetCcCommEngine](Mc2SetCcCommEngine.md)显式设置通信引擎，推荐设置为`CCU_SCHED`。

AI CPU与CCU路径的主要差异如下：

| 项目 | AI CPU路径 | CCU路径 |
| --- | --- | --- |
| commEngine取值 | 2，AICPU_TS，表示AI CPU通信服务端 | 5，CCU_MS；或6，CCU_SCHED，推荐使用6 |
| 服务端启动stream | 必须传入独立的aclrtStream | 传入nullptr，launch stream由资源上下文内部携带 |
| AI Core侧初始化 | hccl.InitV2(contextGM) | hccl.InitV2(contextGM) |
| HCCL模板参数 | HCCL_SERVER_TYPE_AICPU | HCCL_SERVER_TYPE_CCU |
| 退出机制 | AI Core侧调用Finalize后服务端退出 | AI Core侧调用Finalize后服务端退出 |
| rank数 | 必须大于1 | 必须大于1 |

以下示例展示AlltoAll任务使用CCU通信路径时的Host侧调用顺序：

```cpp
void* ccArgs = nullptr;
void* ccResCtx = nullptr;
uint32_t ccResCtxSize = 0U;
constexpr uint8_t kCcuSchedEngine = 6U; // CCU_SCHED
const uint8_t ccType = static_cast<uint8_t>(AscendC::HcclCMDType::HCCL_CMD_ALLTOALL);
char algConfig[] = "sole[mesh]";

Mc2Result ret = Mc2GetCcArgs(&ccArgs);
ret = Mc2SetCcCommEngine(ccArgs, kCcuSchedEngine);
ret = Mc2SetCcSrcDataType(ccArgs, HCCL_DATA_TYPE_FP32);
ret = Mc2SetCcDstDataType(ccArgs, HCCL_DATA_TYPE_FP32);
ret = Mc2SetCcAlgConfig(ccArgs, algConfig);

ret = Mc2AcquireCcResCtx(comm, ccType, ccArgs, &ccResCtx, &ccResCtxSize);
(void)Mc2FreeCcArgs(ccArgs);

ret = Mc2CcKernelLaunch(nullptr, ccResCtx, ccResCtxSize);

// 将ccResCtx传入AI Core核函数，并在核函数内调用InitV2和AlltoAll接口。
all_to_all_kernel<<<1, nullptr, streamAiv>>>(sendBuf, recvBuf, ccResCtx, dataCount);
```

以下示例展示AllGather任务使用AI CPU通信路径时的调用顺序。AI CPU服务端会阻塞等待AI Core客户端消息，因此aicpuStream与streamAiv必须为两条不同的任务流，且建议先同步AI Core任务流、再同步AI CPU任务流。

```cpp
void* ccArgs = nullptr;
void* ccResCtx = nullptr;
uint32_t ccResCtxSize = 0U;
constexpr uint8_t kAicpuTsEngine = 2U; // AICPU_TS
const uint8_t ccType = static_cast<uint8_t>(AscendC::HcclCMDType::HCCL_CMD_ALLGATHER);

Mc2Result ret = Mc2GetCcArgs(&ccArgs);
ret = Mc2SetCcCommEngine(ccArgs, kAicpuTsEngine);
ret = Mc2SetCcSrcDataType(ccArgs, HCCL_DATA_TYPE_FP32);
ret = Mc2SetCcDstDataType(ccArgs, HCCL_DATA_TYPE_FP32);

ret = Mc2AcquireCcResCtx(comm, ccType, ccArgs, &ccResCtx, &ccResCtxSize);
(void)Mc2FreeCcArgs(ccArgs);

ret = Mc2CcKernelLaunch(aicpuStream, ccResCtx, ccResCtxSize);

all_gather_kernel<<<1, nullptr, streamAiv>>>(sendBuf, recvBuf, ccResCtx, sendCount);
```

AI Core核函数中的AI CPU客户端示例片段如下：

```cpp
__global__ __vector__ void all_gather_kernel(
    __gm__ void* sendBuf, __gm__ void* recvBuf, __gm__ void* contextGM, uint64_t sendCount)
{
    if (g_coreType != AscendC::AIV) {
        return;
    }
    AscendC::InitSocState();
    AscendC::Hccl<AscendC::HcclServerType::HCCL_SERVER_TYPE_AICPU> hccl;
    hccl.InitV2((GM_ADDR)contextGM, nullptr);
    AscendC::HcclHandle handleId = hccl.AllGather<true>(
        (GM_ADDR)sendBuf, (GM_ADDR)recvBuf, sendCount,
        AscendC::HcclDataType::HCCL_DATA_TYPE_FP32, 0);
    hccl.Wait(handleId);
    AscendC::SyncAll<true>();
    hccl.Finalize();
    AscendC::PipeBarrier<PIPE_ALL>();
}
```
