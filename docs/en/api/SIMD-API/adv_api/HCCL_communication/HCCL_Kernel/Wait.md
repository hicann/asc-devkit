# Wait

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-02T05:39:55.227Z pushedAt=2026-11-18T04:57:10.212Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
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
- AI Core of Atlas inference products: Not supported
- Vector Core of Atlas inference products: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/HCCL_communication/HCCL_Kernel/Wait_res.md#id1 -->

## Description

Blocks the AI Core. When [fine-grained communication](Iterate.md) is not used, it waits for the communication task corresponding to **handleId** to complete. When [fine-grained communication](Iterate.md) is used, it waits for the sub-communication task of the [step size](../HCCL_Tiling/SetStepSize.md) length corresponding to **handleId** to complete. The order in which **handleId** calls the **Wait** API must be consistent with that of the **Prepare** API. By default, this API works on all cores. Users can also specify it to run on a specific core by calling [GetBlockIdx](../../../basic_api/tool_interface/system_resources_and_variables/GetBlockIdx.md) before calling it.

## Prototype

```
__aicore__ inline int32_t Wait(HcclHandle handleId)
```

## Parameters

**Table 1**  API parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| handleId | Input | Identifier ID of the corresponding communication task. Only the return value of the **Prepare** primitive API can be used.<br>using HcclHandle = int8_t; |

## Return Value

-   0: Success.
-   -1: Failure.

## Constraints

-   Before calling this API, ensure that [InitV2](InitV2.md) and [SetCcTilingV2](SetCcTilingV2.md) have been called.
-   The input parameter **handleId** can only use the return value of the corresponding Prepare primitive API.
-   When this API is called on the AIC core or AIV core, the calling core must be consistent with that of the corresponding Prepare API.
-   For non-[fine-grained communication](Iterate.md), the number of calls to this API must match the repeat count of Prepare. For [fine-grained communication](Iterate.md), the number of calls to this API must match (total number of steps of the communication task / step size) × the repeat count of Prepare. The order of the handleId values passed when calling the Wait API must be the same as that used for the Prepare API.

## Examples

```
REGISTER_TILING_DEFAULT(ReduceScatterCustomTilingData); // ReduceScatterCustomTilingData is the struct defined in the corresponding operator header file.
GET_TILING_DATA_WITH_STRUCT(ReduceScatterCustomTilingData, tilingData, tilingGM);
Hccl hccl;
GM_ADDR contextGM = AscendC::GetHcclContext<0>(); // In an AscendC custom operator kernel function (Kernel), obtain the HCCL context in this way.
__gm__ void* mc2InitTiling = (__gm__ void*)(&tiling->mc2InitTiling);
__gm__ void* mc2CcTiling = (__gm__ void*)(&(tiling->mc2CcTiling));
hccl.InitV2(contextGM, &tilingData);
auto ret = hccl.SetCcTilingV2(offsetof(ReduceScatterCustomTilingData, mc2CcTiling));
if (ret != HCCL_SUCCESS) {
    return;
}
if (AscendC::g_coreType == AIC) {
    HcclHandle handleId =
        hccl.ReduceScatter(sendBuf, recvBuf, 100, HcclDataType::HCCL_DATA_TYPE_INT8, HcclReduceOp::HCCL_REDUCE_SUM, 10);

    for (uint8_t i = 0; i < 10; i++) {
        hccl.Commit(handleId); // Notify the server that it can execute the ReduceScatter task above.
    }
    for (uint8_t i = 0; i < 10; i++) {
        hccl.Wait(handleId); // Blocking API that waits for the ReduceScatter task above to complete.
    }
    hccl.Finalize(); // No further communication tasks follow. Notify the server to exit after executing the ReduceScatter task above.
}
```
