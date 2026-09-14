# UnPad Tiling

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-08T16:57:59.222Z -->

## Description

Used to obtain the UnPad tiling parameters.

## Prototype

```
void GetUnPadMaxMinTmpSize(const platform_ascendc::PlatformAscendC& ascendcPlatform, const ge::Shape& srcShape, const uint32_t typeSize, uint32_t& maxValue, uint32_t& minValue)
```

```
void UnPadTilingFunc(const ge::Shape srcShape, const uint32_t stackBufferSize, const uint32_t typeSize, optiling::UnPadTiling& tiling)
```

```
void UnPadTilingFunc(const ge::Shape srcShape, const uint32_t stackBufferSize, const uint32_t typeSize, AscendC::tiling::UnPadTiling& tiling)
```

## Parameters

**Table 1** **Parameters of the GetUnPadMaxMinTmpSize API**

| Parameter | Input/Output | Description |
| --- | --- | --- |
| ascendcPlatform | Input | Passes the hardware platform information. For the definition of PlatformAscendC, see [Constructors and Destructors](../../../Utils-API/platform_info/PlatformAscendC/constructors_and_destructors.md). |
| srcShape | Input | Shape information of the input tensor. The shape is two-dimensional. |
| typeSize | Input | Size of the input data type, in bytes. For example, if the input data type is half, pass 2 here. |
| maxValue | Output | Maximum temporary space size required for the UnPad API to complete computation. Space exceeding this value is not used by the API. Within the range from the minimum temporary space to the maximum temporary space, as the temporary space increases, the computation performance of the kernel-side API is optimized to a certain extent. To achieve better performance, you can reserve/allocate space based on the actual memory usage. A maximum space size of 0 indicates that no temporary space is required for computation.<br>maxValue is only a reference value and may be larger than the remaining space of the Unified Buffer. In this case, you need to select an appropriate temporary space size based on the remaining space of the Unified Buffer. |
| minValue | Output | Minimum temporary space size required for the UnPad API to complete computation. To ensure functional correctness, the temporary space reserved/allocated during API computation must not be smaller than this value. A minimum space size of 0 indicates that no temporary space is required for computation. |

**Table 2** **Parameters of the UnPadTilingFunc API**

| Parameter | Input/Output | Description |
| --- | --- | --- |
| srcShape | Input | Shape information of the input tensor. The shape is two-dimensional. |
| stackBufferSize | Input | Temporary space size available for the UnPad API computation, in bytes. |
| typeSize | Input | Size of the input data type, in bytes. For example, if the input data type is half, pass 2 here. |
| tiling | Output | Outputs the tiling information required by the UnPad API. |

## Return Value

None

## Constraints

None

## Examples

The following example describes the process of obtaining tiling parameters on the host side when using the UnPad high-level API and how these parameters are used on the kernel side. In the example, the size of the original shape is \[320, 64\], the size of the target shape after unpad is \[320, 63\], and the input data type is half.

1.  Add the UnPadTiling structure parameter to the TilingData structure as a field of the TilingData structure.

    ```
    BEGIN_TILING_DATA_DEF(TilingData)               // Register a tiling class, using the tiling name as the input parameter.
      TILING_DATA_FIELD_DEF(uint32_t, totalLength); // Add a tiling field for the total amount of computation data.
      TILING_DATA_FIELD_DEF(uint32_t, tileNum);     // Add the tiling field, which is the number of computation data blocks on each core.
      ...                                           // Add other tiling fields.
      TILING_DATA_FIELD_DEF_STRUCT(UnPadTiling, unpadTilingData); // Add the UnPadTiling structure parameters to the TilingData structure.
    END_TILING_DATA_DEF;
    ```

2.  In the Tiling implementation function, first call the **GetUnPadMaxMinTmpSize** API to obtain the maximum and minimum temporary space sizes required for the UnPad API to complete computation, and set an appropriate space size based on this range and the actual memory usage. Then, obtain the tiling parameters required by the UnPad kernel-side API based on information such as the input shape and the remaining space available for computation.

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
        std::vector<int64_t> shapeVec = {320,64};
        ge::Shape srcShape(shapeVec);
        uint32_t maxValue = 0;
        uint32_t minValue = 0;
        auto platformInfo = context->GetPlatformInfo();
        auto ascendcPlatform = platform_ascendc::PlatformAscendC(platformInfo);
        AscendC::GetUnPadMaxMinTmpSize(ascendcPlatform, srcShape, sizeof(half), maxValue, minValue);
        // This sample is for illustration only. It obtains and passes the minimum value to ensure correct functionality. You can pass an appropriate space size as needed.
        const uint32_t localWorkSpaceSize = minValue;
        AscendC::UnPadTilingFunc(srcShape, localWorkSpaceSize , sizeof(half), tiling.unpadTilingData);
         ...
        tiling.SaveToBuffer(context->GetRawTilingData()->GetData(), context->GetRawTilingData()->GetCapacity());
        context->GetRawTilingData()->SetDataSize(tiling.GetDataSize());
        context->SetTilingKey(1);
        return ge::GRAPH_SUCCESS;
    }
    } // namespace optiling
    ```

3.  On the corresponding kernel side, obtain the TilingData by calling GET\_TILING\_DATA in the kernel function, and then pass the UnPad Tiling information in the TilingData to the UnPad API for computation. For the complete kernel-side sample, see [Examples](UnPad.md#examples).

    ```
    extern "C" __global__ __aicore__ void func_custom(GM_ADDR x, GM_ADDR y, GM_ADDR z, GM_ADDR workspace, GM_ADDR tiling)
    {
        GET_TILING_DATA(tilingData, tiling);
        KernelFunc op;
        op.Init(x, y, z, tilingData.totalLength, tilingData.tileNum,tilingData.unpadTilingData);
        if (TILING_KEY_IS(1)) {
            op.Process();
        }
    }
    ```
