# SetInputShape

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-05T14:25:52.970Z -->

## Description

Sets the shape of the feature matrix **Input**: **Batch**, **Channel**, **Depth**, **Height**, and **Width**.

## Prototype

```
void SetInputShape(int64_t n, int64_t c, int64_t d, int64_t h, int64_t w)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| n | Input | **Batch** value of the input **Input**. |
| c | Input | **Channel** value of the input **Input**. |
| d | Input | **Depth** value of the input **Input**. |
| h | Input | **Height** value of the input **Input**. |
| w | Input | **Width** value of the input **Input**. |

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

