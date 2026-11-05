# Commit

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-08T11:42:30.010Z pushedAt=2026-10-27T11:42:50.896Z nl=2 -->

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
- Atlas inference products AI Core: Not supported
- Vector Core of Atlas inference products: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/HCCL_communication/HCCL_Kernel/Commit_res.md#id1 -->

## Description

When not in [fine-grained communication](Iterate.md), each call to this API notifies the server to execute the task corresponding to **handleId** once. In [fine-grained communication](Iterate.md), each call to this API notifies the server to execute a sub-communication task of the [step size](../HCCL_Tiling/SetStepSize.md) length corresponding to **handleId**.

## Prototype

```
__aicore__ inline void Commit(HcclHandle handleId)
```

## Parameters

**Table 1**  API parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **handleId** | Input | ID of the corresponding communication task. Only the return value of the **Prepare** primitive API can be used.<br>using HcclHandle = int8_t; |

## Return Value

None

## Constraints

-   Before calling this API, ensure that [InitV2](InitV2.md) and [SetCcTilingV2](SetCcTilingV2.md) have been called.
-   The input parameter **handleId** can only use the return value of the corresponding Prepare primitive API.
-   In non-[fine-grained communication](Iterate.md), the number of calls to this API must be consistent with the repeat count of Prepare. In [fine-grained communication](Iterate.md), the number of calls to this API must be consistent with (the total number of steps of the communication task / step size) × the repeat count of Prepare.
-   When this API is called on an AIC core or an AIV core, the calling core must be consistent with that of the corresponding Prepare API.

## Examples

```
REGISTER_TILING_DEFAULT(ReduceScatterCustomTilingData); // ReduceScatterCustomTilingData is the structure defined in the header file of the corresponding operator.
GET_TILING_DATA_WITH_STRUCT(ReduceScatterCustomTilingData, tilingData, tilingGM);
Hccl hccl;
GM_ADDR contextGM = AscendC::GetHcclContext<0>(); // In the AscendC custom operator kernel, obtain the HCCL context in this way.
hccl.InitV2(contextGM, &tilingData);
auto ret = hccl.SetCcTilingV2(offsetof(ReduceScatterCustomTilingData, mc2CcTiling));
if (ret != HCCL_SUCCESS) {
    return;
}
if (AscendC::g_coreType == AIC) {
    HcclHandle handleId =
        hccl.ReduceScatter(sendBuf, recvBuf, 100, HcclDataType::HCCL_DATA_TYPE_INT8, HcclReduceOp::HCCL_REDUCE_SUM, 10);

    for (uint8_t i = 0; i < 10; i++) {
        hccl.Commit(handleId); // Notify the server that the above ReduceScatter task can be executed.
    }
    for (uint8_t i = 0; i < 10; i++) {
        hccl.Wait(handleId); // Blocking API. Wait until the above ReduceScatter task is completed.
    }
    hccl.Finalize(); // No other communication tasks follow. Notify the server that it can exit after the above ReduceScatter task is executed.
}
```
