# Conv3DBackpropInput Tiling Usage Guidelines

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-11-10T06:28:37.437Z pushedAt=2026-11-18T11:07:26.696Z nl=2 -->

Ascend C provides a set of Conv3DBackpropInput Tiling APIs for obtaining the Tiling parameters required by the Conv3DBackpropInput kernel during computation. You only need to pass the Position, Format, and DType information of Input/GradOutput/Weight, along with other related parameters, and call the API to obtain the relevant parameters in the TConv3DBackpropInputTiling structure described in [Init](../Conv3DBackpropInput_Kernel/Init.md).

The Conv3DBackpropInput Tiling API provides a GetTiling API for obtaining Tiling parameters. The process of obtaining Tiling parameters is as follows:

1.  Create a single-core Tiling object.
2.  Set the parameter type information and Shape information of Input, GradOutput, and Weight. If Padding and Stride parameters exist, set them through the [SetPadding](SetPadding.md) and [SetStride](SetStride.md) APIs.
3.  Call the [GetTiling](GetTiling.md) API to obtain the Tiling information.

The following is an example of using the Conv3DBackpropInput Tiling API to obtain Tiling parameters:

```
#include "tiling/conv_backprop/conv3d_bp_input_tiling.h"

optiling::Conv3DBackpropInputTilingData tilingData;
auto ascendcPlatform = platform_ascendc::PlatformAscendCManager::GetInstance();
ConvBackpropApi::Conv3DBpInputTiling conv3DBpDxTiling(*ascendcPlatform);
conv3DBpDxTiling.SetWeightType(
    Convolution3DBackprop::TPosition::GM, Convolution3DBackprop::ConvFormat::FRACTAL_Z_3D,
    Convolution3DBackprop::ConvDtype::FLOAT32);
conv3DBpDxTiling.SetGradOutputType(
    Convolution3DBackprop::TPosition::GM, Convolution3DBackprop::ConvFormat::NDC1HWC0,
    Convolution3DBackprop::ConvDtype::FLOAT16);
conv3DBpDxTiling.SetInputType(
    Convolution3DBackprop::TPosition::CO1, Convolution3DBackprop::ConvFormat::NDC1HWC0,
    Convolution3DBackprop::ConvDtype::FLOAT16);
conv3DBpDxTiling.SetInputShape(orgN, orgCi, orgDi, orgHi, orgWi);
conv3DBpDxTiling.SetGradOutputShape(orgCo, orgDo, orgHo, orgWo);
conv3DBpDxTiling.SetWeightShape(orgKd, orgKh, orgKw);
conv3DBpDxTiling.SetPadding(padFront, padBack, padUp, padDown, padLeft, padRight);
conv3DBpDxTiling.SetStride(strideD, strideH, strideW);
conv3DBpDxTiling.SetDilation(dilationD, dilationH, dilationW);
int ret = conv3DBpDxTiling.GetTiling(tilingData); // if ret = -1, get tiling failed
```

## Header Files to Include

```
#include "lib/conv_backprop/conv3d_bp_input_tiling.h"
```

