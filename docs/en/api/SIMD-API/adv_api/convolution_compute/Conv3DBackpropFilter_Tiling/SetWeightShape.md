# SetWeightShape

<!-- md-trans-meta sourceCommit=8d4511303e7599d86c4c8f565e5b0a328eb60b1e translatedAt=2026-11-10T06:27:22.426Z pushedAt=2026-11-18T11:07:26.649Z nl=2 -->

## Description

Sets the shape of the weight matrix Weight.

## Prototype

```
void SetWeightShape(int64_t cout, int64_t cin, int64_t d, int64_t h, int64_t w)
```

## Parameters

**Table 1**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| cout | Input | Sets the Channel value of GradOutput. |
| cin | Input | Sets the Channel value of Input. |
| d | Input | Sets the Depth value of Weight. |
| h | Input | Sets the Height value of Weight. |
| w | Input | Sets the Width value of Weight. |

## Return Value

None

## Constraints

None

## Examples

```
optiling::Conv3DBackpropFilterTilingData tilingData;
auto ascendcPlatform = platform_ascendc::PlatformAscendCManager::GetInstance();
ConvBackpropApi::Conv3dBpFilterTiling conv3dBpDwTiling(*ascendcPlatform);
conv3dBpDwTiling.SetWeightShape(cout, cin, d, h, w);
```
