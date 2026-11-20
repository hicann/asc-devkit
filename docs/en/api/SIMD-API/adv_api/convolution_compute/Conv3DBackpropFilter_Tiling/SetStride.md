# SetStride

<!-- md-trans-meta sourceCommit=8d4511303e7599d86c4c8f565e5b0a328eb60b1e translatedAt=2026-11-10T06:27:09.334Z pushedAt=2026-11-18T11:07:26.643Z nl=2 -->

## Description

Sets the stride information.

## Prototype

```
void SetStride(int64_t strideD, int64_t strideH, int64_t strideW)
```

## Parameters

**Table 1**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| strideD | Input | Stride in the Depth direction during convolution forward. |
| strideH | Input | Stride in the Height direction during convolution forward. |
| strideW | Input | Stride in the Width direction during convolution forward. |

## Return Value

None

## Constraints

None

## Examples

```
optiling::Conv3DBackpropFilterTilingData tilingData;
auto ascendcPlatform = platform_ascendc::PlatformAscendCManager::GetInstance();
ConvBackpropApi::Conv3dBpFilterTiling conv3dBpDwTiling(*ascendcPlatform);
conv3dBpDwTiling.SetStride(strideD, strideH, strideW);
```
