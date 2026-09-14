# Conv3DBackpropFilter Tiling Usage Guidelines

<!-- md-trans-meta sourceCommit=2737c34bbaf046b0c4e915117e38247d0f621f63 translatedAt=2026-09-05T14:24:16.952Z -->

Ascend C provides a set of Conv3DBackpropFilter tiling APIs for users to conveniently obtain the tiling parameters required for Conv3DBackpropFilter Kernel computation. Users only need to pass in the Position, Format, DType, and related parameters for Input, GradOutput, and GradWeight, and call the API to retrieve the relevant parameters in the `TConv3DBpFilterTiling` structure from [Init](../Conv3DBackpropFilter-Kernel/Init-120.md).
















The Conv3DBackpropFilter tiling APIs provide a `GetTiling` API for obtaining tiling parameters. The workflow for obtaining tiling parameters is as follows:















1. Create a single-core tiling object.

2. Set the parameter type information and shape information for Input, GradOutput, and GradWeight. If Padding or Stride parameters are present, configure them via the [SetPadding](SetPadding-135.md) and [SetStride](SetStride-136.md) APIs.











3. Call the [GetTiling](GetTiling-128.md) API to retrieve the Tiling information.t
The following example demonstrates how to use the Conv3DBackpropFilter tiling APIs to obtain tiling parameters:


```
#include "tiling/conv_backprop/conv3d_bp_filter_tiling.h"

optiling::Conv3DBackpropFilterTilingData tilingData;
auto ascendcPlatform = platform_ascendc::PlatformAscendCManager::GetInstance();
ConvBackpropApi::Conv3dBpFilterTiling conv3dBpDwTiling(*ascendcPlatform);

conv3dBpDwTiling.SetWeightType(ConvCommonApi::TPosition::CO1,
                                   ConvCommonApi::ConvFormat::FRACTAL_Z_3D,
                                   ConvCommonApi::ConvDtype::FLOAT32);
conv3dBpDwTiling.SetInputType(ConvCommonApi::TPosition::GM,
                                 ConvCommonApi::ConvFormat::NDC1HWC0,
                                 ConvCommonApi::ConvDtype::FLOAT16);
conv3dBpDwTiling.SetGradOutputType(ConvCommonApi::TPosition::GM,
                                   ConvCommonApi::ConvFormat::NDC1HWC0,
                                   ConvCommonApi::ConvDtype::FLOAT16);
conv3dBpDwTiling.SetGradOutputShape(n, c, d, h, w);
conv3dBpDwTiling.SetInputShape(n, c, d, h, w);
conv3dBpDwTiling.SetWeightShape(cout, cin, d, h, w);
conv3dBpDwTiling.SetPadding(padFront, padBack, padUp, padDown, padLeft, padRight);
conv3dBpDwTiling.SetStride(strideD, strideH, strideW);
conv3dBpDwTiling.SetDilation(dilationD, dilationH, dilationW);
int ret = conv3dBpDwTiling.GetTiling(tilingData);    // If ret = -1, the tiling result retrieval fails.
```

## Header Files to Include

```
#include "lib/conv_backprop/conv3d_bp_filter_tiling.h"
```



