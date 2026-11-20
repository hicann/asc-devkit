# RmsNorm Tiling

<!-- md-trans-meta sourceCommit=aa6d602ebc3dbc8f7b50f91dc72aceefdfa67b49 translatedAt=2026-11-10T06:34:18.571Z pushedAt=2026-11-18T11:07:26.837Z nl=2 -->

## Description

Ascend C provides the RmsNorm Tiling API, which helps you obtain the tiling parameters required for RmsNorm kernel-side API computation.

Obtaining tiling parameters involves the following two steps:

1.  Use **GetRmsNormMaxMinTmpSize** to obtain the maximum and minimum temporary space sizes required for RmsNorm API computation.

    The RmsNorm API on the kernel-side requires you to reserve/allocate temporary space for computation. **GetRmsNormMaxMinTmpSize** is used on the host side to obtain the maximum and minimum temporary space sizes to reserve/allocate. Based on this range, you can select an appropriate space size and pass it to the kernel-side as a tiling parameter.

    -   To ensure correct functionality, the reserved or allocated temporary space size must not be smaller than the minimum temporary space size.
    -   Within the range from the minimum to the maximum temporary space size, as the temporary space increases, the computation performance of the kernel-side API improves to a certain extent. To achieve better performance, you can reserve or allocate space based on the actual memory usage.

2.  Use **GetRmsNormTilingInfo** to obtain the tiling parameters required by the RmsNorm kernel-side API.

    The RmsNorm Tiling structure is defined as follows. You do not need to know the details of this tiling structure; you only need to pass it to the kernel-side and then to the RmsNorm high-order API for direct use.

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

## Prototype

```
bool GetRmsNormMaxMinTmpSize(const AscendC::TensorShape& srcShape, const uint32_t typeSize, uint32_t& maxValue, uint32_t& minValue, const bool isBasicBlock = false)
```

```
bool GetRmsNormTilingInfo(const AscendC::TensorShape& srcShape, const AscendC::TensorShape& originSrcShape, const uint32_t stackBufferByteSize, const uint32_t typeSize, optiling::RmsNormTiling& tiling, const bool isBasicBlock = false)
```

```
bool GetRmsNormTilingInfo(const AscendC::TensorShape& srcShape, const AscendC::TensorShape& originSrcShape, const uint32_t stackBufferByteSize, const uint32_t typeSize, AscendC::tiling::RmsNormTiling& tiling, const bool isBasicBlock = false)
```

## Parameters

**Table 1**  Parameters of the GetRmsNormMaxMinTmpSize API

| Parameter | Input/Output | Description |
| --- | --- | --- |
| srcShape | Input | Input shape information. The parameter type is [AscendC::TensorShape](../data_structures/TensorShape.md). |
| typeSize | Input | Size of the input data type, in bytes. For example, if the input data type is half, pass 2 here. |
| maxValue | Output | Maximum temporary space size required by the RmsNorm API to complete the computation, unit: bytes. Space exceeding this value is not used by this API. Within the range from the minimum temporary space size to the maximum temporary space size, as the temporary space increases, the computation performance of the kernel-side API improves to a certain extent. To achieve better performance, you can reserve/allocate space based on the actual memory usage. A maximum space size of 0 indicates that no temporary space is required for computation.<br>maxValue is only a reference value and may be larger than the remaining space of the Unified Buffer (UB). In this case, you need to select an appropriate temporary space size based on the remaining UB space. |
| minValue | Output | Minimum temporary space size required by the RmsNorm API to complete the computation, unit: bytes. To ensure functional correctness, the temporary space reserved/allocated during API computation must not be smaller than this value. A minimum space size of 0 indicates that no temporary space is required for computation. |
| isBasicBlock | Input | Whether to enable basic block computation, consistent with the kernel-side API. The default value is false. |

**Table 2**  Parameters of the GetRmsNormTilingInfo API

| Parameter | Input/Output | Description |
| --- | --- | --- |
| srcShape | Input | Shape information of the input tensor. The parameter type is [AscendC::TensorShape](../data_structures/TensorShape.md). This is the shape after 32B alignment along the H axis. The B/S of srcShape must be consistent with the B/S of originSrcShape. |
| originSrcShape | Input | Original shape information of the input. The parameter type is [AscendC::TensorShape](../data_structures/TensorShape.md). |
| stackBufferByteSize | Input | Remaining space size available for the RmsNorm API computation, in bytes. Obtain the maximum and minimum temporary space sizes through GetRmsNormMaxMinTmpSize, and select an appropriate space size within this range as stackBufferByteSize. |
| typeSize | Input | Size of the input data type, in bytes. For example, if the input data type is half, pass 2 here. |
| tiling | Output | Tiling information required for the RmsNorm computation. |
| isBasicBlock | Input | Whether to enable basic block computation, consistent with the kernel-side API. The default value is false. If basic block is enabled, ensure that the H of originSrcShape is also 32B aligned. |

## Return Value

-   GetRmsNormMaxMinTmpSize returns true/false. true indicates that the maximum and minimum temporary space sizes required for the internal computation of the RmsNorm API are successfully obtained; false indicates a failure. In the case of failure, check whether the input shape meets the requirements.
-   GetRmsNormTilingInfo returns true/false. true indicates that all tiling parameter values of RmsNorm are successfully obtained; false indicates a failure. In the case of failure, check whether the input stackBufferByteSize meets the minimum temporary space requirement. If the isBasicBlock switch is enabled, check whether the input shape meets the basic block requirements.

## Constraints

None

## Examples

1.  Add the RmsNorm Tiling structure parameters to the TilingData structure as a field of the TilingData structure.

    ```
    BEGIN_TILING_DATA_DEF(RmsnormCustomTilingData) // Register a tiling class, using the tiling name as the input parameter.
        TILING_DATA_FIELD_DEF(uint32_t, totalLength);  // Add a tiling field for the total amount of data to compute.
        TILING_DATA_FIELD_DEF(uint32_t, tileNum);      // Add a tiling field for the number of data blocks per core.
        TILING_DATA_FIELD_DEF(uint32_t, tmpBufSize);   // Add the tiling field for the temporary space size.
        ...                                            // Add other tiling fields.
        TILING_DATA_FIELD_DEF_STRUCT(RmsNormTiling, rmsnormTilingData); // Add the RmsNormTiling structure parameter to the TilingData structure.
    END_TILING_DATA_DEF;
    ```

2.  In the Tiling implementation function, first call the **GetRmsNormMaxMinTmpSize** API to obtain the maximum and minimum temporary space sizes required by the RmsNorm API to complete the computation. Based on this range and the actual memory usage, set an appropriate space size. Then, based on the input shape, the remaining space size available for computation, and other information, obtain the tiling parameters required by the RmsNorm kernel-side API.

    ```
    namespace optiling {
    const uint32_t NUM_BLOCKS = 8;
    const uint32_t TILE_NUM = 8;
    static ge::graphStatus TilingFunc(gert::TilingContext* context)
    {
        RmsNormCustomTilingData tiling;
        uint32_t totalLength = context->GetInputTensor(0)->GetShapeSize();
        context->SetSimdNumBlocks(NUM_BLOCKS);
        tiling.set_totalLength(totalLength);
        tiling.set_tileNum(TILE_NUM);
        // Set other Tiling parameters.
        ...
        std::vector<int64_t> shapeVec = {2, 16, 64};
        AscendC::TensorShape srcShape(shapeVec);
        std::vector<int64_t> oriShapeVec = {2, 16, 64};
        AscendC::TensorShape oriSrcShape(oriShapeVec);
        // This sample is for illustration only. The minimum value obtained by GetRmsNormMaxMinTmpSize is  to ensure functional correctness. You can  an appropriate space size as needed.
        uint32_t minValue = 0;
        uint32_t maxValue = 0;
        AscendC::GetRmsNormMaxMinTmpSize(srcShape, sizeof(half), maxValue, minValue, isBasicBlock);
        tiling.set_tmpBufSize(minValue);
        // Obtain the RmsNorm Tiling parameters.
        AscendC::GetRmsNormTilingInfo(srcShape, oriSrcShape, minValue, sizeof(half), tiling.rmsnormTilingData, false);

        ... // Other logic.
        tiling.SaveToBuffer(context->GetRawTilingData()->GetData(), context->GetRawTilingData()->GetCapacity());
        context->GetRawTilingData()->SetDataSize(tiling.GetDataSize());
        context->SetTilingKey(1);
        return ge::GRAPH_SUCCESS;
    }
    } // namespace optiling
    ```

3.  Obtain the TilingData by calling GET\_TILING\_DATA on the kernel side, and then pass the RmsNorm Tiling information in the TilingData to the RmsNorm API for computation. For the complete kernel-side sample, see [RmsNorm](RmsNorm.md).

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
