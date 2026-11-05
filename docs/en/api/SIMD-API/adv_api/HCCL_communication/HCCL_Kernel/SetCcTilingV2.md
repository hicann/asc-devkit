# SetCcTilingV2

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-08T11:56:22.762Z pushedAt=2026-10-27T11:46:26.671Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/HCCL_communication/HCCL_Kernel/SetCcTilingV2_res.md#id1 -->

## Description

Used to set the TilingData address of a communication algorithm configuration in the HCCL client.

## Prototype

```
__aicore__ inline int32_t SetCcTilingV2(uint64_t offset)
```

## Parameters

**Table 1**  API parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| offset | Input | Offset of the communication algorithm configuration [Mc2CcTiling](../HCCL_Tiling/TilingData_struct.md#table678914014562) parameter address relative to the start address of [Mc2InitTiling](../HCCL_Tiling/TilingData_struct.md#table4835205712588). [Mc2CcTiling](../HCCL_Tiling/TilingData_struct.md#table678914014562) is computed on the Host side. For details, see [Table 2 Mc2CcTiling parameter description](../HCCL_Tiling/TilingData_struct.md#table678914014562). It is passed by the framework to the kernel function (Kernel) for use. |

## Return Value

-   HCCL\_SUCCESS, indicating success.
-   HCCL\_FAILED, indicating failure.

## Constraints

-   If this API is called, ensure that [InitV2](InitV2.md) is called before this API.
-   For the same communication algorithm with identical Tiling parameters, this API needs to be called only once before the Prepare API is called. For details, see the example: [Communication with different types and different Tiling parameters](#li71505119260).
-   For the same communication algorithm, if the Tiling parameters differ, calling this API repeatedly overwrites the previous Tiling parameter address. Therefore, call this API again after the Prepare API to set the new Tiling parameters. For details, see the example: [Communication with the same type and different Tiling parameters](#li1163031215116).
-   If this API is called, the TilingData structure must be defined using standard C++ syntax.

## Examples

-   User-defined TilingData structure:

    ```
    class UserCustomTilingData {
        AscendC::tiling::Mc2InitTiling initTiling;
        AscendC::tiling::Mc2CcTiling allGatherTiling;
        AscendC::tiling::Mc2CcTiling allReduceTiling1;
        AscendC::tiling::Mc2CcTiling allReduceTiling2;
        CustomTiling param;
    };
    ```

-   <a name="li71505119260"></a>Communication with different types and different Tiling parameters

    ```
    extern "C" __global__ __aicore__ void userKernel(GM_ADDR aGM, GM_ADDR workspaceGM, GM_ADDR tilingGM)
    {
        REGISTER_TILING_DEFAULT(UserCustomTilingData);
        GET_TILING_DATA_WITH_STRUCT(UserCustomTilingData, tilingData, tilingGM);

        Hccl hccl;
        GM_ADDR contextGM = AscendC::GetHcclContext<0>();
        hccl.InitV2(contextGM, &tilingData);

        // Set the corresponding tiling through SetCcTilingV2 before dispatching the task.
        if (hccl.SetCcTilingV2(offsetof(UserCustomTilingData, allGatherTiling)) != HCCL_SUCCESS ||
            hccl.SetCcTilingV2(offsetof(UserCustomTilingData, allReduceTiling1)) != HCCL_SUCCESS) {
            return;
        }
        const auto agHandleId = hccl.AllGather<true>(sendBuf, recvBuf, dataCount, HcclDataType::HCCL_DATA_TYPE_FP16);
        hccl.Wait(agHandleId);

        const auto arHandleId = hccl.AllReduce<true>(
            sendBuf, recvBuf, dataCount, HcclDataType::HCCL_DATA_TYPE_FP16, HcclReduceOp::HCCL_REDUCE_SUM);
        hccl.Wait(arHandleId);

        hccl.Finalize();
    }
    ```

-   <a name="li1163031215116"></a>Communication with the same type and different Tiling parameters

    ```
    extern "C" __global__ __aicore__ void userKernel(GM_ADDR aGM, GM_ADDR workspaceGM, GM_ADDR tilingGM)
    {
        REGISTER_TILING_DEFAULT(UserCustomTilingData);
        GET_TILING_DATA_WITH_STRUCT(UserCustomTilingData, tilingData, tilingGM);

        Hccl hccl;
        GM_ADDR contextGM = AscendC::GetHcclContext<0>();
        hccl.InitV2(contextGM, &tilingData);

        // Before dispatching the communication task, set the corresponding Tiling parameter address through SetCcTilingV2.
        if (hccl.SetCcTilingV2(offsetof(UserCustomTilingData, allReduceTiling1)) != HCCL_SUCCESS) {
            return;
        }
        const auto arHandleId1 = hccl.AllReduce<true>(
            sendBuf, recvBuf, dataCount, HcclDataType::HCCL_DATA_TYPE_FP16, HcclReduceOp::HCCL_REDUCE_SUM);
        hccl.Wait(arHandleId1);

        // The Tiling parameters of the second AllReduce differ from those of the first. Call SetCcTilingV2 again after the first Prepare.
        if (hccl.SetCcTilingV2(offsetof(UserCustomTilingData, allReduceTiling2)) != HCCL_SUCCESS) {
            return;
        }
        const auto arHandleId2 = hccl.AllReduce<true>(
            sendBuf, recvBuf, dataCount, HcclDataType::HCCL_DATA_TYPE_FP16, HcclReduceOp::HCCL_REDUCE_SUM);
        hccl.Wait(arHandleId2);

        hccl.Finalize();
    }
    ```
