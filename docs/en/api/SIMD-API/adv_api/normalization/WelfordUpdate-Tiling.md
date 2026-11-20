# WelfordUpdate Tiling

<!-- md-trans-meta sourceCommit=aa6d602ebc3dbc8f7b50f91dc72aceefdfa67b49 translatedAt=2026-11-10T06:35:09.563Z pushedAt=2026-11-18T11:07:26.853Z nl=2 -->

## Description

Ascend C provides the WelfordUpdate Tiling API to help you obtain the tiling parameters required for WelfordUpdate kernel function computation.

The main steps for obtaining tiling parameters are as follows:

Specifically, use **GetWelfordUpdateMaxMinTmpSize** to obtain the maximum and minimum temporary space sizes required for the WelfordUpdate API to complete computation.

Computation by the WelfordUpdate API on the kernel function side requires you to reserve/allocate temporary space. **GetWelfordUpdateMaxMinTmpSize** is used on the host side to obtain the maximum and minimum temporary space sizes to reserve/allocate. Based on this range, you select an appropriate space size and pass it to the kernel function side as a tiling parameter.

-   To ensure correct functionality, the reserved or allocated temporary space size must not be smaller than the minimum temporary space size.
-   Within the range from the minimum to the maximum temporary space size, as the temporary space increases, the computation performance of the kernel-side API improves to a certain extent. To achieve better performance, you can reserve or allocate space based on the actual memory usage.

## Prototype

```
void GetWelfordUpdateMaxMinTmpSize(const AscendC::TensorShape& srcShape, const uint32_t typeSizeT, const uint32_t typeSizeU, const bool isReuseSource, const bool isInplace, uint32_t& maxValue, uint32_t& minValue)
```

## Parameters

**Table 1** Parameter description of the GetWelfordUpdateMaxMinTmpSize API

| Parameter | Input/Output | Description |
| --- | --- | --- |
| srcShape | Input | Input shape information {rnLength, abLength}, of the type [AscendC::TensorShape](../data_structures/TensorShape.md). rnLength and abLength have the same meanings as those in the [WelfordUpdate](WelfordUpdate.md) API. |
| typeSizeT | Input | Size of the input (inputX) data type, in bytes. For example, if the input data type is half, pass 2 here. |
| typeSizeU | Input | Size of the mean and variance (outputMean, outputVariance, inputMean, inputVariance) data type, in bytes. For example, if the input data type is float, pass 4 here. |
| isReuseSource | Input | Whether the source operand can be modified. Same as the [WelfordUpdate](WelfordUpdate.md) API. |
| isInplace | Input | Whether the destination operand reuses the source operand. Same as the [WelfordUpdate](WelfordUpdate.md) API. |
| maxValue | Output | Maximum temporary space size required for the WelfordUpdate API to complete computation, in bytes. Space exceeding this value is not used by this API. Within the range from the minimum to the maximum temporary space size, as the temporary space increases, the computation performance of the kernel function side API improves to a certain extent. To achieve better performance, you can reserve/allocate space based on the actual memory usage. A maximum space size of 0 indicates that no temporary space is required for computation.<br>maxValue is only a reference value and may be larger than the remaining space of the Unified Buffer (UB). In this case, you need to select an appropriate temporary space size based on the remaining UB space. |
| minValue | Output | Minimum temporary space size required for the WelfordUpdate API to complete computation, in bytes. To ensure functional correctness, the temporary space reserved/allocated during API computation must not be smaller than this value. A minimum space size of 0 indicates that no temporary space is required for computation. |

## Return Value

None

## Constraints

None

## Examples

1.  Add the WelfordUpdate Tiling structure parameters to the TilingData structure as a field of the TilingData structure.

    ```
    BEGIN_TILING_DATA_DEF(WelfordUpdateCustomTilingData) // Register a tiling class, using the tiling name as the input parameter.
        TILING_DATA_FIELD_DEF(uint32_t, inplace);            // Add a tiling field indicating whether output reuses input.
        TILING_DATA_FIELD_DEF(uint32_t, nLength);
        TILING_DATA_FIELD_DEF(uint32_t, rLength);
        TILING_DATA_FIELD_DEF(uint32_t, abComputeLength);
        TILING_DATA_FIELD_DEF(uint32_t, nRec);
    END_TILING_DATA_DEF;
    REGISTER_TILING_DATA_CLASS(
        WelfordUpdateCustom,
        WelfordUpdateCustomTilingData) // Add the WelfordUpdateCustomTilingData structure parameters to the TilingData structure.
    ```

2.  In the Tiling implementation function, first call **GetWelfordUpdateMaxMinTmpSize** to obtain the maximum/minimum temporary space size required for the WelfordUpdate API to complete computation. Set an appropriate space size within this range based on the actual memory usage, and then obtain the tiling parameters required by the WelfordUpdate kernel function side API based on the input shape, the remaining space available for computation, and other information.

    ```
    namespace optiling {
    static ge::graphStatus TilingFunc(gert::TilingContext* context)
    {
        WelfordUpdateCustomTilingData tiling;
        const gert::RuntimeAttrs* attrs = context->GetAttrs();
        const uint32_t inplace = *(attrs->GetAttrPointer<uint32_t>(0));
        const uint32_t abComputeLength = *(attrs->GetAttrPointer<uint32_t>(1));
        const uint32_t sharedtmpbuffer = *(attrs->GetAttrPointer<uint32_t>(2));

        const gert::StorageShape* x1_shape = context->GetInputShape(1);
        const gert::Shape shape = x1_shape->GetStorageShape();
        auto nLength = shape.GetDim(0);
        auto rLength = shape.GetDim(1);

        std::vector<int64_t> srcDims = {nLength, rLength};
        AscendC::TensorShape srcShape(srcDims);

        uint32_t maxTmpsize = 0;
        uint32_t minTmpsize = 0;
        // This sample is for illustration only. The minimum value obtained by GetWelfordUpdateMaxMinTmpSize is  to ensure functional correctness. You can  an appropriate space size as needed.
        AscendC::GetWelfordUpdateMaxMinTmpSize(srcShape, 4, 4, false, false, maxTmpsize, minTmpsize);
        // auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
        // AscendC::GetWelfordUpdateMaxMinTmpSize(srcShape, 4, 4, false, false, ascendcPlatform, maxTmpsize, minTmpsize);
        ... // Other logic.
        context->SetTilingKey(1);
        tiling.SaveToBuffer(context->GetRawTilingData()->GetData(), context->GetRawTilingData()->GetCapacity());
        context->GetRawTilingData()->SetDataSize(tiling.GetDataSize());
        size_t* currentWorkspace = context->GetWorkspaceSizes(1);
        currentWorkspace[0] = 0;
        return ge::GRAPH_SUCCESS;
    }
    } // namespace optiling
    ```

3.  On the corresponding kernel function side, obtain the TilingData by calling GET\_TILING\_DATA in the kernel function, and then pass the WelfordUpdate Tiling information in the TilingData to the WelfordUpdate API for computation. For the complete kernel function-side sample, see [WelfordUpdate](WelfordUpdate.md).

    ```
    extern "C" __global__ __aicore__ void welford_update_custom(
        GM_ADDR inputX_gm, GM_ADDR mean_gm, GM_ADDR var_gm, GM_ADDR outputMean_gm, GM_ADDR outputVariance_gm,
        GM_ADDR workspace, GM_ADDR tiling)
    {
        GET_TILING_DATA(tilingData, tiling);
        if (TILING_KEY_IS(1)) {
            if (tilingData.inplace) {
                KernelWelfordUpdate<DTYPE_INPUTX, DTYPE_U, true> op;
                op.Init(
                    inputX_gm, mean_gm, var_gm, outputMean_gm, outputVariance_gm, tilingData.nLength, tilingData.rLength,
                    tilingData.abComputeLength);
                op.Process();
            } else {
                KernelWelfordUpdate<DTYPE_INPUTX, DTYPE_U, false> op;
                op.Init(
                    inputX_gm, mean_gm, var_gm, outputMean_gm, outputVariance_gm, tilingData.nLength, tilingData.rLength,
                    tilingData.abComputeLength);
                op.Process();
            }
        }
    }
    ```
