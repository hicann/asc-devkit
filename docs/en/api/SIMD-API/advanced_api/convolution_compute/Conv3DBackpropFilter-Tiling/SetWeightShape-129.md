# SetWeightShape

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-05T14:27:29.782Z -->

## Description

Sets the shape of the weight matrix **Weight**.

## Prototype

```
void SetWeightShape(int64_t cout, int64_t cin, int64_t d, int64_t h, int64_t w)
```

## Parameters

**Table 1**  Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| cout | Input | Sets the Channel value of **GradOutput**. |
| cin | Input | Sets the Channel value of **Input**. |
| d | Input | Sets the Depth value of **Weight**. |
| h | Input | Sets the Height value of **Weight**. |
| w | Input | Sets the Width value of **Weight**. |

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

