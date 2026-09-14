# LayerNormGradBeta Tiling

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-10T02:43:45.112Z -->

## Description

The functions of LayerNormGradBeta Tiling are as follows:

-   Obtain the maximum and minimum temporary space sizes to reserve or apply for on the host side:

    The computation of the **LayerNormGradBeta** API on the kernel side requires you to reserve or apply for temporary space. The **GetLayerNormGradBetaMaxMinTmpSize** API is used to obtain the maximum and minimum temporary space sizes to reserve or apply for on the host side. Based on this range, you can select an appropriate space size and pass it to the kernel side as a Tiling parameter.

    -   To ensure correct functionality, the reserved or applied temporary space size must not be smaller than the minimum temporary space size.
    -   Within the range from the minimum to the maximum temporary space size, as the temporary space increases, the computation performance of the kernel-side API improves to a certain extent. To achieve better performance, you can reserve or apply for space based on the actual memory usage.

-   Obtain the Tiling parameters required by the LayerNormGradBeta kernel-side API through **GetLayerNormGradBetaNDTilingInfo**. You need to pass the input shape, the remaining space size available for the **LayerNormGradBeta** API computation, and the data type used for computation.

    The definition of the LayerNormGradBeta Tiling structure is as follows. You do not need to pay attention to the specific information of this Tiling structure; you only need to pass it to the kernel side and then to the LayerNormGradBeta high-level API for direct use.

    ```
    struct LayerNormGradBetaTiling {
        uint32_t stackBufferSize = 0;
        uint32_t bLength = 0;
        uint32_t sLength = 0;
        uint32_t hLength = 0;
        uint32_t originalHLength = 0;
        uint32_t bshLength = 0;
        uint32_t bsLength = 0;
        uint32_t oneCalSize = 0;
        uint32_t numberOfTmpBuf = 0;
        uint32_t loopRound = 0;
        uint32_t inputTailSize = 0;
        uint32_t inputTailPos = 0;
        uint32_t bsTailSize = 0;
        uint32_t bshCurLength = 0;
        uint32_t bsCurLength = 0;
        uint32_t gammaTempTensorPos = 0;
        uint32_t betaTempTensorPos = 0;
        uint32_t inputDyTmpTensorPos = 0;
        uint32_t resForGammaTmpTensorPos = 0;
        uint32_t reserved = 0;
    };
    ```

## Prototype

```
void GetLayerNormGradBetaMaxMinTmpSize(const ge::Shape& srcShape, const uint32_t typeSize, const bool isReuseSource, uint32_t& maxValue, uint32_t& minValue)
```

```
void GetLayerNormGradBetaNDTilingInfo(const ge::Shape srcShape, const uint32_t stackBufferSize, const uint32_t typeSize, const bool isReuseSource, optiling::LayerNormGradBetaTiling& tiling)
```

```
void GetLayerNormGradBetaNDTilingInfo(const ge::Shape srcShape, const uint32_t stackBufferSize, const uint32_t typeSize, const bool isReuseSource, AscendC::tiling::LayerNormGradBetaTiling& tiling)
```

## Parameters

**Table 1** Parameter list of the GetLayerNormGradBetaMaxMinTmpSize API

| Parameter | Input/Output | Description |
| --- | --- | --- |
| srcShape | Input | Shape information {B, S, storageHLength, originHLength} of the input data **inputDy**, including the shape information of the current input **inputDy** and the original shape information before address alignment (for example, when H-axis padding is performed).<br><br>In the scenarios supported by the API, **storageHLength** and **originHLength** remain the same. |
| typeSize | Input | Size of the input data type, in bytes. For example, if the input data type is **half**, pass 2. |
| isReuseSource | Input | Whether to reuse the memory space of the source operand, consistent with the [LayerNorm](LayerNorm.md) API. |
| maxValue | Output | Maximum temporary space size required for the **LayerNormGradBeta** API to complete computation. Space exceeding this value is not used by the API. Within the range from the minimum temporary space to the maximum temporary space, as the temporary space increases, the computation performance of the kernel-side API is optimized to a certain extent. To achieve better performance, you can reserve/apply for space based on the actual memory usage. A maximum space size of 0 indicates that no temporary space is required for computation.<br>**maxValue** is only a reference value and may be larger than the remaining space of the Unified Buffer. In this case, you need to select an appropriate temporary space size based on the remaining space of the Unified Buffer. |
| minValue | Output | Minimum temporary space size required for the **LayerNormGradBeta** API to complete computation. To ensure functional correctness, the temporary space reserved/applied for during API computation must not be smaller than this value. A minimum space size of 0 indicates that no temporary space is required for computation. |

**Table 2** Parameter list of the GetLayerNormGradBetaNDTilingInfo API

| Parameter | Input/Output | Description |
| --- | --- | --- |
| srcShape | Input | Shape information of the input data **inputDy**, including the shape information of the current input and the original shape information before address alignment. |
| stackBufferSize | Input | Space size available for the API, in number of elements. |
| typeSize | Input | Size of the input data type, in bytes. For example, if the input data type is **half**, pass 2. |
| isReuseSource | Input | Whether the memory space of **inputDy** can be reused. |
| tiling | Output | Tiling information of the input data. |

## Return Value

None

## Constraints

None

## Examples

The following example describes the process of obtaining Tiling parameters on the host side when using the LayerNormGradBeta high-level API, and how these parameters are used on the kernel side. In the example, the input tensor has a shape of \[2, 16, 64\], and the input data type is half.

1.  Add the **LayerNormGradBetaTiling** struct parameters to the **TilingData** struct as a field of the **TilingData** struct.

    ```
    BEGIN_TILING_DATA_DEF(TilingData)               // Register a tiling class, using the tiling name as the input parameter.
      TILING_DATA_FIELD_DEF(uint32_t, totalLength); // Add a tiling field for the total amount of data to compute.
      TILING_DATA_FIELD_DEF(uint32_t, tileNum);     // Add a tiling field for the number of data blocks per core.
      ...                                           // Add other tiling fields.
      TILING_DATA_FIELD_DEF_STRUCT(LayerNormGradBetaTiling, layernormGradBetaTilingData); // Add the LayerNormGradBetaTiling struct parameters to the TilingData struct.
    END_TILING_DATA_DEF;
    ```

2.  In the Tiling implementation function, first call the **GetLayerNormGradBetaMaxMinTmpSize** API to obtain the maximum/minimum temporary space size required for the **LayerNormGradBeta** API to complete the computation. Set an appropriate space size within this range based on the actual memory usage, and then call the **GetLayerNormGradBetaNDTilingInfo** API to obtain the Tiling parameters required by the LayerNormGradBeta kernel-side API based on the input shape, the remaining space available for computation, and other information.

    ```
    namespace optiling {
    const uint32_t NUM_BLOCKS = 8;
    const uint32_t TILE_NUM = 8;
    static ge::graphStatus TilingFunc(gert::TilingContext* context)
    {
        TilingData tiling;
        uint32_t totalLength = context->GetInputTensor(0)->GetShapeSize();
        context->SetBlockDim(NUM_BLOCKS);
        tiling.set_totalLength(totalLength);
        tiling.set_tileNum(TILE_NUM);
        // Set other Tiling parameters.
        ...
        // {B, S, storageHLength, originHLength}
        std::vector<int64_t> shapeVec = {2, 16, 64, 64};
        ge::Shape srcShape(shapeVec);
        // This is only an example. The minimum value obtained by GetLayerNormGradBetaMaxMinTmpSize is passed in to ensure correct functionality. You can pass in an appropriate space size as needed.
        uint32_t max;
        uint32_t min;
        AscendC::GetLayerNormGradBetaMaxMinTmpSize(srcShape, sizeof(half), false, max, min);
        // Obtain the LayerNormGradBeta Tiling parameters.
        AscendC::GetLayerNormGradBetaNDTilingInfo(srcShape, min, sizeof(half), false, tiling.layernormGradBetaTilingData);
         ... // Other logic.
        tiling.SaveToBuffer(context->GetRawTilingData()->GetData(), context->GetRawTilingData()->GetCapacity());
        context->GetRawTilingData()->SetDataSize(tiling.GetDataSize());
        context->SetTilingKey(1);
        return ge::GRAPH_SUCCESS;
    }
    } // namespace optiling
    ```

3.  On the kernel side, the corresponding kernel function calls **GET\_TILING\_DATA** to obtain the **TilingData**, and then passes the **LayerNormGradBetaTiling** information in the **TilingData** to the **LayerNormGradBeta** API for computation. For the complete kernel-side example, see [LayerNormGradBeta](LayerNormGradBeta.md).

    ```
    extern "C" __global__ __aicore__ void func_custom(GM_ADDR x, GM_ADDR y, GM_ADDR z, GM_ADDR workspace, GM_ADDR tiling)
    {
        GET_TILING_DATA(tilingData, tiling);
        KernelFunc op;
        op.Init(x, y, z, tilingData.totalLength, tilingData.tileNum,tilingData.layernormGradBetaTilingData);
        if (TILING_KEY_IS(1)) {
            op.Process();
        }
    }
    ```
