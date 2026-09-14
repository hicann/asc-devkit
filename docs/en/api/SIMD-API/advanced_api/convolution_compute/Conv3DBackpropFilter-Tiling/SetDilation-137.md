# SetDilation

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-05T14:24:38.858Z -->

## Description

Sets the dilation information, that is, the dilation size of the convolution kernel along the Depth, Height, and Width dimensions.

## Prototype

```
void SetDilation(int64_t dilationD, int64_t dilationH, int64_t dilationW)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dilationD | Input | 	Dilation size of the convolution kernel along the Depth dimension. |
| dilationH | Input | Dilation size of the convolution kernel along the Height dimension. |
| dilationW | Input | Dilation size of the convolution kernel along the Width dimension. |

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

