# AlltoAllvWrite

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-08T11:38:46.978Z pushedAt=2026-10-27T11:42:21.653Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Not supported
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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/HCCL_communication/HCCL_Kernel/AlltoAllvWrite_res.md#id1 -->

## Description

**AlltoAllvWrite** is the task dispatch API for collective communication. It returns the task identifier **handleId** to the user.

**AlltoAllvWrite** enables cards in a communicator to send data to and receive data from each other, with customized data volumes for sending to other cards and receiving from other cards, as well as customized offsets of the sent and received data in memory. Based on the parameters in the prototype, the API function is described as follows: the local card sends data with a send address offset of sendOffsets\[i\] bytes and a size of sendSizes\[i\] bytes to the i-th card, **remoteWinOffset** indicates the offset at which the local card's data is written to the peer window, and **localDataSize** indicates the size of data written by each peer card to the local card. Note: The offsets and data volumes here are all in bytes.

![](../../../../figures/251208103727744.png)

## Prototype

```
template <bool commit = false>
__aicore__ inline HcclHandle AlltoAllvWrite(GM_ADDR usrIn, GM_ADDR sendOffsets, GM_ADDR sendSizes, uint64_t remoteWinOffset, uint64_t localDataSize)
```

## Parameters

**Table 1** Template parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| commit | Input | bool type. The value is as follows:<br>**true**: When the Prepare API is called, Commit synchronously notifies the server that it can execute the communication task.<br>**false**: When the Prepare API is called, the server is not notified to execute the communication task. |

**Table 2** API parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| usrIn | Input | Source data buffer address. |
| sendOffsets | Input | Offset of each slice to be sent, in bytes. |
| sendSizes | Input | Data size of each slice to be sent, in bytes. |
| remoteWinOffset | Input | Offset at which the local card's data is written into the peer window, in bytes. |
| localDataSize | Input | Data size written by each peer card into the local card, in bytes. |

## Return Value

Returns the handleId that identifies the task. handleId is greater than or equal to 0. Returns -1 on failure.

## Constraints

-   Before calling this API, ensure that [InitV2](InitV2.md) and [SetCcTilingV2](SetCcTilingV2.md) have been called.
-   If the [template parameter config](HCCL_template_params.md#hccl-template-params) of the HCCL object does not specify the core for dispatching the communication task, this API can be called only on either the AIC core or the AIV core. If the [template parameter config](HCCL_template_params.md#hccl-template-params) of the HCCL object specifies the core for dispatching the communication task, this API can be called on both the AIC core and the AIV core simultaneously. Internally, based on the type of the specified core, the API dispatches the communication task on the corresponding AIC core or AIV core.
-   Within a communicator, the total number of calls to all Prepare APIs and the InterHcclGroupSync API cannot exceed 63.
<!-- npu="950" id7 -->
-   For Ascend 950PR&950DT products, when the communication server is the CCU, only the FullMesh algorithm is supported by default, and the maximum communication data volume per operation cannot exceed 256M.
<!-- end id7 -->

## Examples

```
extern "C" __global__ __aicore__ void alltoallvwrite_custom(
    GM_ADDR xGM, GM_ADDR yGM, GM_ADDR workspaceGM, GM_ADDR tilingGM)
{
    REGISTER_TILING_DEFAULT(
        AllToAllVWriteCustomTilingData); // AllToAllVWriteCustomTilingData is the structure defined in the corresponding operator header file.
    GET_TILING_DATA_WITH_STRUCT(AllToAllVWriteCustomTilingData, tilingData, tilingGM);

    auto&& cfg = tilingData.param;
    uint32_t M = cfg.M;
    uint32_t K = cfg.K;
    uint32_t dataType = cfg.dataType;
    uint32_t dataTypeSize = cfg.dataTypeSize;

    KERNEL_TASK_TYPE_DEFAULT(KERNEL_TYPE_MIX_AIC_1_2);
    Hccl<HcclServerType::HCCL_SERVER_TYPE_CCU> hccl;
    GM_ADDR context = GetHcclContext<HCCL_GROUP_ID_0>();
    hccl.InitV2(context, &tilingData);
    hccl.SetCcTilingV2(offsetof(AllToAllVCustomV3TilingData, mc2CcTiling));
    uint32_t rankDim = hccl.GetRankDim();
    uint32_t rankId = hccl.GetRankId();

    uint64_t perRankDataSize_ = M * K * dataTypeSize / rankDim;
    GM_ADDR sendSizeGM_ = workspaceGM;
    GM_ADDR sendOffsetGM_ = sendSizeGM_ + rankDim * sizeof(uint64_t) * 2;
    __gm__ uint64_t* sendSizes = reinterpret_cast<__gm__ uint64_t*>(sendSizeGM_);
    __gm__ uint64_t* sendOffsets = reinterpret_cast<__gm__ uint64_t*>(sendOffsetGM_);
    // The current CCU communication uses dual-die, so sendSize and sendOffset need to be split equally into die0 and die1 data.
    for (uint32_t i = 0U; i < rankDim; i++) { 
        sendSizes[i] = perRankDataSize_ / 2;
        sendSizes[i + rankDim] = perRankDataSize_ - perRankDataSize_ / 2;
        sendOffsets[i] = i * perRankDataSize_;
        sendOffsets[i + rankDim] = i * perRankDataSize_ + sendSizes[i];
    }
    uint64_t remoteWinOffset = rankId * perRankDataSize_;
    uint64_t localDataSize = perRankDataSize_;
    if (TILING_KEY_IS(1000UL)) {
        if ASCEND_IS_AIV {
            AscendC::HcclHandle handleId = -1;
            handleId = hccl.AlltoAllvWrite<true>(xGM, sendOffsetGM_, sendSizeGM_, remoteWinOffset, localDataSize);
            hccl.Wait(handleId);
            AscendC::SyncAll<true>(); // Synchronize all AIV cores to prevent core 0 from executing too fast and calling hccl.Finalize() prematurely, which would cause other cores to hang on Wait.
            hccl.Finalize();
        }
    }
}
```
