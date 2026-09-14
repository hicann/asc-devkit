# WelfordFinalize Tiling

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-10T02:49:57.725Z -->

## Description

Ascend C provides the **WelfordFinalize Tiling** API to help you obtain the Tiling parameters required for WelfordFinalize kernel computation.

The main steps for obtaining Tiling parameters are as follows:

Specifically, use **GetWelfordFinalizeMaxMinTmpSize** to obtain the maximum and minimum temporary space sizes required for WelfordFinalize computation.

The WelfordFinalize API on the kernel side requires you to reserve/apply for temporary space for computation. **GetWelfordFinalizeMaxMinTmpSize** is used on the host side to obtain the maximum and minimum temporary space sizes to reserve/apply for. Based on this range, you can select an appropriate space size and pass it to the kernel side as a Tiling parameter.

-   To ensure correct functionality, the reserved/applied temporary space size must not be smaller than the minimum temporary space size.
-   Within the range from the minimum to the maximum temporary space size, as the temporary space increases, the computation performance of the kernel-side API improves to a certain extent. To achieve better performance, you can reserve/apply for space based on the actual memory usage.

## Function Prototype

```
void GetWelfordFinalizeMaxMinTmpSize(const ge::Shape& srcShape, const uint32_t typeSize, const bool isReuseSource, uint32_t& maxValue, uint32_t& minValue)
```

## Parameters

**Table 1** Parameter description of the **GetWelfordFinalizeMaxMinTmpSize** API

| Parameter | Input/Output | Description |
| --- | --- | --- |
| srcShape | Input | Shape information {abLength} of the input **inputMean**/**inputVariance**. |
| typeSize | Input | Data type size of the input **inputMean**/**inputVariance**, in bytes. For example, if the input data type is float, pass 4 here. |
| isReuseSource | Input | Whether the source operand can be modified. The value of this parameter is the same as that of the [WelfordFinalize](WelfordFinalize.md) API. |
| maxValue | Output | Maximum temporary space size required for the **WelfordFinalize** API to complete computation. Space beyond this value is not used by this API. Within the range from the minimum temporary space to the maximum temporary space, as the temporary space increases, the computation performance of the kernel-side API improves to a certain extent. To achieve better performance, you can reserve/apply for space based on the actual memory usage. A maximum space size of 0 indicates that no temporary space is required for computation.<br>**maxValue** is only a reference value and may be larger than the remaining space of the Unified Buffer. In this case, you need to select an appropriate temporary space size based on the remaining space of the Unified Buffer. |
| minValue | Output | Minimum temporary space size required for the **WelfordFinalize** API to complete computation. To ensure correct functionality, the temporary space reserved/applied for during API computation must not be smaller than this value. A minimum space size of 0 indicates that no temporary space is required for computation. |

## Return Value Description

None

## Constraints

None

## Examples

1.  Add the **WelfordFinalizeTiling** struct parameters to the **TilingData** struct as a field of the **TilingData** struct.

    ```
    BEGIN_TILING_DATA_DEF(WelfordFinalizeCustomTilingData) // Register a tiling class, using the tiling name as the input parameter.
      TILING_DATA_FIELD_DEF(uint32_t, isCounts); // Add a tiling field.
      TILING_DATA_FIELD_DEF(uint32_t, rnLength);
      TILING_DATA_FIELD_DEF(uint32_t, abLength);
      TILING_DATA_FIELD_DEF(uint32_t, rLength);
      TILING_DATA_FIELD_DEF(uint32_t, head);
      TILING_DATA_FIELD_DEF(uint32_t, headLength);
      TILING_DATA_FIELD_DEF(uint32_t, tail);
      TILING_DATA_FIELD_DEF(uint32_t, tailLength);
    END_TILING_DATA_DEF;
    REGISTER_TILING_DATA_CLASS(WelfordFinalizeCustom, WelfordFinalizeCustomTilingData)// Add the WelfordFinalizeCustomTilingData structure parameters to the TilingData structure.
    ```

2.  In the Tiling implementation function, first call the **GetWelfordFinalizeMaxMinTmpSize** API to obtain the maximum and minimum temporary space sizes required for the **WelfordFinalize** API to complete computation. Set an appropriate space size within this range based on the actual memory usage, and then obtain the tiling parameters required by the WelfordFinalize kernel-side API based on information such as the input shape and the remaining space available for computation.

    ```
    namespace optiling {
    static ge::graphStatus TilingFunc(gert::TilingContext *context)
    {
        WelfordFinalizeCustomTilingData tiling;
        const gert::RuntimeAttrs *attrs = context->GetAttrs();
        const uint32_t isCounts = *(attrs->GetAttrPointer<uint32_t>(0));
        const uint32_t rnLength = *(attrs->GetAttrPointer<uint32_t>(1));
        const uint32_t abLength = *(attrs->GetAttrPointer<uint32_t>(2));
        const uint32_t rLength = *(attrs->GetAttrPointer<uint32_t>(3));
        const uint32_t head = *(attrs->GetAttrPointer<uint32_t>(4));
        const uint32_t headLength = *(attrs->GetAttrPointer<uint32_t>(5));
        const uint32_t tail = *(attrs->GetAttrPointer<uint32_t>(6));
        const uint32_t tailLength = *(attrs->GetAttrPointer<uint32_t>(7));

        std::vector<int64_t> srcDims = {abLength};
        ge::Shape srcShape(srcDims);

        // This sample is for illustration only. The minimum value obtained by GetWelfordFinalizeMaxMinTmpSize is passed in to ensure functional correctness. You can pass in an appropriate space size as needed.
        uint32_t maxTmpsize = 0;
        uint32_t minTmpsize = 0;
        AscendC::GetWelfordFinalizeMaxMinTmpSize(srcShape, 4, false, maxTmpsize, minTmpsize);

        // auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
        // AscendC::GetWelfordFinalizeMaxMinTmpSize(srcShape, 4, false, ascendcPlatform, maxTmpsize, minTmpsize);
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

3.  On the corresponding kernel side, obtain the **TilingData** by calling **GET\_TILING\_DATA** in the kernel function, and then pass the WelfordFinalize Tiling information in the **TilingData** to the **WelfordFinalize** API for computation. For the complete kernel-side sample, see [WelfordFinalize](WelfordFinalize.md).

    ```
    extern "C" __global__ __aicore__ void
    welford_finalize_custom(
        GM_ADDR inputX_gm, GM_ADDR mean_gm, GM_ADDR var_gm, GM_ADDR outputMean_gm, GM_ADDR outputVariance_gm, GM_ADDR workspace, GM_ADDR tiling)
    {
        GET_TILING_DATA(tilingData, tiling);
        if (TILING_KEY_IS(1))
        {
            if (tilingData.isCounts)
            {
                KernelWelfordFinalize<int32_t, true> op;
                op.Init(inputX_gm, mean_gm, var_gm, outputMean_gm, outputVariance_gm, tilingData.rnLength, tilingData.abLength, tilingData.rLength, tilingData.head, tilingData.headLength, tilingData.tail, tilingData.tailLength);
                op.Process();
            }
            else
            {
                KernelWelfordFinalize<int32_t, false> op;
                op.Init(inputX_gm, mean_gm, var_gm, outputMean_gm, outputVariance_gm, tilingData.rnLength, tilingData.abLength, tilingData.rLength, tilingData.head, tilingData.headLength, tilingData.tail, tilingData.tailLength);
                op.Process();
            }
        }
    }
    ```
