# Wait

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-10T02:26:51.078Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Blocks the AI Core. In non-[fine-grained communication](Iterate-99.md) scenarios, it waits for the communication task corresponding to **handleId** to complete. In [fine-grained communication](Iterate-99.md) scenarios, it waits for the sub-communication task of the [step size](../HCCL-Tiling/SetStepSize.md) length corresponding to **handleId** to complete. The order in which **handleId** calls the **Wait** API must be consistent with that of the **Prepare** API. By default, this API runs on all cores. You can also specify it to run on a single core by calling [GetBlockIdx](../../../basic_api/tool_interface/system_resources_and_variables/GetBlockIdx.md) before invoking it.

## Prototype

```
__aicore__ inline int32_t Wait(HcclHandle handleId)
```

## Parameters

**Table 1** API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| handleId | Input | Identifier ID of the corresponding communication task. Only the return value of the **Prepare** primitive API can be used.<br>using HcclHandle = int8_t; |

## Return Value

-   0: Success.
-   -1: Failure.

## Constraints

-   Before calling this API, ensure that [InitV2](InitV2.md) and [SetCcTilingV2](SetCcTilingV2.md) have been called.
-   The input parameter **handleId** can only use the return value of the corresponding Prepare primitive API.
-   When this API is called on an AIC core or an AIV core, the core must be consistent with the core on which the corresponding Prepare API is called.
-   In non-[fine-grained communication](Iterate-99.md) scenarios, the number of calls to this API must be consistent with the repeat count of Prepare. In [fine-grained communication](Iterate-99.md) scenarios, the number of calls to this API must be consistent with the repeat count of the total number of steps/step size \*Prepare. The order in which **handleId** calls the Wait API must be consistent with that of the Prepare API.

## Examples

```
REGISTER_TILING_DEFAULT(ReduceScatterCustomTilingData); //ReduceScatterCustomTilingData is the structure defined in the corresponding operator header file.
GET_TILING_DATA_WITH_STRUCT(ReduceScatterCustomTilingData, tilingData, tilingGM);
Hccl hccl;
GM_ADDR contextGM = AscendC::GetHcclContext<0>();  // In the AscendC custom operator kernel, obtain the HCCL context in this way.
__gm__ void *mc2InitTiling = (__gm__ void *)(&tiling->mc2InitTiling);
__gm__ void *mc2CcTiling = (__gm__ void *)(&(tiling->mc2CcTiling));
hccl.InitV2(contextGM, &tilingData);
auto ret = hccl.SetCcTilingV2(offsetof(ReduceScatterCustomTilingData, mc2CcTiling));
if (ret != HCCL_SUCCESS) {
    return;
}
if (AscendC::g_coreType == AIC) {
    HcclHandle handleId = hccl.ReduceScatter(sendBuf, recvBuf, 100, HcclDataType::HCCL_DATA_TYPE_INT8, HcclReduceOp::HCCL_REDUCE_SUM, 10);

    for (uint8_t i=0; i<10; i++) {
        hccl.Commit(handleId ); // Notify the server that it can execute the preceding ReduceScatter task.
    }
    for (uint8_t i=0; i<10; i++) {
        hccl.Wait(handleId); // Blocking API. Wait until the preceding ReduceScatter task is complete.
    }
    hccl.Finalize(); // No further communication tasks remain. Notify the server that it can exit after executing the preceding ReduceScatter task.
}
```

