# GroupNorm Tiling

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-10T02:38:12.147Z -->

## Description

The GroupNorm Tiling API is used to obtain the Tiling parameters required for GroupNorm kernel computation. Obtaining the Tiling parameters involves the following two steps:

1.  Obtain the maximum and minimum temporary space sizes required for GroupNorm API computation through **GetGroupNormMaxMinTmpSize**.

    The GroupNorm API computation on the kernel side requires you to reserve/apply temporary space. **GetGroupNormMaxMinTmpSize** is used to obtain the maximum and minimum temporary space sizes to reserve/apply on the host side. Based on this range, you can select an appropriate space size as a Tiling parameter and pass it to the kernel side for use.

    -   To ensure correct functionality, the reserved/applied temporary space size must not be smaller than the minimum temporary space size.
    -   Within the range from the minimum temporary space size to the maximum temporary space size, as the temporary space increases, the computation performance of the kernel-side API improves to a certain extent. To achieve better performance, you can reserve/apply space based on the actual memory usage.

2.  Obtain the tiling parameters required by the GroupNorm kernel-side API through **GetGroupNormNDTilingInfo**.

    The GroupNorm Tiling structure is defined as follows. You do not need to concern yourself with the specific details of this tiling structure. You only need to pass it to the kernel side and pass it into the GroupNorm high-level API for direct use.

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

## Function Prototype

```
void GetGroupNormMaxMinTmpSize(const ge::Shape& srcShape, const uint32_t typeSize, const bool isReuseSource, const uint32_t groupNum, uint32_t& maxValue, uint32_t& minValue)
```

```
void GetGroupNormNDTilingInfo(const ge::Shape& srcShape, const uint32_t stackBufferSize, const uint32_t typeSize, const bool isReuseSource, const uint32_t groupNum, optiling::GroupNormTiling& tiling)
```

```
void GetGroupNormNDTilingInfo(const ge::Shape& srcShape, const uint32_t stackBufferSize, const uint32_t typeSize, const bool isReuseSource, const uint32_t groupNum, AscendC::tiling::GroupNormTiling& tiling)
```

## Parameters

**Table 1**  Parameters of the GetGroupNormMaxMinTmpSize API

| API | Input/Output | Function |
| --- | --- | --- |
| **srcShape** | Input | Shape information [N, C, H, W] of the input data **inputX**. |
| **typeSize** | Input | Data type size of the input data **inputX**, in bytes. For example, if the input data type is half, pass 2 here. |
| **isReuseSource** | Input | Whether the intermediate variable can reuse the input memory. |
| **groupNum** | Input | Number of groups in the C dimension. |
| **maxValue** | Output | Outputs the tiling information (maximum temporary space size) required by the **GroupNorm** API.<br><br>Maximum temporary space size required for the **GroupNorm** API to complete computation. Space exceeding this value is not used by the API. Within the range from the minimum temporary space size to the maximum temporary space size, as the temporary space increases, the computation performance of the kernel-side API improves to a certain extent. To achieve better performance, you can reserve or apply for space based on the actual memory usage.<br>**maxValue** is only a reference value and may be larger than the remaining space of the Unified Buffer. In this case, you need to select an appropriate temporary space size based on the remaining space of the Unified Buffer. |
| **minValue** | Output | Outputs the tiling information (minimum temporary space size) required by the **GroupNorm** API.<br><br>Minimum temporary space size required for the **GroupNorm** API to complete computation. To ensure correctness, the temporary space reserved or applied for during API computation must not be smaller than this value. |

**Table 2**  Parameters of the GetGroupNormNDTilingInfo API:

| Parameter Name | Input/Output | Description |
| --- | --- | --- |
| **srcShape** | Input | Shape information [N, C, H, W] of the input data **inputX**. |
| **stackBufferSize** | Input | Space size available for the **GroupNorm** API, in bytes. |
| **typeSize** | Input | Data type size of the input data, in bytes. For example, if the input data type is half, pass 2 here. |
| **isReuseSource** | Input | Whether the memory space of **inputX** can be reused. |
| **groupNum** | Input | Number of groups in the C dimension. |
| **tiling** | Output | Tiling information of the input data. |

## Return Value

None

## Constraints

None

## Examples

The following sample describes how to obtain the Tiling parameters on the host side and how these parameters are used on the kernel side. In the sample, the shape of the input tensor is \[2, 16, 8, 8\], and the input data type is half.

1.  Add the **GroupNormTiling** structure parameters to the **TilingData** structure as a field of the **TilingData** structure.

    ```
    BEGIN_TILING_DATA_DEF(TilingData)  // Register a tiling class, using the tiling name as the input parameter.
      TILING_DATA_FIELD_DEF(uint32_t, n);
      TILING_DATA_FIELD_DEF(uint32_t, c);
      TILING_DATA_FIELD_DEF(uint32_t, h);
      TILING_DATA_FIELD_DEF(uint32_t, w);
      TILING_DATA_FIELD_DEF(uint32_t, group);
      // Add other tiling fields.
      ...
      TILING_DATA_FIELD_DEF_STRUCT(GroupNormTiling, GroupNormTilingData); // Add the GroupNormTiling structure parameters to the TilingData structure.
    END_TILING_DATA_DEF;
    ```

2.  In the Tiling implementation function, first call the **GetGroupNormMaxMinTmpSize** API to obtain the maximum/minimum temporary space size required for the **GroupNorm** API to complete computation, set an appropriate space size based on this range and the actual memory usage, and then obtain the Tiling parameters required by the GroupNorm kernel-side API based on the input shape, the remaining space available for computation, and other information.

    ```
    namespace optiling {
    const uint32_t NUM_BLOCKS = 8;
    const uint32_t TILE_NUM = 8;
    static ge::graphStatus TilingFunc(gert::TilingContext* context)
    {
        TilingData tiling;
        uint32_t totalLength = context->GetInputTensor(0)->GetShapeSize();
        context->SetBlockDim(NUM_BLOCKS);
        tiling.set_tileNum(TILE_NUM);
        // Set other Tiling parameters.
        ...
        std::vector<int64_t> shapeVec = {2, 16, 8, 8}; // {n, c, h, w}
        ge::Shape srcShape(shapeVec);
        uint32_t groupNum=4;
        uint32_t minSize = 0;
        uint32_t maxSize = 0;
        // This sample is for illustration only. Obtain the maximum/minimum temporary space size required for the GroupNorm API to complete computation through the GetGroupNormMaxMinTmpSize API. You can set an appropriate space size based on this range and the actual memory usage.
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

3.  On the kernel side, the corresponding kernel function calls **GET\_TILING\_DATA** to obtain **TilingData**, and then passes the GroupNorm Tiling information in **TilingData** to the **GroupNorm** API for computation.

    ```
    extern "C" __global__ __aicore__ void groupnorm_custom(GM_ADDR inputX_gm, GM_ADDR gamm_gm, GM_ADDR beta_gm, GM_ADDR output_gm, GM_ADDR outputMean_gm, GM_ADDR outputVariance_gm, GM_ADDR tiling)
    {
        GET_TILING_DATA(tilingData, tiling);
        KernelGroupNorm<half, false> op;
        op.Init(inputX_gm, gamm_gm, beta_gm, output_gm, outputMean_gm, outputVariance_gm, tilingData.groupNormTilingData);
        if (TILING_KEY_IS(1)) {
            op.Process();
        }
    }
    ```
