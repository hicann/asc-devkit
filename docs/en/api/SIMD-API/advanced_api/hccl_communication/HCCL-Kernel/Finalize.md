# Finalize

<!-- md-trans-meta sourceCommit=6024fbdeffe5f05ee90ea6f5e52eb8c784f03267 translatedAt=2026-09-10T02:15:51.650Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Notifies the server that no further communication tasks will follow, and exits after execution completes.

## Prototype

```
template <bool sync = true>
__aicore__ inline void Finalize()
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| sync | Input | Whether to wait for the server to complete the communication task. The parameter is of the bool type. The values are as follows:<br>true: default value, indicating that the client detects and waits for the last communication task to complete.<br>false: indicating that the client exits directly without waiting for the communication task to complete.<br><br>For Atlas A3 Training Series Products/Atlas A3 Inference Series Products, this parameter supports the default value true, and supports the value false only when the communication task is [BatchWrite](BatchWrite.md).<br><br>For Atlas A2 Training Series Products/Atlas A2 Inference Series Products, this parameter supports only the default value true. |

## Return Value

None

## Constraints

-   Before calling this API, ensure that [InitV2](InitV2.md) and [SetCcTilingV2](SetCcTilingV2.md) have been called.
-   When this API is called on an AIC core or an AIV core, the calling core must be consistent with that of the corresponding **Prepare** interface.

## Examples

```
REGISTER_TILING_DEFAULT(ReduceScatterCustomTilingData); //ReduceScatterCustomTilingData is the structure defined in the corresponding operator header file.
GET_TILING_DATA_WITH_STRUCT(ReduceScatterCustomTilingData, tilingData, tilingGM);
Hccl hccl;
GM_ADDR contextGM = AscendC::GetHcclContext<0>();  // Obtain the HCCL context in this way in the AscendC custom operator kernel.
hccl.InitV2(contextGM, &tilingData);
hccl.SetCcTilingV2(offsetof(ReduceScatterCustomTilingData, mc2CcTiling));
if (AscendC::g_coreType == AIC) {
    HcclHandle handleId = hccl.ReduceScatter(sendBuf, recvBuf, 100, HcclDataType::HCCL_DATA_TYPE_INT8, HcclReduceOp::HCCL_REDUCE_SUM, 10);
    hccl.Commit(handleId ); // Notify the server that it can execute the preceding ReduceScatter task.
    for (uint8_t i=0; i<10; i++) {
        hccl.Wait(handleId); // Blocking API. Wait until the preceding ReduceScatter task is complete.
    }
    hccl.Finalize<true>(); // No further communication tasks follow. Notify the server that it can exit after executing the preceding ReduceScatter task.
}
```
