# Transpose Tiling

<!-- md-trans-meta sourceCommit=aa6d602ebc3dbc8f7b50f91dc72aceefdfa67b49 translatedAt=2026-11-02T05:03:20.057Z pushedAt=2026-11-18T04:59:32.967Z nl=2 -->

## Description

Used to obtain Transpose Tiling parameters.

## Prototype

> [!NOTE]
>The **GetConfusionTransposeMaxMinTmpSize**, **GetConfusionTransposeTilingInfo**, and **GetConfusionTransposeOnlyTilingInfo** APIs are deprecated and will be removed in a later version. Do not use these APIs. Use the **GetTransposeMaxMinTmpSize** and **GetTransposeTilingInfo** APIs instead.

-   Obtain the minimum temporary space size.

    ```
    void GetTransposeMaxMinTmpSize(const AscendC::TensorShape& srcShape, const uint32_t typeSize, const uint32_t transposeTypeIn, uint32_t& maxValue, uint32_t& minValue)
    ```

    ```
    void GetConfusionTransposeMaxMinTmpSize(const AscendC::TensorShape& srcShape, const uint32_t typeSize, const uint32_t transposeTypeIn, uint32_t& maxValue, uint32_t& minValue)
    ```

-   Obtain Transpose Tiling.

    ```
    void GetTransposeTilingInfo(const AscendC::TensorShape& srcShape, const uint32_t stackBufferSize, const uint32_t typeSize, const uint32_t transposeTypeIn, optiling::ConfusionTransposeTiling& tiling)
    ```

    ```
    void GetTransposeTilingInfo(const AscendC::TensorShape& srcShape, const uint32_t stackBufferSize, const uint32_t typeSize, const uint32_t transposeTypeIn, AscendC::tiling::ConfusionTransposeTiling& tiling)
    ```

    ```
    void GetConfusionTransposeOnlyTilingInfo(const AscendC::TensorShape& srcShape, const uint32_t stackBufferSize, const uint32_t typeSize, optiling::ConfusionTransposeTiling& tiling)
    ```

    ```
    void GetConfusionTransposeOnlyTilingInfo(const AscendC::TensorShape& srcShape, const uint32_t stackBufferSize, const uint32_t typeSize, AscendC::tiling::ConfusionTransposeTiling& tiling)
    ```

    ```
    void GetConfusionTransposeTilingInfo(const AscendC::TensorShape& srcShape, const uint32_t stackBufferSize, const uint32_t typeSize, const uint32_t transposeTypeIn, optiling::ConfusionTransposeTiling& tiling)
    ```

    ```
    void GetConfusionTransposeTilingInfo(const AscendC::TensorShape& srcShape, const uint32_t stackBufferSize, const uint32_t typeSize, const uint32_t transposeTypeIn, AscendC::tiling::ConfusionTransposeTiling& tiling)
    ```

## Parameters

**Table 1** **Parameters of the GetTransposeMaxMinTmpSize API**

| Parameter | Input/Output | Description |
| --- | --- | --- |
| srcShape | Input | Shape information of the input Tensor. The parameter type is [AscendC::TensorShape](../data_structures/TensorShape.md). The specific format of srcShape is as follows:<br><br>Scenario 1: [B, N, S, H/N]<br><br>Scenario 2: [B, N, S, H/N]<br><br>Scenario 3: [B, N, S, H/N]<br><br>Scenario 4: [B, N, S, H/N]<br><br>Scenario 5: [B, N, S, H/N]<br><br>Scenario 6: [B, N, S, H/N]<br><br>Scenario 7: [H, W]<!-- npu="950" id1 --><br><br>Scenario 13: [H, W] or [N, H, W], only supported on Ascend 950PR&950DT products.<br><br>Scenario 14: [N, H, W], only supported on Ascend 950PR&950DT products.<br><br>Scenario 15: [N, H, W], only supported on Ascend 950PR&950DT products.<br><br>Scenario 16: [H, W], only supported on Ascend 950PR&950DT products.<!-- end id1 --> |
| typeSize | Input | Size of the input data type, in bytes. For example, if the input data type is half, pass 2 here. |
| transposeTypeIn | Input | Selects the data layout and reshape type. The corresponding scenario is selected based on the input number. The value range is [1, 7].<!-- npu="950" id2 -->For Ascend 950PR&950DT products, the value range is [1, 7] and [13, 16].<!-- end id2 --><br><br>Scenario 1 (NZ2ND, axis 1 and axis 2 interchange): 1<br><br>Scenario 2 (NZ2NZ, axis 1 and axis 2 interchange): 2<br><br>Scenario 3 (NZ2NZ, tail-axis splitting): 3<br><br>Scenario 4 (NZ2ND, tail-axis splitting): 4<br><br>Scenario 5 (NZ2ND, tail-axis merging): 5<br><br>Scenario 6 (NZ2NZ, tail-axis merging): 6<br><br>Scenario 7 (2D transpose): 7<!-- npu="950" id5 --><br><br>Scenario 13 (2D transpose or transpose of the last two dimensions in a 3D transpose): 13, only supported on Ascend 950PR&950DT products.<br><br>Scenario 14 (interchange of the first and second dimensions in a 3D transpose): 14, only supported on Ascend 950PR&950DT products.<br><br>Scenario 15 (interchange of the first and third dimensions in a 3D transpose): 15, only supported on Ascend 950PR&950DT products.<br><br>Scenario 16 (2D ND2NZ transpose using interleaving instructions): 16, only supported on Ascend 950PR&950DT products.<!-- end id5 --> |
| maxValue | Output | Maximum temporary space size required for the Transpose API to complete computation, in bytes. Space exceeding this value is not used by this API. Within the range from the minimum temporary space to the maximum temporary space, as the temporary space increases, the computation performance of the kernel function side API improves to a certain extent. To achieve better performance, you can reserve/allocate space based on the actual memory usage.<br>maxValue is only a reference value and may be larger than the remaining space of the Unified Buffer (UB). In this case, you need to select an appropriate temporary space size based on the remaining UB space. |
| minValue | Output | Minimum temporary space size required for the Transpose API to complete computation, in bytes. To ensure functional correctness, the temporary space reserved/allocated during API computation must not be smaller than this value. |

**Table 2** **Parameter list of the GetTransposeTilingInfo API**

| Parameter | Input/Output | Description |
| --- | --- | --- |
| srcShape | Input | Input shape information. The parameter type is [AscendC::TensorShape](../data_structures/TensorShape.md). The specific format of srcShape is as follows:<br><br>Scenario 1: [B, N, S, H/N]<br><br>Scenario 2: [B, N, S, H/N]<br><br>Scenario 3: [B, N, S, H/N]<br><br>Scenario 4: [B, N, S, H/N]<br><br>Scenario 5: [B, N, S, H/N]<br><br>Scenario 6: [B, N, S, H/N]<br><br>Scenario 7: [H, W]<!-- npu="950" id3 --><br><br>Scenario 13: [H, W] or [N, H, W], only supported on Ascend 950PR&950DT products.<br><br>Scenario 14: [N, H, W], only supported on Ascend 950PR&950DT products.<br><br>Scenario 15: [N, H, W], only supported on Ascend 950PR&950DT products.<br><br>Scenario 16: [H, W], only supported on Ascend 950PR&950DT products.<!-- end id3 --> |
| stackBufferSize | Input | Space size available for the Transpose API computation, in bytes. |
| typeSize | Input | Size of the input data type, in bytes. For example, if the input data type is half, pass 2 here. |
| transposeTypeIn | Input | Selects the data layout and reshape type. The corresponding scenario is selected based on the input number. The value range is [1, 7].<!-- npu="950" id4 --><br>For Ascend 950PR&950DT products, the value range is [1, 7] and [13, 16].<!-- end id4 --><br><br>Scenario 1 (NZ2ND, axis 1 and axis 2 interchange): 1<br><br>Scenario 2 (NZ2NZ, axis 1 and axis 2 interchange): 2<br><br>Scenario 3 (NZ2NZ, tail-axis splitting): 3<br><br>Scenario 4 (NZ2ND, tail-axis splitting): 4<br><br>Scenario 5 (NZ2ND, tail-axis merging): 5<br><br>Scenario 6 (NZ2NZ, tail-axis merging): 6<br><br>Scenario 7 (2D transpose): 7<!-- npu="950" id6 --><br><br>Scenario 13 (2D transpose or transpose of the last two dimensions in a 3D transpose): 13, only supported on Ascend 950PR&950DT products.<br><br>Scenario 14 (interchange of the first and second dimensions in a 3D transpose): 14, only supported on Ascend 950PR&950DT products.<br><br>Scenario 15 (interchange of the first and third dimensions in a 3D transpose): 15, only supported on Ascend 950PR&950DT products.<br><br>Scenario 16 (2D ND2NZ transpose using interleaving instructions): 16, only supported on Ascend 950PR&950DT products.<!-- end id6 --> |
| tiling | Output | Splitting information of the input data. |

## Return Value

None

## Constraints

None

## Examples

The following example describes the process of obtaining Tiling parameters on the host side when using the Transpose high-level API, and how these parameters are used on the kernel function side. The example uses scenario 1, where the input Tensor shape is \[1, 2, 64, 32\], and the input data type is half.

1.  Add the **ConfusionTransposeTiling** structure parameter to the **TilingData** structure as a field of the **TilingData** structure.

    ```
    BEGIN_TILING_DATA_DEF(TilingData)         // Register a tiling class, using the tiling name as the input parameter.
        TILING_DATA_FIELD_DEF(uint32_t, tileNum); // Add a tiling field, the number of data blocks for the total computation on each core.
        ...                                       // Add other tiling fields.
        TILING_DATA_FIELD_DEF_STRUCT(
            ConfusionTransposeTiling,
            confusionTransposeTilingData); // Add the ConfusionTransposeTiling structure parameter to the TilingData structure.
    END_TILING_DATA_DEF;
    ```

2.  In the Tiling implementation function, first call the **GetTransposeMaxMinTmpSize** API to obtain the maximum and minimum temporary space sizes required for the Transpose API to complete computation, and set an appropriate space size based on this range and the actual memory usage. Then, based on the input shape, the remaining space available for computation, and other information, obtain the Tiling parameters required by the Transpose kernel function side API.

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
        std::vector<int64_t> shapeVec = {1, 2, 64, 32};
        AscendC::TensorShape srcShape(shapeVec);

        uint32_t transposeTypeIn = 1;
        uint32_t maxValue = 0;
        uint32_t minValue = 0;
        AscendC::GetTransposeMaxMinTmpSize(srcShape, sizeof(half), transposeTypeIn, maxValue, minValue);
        // To ensure functional correctness, this example passes the minimum temporary space size. You can pass an appropriate space size as needed.
        const uint32_t stackBufferSize = minValue;
        // Obtain the Tiling parameters required by the Transpose API.
        AscendC::GetTransposeTilingInfo(
            srcShape, stackBufferSize, sizeof(half), transposeTypeIn, tiling.confusionTransposeTilingData);
        ...
        // Other logic.
        tiling.SaveToBuffer(context->GetRawTilingData()->GetData(), context->GetRawTilingData()->GetCapacity());
        context->GetRawTilingData()->SetDataSize(tiling.GetDataSize());
        context->SetTilingKey(1);
        return ge::GRAPH_SUCCESS;
    }
    } // namespace optiling
    ```

3.  On the corresponding kernel function (Kernel) side, **TilingData** is obtained by calling GET\_TILING\_DATA in the kernel function (Kernel), and then the **ConfusionTransposeTiling** information in **TilingData** is passed to the **Transpose** API for computation. For the complete kernel function (Kernel) side example, see [Transpose](Transpose.md).

    ```
    extern "C" __global__ __aicore__ void func_custom(GM_ADDR src_gm, GM_ADDR dst_gm, GM_ADDR workspace, GM_ADDR tiling)
    {
        GET_TILING_DATA(TilingData, tiling);
        KernelTranspose<half> op;
        op.Init(src_gm, dst_gm, TilingData.confusionTransposeTilingData);
        op.Process();
    }
    ```
