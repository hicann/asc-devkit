# LayerNormGrad Tiling

<!-- md-trans-meta sourceCommit=aa6d602ebc3dbc8f7b50f91dc72aceefdfa67b49 translatedAt=2026-11-10T06:34:39.836Z pushedAt=2026-11-18T11:07:26.847Z nl=2 -->

## Description

LayerNormGrad Tiling provides the following functions:

-   Obtain the maximum and minimum temporary space sizes to reserve/allocate on the host side:

    The LayerNormGrad API computation on the kernel side requires you to reserve/allocate temporary space. The **GetLayerNormGradMaxMinTmpSize** API obtains the maximum and minimum temporary space sizes to reserve/allocate on the host side. Based on this range, you can select an appropriate space size and pass it to the kernel side as a Tiling parameter.

    -   To ensure correct functionality, the reserved or allocated temporary space size must not be smaller than the minimum temporary space size.
    -   Within the range from the minimum to the maximum temporary space size, as the temporary space increases, the computation performance of the kernel-side API improves to a certain extent. To achieve better performance, you can reserve or allocate space based on the actual memory usage.

-   Obtain the Tiling parameters required by the LayerNormGrad kernel-side API through **GetLayerNormGradNDTilingInfo**. You need to pass the input shape, the remaining space size available for the LayerNormGrad API computation, and the data type used for computation.

    The LayerNormGrad Tiling structure is defined as follows. You do not need to pay attention to the details of this Tiling structure. You only need to pass it to the kernel side and then to the LayerNormGrad high-level API for direct use.

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
void GetLayerNormGradMaxMinTmpSize(const AscendC::TensorShape& srcShape, const uint32_t typeSize, const bool isReuseSource, uint32_t& maxValue, uint32_t& minValue)
```

```
void GetLayerNormGradNDTilingInfo(const AscendC::TensorShape srcShape, const uint32_t stackBufferSize, const uint32_t typeSize, const bool isReuseSource, optiling::LayerNormGradTiling& tiling)
```

```
void GetLayerNormGradNDTilingInfo(const AscendC::TensorShape srcShape, const uint32_t stackBufferSize, const uint32_t typeSize, const bool isReuseSource, AscendC::tiling::LayerNormGradTiling& tiling)
```

## Parameters

**Table 1** Parameter list of the GetLayerNormGradMaxMinTmpSize API

| Parameter Name | Input/Output | Description |
| --- | --- | --- |
| srcShape | Input | Shape information of the input data inputDy, in the format {B, S, storageHLength, originHLength}. The parameter type is [AscendC::TensorShape](../data_structures/TensorShape.md), which includes the shape information of the current input inputDy and the original shape information before address alignment (if H-axis padding is performed).<br><br>In the scenarios supported by the API, storageHLength and originHLength are the same. |
| typeSize | Input | Size of the input data type, in bytes. For example, if the input data type is half, pass 2 here. |
| isReuseSource | Input | Whether to reuse the memory space of the source operand, consistent with the [LayerNorm](LayerNorm.md) API. |
| maxValue | Output | Maximum temporary space size required for the LayerNormGrad API to complete computation, in bytes. Space exceeding this value is not used by this API. Within the range from the minimum to the maximum temporary space size, as the temporary space increases, the computation performance of the kernel-side API improves to a certain extent. To achieve better performance, you can reserve/allocate space based on the actual memory usage. A maximum space size of 0 indicates that no temporary space is required for computation.<br>maxValue is only a reference value and may be larger than the remaining Unified Buffer (UB) space. In this case, you need to select an appropriate temporary space size based on the remaining UB space. |
| minValue | Output | Minimum temporary space size required for the LayerNormGrad API to complete computation, in bytes. To ensure functional correctness, the temporary space reserved/allocated during API computation must not be smaller than this value. A minimum space size of 0 indicates that no temporary space is required for computation. |

**Table 2** Parameter list of the GetLayerNormGradNDTilingInfo API

| Parameter Name | Input/Output | Description |
| --- | --- | --- |
| srcShape | Input | Shape information of the input data inputDy. The parameter type is [AscendC::TensorShape](../data_structures/TensorShape.md), which includes the shape information of the current input and the original shape information before address alignment. |
| stackBufferSize | Input | Space size available for the API, in number of elements. |
| typeSize | Input | Size of the input data type, in bytes. For example, if the input data type is half, pass 2 here. |
| isReuseSource | Input | Whether the memory space of inputX and inputDy can be reused. |
| tiling | Output | Tiling information of the input data. |

## Return Value

None

## Constraints

None

## Examples

The following example describes how the host side obtains Tiling parameters when using the LayerNormGrad high-level API and how these parameters are used on the kernel side. In this example, the shape of the input tensor is \[2, 16, 64\], and the input data type is half.

1.  Add the LayerNormGradTiling struct parameter to the TilingData struct as a field of the TilingData struct.

    ```
    BEGIN_TILING_DATA_DEF(TilingData)             // Register a Tiling class, using the Tiling name as the input parameter.
        TILING_DATA_FIELD_DEF(uint32_t, totalLength); // Add a Tiling field: total computation data size.
        TILING_DATA_FIELD_DEF(uint32_t, tileNum);     // Add a Tiling field: number of computation data blocks on each core.
        ...                                           // Add other Tiling fields.
        TILING_DATA_FIELD_DEF_STRUCT(
            LayerNormGradTiling, layernormGradTilingData); // Add the LayerNormGradTiling struct parameter to the TilingData struct.
    END_TILING_DATA_DEF;
    ```

2.  In the tiling implementation function, first call the GetLayerNormGradMaxMinTmpSize API to obtain the maximum/minimum temporary space size required for the LayerNormGrad API to complete computation. Set an appropriate space size within this range based on the actual memory usage. Then call the GetLayerNormGradNDTilingInfo API to obtain the Tiling parameters required by the LayerNormGradBeta kernel-side API based on the input shape, the remaining space available for computation, and other information.

    ```
    namespace optiling {
    const uint32_t NUM_BLOCKS = 8;
    const uint32_t TILE_NUM = 8;
    static ge::graphStatus TilingFunc(gert::TilingContext* context)
    {
        TilingData tiling;
        uint32_t totalLength = context->GetInputTensor(0)->GetShapeSize();
        context->SetSimdNumBlocks(NUM_BLOCKS);
        tiling.set_totalLength(totalLength);
        tiling.set_tileNum(TILE_NUM);
        // Set other Tiling parameters.
        ...
        // {B, S, storageHLength, originHLength}
        std::vector<int64_t> shapeVec = {2, 16, 64, 64};
        AscendC::TensorShape srcShape(shapeVec);
        // In this example, the minimum value obtained by GetLayerNormGradMaxMinTmpSize is  only for illustration to ensure correct functionality. You can  an appropriate space size as needed.
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

3.  On the corresponding kernel-side, call GET\_TILING\_DATA in the kernel-side API to obtain TilingData, and then pass the LayerNormGradTiling information in TilingData to the LayerNormGrad API for computation. For the complete kernel-side example, see [Examples](LayerNormGrad.md#examples).

    ```
    extern "C" __global__ __aicore__ void func_custom(GM_ADDR x, GM_ADDR y, GM_ADDR z, GM_ADDR workspace, GM_ADDR tiling)
    {
        GET_TILING_DATA(tilingData, tiling);
        KernelFunc op;
        op.Init(x, y, z, tilingData.totalLength, tilingData.tileNum, tilingData.layernormGradTilingData);
        if (TILING_KEY_IS(1)) {
            op.Process();
        }
    }
    ```
