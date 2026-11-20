# HCCL MC2 Usage

<!-- md-trans-meta sourceCommit=579cf014ea1f1354d4fbc547cc9cca11057b444b translatedAt=2026-11-02T05:42:41.667Z pushedAt=2026-11-18T04:57:17.756Z nl=2 -->

> [!NOTE]Note
> This chapter describes the Host-side APIs of the built-in communication operators of MC2. You can use these APIs to configure communication tasks, apply for communication resource contexts, and start the communication server. The APIs support both AI CPU and CCU communication servers, which share the same set of parameter configuration and resource context APIs.

The HCCL MC2 APIs consist of three parts: Host-side parameter configuration, resource context application, and communication server startup. After you configure the communication task parameters on the Host side, the APIs internally complete algorithm selection, communication resource application, and writing of the device-side resource context.

The communication engine is used to select the execution mode of the communication server: `AICPU_TS` indicates the AI CPU communication server, and `CCU_MS` and `CCU_SCHED` indicate the CCU communication server. Among them, `CCU_SCHED` indicates the CCU scheduling mode, which is recommended for CCU scenarios.

Before formally applying for resources, when the communication engine is set to `CCU_SCHED`, you can call [CheckOpResSufficient](CheckOpResSufficient.md) to pre-check whether the communication resources are sufficient.

The typical process is as follows:

1.  Call [Mc2GetCcArgs](Mc2GetCcArgs.md) to create a communication parameter object.

2.  Call the configuration APIs to fill in the communication parameters.

    -   [Mc2SetCcCommEngine](Mc2SetCcCommEngine.md): Set the communication engine.
    -   [Mc2SetCcSrcDataType](Mc2SetCcSrcDataType.md): Set the input data type.
    -   [Mc2SetCcDstDataType](Mc2SetCcDstDataType.md): Sets the output data type.
    -   [Mc2SetCcReduceType](Mc2SetCcReduceType.md): Sets the Reduce operation type. This is required only for reduction tasks such as AllReduce and ReduceScatter.
    -   [Mc2SetCcAlgConfig](Mc2SetCcAlgConfig.md): Sets the communication algorithm.

3.  Call [Mc2AcquireCcResCtx](Mc2AcquireCcResCtx.md) to apply for a communication resource context and obtain the device-side resource context address and size.

4.  Call [Mc2CcKernelLaunch](Mc2CcKernelLaunch.md) to start the communication server. For the AI CPU path, pass an AI CPU task flow that is independent of the AI Core task. For the CCU path, you can pass nullptr.

5.  In the AI Core kernel function, pass the resource context returned in step 3 to [InitV2](../HCCL_Kernel/InitV2.md), and then call high-level HCCL APIs such as AllGather, AllReduce, and AlltoAll to submit client communication tasks.

6.  After the parameter object is no longer used, call [Mc2FreeCcArgs](Mc2FreeCcArgs.md) to release it.

Steps 3 and 5 in the preceding process must use the same resource context. ccResCtx is the device-side communication resource context address returned by [Mc2AcquireCcResCtx](Mc2AcquireCcResCtx.md), and it contains information about the communication algorithm and communication resources. This resource is managed by the communicator lifecycle, so you do not need to call a separate API to release it.

The parameter object created by [Mc2GetCcArgs](Mc2GetCcArgs.md) uses `AICPU_TS` as the default communication engine, that is, the AI CPU communication server. If you use the CCU path, you must call [Mc2SetCcCommEngine](Mc2SetCcCommEngine.md) to explicitly set the communication engine. `CCU_SCHED` is recommended.

The main differences between the AI CPU and CCU paths are as follows:

| Item | AI CPU Path | CCU Path |
| --- | --- | --- |
| commEngine value | 2, AICPU_TS, indicating the AI CPU communication server | 5, CCU_MS; or 6, CCU_SCHED, with 6 recommended |
| Server startup stream | An independent aclrtStream must be passed | Pass nullptr; the launch stream is carried internally by the resource context |
| AI Core-side initialization | hccl.InitV2(contextGM) | hccl.InitV2(contextGM) |
| HCCL Template Parameters | HCCL_SERVER_TYPE_AICPU | HCCL_SERVER_TYPE_CCU |
| Exit mechanism | The server exits after the AI Core side calls Finalize | The server exits after the AI Core side calls Finalize |
| Number of ranks | Must be greater than 1 | Must be greater than 1 |

The following example shows the Host-side call sequence when the AlltoAll task uses the CCU communication path:

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

// Pass ccResCtx to the AI Core kernel function, and call the InitV2 and AlltoAll APIs inside the kernel function.
all_to_all_kernel<<<1, nullptr, streamAiv>>>(sendBuf, recvBuf, ccResCtx, dataCount);
```

The following example shows the call sequence when the AllGather task uses the AI CPU communication path. The AI CPU server blocks while waiting for AI Core client messages, so aicpuStream and streamAiv must be two different task flows. It is recommended to synchronize the AI Core task flow first and then the AI CPU task flow.

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

The following is an example snippet of the AI CPU client in the AI Core kernel function:

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
