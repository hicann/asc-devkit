# Conv3DBackpropInput Tiling Usage Guidelines

<!-- md-trans-meta sourceCommit=e59e1725727133ce76073dba21b022a1b0c9fd56 translatedAt=2026-09-05T14:35:25.997Z -->

Ascend C provides a set of Conv3DBackpropInput Tiling APIs for users to conveniently obtain the Tiling parameters required for Conv3DBackpropInput Kernel computation. Users only need to pass the **Position**, **Format**, **DType**, and related parameters for **Input**, **GradOutput**, and **Weight**, and call the API to retrieve the relevant parameters in the `TConv3DBackpropInputTiling` structure from [Init](../Conv3DBackpropInput-Kernel/Init-108.md).

The Conv3DBackpropInput Tiling APIs provide a `GetTiling` API for obtaining Tiling parameters. The workflow for obtaining Tiling parameters is as follows:

1. Create a single-core Tiling object.
2. Set the parameter type information and shape information for **Input**, **GradOutput**, and **Weight**. If **Padding** or **Stride** parameters are present, configure them via the [SetPadding](SetPadding-117.md) and [SetStride](SetStride-118.md) APIs.


3. Call the [GetTiling](GetTiling-114.md) API to retrieve the Tiling information.
The following example demonstrates how to use the Conv3DBackpropInput Tiling APIs to obtain Tiling parameters:


```
#include "tiling/conv_backprop/conv3d_bp_input_tiling.h"

optiling::Conv3DBackpropInputTilingData tilingData;
auto ascendcPlatform = platform_ascendc::PlatformAscendCManager::GetInstance();
ConvBackpropApi::Conv3DBpInputTiling conv3DBpDxTiling(*ascendcPlatform);
conv3DBpDxTiling.SetWeightType(Convolution3DBackprop::TPosition::GM,
                                   Convolution3DBackprop::ConvFormat::FRACTAL_Z_3D,
                                   Convolution3DBackprop::ConvDtype::FLOAT32);
conv3DBpDxTiling.SetGradOutputType(Convolution3DBackprop::TPosition::GM,
                                   Convolution3DBackprop::ConvFormat::NDC1HWC0,
                                   Convolution3DBackprop::ConvDtype::FLOAT16);
conv3DBpDxTiling.SetInputType(Convolution3DBackprop::TPosition::CO1,
                                 Convolution3DBackprop::ConvFormat::NDC1HWC0,
                                 Convolution3DBackprop::ConvDtype::FLOAT16);
conv3DBpDxTiling.SetInputShape(orgN, orgCi, orgDi, orgHi, orgWi);
conv3DBpDxTiling.SetGradOutputShape(orgCo, orgDo, orgHo, orgWo);
conv3DBpDxTiling.SetWeightShape(orgKd, orgKh, orgKw);
conv3DBpDxTiling.SetPadding(padFront, padBack, padUp, padDown, padLeft, padRight);
conv3DBpDxTiling.SetStride(strideD, strideH, strideW);
conv3DBpDxTiling.SetDilation(dilationD, dilationH, dilationW);
int ret = conv3DBpDxTiling.GetTiling(tilingData);    // if ret = -1, get tiling failed
```

## Required Header Files

```
#include "lib/conv_backprop/conv3d_bp_input_tiling.h"
```



