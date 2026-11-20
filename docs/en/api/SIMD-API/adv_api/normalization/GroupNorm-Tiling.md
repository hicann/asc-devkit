# GroupNorm Tiling

<!-- md-trans-meta sourceCommit=aa6d602ebc3dbc8f7b50f91dc72aceefdfa67b49 translatedAt=2026-11-10T06:33:02.625Z pushedAt=2026-11-18T11:07:26.818Z nl=2 -->

## Description

The GroupNorm Tiling API is used to obtain the tiling parameters required for GroupNorm kernel-side API computation. Obtaining the tiling parameters involves the following two steps:

1.  Use **GetGroupNormMaxMinTmpSize** to obtain the maximum and minimum temporary space sizes required for GroupNorm API computation.

    The GroupNorm API computation on the kernel-side requires you to reserve/allocate temporary space. **GetGroupNormMaxMinTmpSize** is used to obtain the maximum and minimum temporary space sizes to reserve/allocate on the host side. Based on this range, you can select an appropriate space size as a tiling parameter and pass it to the kernel-side.

    -   To ensure correct functionality, the reserved or allocated temporary space size must not be smaller than the minimum temporary space size.
    -   Within the range from the minimum to the maximum temporary space size, as the temporary space increases, the computation performance of the kernel-side API improves to a certain extent. To achieve better performance, you can reserve or allocate space based on the actual memory usage.

2.  Use **GetGroupNormNDTilingInfo** to obtain the tiling parameters required by the GroupNorm kernel-side API.

    The GroupNorm Tiling structure is defined as follows. You do not need to pay attention to the specific information of this tiling structure. You only need to pass it to the kernel-side and then to the GroupNorm high-level API for direct use.

    ```
    struct GroupNormTiling {
        uint32_t n = 0;
        uint32_t c = 0;
        uint32_t hw = 0;
        uint32_t g = 0;
        uint32_t d = 0;
        uint32_t hwAlignSize = 0;
        uint32_t dhwAlignSize = 0;
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
        float factor = 0;
        bool smallShape = 0;
    };
    ```

## Prototype

```
void GetGroupNormMaxMinTmpSize(const AscendC::TensorShape& srcShape, const uint32_t typeSize, const bool isReuseSource, const uint32_t groupNum, uint32_t& maxValue, uint32_t& minValue)
```

```
void GetGroupNormNDTilingInfo(const AscendC::TensorShape& srcShape, const uint32_t stackBufferSize, const uint32_t typeSize, const bool isReuseSource, const uint32_t groupNum, optiling::GroupNormTiling& tiling)
```

```
void GetGroupNormNDTilingInfo(const AscendC::TensorShape& srcShape, const uint32_t stackBufferSize, const uint32_t typeSize, const bool isReuseSource, const uint32_t groupNum, AscendC::tiling::GroupNormTiling& tiling)
```

## Parameters

**Table 1**  Parameter list of the GetGroupNormMaxMinTmpSize API

| API | Input/Output | Description |
| --- | --- | --- |
| srcShape | Input | Shape information [N, C, H, W] of the input data inputX. The parameter type is [AscendC::TensorShape](../data_structures/TensorShape.md). |
| typeSize | Input | Size of the input data type of inputX, in bytes. For example, if the input data type is half, pass 2 here. |
| isReuseSource | Input | Whether intermediate variables can reuse the input memory. |
| groupNum | Input | Number of groups along the C dimension. |
| maxValue | Output | Outputs the tiling information (maximum temporary space size, in bytes) required by the GroupNorm API.<br><br>The maximum temporary space size required for the GroupNorm API to complete computation. Space exceeding this value is not used by the API. Within the range from the minimum to the maximum temporary space size, as the temporary space increases, the computation performance of the kernel-side API improves to a certain extent. To achieve better performance, you can reserve/allocate space based on the actual memory usage.<br>maxValue is only a reference value and may be larger than the remaining space of the Unified Buffer (UB). In this case, you need to select an appropriate temporary space size based on the remaining UB space. |
| minValue | Output | Outputs the tiling information (minimum temporary space size, in bytes) required by the GroupNorm API.<br><br>The minimum temporary space size required for the GroupNorm API to complete computation. To ensure correct functionality, the temporary space reserved/allocated during API computation must not be smaller than this value. |

**Table 2**  Parameter list of the GetGroupNormNDTilingInfo API:

| Parameter Name | Input/Output | Description |
| --- | --- | --- |
| srcShape | Input | Shape information [N, C, H, W] of the input data inputX. The parameter type is [AscendC::TensorShape](../data_structures/TensorShape.md). |
| stackBufferSize | Input | Space size available for the GroupNorm API, in bytes. |
| typeSize | Input | Size of the input data type, in bytes. For example, if the input data type is half, pass 2 here. |
| isReuseSource | Input | Whether the memory space of inputX can be reused. |
| groupNum | Input | Number of groups along the C dimension. |
| tiling | Output | Tiling information of the input data. |

## Return Value

None

## Constraints

None

## Examples

The following sample describes how to obtain tiling parameters on the host side and how these parameters are used on the kernel-side. In the sample, the shape of the input tensor is \[2, 16, 8, 8\], and the input data type is half.

1.  Add the GroupNormTiling structure parameters to the TilingData structure as a field of the TilingData structure.

    ```
    BEGIN_TILING_DATA_DEF(TilingData) // Register a tiling class, using the tiling name as the input parameter.
        TILING_DATA_FIELD_DEF(uint32_t, n);
        TILING_DATA_FIELD_DEF(uint32_t, c);
        TILING_DATA_FIELD_DEF(uint32_t, h);
        TILING_DATA_FIELD_DEF(uint32_t, w);
        TILING_DATA_FIELD_DEF(uint32_t, group);
        // Add other tiling fields.
        ...
        TILING_DATA_FIELD_DEF_STRUCT(
            GroupNormTiling, GroupNormTilingData); // Add the GroupNormTiling structure parameters to the TilingData structure.
    END_TILING_DATA_DEF;
    ```

2.  In the Tiling implementation function, first call the **GetGroupNormMaxMinTmpSize** API to obtain the maximum/minimum temporary space size required for the GroupNorm API to complete the computation. Set an appropriate space size based on this range and the actual memory usage, and then obtain the Tiling parameters required by the GroupNorm kernel-side API based on the input shape, the remaining space available for computation, and other information.

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
        std::vector<int64_t> shapeVec = {2, 16, 8, 8}; // {n, c, h, w}
        AscendC::TensorShape srcShape(shapeVec);
        uint32_t groupNum = 4;
        uint32_t minSize = 0;
        uint32_t maxSize = 0;
        // This sample is for illustration only. The GetGroupNormMaxMinTmpSize API is used to obtain the maximum/minimum temporary space size required for the GroupNorm API to complete the computation. You can set an appropriate space size based on this range and the actual memory usage.
        AscendC::GetGroupNormMaxMinTmpSize(srcShape, sizeof(half), false, groupNum, maxSize, minSize);
        // Obtain the GroupNorm Tiling parameters.
        AscendC::GetGroupNormNDTilingInfo(srcShape, maxSize, sizeof(half), false, groupNum, tiling.groupNormTilingData);
        ... // Other logic.
        tiling.SaveToBuffer(context->GetRawTilingData()->GetData(), context->GetRawTilingData()->GetCapacity());
        context->GetRawTilingData()->SetDataSize(tiling.GetDataSize());
        context->SetTilingKey(1);
        return ge::GRAPH_SUCCESS;
    }
    } // namespace optiling
    ```

3.  On the corresponding kernel-side, obtain TilingData by calling GET\_TILING\_DATA in the kernel-side API, and then pass the GroupNorm Tiling information in TilingData to the GroupNorm API for computation.

    ```
    extern "C" __global__ __aicore__ void groupnorm_custom(
        GM_ADDR inputX_gm, GM_ADDR gamm_gm, GM_ADDR beta_gm, GM_ADDR output_gm, GM_ADDR outputMean_gm,
        GM_ADDR outputVariance_gm, GM_ADDR tiling)
    {
        GET_TILING_DATA(tilingData, tiling);
        KernelGroupNorm<half, false> op;
        op.Init(inputX_gm, gamm_gm, beta_gm, output_gm, outputMean_gm, outputVariance_gm, tilingData.groupNormTilingData);
        if (TILING_KEY_IS(1)) {
            op.Process();
        }
    }
    ```
