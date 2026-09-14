# SetGradOutputShape

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-05T14:24:46.062Z -->

## Description

Sets the shape of GradOutput: **Batch**, **Channel**, **Depth**, **Height**, and **Width**.

## Prototype

```
void SetGradOutputShape(int64_t n, int64_t c, int64_t d, int64_t h, int64_t w)
```

## Parameters

**Table 1**  Parameter description

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

