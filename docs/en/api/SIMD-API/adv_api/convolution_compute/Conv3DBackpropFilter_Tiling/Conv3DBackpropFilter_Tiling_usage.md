# Conv3DBackpropFilter Tiling Usage Guide

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-11-10T06:25:33.225Z pushedAt=2026-11-18T11:07:26.608Z nl=2 -->

Ascend C provides a set of Conv3DBackpropFilter Tiling APIs for obtaining the Tiling parameters required for Conv3DBackpropFilter kernel computation. You only need to pass the Position, Format, DType, and related parameters of Input/GradOutput/GradWeight, and then call the API to obtain the relevant parameters in the TConv3DBpFilterTiling structure described in [Init](../Conv3DBackpropFilter_Kernel/Init.md).

The Conv3DBackpropFilter Tiling API provides a GetTiling API for obtaining Tiling parameters. The process of obtaining Tiling parameters is as follows:

1.  Create a single-core Tiling object.
2.  Set the parameter type information and Shape information of Input, GradOutput, and GradWeight. If Padding and Stride parameters exist, set them through the [SetPadding](SetPadding.md) and [SetStride](SetStride.md) APIs.
3.  Call the [GetTiling](GetTiling.md) API to obtain the Tiling information.

The following is an example of using the Conv3DBackpropFilter Tiling API to obtain Tiling parameters:

```
#include "tiling/conv_backprop/conv3d_bp_filter_tiling.h"

optiling::Conv3DBackpropFilterTilingData tilingData;
auto ascendcPlatform = platform_ascendc::PlatformAscendCManager::GetInstance();
ConvBackpropApi::Conv3dBpFilterTiling conv3dBpDwTiling(*ascendcPlatform);

conv3dBpDwTiling.SetWeightType(
    ConvCommonApi::TPosition::CO1, ConvCommonApi::ConvFormat::FRACTAL_Z_3D, ConvCommonApi::ConvDtype::FLOAT32);
conv3dBpDwTiling.SetInputType(
    ConvCommonApi::TPosition::GM, ConvCommonApi::ConvFormat::NDC1HWC0, ConvCommonApi::ConvDtype::FLOAT16);
conv3dBpDwTiling.SetGradOutputType(
    ConvCommonApi::TPosition::GM, ConvCommonApi::ConvFormat::NDC1HWC0, ConvCommonApi::ConvDtype::FLOAT16);
conv3dBpDwTiling.SetGradOutputShape(n, c, d, h, w);
conv3dBpDwTiling.SetInputShape(n, c, d, h, w);
conv3dBpDwTiling.SetWeightShape(cout, cin, d, h, w);
conv3dBpDwTiling.SetPadding(padFront, padBack, padUp, padDown, padLeft, padRight);
conv3dBpDwTiling.SetStride(strideD, strideH, strideW);
conv3dBpDwTiling.SetDilation(dilationD, dilationH, dilationW);
int ret = conv3dBpDwTiling.GetTiling(tilingData); // If ret = -1, the tiling result fails to be obtained.
```

## Header Files to Include

```
#include "lib/conv_backprop/conv3d_bp_filter_tiling.h"
```
