# RmsNorm Tiling

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-10T02:47:49.363Z -->

## Description

Ascend C provides the RmsNorm Tiling API, which allows you to obtain the tiling parameters required for RmsNorm kernel computation.

Obtaining tiling parameters involves the following two steps:

1.  Use **GetRmsNormMaxMinTmpSize** to obtain the maximum and minimum temporary space sizes required for RmsNorm API computation.

    The RmsNorm API computation on the kernel side requires you to reserve/allocate temporary space. **GetRmsNormMaxMinTmpSize** is used on the host side to obtain the maximum and minimum temporary space sizes to be reserved/allocated. Based on this range, you can select an appropriate space size and pass it to the kernel side as a tiling parameter.

    -   To ensure correct functionality, the reserved/allocated temporary space size must not be smaller than the minimum temporary space size.
    -   Within the range from the minimum temporary space to the maximum temporary space, as the temporary space increases, the computation performance of the kernel-side API improves to a certain extent. To achieve better performance, you can reserve/allocate space based on the actual memory usage.

2.  Use **GetRmsNormTilingInfo** to obtain the tiling parameters required by the RmsNorm kernel-side API.

    The RmsNorm Tiling structure is defined as follows. You do not need to know the details of this tiling structure; simply pass it to the kernel side and then to the RmsNorm high-level API for direct use.

    ```
    struct RmsNormTiling {
        uint32_t bLength = 0;
        uint32_t sLength = 0;
        uint32_t hLength = 0;
        uint32_t originalHLength = 0;
        float reciprocalOfHLength = 0;
        uint32_t mainBshLength = 0;
        uint32_t mainBsLength = 0;
        uint32_t mainBsLengthAlign = 0;
        uint32_t loopRound = 0;
        uint32_t inputTailPos = 0;
        uint32_t tailBshLength = 0;
        uint32_t tailBsLength = 0;
    };
    ```

## Function Prototype

```
bool GetRmsNormMaxMinTmpSize(const ge::Shape& srcShape, const uint32_t typeSize, uint32_t& maxValue, uint32_t& minValue, const bool isBasicBlock = false)
```

```
bool GetRmsNormTilingInfo(const ge::Shape& srcShape, const ge::Shape& originSrcShape, const uint32_t stackBufferByteSize, const uint32_t typeSize, optiling::RmsNormTiling& tiling, const bool isBasicBlock = false)
```

```
bool GetRmsNormTilingInfo(const ge::Shape& srcShape, const ge::Shape& originSrcShape, const uint32_t stackBufferByteSize, const uint32_t typeSize, AscendC::tiling::RmsNormTiling& tiling, const bool isBasicBlock = false)
```

## Parameters

**Table 1** Parameters of the **GetRmsNormMaxMinTmpSize** API

| Parameter | Input/Output | Description |
| --- | --- | --- |
| srcShape | Input | Input shape information. |
| typeSize | Input | Size of the input data type, in bytes. For example, if the input data type is half, pass 2. |
| maxValue | Output | Maximum temporary space size required for the RmsNorm API to complete the computation. Space beyond this value is not used by this API. Within the range from the minimum temporary space to the maximum temporary space, as the temporary space increases, the kernel-side API computation performance is optimized to a certain extent. To achieve better performance, you can reserve or allocate space based on the actual memory usage. A maximum space size of 0 indicates that no temporary space is required for the computation.<br>maxValue is only a reference value and may be larger than the remaining space of the Unified Buffer. In this case, you need to select an appropriate temporary space size based on the remaining space of the Unified Buffer. |
| minValue | Output | Minimum temporary space size required for the RmsNorm API to complete the computation. To ensure functional correctness, the temporary space reserved or allocated during API computation must not be smaller than this value. A minimum space size of 0 indicates that no temporary space is required for the computation. |
| isBasicBlock | Input | Whether to enable basic block computation, consistent with the kernel-side API. The default value is false. |

**Table 2** Parameters of the GetRmsNormTilingInfo API

| Parameter | Input/Output | Description |
| --- | --- | --- |
| srcShape | Input | Shape information of the input tensor, which is the shape after 32B alignment on the H axis. Ensure that the B/S of **srcShape** is consistent with the B/S of **originSrcShape**. |
| originSrcShape | Input | Original shape information of the input. |
| stackBufferByteSize | Input | Remaining space available for the **RmsNorm** API computation, in bytes. Obtain the maximum and minimum temporary space sizes through **GetRmsNormMaxMinTmpSize**, and select an appropriate space size within this range as the value of **stackBufferByteSize**. |
| typeSize | Input | Size of the input data type, in bytes. For example, if the input data type is half, pass 2. |
| tiling | Output | Tiling information required for the RmsNorm computation. |
| isBasicBlock | Input | Whether to enable basic block computation, consistent with the kernel-side API. The default value is false. If basic block computation is enabled, ensure that the H of **originSrcShape** is also 32B-aligned. |

## Return Value

-   **GetRmsNormMaxMinTmpSize** returns **true** or **false**. **true** indicates that the maximum and minimum temporary space sizes required for the internal computation of the **RmsNorm** API are obtained successfully; **false** indicates a failure, in which case check whether the input shape meets the requirements.
-   **GetRmsNormTilingInfo** returns **true** or **false**. **true** indicates that all **Tiling** parameter values of **RmsNorm **are obtained successfully; **false** indicates a failure, in which case check whether the input **stackBufferByteSize** meets the minimum temporary space requirement. If the **isBasicBlock** switch is enabled, also check whether the input shape meets the basic block requirement.

## Constraints

None

## Examples

1.  Add the **RmsNorm Tiling** struct parameters to the **TilingData** struct as a field of the **TilingData** struct.

    ```
    BEGIN_TILING_DATA_DEF(RmsnormCustomTilingData)  // Register a tiling class, using the tiling name as the input parameter.
      TILING_DATA_FIELD_DEF(uint32_t, totalLength); // Add a tiling field for the total computation data volume.
      TILING_DATA_FIELD_DEF(uint32_t, tileNum);     // Add a tiling field for the number of total computation data blocks on each core.
      TILING_DATA_FIELD_DEF(uint32_t, tmpBufSize);  // Add a tiling field for the temporary space size.
      ...                                           // Add other tiling fields.
      TILING_DATA_FIELD_DEF_STRUCT(RmsNormTiling, rmsnormTilingData); // Add the RmsNormTiling structure parameters to the TilingData structure.
    END_TILING_DATA_DEF;
    ```

2.  In the Tiling implementation function, first call the **GetRmsNormMaxMinTmpSize** API to obtain the maximum/minimum temporary space size required for the RmsNorm API to complete the computation. Based on this range and the actual memory usage, set an appropriate space size, and then obtain the tiling parameters required by the RmsNorm kernel-side API based on the input shape, the remaining space available for computation, and other information.

    ```
    namespace optiling {
    const uint32_t NUM_BLOCKS = 8;
    const uint32_t TILE_NUM = 8;
    static ge::graphStatus TilingFunc(gert::TilingContext* context)
    {
        RmsNormCustomTilingData tiling;
        uint32_t totalLength = context->GetInputTensor(0)->GetShapeSize();
        context->SetBlockDim(NUM_BLOCKS);
        tiling.set_totalLength(totalLength);
        tiling.set_tileNum(TILE_NUM);
        // Set other Tiling parameters.
        ...
        std::vector<int64_t> shapeVec = {2, 16, 64};
        ge::Shape srcShape(shapeVec);
        std::vector<int64_t> oriShapeVec = {2, 16, 64};
        ge::Shape oriSrcShape(oriShapeVec);
        // This sample is for illustration only. The minimum value obtained by GetRmsNormMaxMinTmpSize is passed in to ensure correct functionality. You can pass in an appropriate space size as needed.
        uint32_t minValue = 0;
        uint32_t maxValue = 0;
        AscendC::GetRmsNormMaxMinTmpSize(srcShape, sizeof(half), maxValue, minValue, isBasicBlock);
        tiling.set_tmpBufSize(minValue);
        // Obtain the RmsNorm tiling parameters.
        AscendC::GetRmsNormTilingInfo(srcShape, oriSrcShape, minValue , sizeof(half), tiling.rmsnormTilingData, false);

         ... // Other logic.
        tiling.SaveToBuffer(context->GetRawTilingData()->GetData(), context->GetRawTilingData()->GetCapacity());
        context->GetRawTilingData()->SetDataSize(tiling.GetDataSize());
        context->SetTilingKey(1);
        return ge::GRAPH_SUCCESS;
    }
    } // namespace optiling
    ```

3.  On the kernel side, the corresponding kernel function obtains the **TilingData** by calling **GET\_TILING\_DATA** in the kernel function, and then passes the RmsNorm tiling information in the **TilingData** to the **RmsNorm** API for computation. For the complete kernel-side sample, see [RmsNorm](RmsNorm.md).

    ```
    extern "C" __global__ __aicore__ void rmsnorm_custom(GM_ADDR inputGm, GM_ADDR gammaGm, GM_ADDR outputGm, GM_ADDR tiling)
    {
        GET_TILING_DATA(tilingData, tiling);
        KernelRmsNorm op;
        op.Init(inputGm, gammaGm, outputGm, tilingData.totalLength, tilingData.tileNum, tilingData.rmsnormTilingData);
        if (TILING_KEY_IS(1)) {
            op.Process();
        }
    }
    ```
