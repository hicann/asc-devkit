# Conv3D Template Parameters

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-05T14:03:54.924Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Not supported

- Atlas A3 training products/Atlas A3 inference products: Supported

- Atlas A2 training products/Atlas A2 inference products: Supported

- Atlas 200I/500 A2 inference products: Not supported

- AI Core of Atlas inference products: Not supported

- Vector Core of Atlas inference products: Not supported

- Atlas training products: Not supported

## Description

When creating a Conv3D API object, the following parameters must be passed:

-   Type information for Input, Weight, Output, and Bias (optional). The type information is defined via [ConvType](conv3d_usage_guidelines.md#table19081115275), which includes: memory logical location, data format, and data type.

-   **Conv3dParam** information (optional), used to enable performance optimization templates for different scenarios. **Currently not supported.**

## Prototype

```
template <class INPUT_TYPE, class WEIGHT_TYPE, class OUTPUT_TYPE, class BIAS_TYPE = biasType, class CONV_CFG = Conv3dParam>
using Conv3D = Conv3dIntfExt<Config<ConvApi::ConvDataType<INPUT_TYPE, WEIGHT_TYPE, OUTPUT_TYPE, BIAS_TYPE, CONV_CFG>>, Impl, Intf>
```

## Parameters

**Table 1**  Template parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| INPUT_TYPE | Input | Template parameter of type `ConvType`, specifying the type information for `Input`. |
| WEIGHT_TYPE | Input | Template parameter of type `ConvType`, specifying the type information for `Weight`. |
| OUTPUT_TYPE | Input | Template parameter of type `ConvType`, specifying the type information for `Output`. |
| BIAS_TYPE | Optional input | Template parameter of type `ConvType`, specifying the type information for `Bias`. |
| CONV_CFG | Optional input | Template parameter of type `ConvParam`, used to enable performance optimization templates for different scenarios. The current version only supports the basic template, with performance optimization disabled. |

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
using biasType = ConvApi::ConvType<AscendC::TPosition::GM, ConvFormat::ND, float>; // Optional parameter; can be omitted if Bias is not used.
struct ConvCustom : public ConvApi::ConvParam {
    __aicore__ inline ConvCustom(){};
}; // Optional parameter; the current version only supports the basic template with performance optimization disabled. Can be omitted.

Conv3dApi::Conv3D<inputType, weightType, outputType, biasType, ConvCustom> conv3dApi;
```

