# Query

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-08T11:53:47.244Z pushedAt=2026-10-27T11:45:52.807Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/HCCL_communication/HCCL_Kernel/Query_res.md#id1 -->

## Description

Queries the number of rounds completed by the communication task corresponding to **handleId**, returning at most **repeat** rounds. By default, this API works on all cores. Users can also specify it to run on a specific core by calling [GetBlockIdx](../../../basic_api/tool_interface/system_resources_and_variables/GetBlockIdx.md) before invocation.

## Prototype

```
__aicore__ inline int32_t Query(HcclHandle handleId)
```

## Parameters

**Table 1**  API parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **handleId** | Input | Identifier of the corresponding communication task. Only the return value of the **Prepare** primitive can be used.<br>using HcclHandle = int8_t; |

## Return Value

-   Returns the number of times the communication task corresponding to **handleId** has been executed, with a maximum value of **repeat**.
-   Returns -1 when an exception occurs during execution.

## Constraints

-   Before calling this API, ensure that the [InitV2](InitV2.md) and [SetCcTilingV2](SetCcTilingV2.md) APIs have been called.
-   The input parameter **handleId** can only use the return value of the API corresponding to the Prepare primitive.
-   When this API is called on the AI Core or AI Vector Core, the calling core must be consistent with that of the corresponding Prepare API.

## Examples

```
REGISTER_TILING_DEFAULT(ReduceScatterCustomTilingData); // ReduceScatterCustomTilingData is the structure defined in the corresponding operator header file.
GET_TILING_DATA_WITH_STRUCT(ReduceScatterCustomTilingData, tilingData, tilingGM);
Hccl hccl;
GM_ADDR contextGM = AscendC::GetHcclContext<0>(); // In the AscendC custom operator kernel, obtain the HCCL context in this way.
hccl.InitV2(contextGM, &tilingData);
auto ret = hccl.SetCcTilingV2(offsetof(ReduceScatterCustomTilingData, mc2CcTiling));
if (ret != HCCL_SUCCESS) {
    return;
}
if (AscendC::g_coreType == AIC) {
    auto repeat = 10;
    HcclHandle handleId = hccl.ReduceScatter(
        sendBuf, recvBuf, 100, HcclDataType::HCCL_DATA_TYPE_INT8, HcclReduceOp::HCCL_REDUCE_SUM, repeat);
    hccl.Commit(handleId); // Notify the server that the preceding ReduceScatter task can be executed.
    int32_t finishedCount = hccl.Query(handleId);
    while (hccl.Query(handleId) < repeat) {
    }                // Wait until the communication task corresponding to handleId has been executed repeat times.
    hccl.Finalize(); // No other communication tasks follow. Notify the server that it can exit after executing the preceding ReduceScatter task.
}
```
