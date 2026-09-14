# SoftMax Tiling Usage Guidelines

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-05T13:46:45.547Z -->

Ascend C provides a set of SoftMax Tiling APIs to help you obtain the Tiling parameters required for SoftMax kernel computation.

Obtaining Tiling parameters involves the following two steps:

1.  Obtain the minimum and maximum temporary space sizes required for SoftMax API computation. Note that this step is optional and serves only as a reference for reasonably allocating computation space.
2.  Obtain the Tiling parameters required by the input SoftMax kernel-side API. You need to pass in the input shape, the remaining space available for SoftMax API computation, and the size of the computation data type.

    The SoftMax Tiling structure is defined as follows. You do not need to focus on the specific details of this Tiling structure; you only need to pass it to the kernel side and then to the SoftMax high-level API for direct use.

    ```
    struct SoftMaxTiling {
        uint32_t srcM = 0;
        uint32_t srcK = 0;
        uint32_t srcSize = 0;
        uint32_t outMaxM = 0;
        uint32_t outMaxK = 0;
        uint32_t outMaxSize = 0;
        uint32_t splitM = 0;
        uint32_t splitK = 0;
        uint32_t splitSize = 0;
        uint32_t reduceM = 0;
        uint32_t reduceK = 0;
        uint32_t reduceSize = 0;
        uint32_t rangeM = 0;
        uint32_t tailM = 0;
        uint32_t tailSplitSize = 0;
        uint32_t tailReduceSize = 0;
    };
    ```

For SoftMax/SimpleSoftMax, see [SoftMax/SimpleSoftMax Tiling](SoftMax-SimpleSoftMax-Tiling.md).

For SoftmaxFlash, see [SoftmaxFlash Tiling API](softmaxflash_tiling_api.md).

For SoftmaxGrad, see [SoftmaxGrad Tiling API](softmaxgrad_tiling_api.md).

For SoftmaxFlashV2, see [SoftmaxFlashV2 Tiling API](softmaxflashv2_tiling_api.md).

To determine whether SoftMaxTiling is a basic block tiling, see [IsBasicBlockInSoftMax](IsBasicBlockInSoftMax.md).

## Examples

The following example describes the process of obtaining tiling parameters on the host side when using the SoftMax high-level API, and how these parameters are used on the kernel side. In the example, the input Tensor has a shape of \[320,64\] and a data type of half.

1.  Add the **SoftMaxTiling** structure parameters to the **TilingData** structure as a field of the **TilingData** structure.

    ```
    BEGIN_TILING_DATA_DEF(TilingData)               // Register a tiling class, using the tiling name as the input parameter.
      TILING_DATA_FIELD_DEF(uint32_t, totalLength); // Add a tiling field for the total computation data size.
      TILING_DATA_FIELD_DEF(uint32_t, tileNum);     // Add a tiling field for the number of computation data blocks per core.
      ...                                           // Add other tiling fields.
      TILING_DATA_FIELD_DEF_STRUCT(SoftMaxTiling, softmaxTilingData); // Add the SoftMaxTiling structure parameters to the TilingData structure.
    END_TILING_DATA_DEF;
    ```

2.  In the tiling implementation function, first call the **GetSoftMaxMaxTmpSize/GetSoftMaxMinTmpSize** APIs to obtain the maximum/minimum temporary space size required for the SoftMax API to complete computation, and set an appropriate space size based on this range and the actual memory usage. Then, obtain the tiling parameters required by the SoftMax kernel-side API based on the input shape, the remaining space available for computation, and other information.

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
        // Set other tiling parameters.
        ...
        std::vector<int64_t> shapeVec = {320,64};
        ge::Shape srcShape(shapeVec);
        // This is only an example. The minimum size obtained by GetSoftMaxMinTmpSize is passed in to ensure correct functionality. Developers can pass in an appropriate space size as needed.
        const uint32_t localWorkSpaceSize = AscendC::GetSoftMaxMinTmpSize(srcShape, sizeof(half), false);
        // Obtain the SoftMax Tiling parameters.
        AscendC::SoftMaxTilingFunc(srcShape, sizeof(half), localWorkSpaceSize, tiling.softmaxTilingData);
         ... // Other logic.
        tiling.SaveToBuffer(context->GetRawTilingData()->GetData(), context->GetRawTilingData()->GetCapacity());
        context->GetRawTilingData()->SetDataSize(tiling.GetDataSize());
        context->SetTilingKey(1);
        return ge::GRAPH_SUCCESS;
    }
    } // namespace optiling
    ```

3.  On the kernel side, the corresponding kernel function calls GET\_TILING\_DATA to obtain the TilingData, and then passes the SoftMax Tiling information in the TilingData to the SoftMax API for computation. For the complete kernel-side example, see [Examples](SoftMax.md#examples).

    ```
    extern "C" __global__ __aicore__ void func_custom(GM_ADDR x, GM_ADDR y, GM_ADDR z, GM_ADDR workspace, GM_ADDR tiling)
    {
        GET_TILING_DATA(tilingData, tiling);
        KernelFunc op;
        op.Init(x, y, z, tilingData.totalLength, tilingData.tileNum,tilingData.SoftMaxTiling);
        if (TILING_KEY_IS(1)) {
            op.Process();
        }
    }
    ```

