# SetDilation

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-05T14:35:51.596Z -->

## Description

Sets the dilation information, that is, the expansion size of the convolution kernel in the Depth/Height/Width directions.

## Prototype

```
void SetDilation(int64_t dilationD, int64_t dilationH, int64_t dilationW)
```

## Parameters

**Table 1**  Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dilationD | Input | Expansion size of the convolution kernel Weight in the Depth direction. |
| dilationH | Input | Expansion size of the convolution kernel Weight in the Height direction. |
| dilationW | Input | Expansion size of the convolution kernel Weight in the Width direction. |

## Return Value

None

## Constraints

None

## Examples

```
auto ascendcPlatform = platform_ascendc::PlatformAscendCManager::GetInstance();
ConvBackpropApi::Conv3DBpInputTiling conv3DBpDxTiling(*ascendcPlatform);
conv3DBpDxTiling.SetDilation(dilationD, dilationH, dilationW);
```

