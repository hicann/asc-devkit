# LayerNorm Tiling

<!-- md-trans-meta sourceCommit=aa6d602ebc3dbc8f7b50f91dc72aceefdfa67b49 translatedAt=2026-11-10T06:32:45.818Z pushedAt=2026-11-18T11:07:26.811Z nl=2 -->

## Description

Ascend C provides a set of LayerNorm Tiling APIs for you to obtain the Tiling parameters required for LayerNorm kernel-side API computation.

Obtaining tiling parameters involves the following two steps:

1.  First call **GetLayerNormMaxMinTmpSize** to obtain the maximum and minimum temporary space sizes required for LayerNorm API computation, so that you can allocate the computation space properly.

    The LayerNorm API computation on the kernel-side requires you to reserve/allocate temporary space. **GetLayerNormMaxMinTmpSize** is used to obtain the maximum and minimum temporary space sizes to reserve/allocate on the host side. Based on this range, you can select an appropriate space size and pass it to the kernel-side as a Tiling parameter.

    -   To ensure correct functionality, the reserved or allocated temporary space size must not be smaller than the minimum temporary space size.
    -   Within the range from the minimum to the maximum temporary space size, as the temporary space increases, the computation performance of the kernel-side API improves to a certain extent. To achieve better performance, you can reserve or allocate space based on the actual memory usage.

2.  Call **GetLayerNormNDTilingInfo** to obtain the Tiling parameters required by the LayerNorm API on the kernel-side. You need to  the input shape, the remaining available space for LayerNorm API computation, and the data type used for computation.

    The LayerNorm Tiling structure is defined as follows. You do not need to know the details of this Tiling structure. You only need to pass it to the kernel-side and then to the LayerNorm high-order API for direct use.

    -   Tiling structure required by the LayerNorm API that outputs the normalization result, mean, and variance

        ```
        struct LayerNormTiling {
            uint32_t bLength = 0;
            uint32_t sLength = 0;
            uint32_t hLength = 0;
            uint32_t originalHLength = 0;
            uint32_t inputXSize = 0;
            uint32_t meanVarSize = 0;
            uint32_t numberOfTmpBuf = 0;
            uint32_t meanTmpTensorPos = 0;
            uint32_t meanTmpTensorSize = 0;
            uint32_t varianceTmpTensorPos = 0;
            uint32_t varianceTmpTensorSize = 0;
            uint32_t tmpBufSize = 0;
            uint32_t oneTmpSize = 0;
            uint32_t firstTmpStartPos = 0;
            uint32_t secondTmpStartPos = 0;
            uint32_t thirdTmpStartPos = 0;
            uint32_t loopRound = 0;
            uint32_t inputRoundSize = 0;
            uint32_t inputTailSize = 0;
            uint32_t inputTailPos = 0;
            uint32_t meanVarRoundSize = 0;
            uint32_t meanVarTailSize = 0;
            uint32_t meanVarTailPos = 0;
            uint32_t bshCurLength = 0;
            uint32_t bsCurLength = 0;
            float lastDimValueBack = 0.0;
        };
        ```

    -   Tiling structure required by the LayerNorm API that outputs the normalization result, mean, and inverse of the standard deviation

        ```
        struct LayerNormSeparateTiling {
            uint32_t aLength = 0;
            uint32_t rLength = 0;
            uint32_t halfAddRepeatTimes = 0;
            uint32_t rHeadLength = 0;
            float k2Rec = 0;
            float k2RRec = 0;
            uint32_t inputXSize = 0;
            uint32_t meanVarSize = 0;
            uint32_t numberOfTmpBuf = 0;
            uint32_t varianceTmpTensorPos = 0;
            uint32_t varianceTmpTensorSize = 0;
            uint32_t tmpBufSize = 0;
            uint32_t oneTmpSize = 0;
            uint32_t firstTmpStartPos = 0;
            uint32_t secondTmpStartPos = 0;
            uint32_t thirdTmpStartPos = 0;
            uint32_t loopRound = 0;
            uint32_t inputRoundSize = 0;
            uint32_t inputTailSize = 0;
            uint32_t inputTailPos = 0;
            uint32_t meanVarRoundSize = 0;
            uint32_t meanVarTailSize = 0;
            uint32_t meanVarTailPos = 0;
            uint32_t arCurLength = 0;
            uint32_t aCurLength = 0;
            float rValueBack = 0;
        };
        ```

## Prototype

> [!NOTE] Note
>The GetLayerNormNDTillingInfo API is deprecated and will be removed in a later version. Do not use this API. Use the GetLayerNormNDTilingInfo API instead.

-   GetLayerNormMaxMinTmpSize API
    -   Temporary space required by the LayerNorm API that outputs the normalization result, mean, and variance

        ```
        void GetLayerNormMaxMinTmpSize(const AscendC::TensorShape& srcShape, const uint32_t typeSize, const bool isReuseSource, uint32_t& maxValue, uint32_t& minValue)
        ```

    -   Temporary space required by the LayerNorm API that outputs the normalization result, mean, and inverse of standard deviation

        ```
        void GetLayerNormMaxMinTmpSize(const AscendC::TensorShape& srcShape, const uint32_t typeSize, const bool isReuseSource, const bool isComputeRstd, const bool isOnlyOutput, uint32_t& maxValue, uint32_t& minValue)
        ```

-   GetLayerNormNDTilingInfo/GetLayerNormNDTillingInfo API
    -   Tiling parameters required by the LayerNorm API that outputs the normalization result, mean, and variance

        ```
        void GetLayerNormNDTilingInfo(const AscendC::TensorShape& srcShape, const uint32_t stackBufferSize, const uint32_t typeSize, const bool isReuseSource, optiling::LayerNormTiling& tiling)
        ```

        ```
        void GetLayerNormNDTilingInfo(const AscendC::TensorShape& srcShape, const uint32_t stackBufferSize, const uint32_t typeSize, const bool isReuseSource, AscendC::tiling::LayerNormTiling& tiling)
        ```

    -   Tiling parameters required by the LayerNorm API that outputs the normalization result, mean, and variance (not recommended)

        ```
        void GetLayerNormNDTillingInfo(const AscendC::TensorShape& srcShape, const uint32_t stackBufferSize, const uint32_t typeSize, const bool isReuseSource, optiling::LayerNormTiling& tilling)
        ```

    -   Tiling parameters required by the LayerNorm API that outputs the normalization result, mean, and inverse of standard deviation

        ```
        void GetLayerNormNDTilingInfo(const AscendC::TensorShape& srcShape, const uint32_t stackBufferSize, const uint32_t typeSize, const bool isReuseSource, const bool isComputeRstd, optiling::LayerNormSeparateTiling& tiling)
        ```

        ```
        void GetLayerNormNDTilingInfo(const AscendC::TensorShape& srcShape, const uint32_t stackBufferSize, const uint32_t typeSize, const bool isReuseSource, const bool isComputeRstd, AscendC::tiling::LayerNormSeparateTiling& tiling)
        ```

## Parameters

**Table 1**  GetLayerNormMaxMinTmpSize API parameter list

| API | Input/Output | Description |
| --- | --- | --- |
| srcShape | Input | LayerNorm API that outputs the normalization result, mean, and variance:<br>Shape information {B, S, storageHLength, originHLength} of the input data inputX. The parameter type is [AscendC::TensorShape](../data_structures/TensorShape.md), including the shape information of the current input inputX and the original shape information before address alignment (if an H-axis padding operation exists).<br>In scenarios supported by the API, storageHLength and originHLength remain the same.<br><br>LayerNorm API that outputs the normalization result, mean, and inverse of standard deviation:<br>Shape information {A, R} of the input data inputX. The A-axis length can be dynamically specified in the kernel-side API API, but its range cannot exceed the size of A in this parameter. |
| typeSize | Input | Size of the input data type of inputX, in bytes. For example, if the input data type is half, pass 2 here. |
| isReuseSource | Input | Whether to reuse the memory space of the source operand, consistent with the [LayerNorm](LayerNorm.md) API. |
| isComputeRstd | Input | Whether to compute the inverse of standard deviation rstd. Used in tiling to distinguish the selected LayerNorm API. |
| isOnlyOutput | Input | Whether to output only y without outputting the mean and the inverse of standard deviation rstd. Currently, this parameter supports only false, and the results of y, mean, and rstd are all output. |
| maxValue | Output | Tiling information (maximum temporary space size, in bytes) required by the LayerNorm API.<br><br>Maximum temporary space size required for the LayerNorm API to complete the computation. Space exceeding this value is not used by the API. Within the range from the minimum temporary space to the maximum temporary space, as the temporary space increases, the computation performance of the kernel-side API is optimized to a certain extent. To achieve better performance, you can reserve/allocate space based on the actual memory usage.<br>maxValue is only a reference value and may be larger than the remaining space of the Unified Buffer (UB). In this scenario, you need to select an appropriate temporary space size based on the remaining space of the UB. |
| minValue | Output | Tiling information (minimum temporary space size, in bytes) required by the LayerNorm API.<br><br>Minimum temporary space size required for the LayerNorm API to complete the computation. To ensure correct functionality, the temporary space reserved/allocated during API computation cannot be smaller than this value. |

**Table 2**  GetLayerNormNDTilingInfo and GetLayerNormNDTillingInfo API parameter list

| Parameter Name | Input/Output | Description |
| --- | --- | --- |
| srcShape | Input | LayerNorm API that outputs the normalization result, mean, and variance:<br>Shape information {B, S, storageHLength, originHLength} of the input data inputX. The parameter type is [AscendC::TensorShape](../data_structures/TensorShape.md), including the shape information of the current input inputX and the original shape information before address alignment (if an H-axis padding operation exists).<br><br>LayerNorm API that outputs the normalization result, mean, and inverse of standard deviation:<br>Shape information {A, R} of the input data inputX. The A-axis length can be dynamically specified in the kernel-side API API, but its range cannot exceed the size of A in this parameter. |
| stackBufferSize | Input | Space size available for the LayerNorm API, in bytes. |
| typeSize | Input | Size of the input data type, in bytes. For example, if the input data type is half, pass 2 here. |
| isReuseSource | Input | Whether the memory space of inputX can be reused. |
| isComputeRstd | Input | Whether to compute the inverse of standard deviation rstd. Used in tiling to distinguish the selected LayerNorm API. |
| tilling | Output | Tiling information of the input data. |

## Return Value

None

## Constraints

None

## Examples

The following example describes the process of obtaining Tiling parameters on the host side and how these parameters are used on the kernel-side when using the high-order LayerNorm API that outputs the variance. In the example, the shape size of the input Tensor is \[2, 16, 64\], and the input data type is half.

1.  Add the LayerNormTiling structure parameter to the TilingData structure as a field of the TilingData structure.

    ```
    BEGIN_TILING_DATA_DEF(TilingData)             // Register a Tiling class, using the Tiling name as the input parameter.
        TILING_DATA_FIELD_DEF(uint32_t, totalLength); // Add a Tiling field for the total amount of data to compute.
        TILING_DATA_FIELD_DEF(uint32_t, tileNum);     // Add a tiling field for the number of data blocks computed on each core.
        ...                                           // Add other tiling fields.
        TILING_DATA_FIELD_DEF_STRUCT(
            LayerNormTiling, layernormTilingData); // Add the LayerNormTiling structure parameter to the TilingData structure.
    END_TILING_DATA_DEF;
    ```

2.  In the Tiling implementation function, first call the GetLayerNormMaxMinTmpSize API to obtain the maximum/minimum temporary space size required for the LayerNorm API to complete the computation. Set an appropriate space size within this range based on the actual memory usage. Then call the GetLayerNormNDTilingInfo API to obtain the Tiling parameters required by the LayerNorm kernel-side API based on the input shape, the remaining available space for computation, and other information.

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
        // In this sample, the minimum value obtained by GetLayerNormMaxMinTmpSize is  only for illustration to ensure correct functionality. You can  an appropriate space size as needed.
        uint32_t max;
        uint32_t min;
        AscendC::GetLayerNormMaxMinTmpSize(srcShape, sizeof(half), false, max, min);
        // Obtain the LayerNorm Tiling parameters.
        AscendC::GetLayerNormNDTilingInfo(srcShape, min, sizeof(half), false, tiling.layernormTilingData);
        ... // Other logic.
        tiling.SaveToBuffer(context->GetRawTilingData()->GetData(), context->GetRawTilingData()->GetCapacity());
        context->GetRawTilingData()->SetDataSize(tiling.GetDataSize());
        context->SetTilingKey(1);
        return ge::GRAPH_SUCCESS;
    }
    } // namespace optiling
    ```

3.  On the corresponding kernel-side, call GET\_TILING\_DATA in the kernel-side API to obtain TilingData, and then pass the LayerNormTiling information in TilingData to the LayerNorm API for computation.

    ```
    extern "C" __global__ __aicore__ void func_custom(GM_ADDR x, GM_ADDR y, GM_ADDR z, GM_ADDR workspace, GM_ADDR tiling)
    {
        GET_TILING_DATA(tilingData, tiling);
        KernelFunc op;
        op.Init(x, y, z, tilingData.totalLength, tilingData.tileNum, tilingData.layernormTilingData);
        if (TILING_KEY_IS(1)) {
            op.Process();
        }
    }
    ```

The following sample describes the process of obtaining Tiling parameters on the host side and how these parameters are used on the kernel-side when using the high-order LayerNorm API that outputs the reciprocal of the standard deviation. In the sample, the input tensor has a shape size of \[2, 64\], and the input data type is half.

1.  Add the LayerNormTiling structure parameter to the TilingData structure as a field of the TilingData structure.

    ```
    BEGIN_TILING_DATA_DEF(TilingData)                    // Register a Tiling class, using the Tiling name as the input parameter.
        TILING_DATA_FIELD_DEF(uint32_t, aLength);            // Add a Tiling field: the length of the a-axis.
        TILING_DATA_FIELD_DEF(uint32_t, rLengthWithPadding); // Add a Tiling field: the length of the r-axis after 32B alignment.
        ...                                                  // Add other Tiling fields.
        TILING_DATA_FIELD_DEF_STRUCT(
            LayerNormSeparateTiling, layernormTilingData); // Add the LayerNormSeparateTiling structure parameter to the TilingData structure.
    END_TILING_DATA_DEF;
    ```

2.  In the Tiling implementation function, first call the GetLayerNormMaxMinTmpSize API to obtain the maximum/minimum temporary space size required for the LayerNorm API to complete the computation. Set an appropriate space size within this range based on the actual memory usage. Then call the GetLayerNormNDTilingInfo API to obtain the Tiling parameters required by the LayerNorm kernel-side API based on the input shape, the remaining available space for computation, and other information.

    ```
    namespace optiling {
    const uint32_t NUM_BLOCKS = 1;
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
        // {A, R}
        std::vector<int64_t> shapeVec = {2, 64};
        AscendC::TensorShape srcShape(shapeVec);
        // This sample is for illustration only. The minimum value obtained by GetLayerNormMaxMinTmpSize is  to ensure correct functionality. You can  an appropriate space size as needed.
        uint32_t max;
        uint32_t min;
        AscendC::GetLayerNormMaxMinTmpSize(srcShape, sizeof(half), false, true, false, max, min);
        // Obtain the LayerNorm Tiling parameters.
        AscendC::GetLayerNormNDTilingInfo(srcShape, min, sizeof(half), false, true, tiling.layernormTilingData);
        // auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
        // AscendC::GetLayerNormMaxMinTmpSize(srcShape, sizeof(half), false, true, false, ascendcPlatform, max, min);
        // Obtain the LayerNorm Tiling parameters.
        // AscendC::GetLayerNormNDTilingInfo(srcShape, min, sizeof(half), false, true, ascendcPlatform,
        // tiling.layernormTilingData);
        ... // Other logic.
        tiling.SaveToBuffer(context->GetRawTilingData()->GetData(), context->GetRawTilingData()->GetCapacity());
        context->GetRawTilingData()->SetDataSize(tiling.GetDataSize());
        context->SetTilingKey(1);
        return ge::GRAPH_SUCCESS;
    }
    } // namespace optiling
    ```

3.  On the corresponding kernel-side, the kernel-side API calls GET\_TILING\_DATA to obtain TilingData, and then passes the LayerNormTiling information in TilingData to the LayerNorm API for computation.

    ```
    extern "C" __global__ __aicore__ void func_custom(
        GM_ADDR x, GM_ADDR gamma, GM_ADDR beta, GM_ADDR mean, GM_ADDR rstd, GM_ADDR y, GM_ADDR workspace, GM_ADDR tiling)
    {
        GET_TILING_DATA(tilingData, tiling);
        float epsilon = tilingData.epsilon;
        AscendC::LayerNormPara para(tilingData.aLength, tilingData.rLengthWithPadding);
        KernelFunc op;
        op.Init(x, gamma, beta, mean, rstd, y, epsilon, para, tilingData.layernormTilingData);
        if (TILING_KEY_IS(1)) {
            op.Process();
        }
    }
    ```
