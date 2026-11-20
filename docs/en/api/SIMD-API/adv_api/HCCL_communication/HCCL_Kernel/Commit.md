# Commit

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-02T05:26:28.117Z pushedAt=2026-11-18T04:56:25.321Z nl=2 -->

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
| handleId | Input | Identifier ID of the corresponding communication task. Only the return value of the **Prepare** primitive API can be used.<br>using HcclHandle = int8_t; |

## Return Value

None

## Constraints

-   Before calling this API, ensure that [InitV2](InitV2.md) and [SetCcTilingV2](SetCcTilingV2.md) have been called.
-   The input parameter **handleId** can only use the return value of the corresponding Prepare primitive API.
-   In non-[fine-grained communication](Iterate.md), the number of calls to this API must be consistent with the repeat count of Prepare. In [fine-grained communication](Iterate.md), the number of calls to this API must be consistent with (the total number of steps of the communication task / step size) × the repeat count of Prepare.
-   When this API is called on the AIC core or AIV core, the calling core must be consistent with that of the corresponding Prepare API.

## Examples

```
REGISTER_TILING_DEFAULT(ReduceScatterCustomTilingData); // ReduceScatterCustomTilingData is the structure defined in the corresponding operator header file.
GET_TILING_DATA_WITH_STRUCT(ReduceScatterCustomTilingData, tilingData, tilingGM);
Hccl hccl;
GM_ADDR contextGM = AscendC::GetHcclContext<0>(); // In an AscendC custom operator kernel function, obtain the HCCL context in this way.
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
        hccl.Wait(handleId); // Blocking API. Wait until the ReduceScatter task above is complete.
    }
    hccl.Finalize(); // No further communication tasks. Notify the server that it can exit after executing the ReduceScatter task above.
}
```
