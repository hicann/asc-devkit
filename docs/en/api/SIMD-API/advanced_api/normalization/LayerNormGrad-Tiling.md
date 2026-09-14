# LayerNormGrad Tiling

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-10T02:42:23.545Z -->

## Description

The functions of **LayerNormGrad Tiling** are as follows:

-   Obtain the maximum and minimum temporary space sizes to be reserved/applied for on the host side:

    The computation of the **LayerNormGrad** API on the kernel side requires you to reserve/apply for temporary space. The **GetLayerNormGradMaxMinTmpSize** API is used to obtain the maximum and minimum temporary space sizes to be reserved/applied for on the host side. Based on this range, you can select an appropriate space size and pass it to the kernel side as a Tiling parameter.

    -   To ensure correct functionality, the reserved/applied temporary space size must not be smaller than the minimum temporary space size.
    -   Within the range from the minimum to the maximum temporary space size, as the temporary space increases, the computation performance of the kernel-side API improves to a certain extent. To achieve better performance, you can reserve/apply for space based on the actual memory usage.

-   Obtain the Tiling parameters required by the LayerNormGrad kernel-side API through **GetLayerNormGradNDTilingInfo**. You need to pass in the input shape, the remaining space available for the **LayerNormGrad** API computation, and the data type used for computation.

    The **LayerNormGrad Tiling** struct is defined as follows. You do not need to pay attention to the specific information of this Tiling structure. You only need to pass it to the kernel side and then pass it into the LayerNormGrad high-level API for direct use.

    ```
    struct LayerNormGradTiling {
        uint32_t stackBufferSize = 0;
        uint32_t bLength = 0;
        uint32_t sLength = 0;
        uint32_t hLength = 0;
        uint32_t originalHLength = 0;
        uint32_t oneCalSize = 0;
        uint32_t nohCalSize = 0;
        uint32_t loopNum = 0;
        uint32_t tailSize = 0;
        uint32_t nohTailSize = 0;
        uint32_t tmpTensorBSHPos = 0;
        uint32_t tmpTensorBSHSize = 0;
        uint32_t pdVarTensorPos = 0;
        uint32_t pdVarTensorSize = 0;
        uint32_t pdMeanTensorPos = 0;
        uint32_t pdMeanTensorSize = 0;
        uint32_t x1TensorPos = 0;
        uint32_t x1TensorSize = 0;
        uint32_t x2TensorPos = 0;
        uint32_t x2TensorSize = 0;
        uint32_t x3TensorPos = 0;
        uint32_t x3TensorSize = 0;
        uint32_t tmpTensorPos = 0;
        uint32_t tmpTensorSize = 0;
        uint32_t tmpTensor1Pos = 0;
        uint32_t tmpTensor1Size = 0;
        uint32_t tmpTensor2Pos = 0;
        uint32_t tmpTensor2Size = 0;
        uint32_t lastDimValueBack = 0;
        uint32_t lastDimValueBackMulTwo = 0;
    };
    ```

## Prototype

```
void GetLayerNormGradMaxMinTmpSize(const ge::Shape& srcShape, const uint32_t typeSize, const bool isReuseSource, uint32_t& maxValue, uint32_t& minValue)
```

```
void GetLayerNormGradNDTilingInfo(const ge::Shape srcShape, const uint32_t stackBufferSize, const uint32_t typeSize, const bool isReuseSource, optiling::LayerNormGradTiling& tiling)
```

```
void GetLayerNormGradNDTilingInfo(const ge::Shape srcShape, const uint32_t stackBufferSize, const uint32_t typeSize, const bool isReuseSource, AscendC::tiling::LayerNormGradTiling& tiling)
```

## Parameters

**Table 1** Parameter list of the **GetLayerNormGradMaxMinTmpSize** API

| Parameter Name | Input/Output | Description |
| --- | --- | --- |
| **srcShape** | Input | Shape information {B, S, storageHLength, originHLength} of the input data inputDy, including the shape information of the current input inputDy and the original shape information before address alignment (if H-axis padding is performed).<br><br>In the scenarios supported by the API, storageHLength and originHLength are consistent. |
| **typeSize** | Input | Size of the input data type, in bytes. For example, if the input data type is half, pass 2. |
| **isReuseSource** | Input | Whether to reuse the memory space of the source operand, consistent with the [LayerNorm](LayerNorm.md) API. |
| **maxValue** | Output | Maximum temporary space size required for the **LayerNormGrad** API to complete computation. Space exceeding this value is not used by the API. Within the range from the minimum temporary space to the maximum temporary space, as the temporary space increases, the computation performance of the kernel-side API is optimized to a certain extent. To achieve better performance, you can reserve/apply for space based on the actual memory usage. A maximum space size of 0 indicates that no temporary space is required for computation.<br>**maxValue** is only a reference value and may be larger than the remaining space of the Unified Buffer. In this scenario, you need to select an appropriate temporary space size based on the remaining space of the Unified Buffer. |
| **minValue** | Output | Minimum temporary space size required for the **LayerNormGrad** API to complete computation. To ensure functional correctness, the temporary space reserved/applied for during API computation must not be smaller than this value. A minimum space size of 0 indicates that no temporary space is required for computation. |

**Table 2** Parameter list of the **GetLayerNormGradNDTilingInfo** API

| Parameter Name | Input/Output | Description |
| --- | --- | --- |
| **srcShape** | Input | Shape information of the input data **inputDy**, including the shape information of the current input and the original shape information before address alignment. |
| **stackBufferSize** | Input | Space size available for the API, in number of elements. |
| **typeSize** | Input | Size of the input data type, in bytes. For example, if the input data type is **half**, pass 2. |
| **isReuseSource** | Input | Whether the memory space of **inputX** and **inputDy** can be reused. |
| **tiling** | Output | Tiling information of the input data. |

## Return Value Description

None

## Constraints

None

## Examples

The following example describes how to obtain the Tiling parameters on the host side when using the **LayerNormGrad** high-level API, and how these parameters are used on the kernel side. In the example, the shape of the input tensor is \[2, 16, 64\], and the input data type is half.

1.  Add the **LayerNormGradTiling** struct parameter to the **TilingData** struct as a field of the **TilingData** struct.

    ```
    BEGIN_TILING_DATA_DEF(TilingData)               // Register a tiling class, using the tiling name as the input parameter.
      TILING_DATA_FIELD_DEF(uint32_t, totalLength); // Add a tiling field for the total amount of data to compute.
      TILING_DATA_FIELD_DEF(uint32_t, tileNum);     // Add a tiling field for the number of data blocks on each core.
      ...                                           // Add other tiling fields.
      TILING_DATA_FIELD_DEF_STRUCT(LayerNormGradTiling, layernormGradTilingData); // Add the LayerNormGradTiling structure parameter to the TilingData structure.
    END_TILING_DATA_DEF;
    ```

2.  In the Tiling implementation function, first call the **GetLayerNormGradMaxMinTmpSize** API to obtain the maximum/minimum temporary space size required for the **LayerNormGrad** API to complete computation. Based on this range and the actual memory usage, set an appropriate space size. Then call the **GetLayerNormGradNDTilingInfo** API to obtain the Tiling parameters required by the LayerNormGradBeta kernel-side API based on the input shape, the remaining available space size, and other information.

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
        // This sample is for illustration only. The minimum value obtained by GetLayerNormGradMaxMinTmpSize is passed in to ensure correct functionality. You can pass in an appropriate space size as needed.
        uint32_t max;
        uint32_t min;
        AscendC::GetLayerNormGradMaxMinTmpSize(srcShape, sizeof(half), false, max, min);
        // Obtain the LayerNormGrad Tiling parameters.
        AscendC::GetLayerNormGradNDTilingInfo(srcShape, min, sizeof(half), false, tiling.layernormGradTilingData);
         ... // Other logic.
        tiling.SaveToBuffer(context->GetRawTilingData()->GetData(), context->GetRawTilingData()->GetCapacity());
        context->GetRawTilingData()->SetDataSize(tiling.GetDataSize());
        context->SetTilingKey(1);
        return ge::GRAPH_SUCCESS;
    }
    } // namespace optiling
    ```

3.  On the kernel side, the corresponding kernel function obtains the **TilingData** by calling **GET\_TILING\_DATA**, and then passes the LayerNormGradTiling information in the **TilingData** to the **LayerNormGrad** API for computation. For the complete kernel-side sample, see [Examples](LayerNormGrad.md#examples).

    ```
    extern "C" __global__ __aicore__ void func_custom(GM_ADDR x, GM_ADDR y, GM_ADDR z, GM_ADDR workspace, GM_ADDR tiling)
    {
        GET_TILING_DATA(tilingData, tiling);
        KernelFunc op;
        op.Init(x, y, z, tilingData.totalLength, tilingData.tileNum,tilingData.layernormGradTilingData);
        if (TILING_KEY_IS(1)) {
            op.Process();
        }
    }
    ```
