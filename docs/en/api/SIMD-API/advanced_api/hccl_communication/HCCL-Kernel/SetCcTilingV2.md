# SetCcTilingV2

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-10T02:26:13.155Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Sets the TilingData address of a communication algorithm configuration in the HCCL client.

## Prototype

```
__aicore__ inline int32_t SetCcTilingV2(uint64_t offset)
```

## Parameters

**Table 1** API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| offset | Input | Offset of the parameter address of the communication algorithm configuration [Mc2CcTiling](../HCCL-Tiling/tilingdata_structure.md#table678914014562) relative to the start address of [Mc2InitTiling](../HCCL-Tiling/tilingdata_structure.md#table4835205712588). [Mc2CcTiling](../HCCL-Tiling/tilingdata_structure.md#table678914014562) is computed on the Host side. For details, see [Table 2 Mc2CcTiling parameter description](../HCCL-Tiling/tilingdata_structure.md#table678914014562). It is passed by the framework to the Kernel function for use. |

## Return Value

-   HCCL\_SUCCESS, indicates success.
-   HCCL\_FAILED, indicates failure.

## Constraints

-   If this API is called, ensure that [InitV2](InitV2.md) is called before this API.
-   For the same communication algorithm with identical **Tiling** parameters, this API needs to be called only once before the **Prepare** API is called. See the example: [Communication with different types and different Tiling parameters](#li71505119260).
-   For the same communication algorithm, if the **Tiling** parameters differ, calling this API repeatedly overwrites the previous Tiling parameter address. Therefore, call this API again after the **Prepare** API is called to set the new Tiling parameters. See the example: [Communication with the same type and different Tiling parameters](#li1163031215116).
-   If this API is called, the **TilingData** structure must be defined using standard C++ syntax.

## Examples

-   User-defined **TilingData** structure:

    ```
    class UserCustomTilingData {
        AscendC::tiling::Mc2InitTiling initTiling;
        AscendC::tiling::Mc2CcTiling allGatherTiling;
        AscendC::tiling::Mc2CcTiling allReduceTiling1;
        AscendC::tiling::Mc2CcTiling allReduceTiling2;
        CustomTiling param;
    };
    ```

-   <a name="li71505119260"></a>Communications with different types and different Tiling parameters

    ```
    extern "C" __global__ __aicore__ void userKernel(GM_ADDR aGM, GM_ADDR workspaceGM, GM_ADDR tilingGM) {
        REGISTER_TILING_DEFAULT(UserCustomTilingData);
        GET_TILING_DATA_WITH_STRUCT(UserCustomTilingData, tilingData, tilingGM);

        Hccl hccl;
        GM_ADDR contextGM = AscendC::GetHcclContext<0>();
        hccl.InitV2(contextGM, &tilingData);

        // Before dispatching the task, set the corresponding tiling through SetCcTilingV2.
        if (hccl.SetCcTilingV2(offsetof(UserCustomTilingData, allGatherTiling)) != HCCL_SUCCESS ||
            hccl.SetCcTilingV2(offsetof(UserCustomTilingData, allReduceTiling1)) != HCCL_SUCCESS) {
            return;
        }
        const auto agHandleId = hccl.AllGather<true>(sendBuf, recvBuf, dataCount, HcclDataType::HCCL_DATA_TYPE_FP16);
        hccl.Wait(agHandleId);

        const auto arHandleId = hccl.AllReduce<true>(sendBuf, recvBuf, dataCount, HcclDataType::HCCL_DATA_TYPE_FP16, HcclReduceOp::HCCL_REDUCE_SUM);
        hccl.Wait(arHandleId);

        hccl.Finalize();
    }
    ```

-   <a name="li1163031215116"></a>Communications with the same type but different Tiling parameters

    ```
    extern "C" __global__ __aicore__ void userKernel(GM_ADDR aGM, GM_ADDR workspaceGM, GM_ADDR tilingGM) {
        REGISTER_TILING_DEFAULT(UserCustomTilingData);
        GET_TILING_DATA_WITH_STRUCT(UserCustomTilingData, tilingData, tilingGM);

        Hccl hccl;
        GM_ADDR contextGM = AscendC::GetHcclContext<0>();
        hccl.InitV2(contextGM, &tilingData);

        // Before dispatching the communication task, set the corresponding Tiling parameter address through SetCcTilingV2.
        if (hccl.SetCcTilingV2(offsetof(UserCustomTilingData, allReduceTiling1)) != HCCL_SUCCESS) {
            return;
        }
        const auto arHandleId1 = hccl.AllReduce<true>(sendBuf, recvBuf, dataCount, HcclDataType::HCCL_DATA_TYPE_FP16, HcclReduceOp::HCCL_REDUCE_SUM);
        hccl.Wait(arHandleId1);

        // The Tiling parameters of the second AllReduce differ from those of the first. Call SetCcTilingV2 again after the first Prepare.
        if (hccl.SetCcTilingV2(offsetof(UserCustomTilingData, allReduceTiling2)) != HCCL_SUCCESS) {
            return;
        }
        const auto arHandleId2 = hccl.AllReduce<true>(sendBuf, recvBuf, dataCount, HcclDataType::HCCL_DATA_TYPE_FP16, HcclReduceOp::HCCL_REDUCE_SUM);
        hccl.Wait(arHandleId2);

        hccl.Finalize();
    }
    ```

