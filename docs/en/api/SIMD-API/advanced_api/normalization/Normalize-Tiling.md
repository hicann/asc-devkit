# Normalize Tiling

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-10T02:45:20.599Z -->

## Description

Ascend C provides the Normalize Tiling API to help you obtain the Tiling parameters required for Normalize kernel computation.

Specifically, **GetNormalizeMaxMinTmpSize** is used to obtain the maximum and minimum temporary space sizes required for the Normalize API to complete the computation.

The Normalize API on the kernel side requires you to reserve/apply temporary space. **GetNormalizeMaxMinTmpSize** is used on the host side to obtain the maximum and minimum temporary space sizes to reserve/apply. Based on this range, you can select an appropriate space size and pass it to the kernel side as a Tiling parameter.

-   To ensure correct functionality, the reserved/applied temporary space size must not be smaller than the minimum temporary space size.
-   Within the range from the minimum temporary space size to the maximum temporary space size, as the temporary space increases, the computation performance of the kernel-side API improves to a certain extent. To achieve better performance, you can reserve/apply space based on the actual memory usage.

## Prototype

```
void GetNormalizeMaxMinTmpSize(const ge::Shape& srcShape, const uint32_t typeSizeU, const uint32_t typeSizeT, const bool isReuseSource, const bool isComputeRstd, const bool isOnlyOutput, uint32_t& maxValue, uint32_t& minValue)
```

## Parameters

**Table 1** Parameters of the GetNormalizeMaxMinTmpSize API

| Parameter | Input/Output | Description |
| --- | --- | --- |
| srcShape | Input | Shape information {A, R} of the Normalize input data inputX. |
| typeSizeU | Input | Data type size of the input data gamma and beta, in bytes. For example, if the input data type is float, pass 4. |
| typeSizeT | Input | Data type size of the input data inputX, in bytes. For example, if the input data type is float, pass 4. |
| isReuseSource | Input | Whether to reuse the memory space of the source operand, consistent with the [Normalize](Normalize.md) API. |
| isComputeRstd | Input | Whether to compute rstd. Only **true** is supported for this parameter. |
| isOnlyOutput | Input | Whether to output only y without outputting the reciprocal of the standard deviation rstd. Currently, only **false** is supported for this parameter, indicating that both y and rstd are output. |
| maxValue | Output | Outputs the tiling information (Maximum Temporary Space Size) required by the Normalize API.<br><br>Maximum temporary space size required for the Normalize API to complete the computation. Space beyond this value is not used by the API. Within the range from the minimum temporary space size to the maximum temporary space size, as the temporary space increases, the computation performance of the kernel-side API is optimized to a certain extent. To achieve better performance, you can reserve/apply space based on the actual memory usage.<br>maxValue is only a reference value and may be larger than the remaining space of the Unified Buffer. In this case, you need to select an appropriate temporary space size based on the remaining space of the Unified Buffer. |
| minValue | Output | Outputs the tiling information (minimum temporary space size) required by the Normalize API.<br><br>Minimum temporary space size required for the Normalize API to complete the computation. To ensure correct functionality, the temporary space reserved/applied during API computation must not be smaller than this value. |

## Return Value

None

## Constraints

None

## Examples

1.  Add the parameters required by the Normalize API to the **TilingData** struct as a field of the **TilingData** struct.

    ```
    BEGIN_TILING_DATA_DEF(NormalizeCustomTilingData)
      TILING_DATA_FIELD_DEF(float, epsilon);
      TILING_DATA_FIELD_DEF(uint32_t, isNoBeta);
      TILING_DATA_FIELD_DEF(uint32_t, isNoGamma);
      TILING_DATA_FIELD_DEF(uint32_t, isOnlyOutput);
      TILING_DATA_FIELD_DEF(uint32_t, aLength);
      TILING_DATA_FIELD_DEF(uint32_t, rLength);
      TILING_DATA_FIELD_DEF(uint32_t, rLengthWithPadding);
      ...                                           // Add other tiling fields.
    END_TILING_DATA_DEF;
    ```

2.  In the Tiling implementation function, first call the **GetNormalizeMaxMinTmpSize** API to obtain the maximum/minimum temporary space size required for the Normalize API to complete the computation. Set an appropriate space size within this range based on the actual memory usage, and then obtain the tiling parameters required by the Normalize kernel-side API based on the input shape, the remaining space available for computation, and other information.

    ```
    namespace optiling {
    static ge::graphStatus TilingFunc(gert::TilingContext *context)
    {
        NormalizeCustomTilingData tiling;
        const gert::RuntimeAttrs *attrs = context->GetAttrs();
        const float epsilon = *(attrs->GetAttrPointer<float>(0));
        const uint32_t isNoBeta = *(attrs->GetAttrPointer<uint32_t>(1));
        const uint32_t isNoGamma = *(attrs->GetAttrPointer<uint32_t>(2));
        const uint32_t isOnlyOutput = *(attrs->GetAttrPointer<uint32_t>(3));
        const gert::StorageShape* x1_shape = context->GetInputShape(0);
        ...// Other logic.
        const gert::Shape shape = x1_shape->GetStorageShape();
        uint32_t aLength = shape.GetDim(0);
        uint32_t rLength = shape.GetDim(1);
        uint32_t rLengthWithPadding = (rLength + alignNum - 1) / alignNum * alignNum;
        std::vector<int64_t> srcDims = {aLength, rLength};
        ge::Shape srcShape(srcDims);

        uint32_t maxTmpsize = 0;
        uint32_t minTmpsize = 0;

        AscendC::GetNormalizeMaxMinTmpSize(srcShape, typeSizeU, typeSizeT, false, true, isOnlyOutput, maxTmpsize, minTmpsize);
        // auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
        // AscendC::GetNormalizeMaxMinTmpSize(srcShape, typeSizeU, typeSizeT, false, true, isOnlyOutput, ascendcPlatform, maxTmpsize, minTmpsize);

        ... // Other logic.
        context->SetTilingKey(1);
        tiling.SaveToBuffer(context->GetRawTilingData()->GetData(), context->GetRawTilingData()->GetCapacity());
        context->GetRawTilingData()->SetDataSize(tiling.GetDataSize());
        size_t *currentWorkspace = context->GetWorkspaceSizes(1);
        currentWorkspace[0] = 0;
        return ge::GRAPH_SUCCESS;
    }
    } // namespace optiling
    ```

3.  On the corresponding kernel side, call GET\_TILING\_DATA in the kernel function to obtain the TilingData, and then pass the Normalize Tiling information in the TilingData to the Normalize API for computation. For the complete kernel-side sample, see [Normalize](Normalize.md).

    ```
    extern "C" __global__ __aicore__ void normalize_custom(GM_ADDR x, GM_ADDR mean, GM_ADDR variance, GM_ADDR gamma, GM_ADDR beta, GM_ADDR rstd, GM_ADDR y, GM_ADDR workspace, GM_ADDR tiling) {
        GET_TILING_DATA(tilingData, tiling);
        float epsilon = tilingData.epsilon;
        NormalizePara para(tilingData.aLength, tilingData.rLength, tilingData.rLengthWithPadding);
        if (TILING_KEY_IS(1)) {
          if (!tilingData.isNoBeta && !tilingData.isNoGamma) {
              KernelNormalize<NLCFG_NORM> op;
              op.Init(x, mean, variance, gamma, beta, rstd, y, epsilon, para);
              op.Process();
          } else if (!tilingData.isNoBeta && tilingData.isNoGamma) {
              KernelNormalize<NLCFG_NOGAMMA> op;
              op.Init(x, mean, variance, gamma, beta, rstd, y, epsilon, para);
              op.Process();
          } else if (tilingData.isNoBeta && !tilingData.isNoGamma) {
              KernelNormalize<NLCFG_NOBETA> op;
              op.Init(x, mean, variance, gamma, beta, rstd, y, epsilon, para);
              op.Process();
          } else if (tilingData.isNoBeta && tilingData.isNoGamma) {
              KernelNormalize<NLCFG_NOOPT> op;
              op.Init(x, mean, variance, gamma, beta, rstd, y, epsilon, para);
              op.Process();
          }
        }
      }
    ```
