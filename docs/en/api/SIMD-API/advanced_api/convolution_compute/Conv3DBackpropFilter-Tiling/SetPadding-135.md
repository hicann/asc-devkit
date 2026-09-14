# SetPadding

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-05T14:26:45.514Z -->

## Description

Sets the padding information.

## Prototype

```
void SetPadding(int64_t padFront, int64_t padBack, int64_t padUp, int64_t padDown, int64_t padLeft, int64_t padRight)
```

## Parameters

**Table 1**  Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| padFront | Input | Front padding size of the Input Depth dimension during convolution forward. |
| padBack | Input | Back padding size of the Input Depth dimension during convolution forward. |
| padUp | Input | Up padding size of the Input Height dimension during convolution forward. |
| padDown | Input | Down padding size of the Input Height dimension during convolution forward. |
| padLeft | Input | Left padding size of the Input Width dimension during convolution forward. |
| padRight | Input | Right padding size of the Input Width dimension during convolution forward. |

## Return Value

None

## Constraints

None

## Examples

```
optiling::Conv3DBackpropFilterTilingData tilingData;
auto ascendcPlatform = platform_ascendc::PlatformAscendCManager::GetInstance();
ConvBackpropApi::Conv3dBpFilterTiling conv3dBpDwTiling(*ascendcPlatform);
conv3dBpDwTiling.SetPadding(padFront, padBack, padUp, padDown, padLeft, padRight);
```

