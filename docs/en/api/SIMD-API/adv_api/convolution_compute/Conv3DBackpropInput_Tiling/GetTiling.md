# GetTiling

<!-- md-trans-meta sourceCommit=8d4511303e7599d86c4c8f565e5b0a328eb60b1e translatedAt=2026-11-10T06:28:49.796Z pushedAt=2026-11-18T11:07:26.702Z nl=2 -->

## Description

Obtains the Tiling parameters.

## Prototype

```
int64_t GetTiling(optiling::Conv3DBackpropInputTilingData& tiling)
```

```
int64_t GetTiling(AscendC::tiling::Conv3DBackpropInputTilingData& tiling)
```

## Parameters

**Table 1**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| tiling | Output | Tiling structure of Conv3DBackpropInput, used to store the final Tiling result. For details about the TConv3DBackpropInputTiling structure, see [TConv3DApiTiling Structure](TConv3DBackpropInputTiling_struct.md). |

## Return Value

If the return value is not equal to -1, the Tiling computation succeeds and you can use the values in this Tiling structure. If the return value is -1, the Tiling computation fails and the Tiling result cannot be used.

## Constraints

None

## Examples

```
// Build a tiling call example for the Conv3dBackpropInput operator.
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
ConvBackpropApi::Conv3DBpInputTiling tiling(ascendcPlatform);
tiling.SetWeightShape(256, 128, 3, 3, 3);
tiling.SetInputShape(1, 128, 11, 128, 128);
tiling.SetGradOutputShape(1, 256, 9, 128, 128);
tiling.SetPadding(0, 0, 1, 1, 1, 1);
tiling.SetDilation(1, 1, 1);
tiling.SetStride(1, 1, 1);
optiling::Conv3DBackpropInputTilingData tilingData;
int ret = tiling.GetTiling(tilingData); // Obtain the tiling parameters.
AscendC::tiling::Conv3DBackpropInputTilingData tilingDataNotOp;
ret = tiling.GetTiling(tilingDataNotOp); // Obtain the tiling parameters using AscendC::tiling::Conv3DBackpropInputTilingData.
```

```
// Build a tiling call example for the Conv3dTranspose operator.
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
ConvBackpropApi::Conv3DBpInputTiling tiling(ascendcPlatform);
tiling.SetWeightShape(256, 128, 3, 3, 3);
tiling.SetGradOutputShape(256, 9, 128, 128); // Equivalent to the input X of Conv3dTranspose.
tiling.SetPadding(0, 0, 1, 1, 1, 1);
tiling.SetDilation(1, 1, 1);
tiling.SetStride(1, 1, 1);
tiling.SetOutputPadding(0, 0, 0); // Pad the output Y of Conv3dTranspose.
optiling::Conv3DBackpropInputTilingData tilingData;
int ret = tiling.GetTiling(tilingData); // Obtain the tiling parameters.
```
