# AlltoAllvWrite

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-10T02:12:30.270Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

**AlltoAllvWrite** is the task dispatch API for collective communication, which returns the task identifier **handleId** to you.

**AlltoAllvWrite** enables cards in a communication domain to send and receive data with each other, with customizable data volumes sent to and received from other cards by each card, as well as customizable memory offsets for the sent and received data. Combined with the parameters in the prototype, the API function is described as follows: the local card sends data of size sendSizes\[i\] bytes at a send address offset of sendOffsets\[i\] bytes to the i-th card, where **remoteWinOffset** indicates the address offset of the data sent by the peer card, and **localDataSize** indicates the size of the data sent to the local card. Note that the offsets and data volumes here are all in bytes.

![](../../../../figures/251208103727744.png)

## Prototype

```
template <bool commit = false>
__aicore__ inline HcclHandle AlltoAllvWrite(GM_ADDR usrIn, GM_ADDR sendOffsets, GM_ADDR sendSizes, uint64_t remoteWinOffset, uint64_t localDataSize)
```

## Parameters

**Table 1** Template parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| commit | Input | bool type. The value can be one of the following:<br>**true**: When the **Prepare** API is called, **Commit** synchronously notifies the server that the communication task can be executed.<br>**false**: When the **Prepare** API is called, the server is not notified to execute the communication task. |

**Table 2** API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| usrIn | Input | Source data buffer address. |
| sendOffsets | Input | Offset of each slice to be sent, in bytes. |
| sendSizes | Input | Data size of each slice to be sent, in bytes. |
| remoteWinOffset | Input | Data offset sent by the peer card, in bytes. |
| localDataSize | Input | Data size sent to the local card, in bytes. |

## Return Value

Returns the identifier **handleId** of the task, where **handleId** is greater than or equal to 0. If the call fails, -1 is returned.

## Constraints

-   Before calling this API, ensure that [InitV2](InitV2.md) and [SetCcTilingV2](SetCcTilingV2.md) have been called.
-   If the [config template parameter](hccl_template_parameter.md#hccl-template-params) of the HCCL object does not specify the core that dispatches the communication task, this API can be called only on either the AIC core or the AIV core. If the [config template parameter](hccl_template_parameter.md#hccl-template-params) of the HCCL object specifies the core that dispatches the communication task, this API can be called on both the AIC core and the AIV core simultaneously. Internally, the API dispatches the communication task on the corresponding AIC core or AIV core based on the type of the specified core.
-   Within a communication domain, the total number of calls to all Prepare APIs and the InterHcclGroupSync API must not exceed 63.
-   For Ascend 950PR/Ascend 950DT, when the communication server is the CCU, the maximum amount of communication data in a single operation must not exceed 256M.

## Examples

```
extern "C" __global__ __aicore__ void alltoallvwrite_custom(GM_ADDR xGM, GM_ADDR yGM, GM_ADDR workspaceGM, GM_ADDR tilingGM) {

    REGISTER_TILING_DEFAULT(AllToAllVWriteCustomTilingData); //AllToAllVWriteCustomTilingData is the structure defined in the corresponding operator header file.
    GET_TILING_DATA_WITH_STRUCT(AllToAllVWriteCustomTilingData, tilingData, tilingGM);

    auto &&cfg       = tilingData.param;
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
    __gm__ uint64_t *sendSizes = reinterpret_cast<__gm__ uint64_t *>(sendSizeGM_);
    __gm__ uint64_t *sendOffsets = reinterpret_cast<__gm__ uint64_t *>(sendOffsetGM_);
    for (uint32_t i = 0U; i < rankDim; i++) { // The current CCU communication uses dual dies, so sendSize and sendOffset must be split equally into die0 and die1 data.
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
            AscendC::SyncAll<true>();  // Synchronize all AIV cores to prevent core 0 from executing too fast and calling hccl.Finalize() prematurely, which would cause other cores to hang on Wait.
            hccl.Finalize();
        }
    }
}
```
