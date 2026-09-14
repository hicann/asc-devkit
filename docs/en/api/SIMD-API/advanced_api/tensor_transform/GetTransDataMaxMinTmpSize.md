# GetTransDataMaxMinTmpSize

<!-- md-trans-meta sourceCommit=29d3208111c9988196c0c653d3794b0477c81f15 translatedAt=2026-09-08T15:59:53.036Z -->

## Description

The TransData API on the kernel side requires you to reserve/allocate temporary space for computation. This API is used to obtain the maximum and minimum temporary space sizes to reserve/allocate on the host side. Based on this range, you can select an appropriate space size and pass it to the kernel side as a Tiling parameter.

-   To ensure correctness, the reserved/allocated temporary space size must not be smaller than the minimum temporary space size.
-   Within the range from the minimum to the maximum temporary space, as the temporary space increases, the computation performance of the kernel-side API improves to a certain extent. To achieve better performance, you can reserve/allocate space based on the actual memory usage. For this API, **the maximum temporary space is currently equal to the minimum temporary space**.

## Prototype

```
bool GetTransDataMaxMinTmpSize(const platform_ascendc::PlatformAscendC& platform, const ge::Shape& srcShape, const ge::Shape& dstShape,const ge::DataType dataType, const TransDataConfig &config, uint32_t& maxValue, uint32_t& minValue)
```

## Parameters

**Table 1**  Parameter list

| Parameter | Input/Output | Description |
| --- | --- | --- |
| platform | Input | Passes the hardware platform information. For the definition of **PlatformAscendC**, see [Constructors and Destructors](../../../Utils-API/platform_info/PlatformAscendC/constructors_and_destructors.md). |
| srcShape | Input | Shape size of the input source operand. The parameter value must be consistent with the shape information in the **params.srcLayout** parameter of the TransData API. |
| dstShape | Input | Shape size of the output destination operand. The parameter value must be consistent with the shape information in the **params.dstLayout** parameter of the TransData API. |
| dataType | Input | Input data type, of the **ge::DataType** type. Currently, only **half**/**float**/**uint16_t**/**int16_t** data types are supported as input. |
| config | Input | Data format conversion scenario. The parameter value must be consistent with the **config** parameter of the TransData API. Currently supported conversion scenarios are: NCDHW -> NDC1HWC0, NDC1HWC0 -> NCDHW, NCDHW -> FRACTAL_Z_3D, and FRACTAL_Z_3D -> NCDHW. The type is **TransDataConfig**, whose definition is shown in the following code. |
| maxValue | Output | Maximum temporary space size required for the TransData API to complete computation. Space exceeding this value is not used by this API. <br>**maxValue** is only a reference value and may be larger than the remaining space of the Unified Buffer. In this case, you need to select an appropriate temporary space size based on the remaining space of the Unified Buffer. |
| minValue | Output | Minimum temporary space size required for the TransData API to complete computation. To ensure correctness, the temporary space reserved/allocated during API computation must not be smaller than this value. |

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

GetTransDataMaxMinTmpSize returns **true**/**false**. **true** indicates that the maximum and minimum temporary space sizes required for the internal computation of the TransData API are obtained successfully; **false** indicates that the acquisition fails.

## Constraints

None

## Examples

For the complete call example, see [More Samples](../math_compute/more_samples_83.md).

```
// Convert the input shape (1,16,2,4,4) in NCDHW to the output shape (1,2,1,4,4,16) in NDC1HWC0. The input data type of the operator is half.
uint32_t maxSize;
uint32_t minSize;
int32_t n = 1, c = 16, d = 2, h = 4, w = 4, c1 = 1, c0 = 16;
auto ncdhwShape = ge::Shape({ n, c, d, h, w });
auto ndc1hwc0Shape = ge::Shape({ n, d, c1, h, w, c0});
auto plat = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
TransDataConfig config = {DataFormat::NCDHW, DataFormat::NDC1HWC0};
bool ret = GetTransDataMaxMinTmpSize(plat, ncdhwShape, ndc1hwc0Shape, ge::DataType::DT_FLOAT16, config, maxSize, minSize);
```
