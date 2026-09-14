# SetPadding

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-05T14:38:34.569Z -->

## Description

Sets the padding information.

## Prototype

```
void SetPadding(int64_t padFront, int64_t padBack, int64_t padUp, int64_t padDown, int64_t padLeft, int64_t padRight)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **padFront** | Input | Front padding size of the Input Depth dimension in the convolution forward process. |
| **padBack** | Input | Back padding size of the Input Depth dimension in the convolution forward process. |
| **padUp** | Input | Up padding size of the Input Height dimension in the convolution forward process. |
| **padDown** | Input | Down padding size of the Input Height dimension in the convolution forward process. |
| **padLeft** | Input | Left padding size of the Input Width dimension in the convolution forward process. |
| **padRight** | Input | Right padding size of the Input Width dimension in the convolution forward process. |

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

