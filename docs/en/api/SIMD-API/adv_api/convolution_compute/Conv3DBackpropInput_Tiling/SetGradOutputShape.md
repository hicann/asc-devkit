# SetGradOutputShape

<!-- md-trans-meta sourceCommit=ae444a8b0a91a144a05d68a7cd8d1d0e84475d04 translatedAt=2026-11-10T06:28:54.052Z pushedAt=2026-11-18T11:07:26.709Z nl=2 -->

## Description

Sets the shape of GradOutput: Batch, Channel, Depth, Height, and Width.

## Prototype

```
bool SetGradOutputShape(int64_t n, int64_t c, int64_t d, int64_t h, int64_t w)
```

## Parameters

**Table 1**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| n | Input | Batch value of the input GradOutput. |
| c | Input | Channel value of the input GradOutput. |
| d | Input | Depth value of the input GradOutput. |
| h | Input | Height value of the input GradOutput. |
| w | Input | Width value of the input GradOutput. |

## Return Value

true indicates that the setting is successful, and false indicates that the setting fails.

## Constraints

None

## Examples

```
auto ascendcPlatform = platform_ascendc::PlatformAscendCManager::GetInstance();
ConvBackpropApi::Conv3DBpInputTiling conv3DBpDxTiling(*ascendcPlatform);
conv3DBpDxTiling.SetGradOutputShape(n, c, d, h, w);
```
