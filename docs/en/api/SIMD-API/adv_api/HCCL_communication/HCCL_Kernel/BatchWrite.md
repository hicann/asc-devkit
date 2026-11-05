# BatchWrite

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-08T11:41:24.353Z pushedAt=2026-10-27T11:42:43.512Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Not supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
- Vector Core of Atlas inference products: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/HCCL_communication/HCCL_Kernel/BatchWrite_res.md#id1 -->

## Description

**BatchWrite** is the task dispatch API for collective communication, which returns the task identifier **handleId** to the user. **BatchWrite** implements point-to-point communication, a communication mode that directly transfers data and can simultaneously send multiple data blocks to different Global Memory addresses.

<!-- npu="A3" id7 -->
For Atlas A3 products, **BatchWrite** communication can be performed between the same or different Ascend AI Servers.
<!-- end id7 -->

<!-- npu="910b" id8 -->
For Atlas A2 products, BatchWrite communication must be performed between different Ascend AI Servers (a general term for server forms composed of Ascend NPU devices, typically 8-card or 16-card).
<!-- end id8 -->

**Figure 1** BatchWrite diagram  
![](../../../../figures/batchwrite_diagram.png "BatchWrite diagram")

## Prototype

```
template <bool commit = false>
__aicore__ inline HcclHandle BatchWrite(GM_ADDR batchWriteInfo, uint32_t itemNum, uint16_t queueID = 0U)
```

## Parameters

**Table 1** Template parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| commit | Input | bool type. The value is as follows:<br>true: When the Prepare API is called, Commit synchronously notifies the server that the communication task can be executed.<br>false: When the Prepare API is called, the server is not notified to execute the communication task. |

**Table 2** API parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| batchWriteInfo | Input | Global Memory address of the communication task information. The related information of a group of communication data must be saved in the specified format. When executing a communication task, multiple groups of communication task information can be specified at the same time, and data is sent in batches when the communication task is executed. The structure is defined as shown in the following code. <!-- npu="A3" id9 --><br><br>For Atlas A3 products:<br>type: Reserved parameter, with a value of 0.<br>res1[5]: Reserved parameter. This value does not need to be filled in.<br>length: Length of the data to be copied.<br>srcAddrLow: Lower 32 bits of the source address of the data to be copied.<br>srcAddrHigh: Higher 32 bits of the source address of the data to be copied.<br>dstAddrLow: Lower 32 bits of the destination address of the data to be copied.<br>dstAddrHigh: Higher 32 bits of the destination address of the data to be copied.<br>res2[4]: Reserved parameter. This value does not need to be filled in.<!-- end id9 --><!-- npu="910b" id10 --><br><br>For Atlas A2 products:<br>localBuf: Window address of the data sent by the local end.<br>remoteBuf: Window address of the data received by the remote side.<br>count: Number of data items sent by this communication task.<br>dataType: Data type sent by this communication task. For supported types, see [HcclDataType parameter description](HCCL_usage.md#table116710585514).<br>remoteRankId: Card number of the destination card to which this communication task sends data.<!-- end id10 --> |
| itemNum | Input | Number of batch tasks. The value of this parameter must be consistent with the number of groups of communication task information in batchWriteInfo.<!-- npu="A3" id11 --><br><br>For Atlas A3 products, the value of this parameter cannot be greater than or equal to 2048.<!-- end id11 --> |
| queueID | Input | Specifies the ID of the queue where the current communication resides. The default value is 0.<!-- npu="910b" id12 --><br><br>For Atlas A2 products, this parameter supports only the value 0.<!-- end id12 --> |

<!-- npu="A3" id14 -->
```
// Atlas A3 products
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
```
<!-- end id14 -->

<!-- npu="910b" id15 -->
```
// Atlas A2 products
struct BatchWriteItem {
    uint64_t localBuf;
    uint64_t remoteBuf;
    uint64_t count;
    uint32_t dataType;
    uint32_t remoteRankId;
};
```
<!-- end id15 -->

## Return Value

Returns the identifier handleId of the task. handleId is greater than or equal to 0. If the call fails, -1 is returned.

## Constraints

-   Before calling this API, ensure that the [InitV2](InitV2.md) and [SetCcTilingV2](SetCcTilingV2.md) APIs have been called.
-   If the [config template parameter](HCCL_template_params.md#hccl-template-params) of the HCCL object does not specify the core that delivers the communication task, this API can be called only on either the AIC core or the AIV core. If the [config template parameter](HCCL_template_params.md#hccl-template-params) of the HCCL object specifies the core that delivers the communication task, this API can be called on both the AIC core and the AIV core at the same time. Internally, the API delivers the communication task on only one of the AIC core and the AIV core based on the type of the specified core.
-   Within a communicator, the total number of calls to all Prepare APIs and the InterHcclGroupSync API cannot exceed 63.
<!-- npu="910b" id13 -->
-   For Atlas A2 products, the current API supports only communication between different AI Servers, and the destination card ID specified in the communication task information cannot be the local card ID.
<!-- end id13 -->
-   Before the communication task information is written to **batchWriteInfo**, call [DataCacheCleanAndInvalid](../../../basic_api/cache_control/DataCacheCleanAndInvalid.md) to ensure that the expected data is successfully flushed to Global Memory.

## Examples

<!-- npu="910b" id16 -->
-   Point-to-point communication between different AI Servers

    On Atlas A2 products, assume that the local card needs to send different data to the specified locations of cards 2 and 3 on other AI Servers. Call **BatchWrite** once to implement batch point-to-point communication.

    ```
    struct BatchWriteItem {
        uint64_t localBuf;     // Window address of the data to be sent on the local end.
        uint64_t remoteBuf;    // Window address of the data to be received on the remote side.
        uint64_t count;        // Number of data items to send.
        uint32_t dataType;     // Data type to send.
        uint32_t remoteRankId; // Destination card ID of the data to be sent.
    }; // Define the format as agreed by the API.

    extern "C" __global__ __aicore__ void BatchWrite_custom(GM_ADDR inputGM, GM_ADDR workspace, GM_ADDR tilingGM)
    {
        REGISTER_TILING_DEFAULT(BatchWriteCustomTilingData); // BatchWriteCustomTilingData is the structure defined in the corresponding operator header file.
        GET_TILING_DATA_WITH_STRUCT(BatchWriteCustomTilingData, tilingData, tilingGM);
        GM_ADDR contextGM = AscendC::GetHcclContext<0>();

        if constexpr (g_coreType == AscendC::AIV) {
            Hccl hccl;
            hccl.InitV2(contextGM, &tilingData);
            hccl.SetCcTilingV2(offsetof(BatchWriteCustomTilingData, mc2CcTiling));

            __gm__ BatchWriteItem* sendInfo = reinterpret_cast<__gm__ BatchWriteItem*>(workspace);

            // Move the data to be sent from inputGM to the window address specified in localBuf in advance.
            sendInfo->localBuf = hccl.GetWindowsOutAddr(hccl.GetRankId());
            // The receive address on the remote side must also be a window address. The receiving side needs to consider whether to move the data to the output or workspace.
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
            tempTensor.SetGlobalBuffer((__gm__ int64_t*)sendInfo);
            DataCacheCleanAndInvalid<int64_t, CacheLine::SINGLE_CACHE_LINE, DcciDst::CACHELINE_OUT>(tempTensor);

            auto handleId = hccl.BatchWrite<true>(sendInfo, 2U);
            // wait only indicates that the local side has finished sending. Whether the remote side has received the data must be determined on the remote side.
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
        context->SetSimdNumBlocks(coreNum);
        context->SetTilingKey(1000);

        // Omit irrelevant code.
        SdmaBatchWriteCustomTilingData* tiling = context->GetTilingData<SdmaBatchWriteCustomTilingData>();
        AscendC::Mc2CcTilingConfig mc2CcTilingConfig(groupName, HCCL_CMD_BATCH_WRITE, "BatchWrite=level0:fullmesh", 0);
        mc2CcTilingConfig.GetTiling(tiling->mc2InitTiling);
        mc2CcTilingConfig.GetTiling(tiling->mc2CcTiling);
        return ge::GRAPH_SUCCESS;
    }
    ```
<!-- end id16 -->

<!-- npu="A3" id17 -->
-   <a name="batchwrite_multi_queue"></a>Point-to-point communication over multiple queues

    On Atlas A3 products, to copy a segment of data to two different Global Memory locations, you can call the BatchWrite API once to implement batch point-to-point communication.

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

        if constexpr (g_coreType == AscendC::AIV) {
            Hccl hccl;
            hccl.InitV2(contextGM, &tilingData);
            hccl.SetCcTilingV2(offsetof(BatchWriteCustomTilingData, mc2CcTiling));

            __gm__ BatchWriteItem* sendInfo = reinterpret_cast<__gm__ BatchWriteItem*>(inputGM);

            sendInfo->type = 0UL;
            sendInfo->length = 64U;
            sendInfo->srcAddrLow = static_cast<uint32_t>((uint64_t)(inputGM) & 0xFFFFFFFF);
            sendInfo->srcAddrHigh = static_cast<uint32_t>(((uint64_t)(inputGM) >> 32) & 0xFFFFFFFF);
            sendInfo->dstAddrLow = static_cast<uint32_t>((uint64_t)(outputGM1) & 0xFFFFFFFF);
            sendInfo->dstAddrHigh = static_cast<uint32_t>(((uint64_t)(outputGM1) >> 32) & 0xFFFFFFFF);

            // Multiple communication tasks can be assembled to implement batch sending.
            (sendInfo + 1)->type = 0UL;
            (sendInfo + 1)->length = 64U;
            (sendInfo + 1)->srcAddrLow = static_cast<uint32_t>((uint64_t)(inputGM) & 0xFFFFFFFF);
            (sendInfo + 1)->srcAddrHigh = static_cast<uint32_t>(((uint64_t)(inputGM) >> 32) & 0xFFFFFFFF);
            (sendInfo + 1)->dstAddrLow = static_cast<uint32_t>((uint64_t)(outputGM2) & 0xFFFFFFFF);
            (sendInfo + 1)->dstAddrHigh = static_cast<uint32_t>(((uint64_t)(outputGM2) >> 32) & 0xFFFFFFFF);

            // Ensure that the data in the cache has been flushed to the GM address.
            GlobalTensor<int64_t> tempTensor;
            tempTensor.SetGlobalBuffer((__gm__ int64_t*)sendInfo);
            DataCacheCleanAndInvalid<int64_t, CacheLine::SINGLE_CACHE_LINE, DcciDst::CACHELINE_OUT>(tempTensor);

            // Deploy the two copies on queue 0 and queue 1 respectively.
            auto handleId0 = hccl.BatchWrite<true>(sendInfo, 1U, 0U);
            auto handleId1 = hccl.BatchWrite<true>(sendInfo, 1U, 1U);

            // Block the BatchWrite communication tasks on all queues. All queues wait until all communication tasks are complete before continuing, achieving synchronization across all queues.
            const uint16_t queueNum = hccl.GetQueueNum();
            for (uint16_t i = 0U; i < queueNum; ++i) {
                hccl.QueueBarrier<ScopeType::ALL>(i);
            }

            // Finalize can exit without waiting for all communication tasks on the server side to complete, releasing AIV core resources as early as possible.
            hccl.Finalize<false>();
            AscendC::SyncAll();
        }
    }
    ```

    When the amount of communication data is large, you can call **SetAicpuBlockDim**, [SetCommBlockNum](../HCCL_Tiling/SetCommBlockNum.md), and [SetQueueNum](../HCCL_Tiling/SetQueueNum.md) in the Tiling process to improve operator performance through a concurrency mechanism.

    In the following sample code, the number of cores participating in BatchWrite communication is 24, the number of communication queues is 2, and the total number of queues is 24\*2, that is, 48. Meanwhile, the number of AI CPU cores on the server side is 4, so each AI CPU core only needs to orchestrate tasks on 48/4, that is, 12 communication queues, which improves communication efficiency.

    ```
    static ge::graphStatus BatchWriteTilingFunc(gert::TilingContext* context)
    {
        // Omit irrelevant code.
        auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
        const auto aicCoreNum = ascendcPlatform.GetCoreNumAic();
        auto coreNum = use_aiv ? aicCoreNum * 2 : aicCoreNum;
        context->SetAicpuBlockDim(4U);
        context->SetSimdNumBlocks(coreNum);
        context->SetTilingKey(1000);

        // Omit irrelevant code.
        SdmaBatchWriteCustomTilingData* tiling = context->GetTilingData<SdmaBatchWriteCustomTilingData>();
        AscendC::Mc2CcTilingConfig mc2CcTilingConfig(groupName, HCCL_CMD_BATCH_WRITE, "BatchWrite=level0:fullmesh", 0);
        mc2CcTilingConfig.SetCommBlockNum(24U);
        mc2CcTilingConfig.SetQueueNum(2U);
        mc2CcTilingConfig.GetTiling(tiling->mc2InitTiling);
        mc2CcTilingConfig.GetTiling(tiling->mc2CcTiling);
        return ge::GRAPH_SUCCESS;
    }
    ```
<!-- end id17 -->
