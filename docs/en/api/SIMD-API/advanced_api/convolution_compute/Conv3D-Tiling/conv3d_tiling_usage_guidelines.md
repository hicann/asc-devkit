# Conv3D Tiling Usage Guidelines

<!-- md-trans-meta sourceCommit=e59e1725727133ce76073dba21b022a1b0c9fd56 translatedAt=2026-09-05T14:12:13.429Z -->

Ascend C provides a set of Conv3D tiling APIs for obtaining the tiling parameters required for Kernel computation of the Conv3D forward operator. You only need to pass in the Position, Format, and DType information of Input/Weight/Bias/Output, along with related parameters, and call the API to obtain the relevant parameters in the **TConv3DApiTiling** structure in [Init](../Conv3D-Kernel/Init-101.md).

The Conv3D tiling API provides a single-core tiling API for the Conv3D single-core computation scenario. The process of obtaining tiling parameters is as follows:

1.  Create a single-core tiling object.
2.  Set the parameter type information and Shape information of Input, Weight, Bias, and Output. If Padding, Stride, and Dilation parameters exist, configure them through the [SetPadding](SetPadding.md), [SetStride](SetStride.md), and [SetDilation](SetDilation.md) APIs.
3.  Call the [GetTiling](GetTiling-105.md) API to obtain the tiling information.

The following is an example of using the Conv3D tiling API to obtain tiling parameters:

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
// Set the input/output types.
conv3dApiTiling.SetInputType(TPosition::GM, inputFormat, inputDtype);
conv3dApiTiling.SetWeightType(TPosition::GM, weightFormat, weightDtype);
conv3dApiTiling.SetOutputType(TPosition::CO1, outputFormat, outputDtype);
if (biasFlag) {
   conv3dApiTiling.SetBiasType(TPosition::GM, biasFormat, biasDtype);
}
// Call the GetTiling API to obtain the intra-core tiling strategy. A return value of -1 indicates that tiling data acquisition failed.
if (conv3dApiTiling.GetTiling(tilingData.conv3dApiTilingData) == -1) {
   return false;
}
```

## Header Files to Include

```
#include "lib/conv/conv3d/conv3d_tiling.h"
```

