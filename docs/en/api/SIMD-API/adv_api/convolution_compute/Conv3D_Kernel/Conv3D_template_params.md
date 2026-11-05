# Conv3D Template Parameters

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T09:08:48.977Z pushedAt=2026-10-29T02:14:34.144Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Not supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
- Vector Core of Atlas inference products: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/convolution_compute/Conv3D_Kernel/Conv3D_template_params_res.md#id1 -->

## Description

When creating a Conv3D object, you need to pass:

-   Parameter type information of Input, Weight, Output, and Bias (optional). The type information is defined by [ConvType](Conv3D_usage.md#table19081115275), including the memory logical location, data format, and data type.
-   Conv3dParam information (optional), used to enable performance optimization templates for different scenarios. **Currently not supported.**

## Prototype

```
template <class INPUT_TYPE, class WEIGHT_TYPE, class OUTPUT_TYPE, class BIAS_TYPE = biasType, class CONV_CFG = Conv3dParam>
using Conv3D = Conv3dIntfExt<Config<ConvApi::ConvDataType<INPUT_TYPE, WEIGHT_TYPE, OUTPUT_TYPE, BIAS_TYPE, CONV_CFG>>, Impl, Intf>
```

## Parameters

**Table 1**  Template parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| INPUT_TYPE | Input | ConvType type template parameter that specifies the parameter type information of Input. |
| WEIGHT_TYPE | Input | ConvType type template parameter that specifies the parameter type information of Weight. |
| OUTPUT_TYPE | Input | ConvType type template parameter that specifies the parameter type information of Output. |
| BIAS_TYPE | Optional input | ConvType type template parameter that specifies the parameter type information of Bias. |
| CONV_CFG | Optional input | ConvParam type template parameter used to enable performance optimization templates for different scenarios. In the current version, only the basic template is supported, and performance optimization is not enabled. |

## Return Value

None

## Constraints

None

## Examples

```
#include "lib/conv/conv3d/conv3d_api.h"

using inputType = ConvApi::ConvType<AscendC::TPosition::GM, ConvFormat::NDC1HWC0, bfloat16_t>;
using weightType = ConvApi::ConvType<AscendC::TPosition::GM, ConvFormat::FRACTAL_Z_3D, bfloat16_t>;
using outputType = ConvApi::ConvType<AscendC::TPosition::GM, ConvFormat::NDC1HWC0, bfloat16_t>;
// Optional parameter. It can be omitted when Bias is not used.
using biasType = ConvApi::ConvType<AscendC::TPosition::GM, ConvFormat::ND, float>;
struct ConvCustom : public ConvApi::ConvParam {
    __aicore__ inline ConvCustom(){};
}; // Optional parameter. In the current version, only the basic template is supported, and performance optimization is not enabled. It can be omitted.

Conv3dApi::Conv3D<inputType, weightType, outputType, biasType, ConvCustom> conv3dApi;
```
