# GetTiling

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-05T14:35:40.107Z -->

## Description

Obtains the Tiling parameters.

## Prototype

```
int64_t GetTiling(optiling::Conv3DBackpropInputTilingData &tiling)
```

```
int64_t GetTiling(AscendC::tiling::Conv3DBackpropInputTilingData &tiling)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| tiling | Output | Tiling structure of **Conv3DBackpropInput**, used to store the final Tiling result. For details about the **TConv3DBackpropInputTiling** structure, see [TConv3DApiTiling Structure](tconv3dbackpropinputtiling_structure.md). |

## Return Value

If the return value is not -1, the Tiling calculation succeeds and the user can use the values in this Tiling structure. If the return value is -1, the Tiling calculation fails and the Tiling result cannot be used.

## Constraints

None

## Examples

```
// Build the call example for Conv3dBackpropInput operator tiling.
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
ConvBackpropApi::Conv3DBpInputTiling tiling(ascendcPlatform);
tiling.SetWeightShape(256, 128, 3, 3, 3);
tiling.SetInputShape(1, 128, 11, 128, 128);
tiling.SetGradOutputShape(1, 256, 9, 128, 128);
tiling.SetPadding(0, 0, 1, 1, 1, 1);
tiling.SetDilation(1, 1, 1);
tiling.SetStride(1, 1, 1);
optiling::Conv3DBackpropInputTilingData tilingData;
int ret = tiling.GetTiling(tilingData);  // Obtain the Tiling parameters.
AscendC::tiling::Conv3DBackpropInputTilingData tilingDataNotOp;
ret = tiling.GetTiling(tilingDataNotOp); // Obtain the Tiling parameters using AscendC::tiling::Conv3DBackpropInputTilingData.
```

```
// Build the call example for Conv3dTranspose operator tiling.
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
ConvBackpropApi::Conv3DBpInputTiling tiling(ascendcPlatform);
tiling.SetWeightShape(256, 128, 3, 3, 3);
tiling.SetGradOutputShape(256, 9, 128, 128); // Equivalent to the input X of Conv3dTranspose.
tiling.SetPadding(0, 0, 1, 1, 1, 1);
tiling.SetDilation(1, 1, 1);
tiling.SetStride(1, 1, 1);
tiling.SetOutputPadding(0, 0, 0); // Pad the output Y of Conv3dTranspose.
optiling::Conv3DBackpropInputTilingData tilingData;
int ret = tiling.GetTiling(tilingData);  // Obtain the Tiling parameters.
```

