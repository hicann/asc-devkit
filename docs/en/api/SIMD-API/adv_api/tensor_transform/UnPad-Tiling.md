# UnPad Tiling

<!-- md-trans-meta sourceCommit=aa6d602ebc3dbc8f7b50f91dc72aceefdfa67b49 translatedAt=2026-11-02T05:02:14.469Z pushedAt=2026-11-18T04:59:37.405Z nl=2 -->

## Description

Used to obtain UnPad Tiling parameters.

## Prototype

```
void GetUnPadMaxMinTmpSize(const platform_ascendc::PlatformAscendC& ascendcPlatform, const AscendC::TensorShape& srcShape, const uint32_t typeSize, uint32_t& maxValue, uint32_t& minValue)
```

```
void UnPadTilingFunc(const AscendC::TensorShape srcShape, const uint32_t stackBufferSize, const uint32_t typeSize, optiling::UnPadTiling& tiling)
```

```
void UnPadTilingFunc(const AscendC::TensorShape srcShape, const uint32_t stackBufferSize, const uint32_t typeSize, AscendC::tiling::UnPadTiling& tiling)
```

## Parameters

**Table 1** **GetUnPadMaxMinTmpSize API parameters**

| Parameter | Input/Output | Description |
| --- | --- | --- |
| ascendcPlatform | Input | Passes the hardware platform information. For the definition of PlatformAscendC, see [Constructor and Destructor](../../../Utils-API/platform_info/PlatformAscendC/constructor_and_destructor.md). |
| srcShape | Input | Shape information of the input tensor. The parameter type is [AscendC::TensorShape](../data_structures/TensorShape.md), and the shape is two-dimensional. |
| typeSize | Input | Size of the input data type, in bytes. For example, if the input data type is half, pass 2 here. |
| maxValue | Output | Maximum temporary space size required by the UnPad API to complete the computation, in bytes. Space exceeding this value is not used by the API. Within the range from the minimum temporary space to the maximum temporary space, as the temporary space increases, the computation performance of the kernel function (Kernel)-side API improves to a certain extent. To achieve better performance, you can reserve/allocate space based on the actual memory usage. A maximum space size of 0 indicates that no temporary space is required for the computation.<br>maxValue is only a reference value and may be larger than the remaining space of the Unified Buffer (UB). In this scenario, you need to select an appropriate temporary space size based on the remaining space of the UB. |
| minValue | Output | Minimum temporary space size required by the UnPad API to complete the computation, in bytes. To ensure functional correctness, the temporary space reserved/allocated during API computation must not be smaller than this value. A minimum space size of 0 indicates that no temporary space is required for the computation. |

**Table 2** **UnPadTilingFunc API parameters**

| Parameter | Input/Output | Description |
| --- | --- | --- |
| srcShape | Input | Shape information of the input tensor. The parameter type is [AscendC::TensorShape](../data_structures/TensorShape.md), and the shape is two-dimensional. |
| stackBufferSize | Input | Temporary space size available for the UnPad API computation, in bytes. |
| typeSize | Input | Size of the input data type, in bytes. For example, if the input data type is half, pass 2 here. |
| tiling | Output | Outputs the tiling information required by the UnPad API. |

## Return Value

None

## Constraints

None

## Examples

The following example describes the process of obtaining Tiling parameters on the host side when using the UnPad high-level API, and how these parameters are used on the kernel function (Kernel) side. In the example, the size of the original shape is \[320, 64\], the size of the target shape after unpad is \[320, 63\], and the input data type is half.

1.  Add the UnPadTiling structure parameters to the TilingData structure as a field of the TilingData structure.

    ```
    BEGIN_TILING_DATA_DEF(TilingData)             // Register a tiling class, using the tiling name as the input parameter.
        TILING_DATA_FIELD_DEF(uint32_t, totalLength); // Add a tiling field for the total amount of data to compute.
        TILING_DATA_FIELD_DEF(uint32_t, tileNum);     // Add a tiling field for the number of data blocks per core.
        ...                                           // Add other tiling fields.
        TILING_DATA_FIELD_DEF_STRUCT(UnPadTiling, unpadTilingData); // Add the UnPadTiling structure parameter to the TilingData structure.
    END_TILING_DATA_DEF;
    ```

2.  In the Tiling implementation function, first call the **GetUnPadMaxMinTmpSize** API to obtain the maximum and minimum temporary space sizes required by the UnPad API to complete the computation, and set an appropriate space size based on this range and the actual memory usage. Then, based on the input shape, the remaining space available for computation, and other information, obtain the tiling parameters required by the UnPad kernel function (Kernel)-side API.

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
        // Set other Tiling parameters.
        ...
        std::vector<int64_t> shapeVec = {320, 64};
        AscendC::TensorShape srcShape(shapeVec);
        uint32_t maxValue = 0;
        uint32_t minValue = 0;
        auto platformInfo = context->GetPlatformInfo();
        auto ascendcPlatform = platform_ascendc::PlatformAscendC(platformInfo);
        AscendC::GetUnPadMaxMinTmpSize(ascendcPlatform, srcShape, sizeof(half), maxValue, minValue);
        // This sample is for illustration only. Obtain and pass the minimum value to ensure correct functionality. You can pass an appropriate space size as needed.
        const uint32_t localWorkSpaceSize = minValue;
        AscendC::UnPadTilingFunc(srcShape, localWorkSpaceSize, sizeof(half), tiling.unpadTilingData);
        ...
        tiling.SaveToBuffer(context->GetRawTilingData()->GetData(), context->GetRawTilingData()->GetCapacity());
        context->GetRawTilingData()->SetDataSize(tiling.GetDataSize());
        context->SetTilingKey(1);
        return ge::GRAPH_SUCCESS;
    }
    } // namespace optiling
    ```

3.  On the corresponding kernel function (Kernel) side, call GET\_TILING\_DATA in the kernel function (Kernel) to obtain TilingData, and then pass the UnPad Tiling information in TilingData to the UnPad API for computation. For the complete kernel function (Kernel) side example, see [Examples](UnPad.md#examples).

    ```
    extern "C" __global__ __aicore__ void func_custom(GM_ADDR x, GM_ADDR y, GM_ADDR z, GM_ADDR workspace, GM_ADDR tiling)
    {
        GET_TILING_DATA(tilingData, tiling);
        KernelFunc op;
        op.Init(x, y, z, tilingData.totalLength, tilingData.tileNum, tilingData.unpadTilingData);
        if (TILING_KEY_IS(1)) {
            op.Process();
        }
    }
    ```
