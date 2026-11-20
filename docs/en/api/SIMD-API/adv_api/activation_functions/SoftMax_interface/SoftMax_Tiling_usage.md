# SoftMax Tiling Usage Guidelines

<!-- md-trans-meta sourceCommit=12a14b50a4f8b34df0b2db2cb19ae1e957b3bde2 translatedAt=2026-11-10T06:16:37.607Z pushedAt=2026-11-18T11:07:26.420Z nl=2 -->

Ascend C provides a set of SoftMax Tiling APIs to help you obtain the Tiling parameters required for SoftMax kernel-side API computation.

Obtaining tiling parameters involves the following two steps:

1.  Obtain the minimum and maximum temporary space sizes required for SoftMax API computation. Note that this step is optional and serves only as a reference for reasonably allocating computation space.
2.  Obtain the Tiling parameters required by the input SoftMax kernel-side API. You need to  the input shape of the [AscendC::TensorShape](../../data_structures/TensorShape.md) type, the remaining space available for SoftMax API computation, and the size of the computation data type.

    The SoftMax Tiling structure is defined as follows. You do not need to focus on the details of this Tiling structure; you only need to pass it to the kernel-side and then to the SoftMax high-order API for direct use.

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

For SoftMax/SimpleSoftMax, see [SoftMax/SimpleSoftMax Tiling](SoftMax-SimpleSoftMax-Tiling.md);

For SoftmaxFlash, see [SoftmaxFlash Tiling API](SoftmaxFlash_Tiling_interface.md);

For SoftmaxGrad, see [SoftmaxGrad Tiling API](SoftmaxGrad_Tiling_interface.md);

For SoftmaxFlashV2, see [SoftmaxFlashV2 Tiling API](SoftmaxFlashV2_Tiling_interface.md);

To determine whether SoftMaxTiling is basic block tiling, see [IsBasicBlockInSoftMax](IsBasicBlockInSoftMax.md).

## Examples

The following example describes how to obtain Tiling parameters on the host side when using the SoftMax high-order API, and how these parameters are used on the kernel-side. In the example, the input tensor has a shape of \[320,64\] and a data type of half.

1.  Add the SoftMaxTiling structure parameters to the TilingData structure as a field of the TilingData structure.

    ```
    BEGIN_TILING_DATA_DEF(TilingData)               // Register a tiling class, using the tiling name as the input parameter.
      TILING_DATA_FIELD_DEF(uint32_t, totalLength); // Add a tiling field: total computation data size.
      TILING_DATA_FIELD_DEF(uint32_t, tileNum);     // Add a tiling field: number of computation data blocks per core.
      ...                                           // Add other tiling fields.
      TILING_DATA_FIELD_DEF_STRUCT(SoftMaxTiling, softmaxTilingData); // Add the SoftMaxTiling structure parameters to the TilingData structure.
    END_TILING_DATA_DEF;
    ```

2.  In the tiling implementation function, first call the **GetSoftMaxMaxTmpSize/GetSoftMaxMinTmpSize** API to obtain the maximum/minimum temporary space size required for the SoftMax API to complete the computation, and set an appropriate space size within this range based on the actual memory usage. Then, obtain the Tiling parameters required by the SoftMax kernel-side API based on the input shape, the remaining available space for computation, and other information.

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
        // Set other tiling parameters.
        ...
        std::vector<int64_t> shapeVec = {320,64};
        AscendC::TensorShape srcShape(shapeVec);
        // This sample only serves as an illustration. The minimum value obtained by GetSoftMaxMinTmpSize is  to ensure correct functionality. You can  an appropriate space size as needed.
        const uint32_t localWorkSpaceSize = AscendC::GetSoftMaxMinTmpSize(srcShape, sizeof(half), false);
        // Obtain the SoftMax tiling parameters.
        AscendC::SoftMaxTilingFunc(srcShape, sizeof(half), localWorkSpaceSize, tiling.softmaxTilingData);
         ... // Other logic.
        tiling.SaveToBuffer(context->GetRawTilingData()->GetData(), context->GetRawTilingData()->GetCapacity());
        context->GetRawTilingData()->SetDataSize(tiling.GetDataSize());
        context->SetTilingKey(1);
        return ge::GRAPH_SUCCESS;
    }
    } // namespace optiling
    ```

3.  On the kernel-side, call GET\_TILING\_DATA in the kernel-side API to obtain the TilingData, and then pass the SoftMax tiling information in the TilingData to the SoftMax API for computation. For the complete kernel-side example, see [Examples](SoftMax.md#examples).

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
