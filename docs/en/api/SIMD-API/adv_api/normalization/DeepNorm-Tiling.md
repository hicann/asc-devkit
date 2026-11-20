# DeepNorm Tiling

<!-- md-trans-meta sourceCommit=aa6d602ebc3dbc8f7b50f91dc72aceefdfa67b49 translatedAt=2026-11-10T06:31:55.946Z pushedAt=2026-11-18T11:07:26.798Z nl=2 -->

## Description

Ascend C provides the DeepNorm Tiling API, which helps you obtain the Tiling parameters required for DeepNorm kernel-side API computation.

Obtaining tiling parameters involves the following two steps:

1.  Use **GetDeepNormMaxMinTmpSize** to obtain the maximum and minimum temporary space sizes required for DeepNorm API computation.

    The DeepNorm API computation on the kernel-side requires you to reserve/allocate temporary space. **GetDeepNormMaxMinTmpSize** is used on the host side to obtain the maximum and minimum temporary space sizes to reserve/allocate. Based on this range, you can select an appropriate space size and pass it to the kernel-side as a Tiling parameter.

    -   To ensure correct functionality, the reserved or allocated temporary space size must not be smaller than the minimum temporary space size.
    -   Within the range from the minimum to the maximum temporary space size, as the temporary space increases, the computation performance of the kernel-side API improves to a certain extent. To achieve better performance, you can reserve or allocate space based on the actual memory usage.

2.  Use **GetDeepNormTilingInfo** to obtain the Tiling parameters required by the DeepNorm kernel-side API.

    The DeepNormTiling structure is defined as follows. You do not need to focus on the specific information of this Tiling structure. You only need to pass it to the kernel-side and then to the DeepNorm high-level API for direct use.

    ```
    struct DeepNormTiling {
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
        float lastDimValueBack = 0;
    };
    ```

## Prototype

```
bool GetDeepNormMaxMinTmpSize(const AscendC::TensorShape& srcShape, const uint32_t typeSize, const bool isReuseSource, const bool isBasicBlock, uint32_t& maxValue, uint32_t& minValue)
```

```
bool GetDeepNormTilingInfo(const AscendC::TensorShape& srcShape, const AscendC::TensorShape& originSrcShape, const uint32_t stackBufferSize, const uint32_t typeSize, const bool isReuseSource, const bool isBasicBlock, optiling::DeepNormTiling& tiling)
```

```
bool GetDeepNormTilingInfo(const AscendC::TensorShape& srcShape, const AscendC::TensorShape& originSrcShape, const uint32_t stackBufferSize, const uint32_t typeSize, const bool isReuseSource, const bool isBasicBlock, AscendC::tiling::DeepNormTiling& tiling)
```

## Parameters

**Table 1**  Parameter description of the GetDeepNormMaxMinTmpSize API

| Parameter | Input/Output | Description |
| --- | --- | --- |
| srcShape | Input | Input shape information. The parameter type is [AscendC::TensorShape](../data_structures/TensorShape.md). |
| typeSize | Input | Size of the input data type, in bytes. For example, if the input data type is half, pass 2 here. |
| isReuseSource | Input | Whether to reuse the space of the source operand, consistent with the DeepNorm API. |
| isBasicBlock | Input | Whether srcShape conforms to the basic block definition: the length of the last axis H is a multiple of 64 (not exceeding 2040), and B*S is a multiple of 8. |
| maxValue | Output | Maximum temporary space size required for the DeepNorm API to complete computation, unit: byte. Space exceeding this value is not used by this API. Within the range from the minimum temporary space to the maximum temporary space, as the temporary space increases, the computation performance of the kernel-side API is optimized to a certain extent. To achieve better performance, you can reserve/allocate space based on the actual memory usage. A maximum space size of 0 indicates that no temporary space is required for computation. <br>maxValue is only a reference value and may be larger than the remaining space of the Unified Buffer (UB). In this case, you need to select an appropriate temporary space size based on the remaining space of the UB. |
| minValue | Output | Minimum temporary space size required for the DeepNorm API to complete computation, unit: byte. To ensure correct functionality, the temporary space reserved/allocated during API computation must not be smaller than this value. A minimum space size of 0 indicates that no temporary space is required for computation. |

**Table 2**  Parameter description of the GetDeepNormTilingInfo API

| Parameter | Input/Output | Description |
| --- | --- | --- |
| srcShape | Input | Input shape information [B, S, H]. The parameter type is [AscendC::TensorShape](../data_structures/TensorShape.md). |
| originSrcShape | Input | Input shape information [B, S, originH] before 32B alignment. The parameter type is [AscendC::TensorShape](../data_structures/TensorShape.md). The length of originH must be within the range (0, H]. If isBasicBlock is set to true, originH must be the same as H. |
| stackBufferSize | Input | Buffer size of the temporary space, in bytes. Obtain the maximum and minimum temporary space sizes through GetDeepNormMaxMinTmpSize, and select an appropriate space size within this range as stackBufferByteSize to . |
| typeSize | Input | Size of the input data type, in bytes. For example, if the input data type is half, pass 2 here. |
| isReuseSource | Input | Whether to reuse the space of the source operand, consistent with the DeepNorm API. |
| isBasicBlock | Input | Whether srcShape conforms to the basic block definition: the length of the last axis H is a multiple of 64 (not exceeding 2040), and B*S is a multiple of 8. |
| tiling | Output | Tiling information required for DeepNorm computation. |

## Return Value

-   GetDeepNormMaxMinTmpSize returns true or false. true indicates that the maximum and minimum temporary space sizes required for the internal computation of the DeepNorm API are obtained successfully; false indicates a failure.
-   GetDeepNormTilingInfo returns true or false. true indicates that all Tiling parameter values of DeepNorm are obtained successfully; false indicates a failure.

## Constraints

None

## Examples

1.  Add the DeepNorm Tiling structure parameters to the TilingData structure as a field of the TilingData structure.

    ```
    BEGIN_TILING_DATA_DEF(TilingData)             // Register a tiling class, using the tiling name as the input parameter.
        TILING_DATA_FIELD_DEF(uint32_t, totalLength); // Add a tiling field for the total amount of computation data.
        TILING_DATA_FIELD_DEF(uint32_t, tileNum);     // Add a tiling field for the number of computation data blocks on each core.
        ...                                           // Add other tiling fields.
        TILING_DATA_FIELD_DEF_STRUCT(
            DeepNormTiling, deepnormTilingData); // Add the DeepNormTiling structure parameter to the TilingData structure.
    END_TILING_DATA_DEF;
    ```

2.  In the Tiling implementation function, first call the **GetDeepNormMaxMinTmpSize** API to obtain the maximum and minimum temporary space sizes required for the DeepNorm API to complete computation. Set an appropriate space size within this range based on the actual memory usage, and then obtain the Tiling parameters required by the DeepNorm kernel-side API based on the input shape, the remaining space available for computation, and other information.

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
        std::vector<int64_t> shapeVec = {2, 16, 64};
        std::vector<int64_t> oriShapeVec = {2, 16, 64};
        AscendC::TensorShape srcShape(shapeVec);
        AscendC::TensorShape originSrcShape(oriShapeVec);

        // This sample only serves as an illustration. The minimum value obtained by GetDeepNormMaxMinTmpSize is  to ensure correct functionality. You can  an appropriate space size as needed.
        uint32_t minValue = 0;
        uint32_t maxValue = 0;
        AscendC::GetDeepNormMaxMinTmpSize(srcShape, sizeof(half), isReuseSrc, isBasicBlock, maxValue, minValue);
        // Obtain the DeepNorm Tiling parameters.
        AscendC::GetDeepNormTilingInfo(
            srcShape, originSrcShape, minValue, sizeof(half), isReuseSrc, isBasicBlock, tiling.deepnormTilingData);

        ... // Other logic.
        tiling.SaveToBuffer(context->GetRawTilingData()->GetData(), context->GetRawTilingData()->GetCapacity());
        context->GetRawTilingData()->SetDataSize(tiling.GetDataSize());
        context->SetTilingKey(1);
        return ge::GRAPH_SUCCESS;
    }
    } // namespace optiling
    ```

3.  On the corresponding kernel-side, call GET\_TILING\_DATA in the kernel-side API to obtain TilingData, and then pass the DeepNorm Tiling information in TilingData to the DeepNorm API for computation. For the complete kernel-side sample, see [DeepNorm](DeepNorm.md).

    ```
    extern "C" __global__ __aicore__ void deepnorm_custom(
        GM_ADDR inputX, GM_ADDR inputGx, GM_ADDR beta, GM_ADDR gamma, GM_ADDR output, GM_ADDR outputMean,
        GM_ADDR outputVariance, GM_ADDR tiling)
    {
        GET_TILING_DATA(tilingData, tiling);
        KernelDeepNorm op;
        op.Init(
            inputX, inputGx, beta, gamma, output, outputMean, outputVariance, tilingData.totalLength, tilingData.tileNum,
            tilingData.deepnormTilingData);
        if (TILING_KEY_IS(1)) {
            op.Process();
        }
    }
    ```
