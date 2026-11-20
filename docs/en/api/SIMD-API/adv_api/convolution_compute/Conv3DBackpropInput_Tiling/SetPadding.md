# SetPadding

<!-- md-trans-meta sourceCommit=8d4511303e7599d86c4c8f565e5b0a328eb60b1e translatedAt=2026-11-10T06:29:23.399Z pushedAt=2026-11-18T11:07:26.724Z nl=2 -->

## Description

Sets the padding information.

## Prototype

```
void SetPadding(int64_t padFront, int64_t padBack, int64_t padUp, int64_t padDown, int64_t padLeft, int64_t padRight)
```

## Parameters

**Table 1**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| padFront | Input | Padding size in the forward direction of the Input Depth dimension during the convolution forward process. |
| padBack | Input | Padding size in the backward direction of the Input Depth dimension during the convolution forward process. |
| padUp | Input | Padding size in the upward direction of the Input Height dimension during the convolution forward process. |
| padDown | Input | Padding size in the downward direction of the Input Height dimension during the convolution forward process. |
| padLeft | Input | Padding size in the left direction of the Input Width dimension during the convolution forward process. |
| padRight | Input | Padding size in the right direction of the Input Width dimension during the convolution forward process. |

## Return Value

None

## Constraints

None

## Examples

```
auto ascendcPlatform = platform_ascendc::PlatformAscendCManager::GetInstance();
ConvBackpropApi::Conv3DBpInputTiling conv3DBpDxTiling(*ascendcPlatform);
conv3DBpDxTiling.SetPadding(padFront, padBack, padUp, padDown, padLeft, padRight);
```
