# Query

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-10T02:23:53.747Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Not supported
- Atlas A3 Training Series Products/Atlas A3 Inference Series Products: Supported
- Atlas A2 Training Series Products/Atlas A2 Inference Series Products: Supported
- Atlas 200I/500 A2 Inference Product: Not supported
- Atlas Inference Series Products AI Core: Not supported
- Atlas Inference Series Products Vector Core: Not supported
- Atlas Training Series Products: Not supported

## Description

Queries the number of rounds of the communication task corresponding to **handleId** that have been completed, returning at most **repeat** rounds. By default, this API runs on all cores. You can also call [GetBlockIdx](../../../basic_api/tool_interface/system_resources_and_variables/GetBlockIdx.md) before invoking this API to specify that it runs on a specific core.

## Prototype

```
__aicore__ inline int32_t Query(HcclHandle handleId)
```

## Parameters

**Table 1**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **handleId** | Input | ID of the corresponding communication task. Only the return value of the **Prepare** primitive API can be used.<br>using HcclHandle = int8_t; |

## Return Value

-   Returns the number of times the communication task corresponding to **handleId** has been executed, with a maximum value of **repeat**.
-   Returns -1 when an exception occurs.

## Constraints

-   Before calling this API, ensure that [InitV2](InitV2.md) and [SetCcTilingV2](SetCcTilingV2.md) have been called.
-   The input parameter **handleId** can only use the return value of the API corresponding to the Prepare primitive.
-   When this API is called on an AIC core or an AIV core, the core must be consistent with the core on which the corresponding Prepare API is called.

## Examples

```
REGISTER_TILING_DEFAULT(ReduceScatterCustomTilingData); //ReduceScatterCustomTilingData is the structure defined in the corresponding operator header file.
GET_TILING_DATA_WITH_STRUCT(ReduceScatterCustomTilingData, tilingData, tilingGM);
Hccl hccl;
GM_ADDR contextGM = AscendC::GetHcclContext<0>();  // Obtain the HCCL context in this way in the AscendC custom operator kernel.
hccl.InitV2(contextGM, &tilingData);
auto ret = hccl.SetCcTilingV2(offsetof(ReduceScatterCustomTilingData, mc2CcTiling));
if (ret != HCCL_SUCCESS) {
  return;
}
if (AscendC::g_coreType == AIC) {
    auto repeat = 10;
    HcclHandle handleId = hccl.ReduceScatter(sendBuf, recvBuf, 100, HcclDataType::HCCL_DATA_TYPE_INT8, HcclReduceOp::HCCL_REDUCE_SUM, repeat);
    hccl.Commit(handleId ); // Notify the server that it can execute the preceding ReduceScatter task.
    int32_t finishedCount = hccl.Query(handleId);
    while (hccl.Query(handleId) < repeat) {} // Wait until the communication task corresponding to handleId has been executed repeat times.
    hccl.Finalize(); // No further communication tasks remain. Notify the server that it can exit after executing the preceding ReduceScatter task.
}
```

