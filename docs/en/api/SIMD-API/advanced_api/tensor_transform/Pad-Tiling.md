# Pad Tiling

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-08T16:01:22.015Z -->

## Description

Used to obtain Pad tiling parameters.

## Prototype

```
void GetPadMaxMinTmpSize(const ge::Shape& srcShape, const uint32_t typeSize, uint32_t& maxValue, uint32_t& minValue)
```

```
void PadTilingFunc(const ge::Shape srcShape, const ge::Shape oriSrcShape, const uint32_t stackBufferSize, const uint32_t typeSize, optiling::PadTiling& tiling)
```

```
void PadTilingFunc(const ge::Shape srcShape, const ge::Shape oriSrcShape, const uint32_t stackBufferSize, const uint32_t typeSize, AscendC::tiling::PadTiling& tiling)
```

## Parameters

**Table 1** **Parameter description of the GetPadMaxMinTmpSize API**

| Parameter | Input/Output | Description |
| --- | --- | --- |
| srcShape | Input | Shape information of the input tensor. The shape is two-dimensional. |
| typeSize | Input | Size of the input data type, in bytes. For example, if the input data type is half, pass 2 here. |
| maxValue | Output | Maximum temporary space size required for the Pad API to complete computation. Space exceeding this value is not used by the API. Within the range from the minimum temporary space to the maximum temporary space, as the temporary space increases, the computation performance of the kernel-side API is optimized to a certain extent. To achieve better performance, you can reserve/apply for space based on the actual memory usage.<br>maxValue is only a reference value and may be larger than the remaining space of the Unified Buffer. In this case, you need to select an appropriate temporary space size based on the remaining space of the Unified Buffer. |
| minValue | Output | Minimum temporary space size required for the Pad API to complete computation. To ensure functional correctness, the temporary space reserved/applied for during API computation must not be smaller than this value. |

**Table 2** **Parameter description of the PadTilingFunc API**

| Parameter | Input/Output | Description |
| --- | --- | --- |
| srcShape | Input | Shape information of the input tensor. The shape is two-dimensional. (Valid data + redundant data) |
| oriSrcShape | Input | Original shape information of the input tensor. The shape is two-dimensional. (Valid data) |
| stackBufferSize | Input | Temporary space size available for the Pad API computation, in bytes. |
| typeSize | Input | Size of the input data type, in bytes. For example, if the input data type is half, pass 2 here. |
| tiling | Output | Outputs the tiling information required by the Pad API. |

## Return Value

None

## Constraints

None

## Examples

The following example describes the process of obtaining tiling parameters on the host side when using the Pad high-level API, and how these parameters are used on the kernel side. In the example, the shape information and original shape information of the input tensor are \[320, 63\], and the input data type is half.

1.  Add the PadTiling structure parameters to the TilingData structure as a field of the TilingData structure.

    ```
    BEGIN_TILING_DATA_DEF(TilingData)               // Register a tiling class, using the tiling name as the input parameter.
      TILING_DATA_FIELD_DEF(uint32_t, totalLength); // Add a tiling field for the total computation data size.
      TILING_DATA_FIELD_DEF(uint32_t, tileNum);     // Add the tiling field, which is the number of data blocks into which the total computation data on each core is divided.
      ...                                           // Add other tiling fields.
      TILING_DATA_FIELD_DEF_STRUCT(PadTiling, padTilingData); // Add the PadTiling structure parameter to the TilingData structure.
    END_TILING_DATA_DEF;
    ```

2.  In the Tiling implementation function, first call the **GetPadMaxMinTmpSize** API to obtain the maximum and minimum temporary space sizes required for the Pad API to complete computation, and set an appropriate space size based on this range and the actual memory usage. Then, based on the input shape, the remaining space available for computation, and other information, obtain the tiling parameters required by the Pad kernel-side API.

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
        // Set other Tiling parameters.
        ...
        std::vector<int64_t> shapeVec = {320,63};
        ge::Shape srcShape(shapeVec);
        std::vector<int64_t> oriShapeVec = {320,63};
        ge::Shape oriSrcShape(oriShapeVec);

        uint32_t maxValue = 0;
        uint32_t minValue = 0;
        AscendC::GetPadMaxMinTmpSize(srcShape, sizeof(half), maxValue, minValue);
        // This sample is for illustration only. The minimum value is obtained and passed in to ensure correct functionality. You can pass in an appropriate space size as needed.
        const uint32_t localWorkSpaceSize = minValue;
        AscendC::PadTilingFunc(srcShape, oriSrcShape, localWorkSpaceSize , sizeof(half), tiling.padTilingData);
        // Other logic.
        ...
        tiling.SaveToBuffer(context->GetRawTilingData()->GetData(), context->GetRawTilingData()->GetCapacity());
        context->GetRawTilingData()->SetDataSize(tiling.GetDataSize());
        context->SetTilingKey(1);
        return ge::GRAPH_SUCCESS;
    }
    } // namespace optiling
    ```

3.  On the corresponding kernel side, obtain the TilingData by calling GET\_TILING\_DATA in the kernel function, and then pass the Pad Tiling information in the TilingData to the Pad API for computation. For the complete kernel-side sample, see [Examples](Pad.md#examples).

    ```
    extern "C" __global__ __aicore__ void func_custom(GM_ADDR x, GM_ADDR y, GM_ADDR z, GM_ADDR workspace, GM_ADDR tiling)
    {
        GET_TILING_DATA(tilingData, tiling);
        KernelFunc op;
        op.Init(x, y, z, tilingData.totalLength, tilingData.tileNum,tilingData.padTilingData);
        if (TILING_KEY_IS(1)) {
            op.Process();
        }
    }
    ```
