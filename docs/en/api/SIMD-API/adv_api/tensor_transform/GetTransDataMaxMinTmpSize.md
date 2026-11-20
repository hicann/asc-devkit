# GetTransDataMaxMinTmpSize

<!-- md-trans-meta sourceCommit=aa6d602ebc3dbc8f7b50f91dc72aceefdfa67b49 translatedAt=2026-11-02T04:55:43.718Z pushedAt=2026-11-18T04:59:18.289Z nl=2 -->

## Description

The computation of the TransData API on the kernel function (Kernel) side requires you to reserve/allocate temporary space. This API is used to obtain the maximum and minimum temporary space sizes to reserve/allocate on the host side. Based on this range, you select an appropriate space size and pass it to the kernel function (Kernel) side as a Tiling parameter.

-   To ensure correct functionality, the reserved/allocated temporary space size must not be smaller than the minimum temporary space size.
-   Within the range from the minimum to the maximum temporary space size, as the temporary space increases, the computation performance of the kernel function (Kernel) side API is optimized to a certain extent. To achieve better performance, you can reserve/allocate space based on the actual memory usage. For this API, **the maximum temporary space is currently equal to the minimum temporary space**.

## Prototype

```
bool GetTransDataMaxMinTmpSize(const platform_ascendc::PlatformAscendC& platform, const AscendC::TensorShape& srcShape, const AscendC::TensorShape& dstShape,const AscendC::TensorDataType dataType, const TransDataConfig& config, uint32_t& maxValue, uint32_t& minValue)
```

## Parameters

**Table 1**  API parameters

| Parameter | Input/Output | Function |
| --- | --- | --- |
| platform | Input | Passes the hardware platform information. For the definition of PlatformAscendC, see [Constructor and Destructor](../../../Utils-API/platform_info/PlatformAscendC/constructor_and_destructor.md). |
| srcShape | Input | Shape size of the input source operand. The parameter type is [AscendC::TensorShape](../data_structures/TensorShape.md), and the parameter value is consistent with the shape information in the **params.srcLayout** parameter of the **TransData** API. |
| dstShape | Input | Shape size of the output destination operand. The parameter type is [AscendC::TensorShape](../data_structures/TensorShape.md), and the parameter value is consistent with the shape information in the **params.dstLayout** parameter of the **TransData** API. |
| dataType | Input | Input data type. The parameter type is [AscendC::TensorDataType](../data_structures/TensorDataType.md). Currently, only half/float/uint16_t/int16_t data types are supported for input. |
| config | Input | Data format conversion scenario. The parameter value is consistent with the **config** parameter of the **TransData** API. The currently supported conversion scenarios are: NCDHW -> NDC1HWC0, NDC1HWC0 -> NCDHW, NCDHW -> FRACTAL_Z_3D, and FRACTAL_Z_3D -> NCDHW. The type is **TransDataConfig**, defined as shown in the following code. |
| maxValue | Output | Maximum temporary space size required for the TransData API to complete computation, in bytes. Space exceeding this value is not used by this API. <br>maxValue is only a reference value and may be larger than the remaining UB space. In this scenario, you need to select an appropriate temporary space size based on the remaining UB space. |
| minValue | Output | Minimum temporary space size required for the TransData API to complete computation, in bytes. To ensure correct functionality, the temporary space reserved/allocated during API computation must not be smaller than this value. |

```
struct TransDataConfig {
    DataFormat srcFormat;
    DataFormat dstFormat;
};

enum class DataFormat : uint8_t {
    ND = 0,
    NZ,
    NCHW,
    NC1HWC0,
    NHWC,
    NCDHW,
    NDC1HWC0,
    FRACTAL_Z_3D,
};
```

## Return Value

**GetTransDataMaxMinTmpSize** returns true/false. **true** indicates that the maximum and minimum temporary space sizes required for the internal computation of the **TransData** API are successfully obtained; **false** indicates that the acquisition fails.

## Constraints

None

## Examples

For the complete call example, see [More Examples](../math_compute/more_examples.md).

```
// Convert the input shape (1,16,2,4,4) in NCDHW to the output shape (1,2,1,4,4,16) in NDC1HWC0. The input data type of the operator is half.
uint32_t maxSize;
uint32_t minSize;
int32_t n = 1, c = 16, d = 2, h = 4, w = 4, c1 = 1, c0 = 16;
auto ncdhwShape = AscendC::TensorShape({n, c, d, h, w});
auto ndc1hwc0Shape = AscendC::TensorShape({n, d, c1, h, w, c0});
auto plat = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
TransDataConfig config = {DataFormat::NCDHW, DataFormat::NDC1HWC0};
bool ret =
    GetTransDataMaxMinTmpSize(plat, ncdhwShape, ndc1hwc0Shape, AscendC::TensorDataType::DT_FLOAT16, config, maxSize, minSize);
```
