# SetGradOutputShape

<!-- md-trans-meta sourceCommit=8d4511303e7599d86c4c8f565e5b0a328eb60b1e translatedAt=2026-11-10T06:26:30.155Z pushedAt=2026-11-18T11:07:26.624Z nl=2 -->

## Description

Sets the shape of GradOutput: Batch, Channel, Depth, Height, and Width.

## Prototype

```
void SetGradOutputShape(int64_t n, int64_t c, int64_t d, int64_t h, int64_t w)
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

None

## Constraints

None

## Examples

```
auto ascendcPlatform = platform_ascendc::PlatformAscendCManager::GetInstance();
ConvBackpropApi::Conv3dBpFilterTiling conv3dBpDwTiling(*ascendcPlatform);
conv3dBpDwTiling.SetGradOutputShape(n, c, d, h, w);
```
