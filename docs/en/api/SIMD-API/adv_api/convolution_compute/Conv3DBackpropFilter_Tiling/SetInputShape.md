# SetInputShape

<!-- md-trans-meta sourceCommit=8d4511303e7599d86c4c8f565e5b0a328eb60b1e translatedAt=2026-11-10T06:26:53.247Z pushedAt=2026-11-18T11:07:26.636Z nl=2 -->

## Description

Sets the shape of the feature matrix Input: Batch, Channel, Depth, Height, and Width.

## Prototype

```
void SetInputShape(int64_t n, int64_t c, int64_t d, int64_t h, int64_t w)
```

## Parameters

**Table 1**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| n | Input | Batch value of Input. |
| c | Input | Channel value of Input. |
| d | Input | Depth value of Input. |
| h | Input | Height value of Input. |
| w | Input | Width value of Input. |

## Return Value

None

## Constraints

None

## Examples

```
auto ascendcPlatform = platform_ascendc::PlatformAscendCManager::GetInstance();
ConvBackpropApi::Conv3dBpFilterTiling conv3dBpDwTiling(*ascendcPlatform);
conv3dBpDwTiling.SetInputShape(n, c, d, h, w);
```
