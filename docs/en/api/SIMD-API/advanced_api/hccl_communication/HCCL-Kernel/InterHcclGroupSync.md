# InterHcclGroupSync

<!-- md-trans-meta sourceCommit=38116dea9ef9dd53dcd766a2d868232e570c53b8 translatedAt=2026-09-10T02:21:56.349Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Not supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Used to wait for the completion of a communication task across communication domains. After this API is called, all communication tasks subsequently issued in the current communication domain wait until the communication task identified by **srcHandleID** in the communication domain identified by **srcGroupID** completes execution before they start executing.

## Prototype

```
__aicore__ inline void InterHcclGroupSync(int8_t srcGroupID, HcclHandle srcHandleID)
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| srcGroupID | Input | Communication domain ID, that is, the ID of the communication domain where the communication task awaited by subsequent communication tasks resides. |
| srcHandleID | Input | Communication task, that is, the **HcclHandle** identifier of the communication task awaited by subsequent communication tasks. |

## Return Value

None

## Constraints

-   Before calling this API, ensure that [InitV2](InitV2.md) and [SetCcTilingV2](SetCcTilingV2.md) have been called.
-   When this API is called on an AIC core or an AIV core, the core must be the same as the one on which the corresponding Prepare API is called.
-   Within a communication domain, the total number of calls to all Prepare APIs and InterHcclGroupSync must not exceed 63.

## Examples

This example constructs a communication fusion operator that has one input **xGM**, two outputs **alltoallGM** and **allgatherGM**, and **tilingGM** for passing in tiling data. The operator contains two communication domains. First, communication domain 0 performs AlltoAll communication on the input and outputs the result to **alltoallGM**. After the result data is output to **alltoallGM**, communication domain 1 uses this result **alltoallGM** as the input of AllGather communication and outputs the communication result to **allgatherGM**.

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
    if (AscendC::g_coreType == AIV) { // Use only AIV cores for communication.
        hccl0.InitV2(contextGM0, &tilingData);
        hccl1.InitV2(contextGM1, &tilingData);
        hccl0.SetCcTilingV2(offsetof(AlltoAllAllGatherCustomTilingData, alltoallTiling));
        hccl1.SetCcTilingV2(offsetof(AlltoAllAllGatherCustomTilingData, allgatherTiling));

        // Communication domain 0 issues one AlltoAll task.
        auto group0_handle = hccl0.AlltoAll(xGM, alltoallGM, dataCount, dtype, strideCount);

        // Communication domain 1 issues a cross-domain dependency task to ensure that the subsequent AllGather task in communication domain 1 starts only after the AlltoAll in communication domain 0 completes.
        hccl1.InterHcclGroupSync(0, group0_handle);
        // Communication domain 1 issues one AllGather task.
        auto group1_handle = hccl1.AllGather(alltoallGM, allgatherGM, dataCount, dtype, strideCount);

        hccl0.Commit(group0_handle);
        hccl1.Commit(group1_handle);
        hccl0.Wait(group0_handle);
        hccl1.Wait(group1_handle);

        AscendC::SyncAll<true>(); // Synchronize all AIV cores to prevent core 0 from executing too fast and calling hccl.Finalize() prematurely, which would cause other cores to hang on Wait.
        hccl0.Finalize();
        hccl1.Finalize();
    }
}
```
