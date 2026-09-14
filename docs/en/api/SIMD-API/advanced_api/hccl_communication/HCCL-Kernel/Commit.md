# Commit

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-10T02:14:11.802Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 Inference Product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

In non-[fine-grained communication](Iterate-99.md) mode, each call to this API notifies the server to execute the task corresponding to **handleId** once. In [fine-grained communication](Iterate-99.md) mode, each call to this API notifies the server to execute a sub-communication task of the [step size](../HCCL-Tiling/SetStepSize.md) length corresponding to **handleId**.

## Function Prototype

```
__aicore__ inline void Commit(HcclHandle handleId)
```

## Parameters

**Table 1** API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| handleId | Input | ID of the corresponding communication task. Only the return value of the **Prepare** primitive API can be used.<br>using HcclHandle = int8_t; |

## Return Value

None

## Constraints

-   Before calling this API, ensure that [InitV2](InitV2.md) and [SetCcTilingV2](SetCcTilingV2.md) have been called.
-   The input parameter **handleId** can only use the return value of the corresponding **Prepare** primitive interface.
-   In non-[Fine-grained Communication](Iterate-99.md) mode, the number of calls to this API should be consistent with the **repeat** count of **Prepare**. In [Fine-grained Communication](Iterate-99.md) mode, the number of calls to this API should be consistent with the **repeat** count of the total number of steps/step size \*Prepare of the communication task.
-   When this API is called on an AIC core or AIV core, the calling core must be consistent with that of the corresponding **Prepare** API.

## Examples

```
REGISTER_TILING_DEFAULT(ReduceScatterCustomTilingData); //ReduceScatterCustomTilingData is a structure defined in the corresponding operator header file.
GET_TILING_DATA_WITH_STRUCT(ReduceScatterCustomTilingData, tilingData, tilingGM);
Hccl hccl;
GM_ADDR contextGM = AscendC::GetHcclContext<0>();  // In an AscendC custom operator kernel, obtain the HCCL context in this way.
hccl.InitV2(contextGM, &tilingData);
auto ret = hccl.SetCcTilingV2(offsetof(ReduceScatterCustomTilingData, mc2CcTiling));
if (ret != HCCL_SUCCESS) {
    return;
}
if (AscendC::g_coreType == AIC) {
    HcclHandle handleId = hccl.ReduceScatter(sendBuf, recvBuf, 100, HcclDataType::HCCL_DATA_TYPE_INT8, HcclReduceOp::HCCL_REDUCE_SUM, 10);

    for (uint8_t i=0; i<10; i++) {
        hccl.Commit(handleId); // Notify the server that it can execute the ReduceScatter task above.
    }
    for (uint8_t i=0; i<10; i++) {
        hccl.Wait(handleId); // Blocking API. Wait until the ReduceScatter task above is complete.
    }
    hccl.Finalize(); // No further communication tasks remain. After notifying the server to execute the ReduceScatter task above, it can exit.
}
```

