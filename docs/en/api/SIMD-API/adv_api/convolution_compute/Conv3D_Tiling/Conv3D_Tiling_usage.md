# Conv3D Tiling Usage Guide

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-11-10T06:22:38.614Z pushedAt=2026-11-18T11:07:26.529Z nl=2 -->

Ascend C provides a set of Conv3D Tiling APIs for conveniently obtaining the Tiling parameters required for the Conv3D forward operator kernel computation. You only need to pass the Position, Format, and DType of Input/Weight/Bias/Output, along with related parameters, and call the API to obtain the relevant parameters in the TConv3DApiTiling structure described in [Init](../Conv3D_Kernel/Init.md).

The Conv3D Tiling API provides a single-core Tiling API for the Conv3D single-core computation scenario. The process of obtaining Tiling parameters is as follows:

1.  Create a single-core Tiling object.
2.  Set the parameter type information and Shape information of Input, Weight, Bias, and Output. If Padding, Stride, or Dilation parameters exist, configure them through the [SetPadding](SetPadding.md), [SetStride](SetStride.md), and [SetDilation](SetDilation.md) APIs.
3.  Call the [GetTiling](GetTiling.md) API to obtain the Tiling information.

The following is an example of using the Conv3D Tiling API to obtain Tiling parameters:

```
// Instantiate the Conv3D API.
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
Conv3dTilingApi::Conv3dTiling conv3dApiTiling(ascendcPlatform);
// Set the original input/output specifications, single-core specifications, parameters, and so on.
conv3dApiTiling.SetGroups(groups);
conv3dApiTiling.SetOrgWeightShape(cout, kd, kh, kw);
conv3dApiTiling.SetOrgInputShape(cin, di, hi, wi);
conv3dApiTiling.SetPadding(padh, padt, padu, padd, padl, padr);
conv3dApiTiling.SetDilation(dilationH, dilationW, dilationD);
conv3dApiTiling.SetStride(strideH, strideW, strideD);
conv3dApiTiling.SetSingleWeightShape(cin, kd, kh, kw);
conv3dApiTiling.SetSingleOutputShape(singleCoreCo, singleCoreDo, singleCoreMo);
// Set the input/output type.
conv3dApiTiling.SetInputType(TPosition::GM, inputFormat, inputDtype);
conv3dApiTiling.SetWeightType(TPosition::GM, weightFormat, weightDtype);
conv3dApiTiling.SetOutputType(TPosition::CO1, outputFormat, outputDtype);
if (biasFlag) {
    conv3dApiTiling.SetBiasType(TPosition::GM, biasFormat, biasDtype);
}
// Call the GetTiling API to obtain the intra-core splitting strategy. A return value of -1 indicates that tiling acquisition failed.
if (conv3dApiTiling.GetTiling(tilingData.conv3dApiTilingData) == -1) {
    return false;
}
```

## Header Files to Include

```
#include "lib/conv/conv3d/conv3d_tiling.h"
```

