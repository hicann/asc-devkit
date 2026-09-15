# BatchWrite

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-10T02:16:49.827Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Not supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

This is the task submission API for collective communication **BatchWrite**, which returns the identifier **handleId** of the task to you. **BatchWrite** implements point-to-point communication, a communication mode that directly transfers data and can simultaneously send multiple data blocks to different Global Memory addresses.

For Atlas A3 training products/Atlas A3 inference products, **BatchWrite** communication is supported between the same or different Ascend AI Servers.

For Atlas A2 training products/Atlas A2 inference products, **BatchWrite** communication must be performed between different Ascend AI Servers (a general term for server forms composed of Ascend NPU devices, typically 8-card or 16-card).

**Figure 1**  BatchWrite schematic diagram  
![](../../../../figures/batchwrite_schematic_diagram.png "BatchWrite schematic diagram")

## Prototype

```
template <bool commit = false>
__aicore__ inline HcclHandle BatchWrite(GM_ADDR batchWriteInfo, uint32_t itemNum, uint16_t queueID = 0U)
```

## Parameters

**Table 1** Template parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **commit** | Input | bool type. The value can be:<br>**true**: When the **Prepare** API is called, **Commit** synchronously notifies the server that this communication task can be executed.<br>**false**: When the **Prepare** API is called, the server is not notified to execute this communication task. |

**Table 2** API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **batchWriteInfo** | Input | Global Memory address of the communication task information. The related information of a group of communication data must be saved in the specified format. When executing a communication task, multiple groups of communication task information can be specified at the same time to send data in batches. The structure is defined as shown in the following code.<br><br>For Atlas A3 training products/Atlas A3 inference products:<br>**type**: Reserved parameter, with a value of 0.<br>**res1[5]**: Reserved parameter. This value does not need to be filled in.<br>**length**: Length of the data to be copied.<br>**srcAddrLow**: Lower 32 bits of the source address of the data to be copied.<br>**srcAddrHigh**: Upper 32 bits of the source address of the data to be copied.<br>**dstAddrLow**: Lower 32 bits of the destination address of the data to be copied.<br>**dstAddrHigh**: Upper 32 bits of the destination address of the data to be copied.<br>**res2[4]**: Reserved parameter. This value does not need to be filled in.<br><br>For Atlas A2 training products/Atlas A2 inference products:<br>**localBuf**: Window address of the data sent by the local end.<br>**remoteBuf**: Window address of the data received by the peer end.<br>**count**: Number of data elements sent by this communication task.<br>**dataType**: Data type of the data sent by this communication task. For supported types, see [HcclDataType parameter description](hccl_usage.md#table116710585514).<br>**remoteRankId**: Card number of the destination card to which this communication task sends data. |
| **itemNum** | Input | Number of batch tasks. The value of this parameter must be consistent with the number of groups of communication task information in **batchWriteInfo**.<br><br>For Atlas A3 training products/Atlas A3 inference products, the value of this parameter cannot be greater than or equal to 2048. |
| **queueID** | Input | ID of the queue where the current communication resides, with a default value of 0.<br><br>For Atlas A2 training products/Atlas A2 inference products, this parameter supports only the value 0. |

```
// Atlas A3 training products/Atlas A3 inference products.
struct BatchWriteItem {
    uint64_t type;
    uint32_t res1[5];
    uint32_t length;
    uint32_t srcAddrLow;
    uint32_t srcAddrHigh;
    uint32_t dstAddrLow;
    uint32_t dstAddrHigh;
    uint32_t res2[4];
};

// Atlas A2 training products/Atlas A2 inference products.
struct BatchWriteItem {
    uint64_t localBuf;
    uint64_t remoteBuf;
    uint64_t count;
    uint32_t dataType;
    uint32_t remoteRankId;
};
```

## Return Value

Returns the identifier **handleId** of the task, where **handleId** is greater than or equal to 0. If the call fails, -1 is returned.

## Constraints

-   Before calling this API, ensure that [InitV2](InitV2.md) and [SetCcTilingV2](SetCcTilingV2.md) have been called.
-   If the [config template parameter](hccl_template_parameter.md#hccl-template-params) of the HCCL object does not specify the core for delivering communication tasks, this API can be called only on either the AIC core or the AIV core. If the [config template parameter](hccl_template_parameter.md#hccl-template-params) of the HCCL object specifies the core for delivering communication tasks, this API can be called on both the AIC core and the AIV core simultaneously. Internally, the API delivers the communication task on only one of the AIC core and the AIV core based on the specified core type.
-   Within a communicator, the total number of calls to all Prepare APIs and the InterHcclGroupSync API cannot exceed 63.
-   For Atlas A2 training products/Atlas A2 inference products, this API supports only communication between different AI Servers, and the destination card number specified in the communication task information cannot be the local card number.
-   Before writing the communication task information to batchWriteInfo, call [DataCacheCleanAndInvalid](../../../basic_api/cache_control/DataCacheCleanAndInvalid.md) to ensure that the expected data is successfully flushed to Global Memory.

## Examples

-   Point-to-point communication between different AI Servers

    On Atlas A2 training products/Atlas A2 inference products, assume that the local card needs to send different data to specified locations on cards 2 and 3 of other AI Servers. By calling the BatchWrite API once, batch point-to-point communication is implemented.

    ```
    struct BatchWriteItem {
        uint64_t localBuf;     // Window address of the local end for sending data.
        uint64_t remoteBuf;    // Window address of the peer end for receiving data.
        uint64_t count;        // Number of data elements to send.
        uint32_t dataType;     // Data type to send.
        uint32_t remoteRankId; // Destination card ID for sending data.
    }; // Define the format as agreed by the API.

    extern "C" __global__ __aicore__ void BatchWrite_custom(GM_ADDR inputGM, GM_ADDR workspace, GM_ADDR tilingGM)
    {
        REGISTER_TILING_DEFAULT(BatchWriteCustomTilingData); // BatchWriteCustomTilingData is the structure defined in the corresponding operator header file.
        GET_TILING_DATA_WITH_STRUCT(BatchWriteCustomTilingData, tilingData, tilingGM);
        GM_ADDR contextGM = AscendC::GetHcclContext<0>();

        if constexpr (g_coreType == AscendC::AIV) {
            Hccl hccl;
            hccl.InitV2(contextGM, &tilingData);
            hccl.SetCcTilingV2(offsetof(BatchWriteCustomTilingData, mc2CcTiling));

            __gm__ BatchWriteItem *sendInfo = reinterpret_cast<__gm__ BatchWriteItem *>(workspace);

            // Move the data to be sent from inputGM to the window address specified by localBuf in advance.
            sendInfo->localBuf = hccl.GetWindowsOutAddr(hccl.GetRankId());
            // The receive address on the peer end must also be a window address. The receiving end needs to determine whether to move the data to the output or workspace.
            sendInfo->remoteBuf = hccl.GetWindowsInAddr(2U);
            sendInfo->count = 16U;
            sendInfo->dataType = HcclDataType::HCCL_DATA_TYPE_FP16;
            sendInfo->remoteRankId = 2U;

            // Assemble multiple communication tasks to implement batch sending.
            (sendInfo + 1)->localBuf = hccl.GetWindowsOutAddr(hccl.GetRankId());
            (sendInfo + 1)->remoteBuf = hccl.GetWindowsInAddr(3U);
            (sendInfo + 1)->count = 32U;
            (sendInfo + 1)->dataType = HcclDataType::HCCL_DATA_TYPE_BFP16;
            (sendInfo + 1)->remoteRankId = 3U;

            // Ensure that the data in the cache has been flushed to the GM address.
            GlobalTensor<int64_t> tempTensor;
            tempTensor.SetGlobalBuffer((__gm__ int64_t *)sendInfo);
            DataCacheCleanAndInvalid<int64_t, CacheLine::SINGLE_CACHE_LINE, DcciDst::CACHELINE_OUT>(tempTensor);

            auto handleId = hccl.BatchWrite<true>(sendInfo, 2U);
            // wait only indicates that the local end has finished sending. Whether the peer end has received the data must be determined on the peer end.
            hccl.Wait(handleId);
            AscendC::SyncAll();
            hccl.Finalize();
        }
    }
    ```

    When the amount of communication data is large, you can call the **SetAicpuBlockDim** API in the Tiling process to set the number of AI CPU cores. The operator automatically selects the optimal core among multiple AI CPU cores for communication to achieve better performance. It is recommended to set the number of schedulable AI CPU cores to 5.

    ```
    static ge::graphStatus BatchWriteTilingFunc(gert::TilingContext* context)
    {
        // Omit irrelevant code.
        auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
        const auto aicCoreNum = ascendcPlatform.GetCoreNumAic();
        auto coreNum = use_aiv ? aicCoreNum * 2 : aicCoreNum;
        context->SetAicpuBlockDim(5U);
        context->SetBlockDim(coreNum);
        context->SetTilingKey(1000);

        // Omit irrelevant code.
        SdmaBatchWriteCustomTilingData *tiling = context->GetTilingData<SdmaBatchWriteCustomTilingData>();
        AscendC::Mc2CcTilingConfig mc2CcTilingConfig(groupName, 18, "BatchWrite=level0:fullmesh", 0);
        mc2CcTilingConfig.GetTiling(tiling->mc2InitTiling);
        mc2CcTilingConfig.GetTiling(tiling->mc2CcTiling);
        return ge::GRAPH_SUCCESS;
    }
    ```

<a id="multi-queue-p2p"></a>
-   Point-to-point communication over multiple queues

    On Atlas A3 training products/Atlas A3 inference products, to copy a segment of data to two different Global Memory addresses, you can call the BatchWrite API once to implement batch point-to-point communication.

    ```
    struct BatchWriteItem {
        uint64_t type;
        uint32_t res1[5];
        uint32_t length;
        uint32_t srcAddrLow;
        uint32_t srcAddrHigh;
        uint32_t dstAddrLow;
        uint32_t dstAddrHigh;
        uint32_t res2[4];
    }; // Define the format as agreed by the API.

    extern "C" __global__ __aicore__ void BatchWrite_custom(
        GM_ADDR inputGM, GM_ADDR outputGM1, GM_ADDR outputGM2, GM_ADDR workspace, GM_ADDR tilingGM)
    {
        REGISTER_TILING_DEFAULT(BatchWriteCustomTilingData); // BatchWriteCustomTilingData is the structure defined in the corresponding operator header file.
        GET_TILING_DATA_WITH_STRUCT(BatchWriteCustomTilingData, tilingData, tilingGM);
        GM_ADDR contextGM = AscendC::GetHcclContext<0>();

        if constexpr (g_coreType == AscendC::AIV) {
            Hccl hccl;
            hccl.InitV2(contextGM, &tilingData);
            hccl.SetCcTilingV2(offsetof(BatchWriteCustomTilingData, mc2CcTiling));

            __gm__ BatchWriteItem *sendInfo = reinterpret_cast<__gm__ BatchWriteItem *>(inputGM);

            sendInfo->type = 0UL;
            sendInfo->length = 64U;
            sendInfo->srcAddrLow = static_cast<uint32_t>((uint64_t)(inputGM) & 0xFFFFFFFF);
            sendInfo->srcAddrHigh = static_cast<uint32_t>(((uint64_t)(inputGM) >> 32) & 0xFFFFFFFF);
            sendInfo->dstAddrLow = static_cast<uint32_t>((uint64_t)(outputGM1) & 0xFFFFFFFF);
            sendInfo->dstAddrHigh = static_cast<uint32_t>(((uint64_t)(outputGM1) >> 32) & 0xFFFFFFFF);

            // Assemble multiple communication tasks to implement batch sending.
            (sendInfo + 1)->type = 0UL;
            (sendInfo + 1)->length = 64U;
            (sendInfo + 1)->srcAddrLow = static_cast<uint32_t>((uint64_t)(inputGM) & 0xFFFFFFFF);
            (sendInfo + 1)->srcAddrHigh = static_cast<uint32_t>(((uint64_t)(inputGM) >> 32) & 0xFFFFFFFF);
            (sendInfo + 1)->dstAddrLow = static_cast<uint32_t>((uint64_t)(outputGM2) & 0xFFFFFFFF);
            (sendInfo + 1)->dstAddrHigh = static_cast<uint32_t>(((uint64_t)(outputGM2) >> 32) & 0xFFFFFFFF);

            // Ensure that the data in the cache has been flushed to the GM address.
            GlobalTensor<int64_t> tempTensor;
            tempTensor.SetGlobalBuffer((__gm__ int64_t *)sendInfo);
            DataCacheCleanAndInvalid<int64_t, CacheLine::SINGLE_CACHE_LINE, DcciDst::CACHELINE_OUT>(tempTensor);

            // Deploy the two copies on queue 0 and queue 1, respectively.
            auto handleId0 = hccl.BatchWrite<true>(sendInfo, 1U, 0U);
            auto handleId1 = hccl.BatchWrite<true>(sendInfo, 1U, 1U);

            // Block the BatchWrite communication tasks on all queues. All queues wait until the communication tasks are complete before continuing execution, thereby synchronizing all queues.
            const uint16_t queueNum = hccl.GetQueueNum();
            for (uint16_t i = 0U; i < queueNum; ++i) {
                hccl.QueueBarrier<ScopeType::ALL>(i);
            }

            // Finalize can exit without waiting for all communication tasks on the server side to complete, releasing the AIV core resources as early as possible.
            hccl.Finalize<false>();
            AscendC::SyncAll();
        }
    }
    ```

    When the amount of communication data is large, you can call the SetAicpuBlockDim, [SetCommBlockNum](../HCCL-Tiling/SetCommBlockNum.md), and [SetQueueNum](../HCCL-Tiling/SetQueueNum.md) APIs in the Tiling process to improve operator performance through a concurrency mechanism.

    In the following sample code, the number of cores participating in BatchWrite communication is 24, the number of communication queues is 2, and the total number of queues =24\*2 is 48. Meanwhile, the number of server-side AI CPU cores is 4, so each AI CPU core only needs to schedule tasks on 48/4, that is, 12 communication queues, which improves communication efficiency.

    ```
    static ge::graphStatus BatchWriteTilingFunc(gert::TilingContext* context)
    {
        // Omit irrelevant code.
        auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
        const auto aicCoreNum = ascendcPlatform.GetCoreNumAic();
        auto coreNum = use_aiv ? aicCoreNum * 2 : aicCoreNum;
        context->SetAicpuBlockDim(4U);
        context->SetBlockDim(coreNum);
        context->SetTilingKey(1000);

        // Omit irrelevant code.
        SdmaBatchWriteCustomTilingData *tiling = context->GetTilingData<SdmaBatchWriteCustomTilingData>();
        AscendC::Mc2CcTilingConfig mc2CcTilingConfig(groupName, 18, "BatchWrite=level0:fullmesh", 0);
        mc2CcTilingConfig.SetCommBlockNum(24U);
        mc2CcTilingConfig.SetQueueNum(2U);
        mc2CcTilingConfig.GetTiling(tiling->mc2InitTiling);
        mc2CcTilingConfig.GetTiling(tiling->mc2CcTiling);
        return ge::GRAPH_SUCCESS;
    }
    ```
