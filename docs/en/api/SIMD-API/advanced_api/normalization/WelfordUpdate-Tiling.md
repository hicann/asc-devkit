# WelfordUpdate Tiling

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-10T02:51:09.404Z -->

## Description

Ascend C provides the WelfordUpdate Tiling API, which helps you obtain the Tiling parameters required for WelfordUpdate kernel computation.

The main steps for obtaining the Tiling parameters are as follows:

Specifically, use **GetWelfordUpdateMaxMinTmpSize** to obtain the maximum and minimum temporary buffer sizes required for WelfordUpdate computation.

The **WelfordUpdate** API on the kernel side requires you to reserve/apply for temporary buffer space during computation. **GetWelfordUpdateMaxMinTmpSize** is used on the host side to obtain the maximum and minimum temporary buffer sizes to reserve/apply for. Based on this range, you can select an appropriate buffer size and pass it to the kernel side as a Tiling parameter.

-   To ensure correct functionality, the reserved/applied temporary buffer size must not be smaller than the minimum temporary buffer size.
-   Within the range from the minimum to the maximum temporary buffer size, as the temporary buffer size increases, the computation performance of the kernel-side API improves to a certain extent. To achieve better performance, you can reserve/apply for buffer space based on the actual memory usage.

## Function Prototype

```
void GetWelfordUpdateMaxMinTmpSize(const ge::Shape& srcShape, const uint32_t typeSizeT, const uint32_t typeSizeU, const bool isReuseSource, const bool isInplace, uint32_t& maxValue, uint32_t& minValue)
```

## Parameters

**Table 1** Parameters of the **GetWelfordUpdateMaxMinTmpSize** API

| Parameter | Input/Output | Description |
| --- | --- | --- |
| srcShape | Input | Input shape information {rnLength, abLength}. The meanings of rnLength and abLength are the same as those in the [WelfordUpdate](WelfordUpdate.md) API. |
| typeSizeT | Input | Data type size of the input (inputX), in bytes. For example, if the input data type is half, pass 2. |
| typeSizeU | Input | Data type size of the mean and variance (outputMean, outputVariance, inputMean, and inputVariance), in bytes. For example, if the input data type is float, pass 4. |
| isReuseSource | Input | Whether the source operand can be modified. This is the same as in the [WelfordUpdate](WelfordUpdate.md) API. |
| isInplace | Input | Whether the destination operand reuses the source operand. This is the same as in the [WelfordUpdate](WelfordUpdate.md) API. |
| maxValue | Output | Maximum temporary space size required for the **WelfordUpdate** API to complete computation. Space beyond this value is not used by the API. Within the range from the minimum temporary space to the maximum temporary space, as the temporary space increases, the computation performance of the kernel-side API is optimized to a certain extent. To achieve better performance, you can reserve or apply for space based on the actual memory usage. A maximum space size of 0 indicates that no temporary space is required for computation.<br>maxValue is only a reference value and may be larger than the remaining space of the Unified Buffer. In this case, you need to select an appropriate temporary space size based on the remaining space of the Unified Buffer. |
| minValue | Output | Minimum temporary space size required for the **WelfordUpdate** API to complete computation. To ensure correct functionality, the temporary space reserved or applied for during API computation must not be smaller than this value. A minimum space size of 0 indicates that no temporary space is required for computation. |

## Return Value

None

## Constraints

None

## Examples

1.  Add the **WelfordUpdate** Tiling struct parameters to the **TilingData** struct as a field of the **TilingData** struct.

    ```
    BEGIN_TILING_DATA_DEF(WelfordUpdateCustomTilingData) // Register a tiling class, using the tiling name as the input parameter.
      TILING_DATA_FIELD_DEF(uint32_t, inplace); // Add a tiling field indicating whether the output reuses the input.
      TILING_DATA_FIELD_DEF(uint32_t, nLength);
      TILING_DATA_FIELD_DEF(uint32_t, rLength);
      TILING_DATA_FIELD_DEF(uint32_t, abComputeLength);
      TILING_DATA_FIELD_DEF(uint32_t, nRec);
    END_TILING_DATA_DEF;
    REGISTER_TILING_DATA_CLASS(WelfordUpdateCustom, WelfordUpdateCustomTilingData) // Add the WelfordUpdateCustomTilingData structure parameters to the TilingData structure.
    ```

2.  In the Tiling implementation function, first call the **GetWelfordUpdateMaxMinTmpSize** API to obtain the maximum and minimum temporary space sizes required for the **WelfordUpdate** API to complete the computation. Set an appropriate space size within this range based on the actual memory usage. Then, obtain the tiling parameters required by the WelfordUpdate kernel-side API based on the input shape, the remaining space available for computation, and other information.

    ```
    namespace optiling {
    static ge::graphStatus TilingFunc(gert::TilingContext *context)
    {
        WelfordUpdateCustomTilingData tiling;
        const gert::RuntimeAttrs *attrs = context->GetAttrs();
        const uint32_t inplace = *(attrs->GetAttrPointer<uint32_t>(0));
        const uint32_t abComputeLength = *(attrs->GetAttrPointer<uint32_t>(1));
        const uint32_t sharedtmpbuffer = *(attrs->GetAttrPointer<uint32_t>(2));

        const gert::StorageShape *x1_shape = context->GetInputShape(1);
        const gert::Shape shape = x1_shape->GetStorageShape();
        auto nLength = shape.GetDim(0);
        auto rLength = shape.GetDim(1);

        std::vector<int64_t> srcDims = {nLength, rLength};
        ge::Shape srcShape(srcDims);

        uint32_t maxTmpsize = 0;
        uint32_t minTmpsize = 0;
        // This is only an example. Obtain the minimum value through GetWelfordUpdateMaxMinTmpSize and pass it in to ensure correct functionality. You can pass in an appropriate space size as needed.
        AscendC::GetWelfordUpdateMaxMinTmpSize(srcShape, 4, 4, false, false, maxTmpsize, minTmpsize);
        // auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
        // AscendC::GetWelfordUpdateMaxMinTmpSize(srcShape, 4, 4, false, false, ascendcPlatform, maxTmpsize, minTmpsize);
        ... // Other logic.
        context->SetTilingKey(1);
        tiling.SaveToBuffer(context->GetRawTilingData()->GetData(), context->GetRawTilingData()->GetCapacity());
        context->GetRawTilingData()->SetDataSize(tiling.GetDataSize());
        size_t *currentWorkspace = context->GetWorkspaceSizes(1);
        currentWorkspace[0] = 0;
        return ge::GRAPH_SUCCESS;
    }
    } // namespace optiling
    ```

3.  On the corresponding kernel side, call **GET\_TILING\_DATA** in the kernel function to obtain the **TilingData**, and then pass the **WelfordUpdate Tiling** information in the **TilingData** to the **WelfordUpdate** API for computation. For the complete kernel-side example, see [WelfordUpdate](WelfordUpdate.md).

    ```
    extern "C" __global__ __aicore__ void
    welford_update_custom(
        GM_ADDR inputX_gm, GM_ADDR mean_gm, GM_ADDR var_gm, GM_ADDR outputMean_gm, GM_ADDR outputVariance_gm, GM_ADDR workspace, GM_ADDR tiling)
    {
        GET_TILING_DATA(tilingData, tiling);
        if (TILING_KEY_IS(1))
        {
            if (tilingData.inplace)
            {
                KernelWelfordUpdate<DTYPE_INPUTX, DTYPE_U, true> op;
                op.Init(inputX_gm, mean_gm, var_gm, outputMean_gm, outputVariance_gm, tilingData.nLength, tilingData.rLength, tilingData.abComputeLength);
                op.Process();
            }
            else
            {
                KernelWelfordUpdate<DTYPE_INPUTX, DTYPE_U, false> op;
                op.Init(inputX_gm, mean_gm, var_gm, outputMean_gm, outputVariance_gm, tilingData.nLength, tilingData.rLength, tilingData.abComputeLength);
                op.Process();
            }
        }
    }
    ```
