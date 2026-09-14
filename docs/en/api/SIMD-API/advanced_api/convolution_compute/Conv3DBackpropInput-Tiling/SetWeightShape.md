# SetWeightShape

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-05T14:39:22.626Z -->

## Description

Sets the shape of the weight matrix **Weight**.

## Prototype

```
bool SetWeightShape(int64_t cout, int64_t cin, int64_t d, int64_t h, int64_t w)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| cout | Input | Sets the output channel size of the forward convolution, which is consistent with the channel size of **GradOutput**. |
| cin | Input | Sets the input channel size of the forward convolution, which is consistent with the channel size of **GradInput**. |
| d | Input | Sets the depth value of **weight**. |
| h | Input | Sets the height value of **weight**. |
| w | Input | Sets the width value of **weight**. |

## Return Value

Returns **true** on success and **false** on failure.

## Constraints

None

## Examples

```
auto ascendcPlatform = platform_ascendc::PlatformAscendCManager::GetInstance();
ConvBackpropApi::Conv3DBpInputTiling con3dBpDxTiling(*ascendcPlatform);
con3dBpDxTiling.SetWeightShape(cout, cin, d, h, w);
```

