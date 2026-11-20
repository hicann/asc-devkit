# SetDilation

<!-- md-trans-meta sourceCommit=8d4511303e7599d86c4c8f565e5b0a328eb60b1e translatedAt=2026-11-10T06:26:20.367Z pushedAt=2026-11-18T11:07:26.621Z nl=2 -->

## Description

Sets the Dilation information, that is, the dilation size of the convolution kernel in the Depth/Height/Width directions.

## Prototype

```
void SetDilation(int64_t dilationD, int64_t dilationH, int64_t dilationW)
```

## Parameters

**Table 1**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dilationD | Input | Dilation size of the convolution kernel Weight in the Depth direction. |
| dilationH | Input | Dilation size of the convolution kernel Weight in the Height direction. |
| dilationW | Input | Dilation size of the convolution kernel Weight in the Width direction. |

## Return Value

None

## Constraints

None

## Examples

```
optiling::Conv3DBackpropFilterTilingData tilingData;
auto ascendcPlatform = platform_ascendc::PlatformAscendCManager::GetInstance();
ConvBackpropApi::Conv3dBpFilterTiling conv3dBpDwTiling(*ascendcPlatform);
conv3dBpDwTiling.SetDilation(dilationD, dilationH, dilationW);
```
