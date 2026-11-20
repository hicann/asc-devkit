# BatchNorm Tiling

<!-- md-trans-meta sourceCommit=aa6d602ebc3dbc8f7b50f91dc72aceefdfa67b49 translatedAt=2026-11-10T06:32:08.365Z pushedAt=2026-11-18T11:07:26.802Z nl=2 -->

## Description

The BatchNorm Tiling API is used to get the tiling parameters required for BatchNorm kernel-side computation. Getting the tiling parameters involves the following two steps:

1.  Use **GetBatchNormMaxMinTmpSize** to get the maximum and minimum temporary space sizes required for BatchNorm API computation.

    The BatchNorm API computation on the kernel side requires you to reserve/allocate temporary space. **GetBatchNormMaxMinTmpSize** is used to get the maximum and minimum temporary space sizes to reserve/allocate on the host side. Based on this range, you can select an appropriate space size and pass it to the kernel side as a tiling parameter.

    -   To ensure correct functionality, the reserved or allocated temporary space size must not be smaller than the minimum temporary space size.
    -   Within the range from the minimum to the maximum temporary space size, as the temporary space increases, the computation performance of the kernel-side API improves to a certain extent. To achieve better performance, you can reserve or allocate space based on the actual memory usage.

2.  Use **GetBatchNormNDTilingInfo** to get the tiling parameters required by the BatchNorm kernel-side API.

    The BatchNorm Tiling structure is defined as follows. You do not need to know the specific information of this tiling structure. You only need to pass it to the kernel side and pass it into the BatchNorm high-level API for direct use.

    ```
    struct BatchNormTiling {
        uint32_t originalBLength = 0;
        uint32_t meanVarSize = 0;
        uint32_t meanTmpTensorPos = 0;
        uint32_t varianceTmpTensorPos = 0;
        uint32_t tmpBufSize = 0;
        uint32_t oneTmpSize = 0;
        uint32_t firstTmpStartPos = 0;
        uint32_t secondTmpStartPos = 0;
        uint32_t thirdTmpStartPos = 0;
        uint32_t loopRound = 0;
        uint32_t inputTailSize = 0;
        uint32_t inputTailPos = 0;
        uint32_t meanVarTailSize = 0;
        uint32_t meanVarTailPos = 0;
        uint32_t bshCurLength = 0;
        uint32_t shCurLength = 0;
        float firstDimValueBack = 0;
        uint32_t castHalfRepStride = 0;
        uint32_t shCurLengthBlockNum = 0;
        uint32_t castHalfOutRepStride = 0;
    };
    ```

## Prototype

```
bool GetBatchNormMaxMinTmpSize(const AscendC::TensorShape& srcShape, const AscendC::TensorShape& originSrcShape, const uint32_t typeSize, const bool isReuseSource, uint32_t& maxValue,uint32_t& minValue, const bool isBasicBlock = false)
```

```
bool GetBatchNormNDTilingInfo(const AscendC::TensorShape& srcShape, const AscendC::TensorShape& originSrcShape, const uint32_t stackBufferByteSize, const uint32_t typeSize, const bool isReuseSource, optiling::BatchNormTiling& tilling, const bool isBasicBlock = false)
```

```
bool GetBatchNormNDTilingInfo(const AscendC::TensorShape& srcShape, const AscendC::TensorShape& originSrcShape, const uint32_t stackBufferByteSize, const uint32_t typeSize, const bool isReuseSource, AscendC::tiling::BatchNormTiling& tilling, const bool isBasicBlock = false)
```

## Parameters

**Table 1**  GetBatchNormMaxMinTmpSize API parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| srcShape | Input | Shape information [B, S, H] of the input data inputX. The parameter type is [AscendC::TensorShape](../data_structures/TensorShape.md). S*H must be 32B-aligned. |
| originSrcShape | Input | Origin shape information [originB, originS, originH] of the input data inputX. The parameter type is [AscendC::TensorShape](../data_structures/TensorShape.md). |
| typeSize | Input | Size of the input data type, in bytes. For example, if the input data type is half, pass 2 here. |
| isReuseSource | Input | Whether intermediate variables can reuse the input memory. This parameter is reserved. Pass the default value false. |
| maxValue | Output | Maximum temporary space size required for the BatchNorm API to complete computation, in bytes. Space beyond max is not used by this API. Within the min-max range, the larger the reserved/allocated space, the better the API computation performance. To achieve better performance, you can reserve/allocate space based on the actual memory usage. A maxValue of 0 indicates that no temporary space is required for computation.<br>maxValue is only a reference value and may be larger than the remaining space of the Unified Buffer (UB). In this case, you need to select an appropriate temporary space size based on the remaining UB space. |
| minValue | Output | Minimum temporary space size required for the BatchNorm API to complete computation, in bytes. To ensure functional correctness, the temporary space reserved/allocated during API computation must not be smaller than min. A minimum space of 0 indicates that no temporary space is required for computation. |
| isBasicBlock | Input | Whether to enable the basic block. This is consistent with the BatchNorm API. |

**Table 2**  GetBatchNormNDTilingInfo API parameters

| Parameter Name | Input/Output | Description |
| --- | --- | --- |
| srcShape | Input | Shape information [B, S, H] of the input data inputX. The parameter type is [AscendC::TensorShape](../data_structures/TensorShape.md). S*H must be 32B-aligned. |
| originSrcShape | Input | Origin shape information [originB, originS, originH] of the input data inputX. The parameter type is [AscendC::TensorShape](../data_structures/TensorShape.md). |
| stackBufferByteSize | Input | Space size available for the BatchNorm API, in bytes. |
| typeSize | Input | Size of the input data type, in bytes. |
| isReuseSource | Input | Whether intermediate variables can reuse the input memory. This parameter is reserved. Pass the default value false. |
| tilling | Output | Tiling information of the input data. |
| isBasicBlock | Input | Whether to enable the basic block. This is consistent with the BatchNorm API. |

## Return Value

-   GetBatchNormMaxMinTmpSize returns true or false. true indicates that the maximum and minimum temporary space sizes required for the internal computation of the BatchNorm API are obtained successfully; false indicates a failure.
-   GetBatchNormNDTilingInfo returns true or false. true indicates that all tiling parameters of BatchNorm are obtained successfully; false indicates a failure.

## Constraints

None

## Examples

The following example describes the process of obtaining tiling parameters on the host side and how these parameters are used on the kernel side. In the example, the shape of the input tensor is \[16, 16, 16\], and the input data type is half.

1.  Add the BatchNormTiling structure parameters to the TilingData structure as a field of the TilingData structure.

    ```
    BEGIN_TILING_DATA_DEF(TilingData)                 // Register a tiling class, using the tiling name as the input parameter.
        TILING_DATA_FIELD_DEF(uint32_t, tileNum);         // Add a tiling field: the number of data blocks computed on each core.
        TILING_DATA_FIELD_DEF(uint32_t, bLength);         // Add a tiling field: the length of the b dimension of the input shape.
        TILING_DATA_FIELD_DEF(uint32_t, sLength);         // Add the tiling field: length of the s dimension of the input shape.
        TILING_DATA_FIELD_DEF(uint32_t, hLength);         // Add the tiling field: length of the h dimension of the input shape.
        TILING_DATA_FIELD_DEF(uint32_t, originalBLength); // Add the tiling field: original length of the b dimension of the input shape.
        ...                                               // Add other tiling fields.
        TILING_DATA_FIELD_DEF_STRUCT(
            BatchNormTiling, batchNormTilingData); // Add the BatchNormTiling structure parameter to the TilingData structure.
    END_TILING_DATA_DEF;
    ```

2.  In the Tiling implementation function, first call the **GetBatchNormMaxMinTmpSize** API to get the maximum/minimum temporary space size required for the BatchNorm API to complete the computation. Set an appropriate space size within this range based on the actual memory usage, and then get the tiling parameters required by the BatchNorm kernel-side API based on information such as the input shape and the remaining space size available for computation.

    ```
    namespace optiling {
    const uint32_t NUM_BLOCKS = 8;
    const uint32_t TILE_NUM = 8;
    static ge::graphStatus TilingFunc(gert::TilingContext* context)
    {
        TilingData tiling;
        uint32_t totalLength = context->GetInputTensor(0)->GetShapeSize();
        context->SetSimdNumBlocks(NUM_BLOCKS);
        tiling.set_tileNum(TILE_NUM);
        // Set other Tiling parameters.
        ...
        std::vector<int64_t> shapeVec = {16, 16, 16};   //{b,s,h}
        std::vector<int64_t> originShapeVec = {15, 16, 16}; //{originB,originS,originH}
        AscendC::TensorShape srcShape(shapeVec);
        AscendC::TensorShape originSrcShape(originShapeVec);
        uint32_t minSize = 0;
        uint32_t maxSize = 0;
        // This sample is for illustration only. It gets the minimum value through GetBatchNormMaxMinTmpSize and passes it in to ensure correct functionality. You can  an appropriate space size as needed.
        AscendC::GetBatchNormMaxMinTmpSize(srcShape, originSrcShape, sizeof(half), false, maxSize, minSize, false);
        // Get the BatchNorm Tiling parameters.
        AscendC::GetBatchNormNDTilingInfo(
            srcShape, originSrcShape, minSize, sizeof(half), false, tiling.batchNormTilingData, false);
        ... // Other logic.
        tiling.SaveToBuffer(context->GetRawTilingData()->GetData(), context->GetRawTilingData()->GetCapacity());
        context->GetRawTilingData()->SetDataSize(tiling.GetDataSize());
        context->SetTilingKey(1);
        return ge::GRAPH_SUCCESS;
    }
    } // namespace optiling
    ```

3.  Obtain the TilingData by calling GET\_TILING\_DATA on the kernel side and then pass the BatchNormTiling information in the TilingData to the BatchNorm API to participate in the computation. For the complete kernel-side sample, see [BatchNorm](BatchNorm.md).

    ```
    extern "C" __global__ __aicore__ void func_custom(
        GM_ADDR inputX_gm, GM_ADDR gamm_gm, GM_ADDR beta_gm, GM_ADDR output_gm, GM_ADDR outputMean_gm,
        GM_ADDR outputVariance_gm, GM_ADDR tiling)
    {
        GET_TILING_DATA(tilingData, tiling);
        KernelBatchnorm<half, false, false> op;
        op.Init(inputX_gm, gamm_gm, beta_gm, output_gm, outputMean_gm, outputVariance_gm, tilingData.batchNormTilingData);
        if (TILING_KEY_IS(1)) {
            op.Process();
        }
    }
    ```
