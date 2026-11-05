# InterHcclGroupSync

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-08T11:50:42.531Z pushedAt=2026-10-27T11:45:25.713Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Not supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Supported
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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/HCCL_communication/HCCL_Kernel/InterHcclGroupSync_res.md#id1 -->

## Description

Waits for a communication task across communicators to complete. After this API is called, all communication tasks subsequently issued in the current communicator wait until the communication task identified by **srcHandleID** in the communicator identified by **srcGroupID** completes before they start executing.

## Prototype

```
__aicore__ inline void InterHcclGroupSync(int8_t srcGroupID, HcclHandle srcHandleID)
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **srcGroupID** | Input | Communicator ID, that is, the ID of the communicator in which the communication task to be waited for resides. |
| **srcHandleID** | Input | Communication task, that is, the HcclHandle identifier of the communication task to be waited for. |

## Return Value

None

## Constraints

-   Before calling this API, ensure that [InitV2](InitV2.md) and [SetCcTilingV2](SetCcTilingV2.md) have been called.
-   When this API is called on an AIC core or an AIV core, the core must be consistent with the core on which the corresponding **Prepare** API is called.
-   Within a communicator, the total number of calls to all **Prepare** APIs and **InterHcclGroupSync** must not exceed 63.

## Examples

This example constructs a communication fusion operator that has one input **xGM**, two outputs **alltoallGM** and **allgatherGM**, and **tilingGM** for passing tiling data. The operator contains two communicators. First, communicator 0 performs AlltoAll communication on the input and outputs the result to **alltoallGM**. After the result data is output to **alltoallGM**, communicator 1 uses the result **alltoallGM** as the input of AllGather communication and outputs the communication result to **allgatherGM**.

```
extern "C" __global__ __aicore__ void alltoall_allgather_custom(GM_ADDR xGM, GM_ADDR alltoallGM, GM_ADDR allgatherGM, GM_ADDR tilingGM)
{
    REGISTER_TILING_DEFAULT(
        AlltoAllAllGatherCustomTilingData); // AlltoAllAllGatherCustomTilingData is the structure defined in the corresponding operator header file.
    GET_TILING_DATA_WITH_STRUCT(AlltoAllAllGatherCustomTilingData, tilingData, tilingGM);
    GM_ADDR contextGM0 = AscendC::GetHcclContext<0>();
    GM_ADDR contextGM1 = AscendC::GetHcclContext<1>();

    Hccl hccl0;
    Hccl hccl1;
    HcclDataType dtype = HcclDataType::HCCL_DATA_TYPE_FP16;
    const uint64_t dataCount = 10U;
    const uint64_t strideCount = 0U;
    if (AscendC::g_coreType == AIV) { // Use only the AIV core for communication.
        hccl0.InitV2(contextGM0, &tilingData);
        hccl1.InitV2(contextGM1, &tilingData);
        hccl0.SetCcTilingV2(offsetof(AlltoAllAllGatherCustomTilingData, alltoallTiling));
        hccl1.SetCcTilingV2(offsetof(AlltoAllAllGatherCustomTilingData, allgatherTiling));

        // Submit one AlltoAll task on communicator 0.
        auto group0_handle = hccl0.AlltoAll(xGM, alltoallGM, dataCount, dtype, strideCount);

        // Submit a cross-domain dependency task on communicator 1 to ensure that the subsequent AllGather task on communicator 1 starts only after the AlltoAll task on communicator 0 finishes.
        hccl1.InterHcclGroupSync(0, group0_handle);
        // Submit one AllGather task on communicator 1.
        auto group1_handle = hccl1.AllGather(alltoallGM, allgatherGM, dataCount, dtype, strideCount);

        hccl0.Commit(group0_handle);
        hccl1.Commit(group1_handle);
        hccl0.Wait(group0_handle);
        hccl1.Wait(group1_handle);

        AscendC::SyncAll<true>(); // Synchronize all AIV cores to prevent core 0 from running too fast and calling the hccl.Finalize() API in advance, which would cause other cores to hang in Wait.
        hccl0.Finalize();
        hccl1.Finalize();
    }
}
```
