# SetOutputPadding

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-05T14:38:21.400Z -->

## Description

When constructing a **Conv3DTranspose** operator, this API is used to set the output padding size, which is used to derive the output shape. When constructing a **Conv3DBackpropInput** operator, this API has no practical meaning and must not be used.

## Prototype

```
bool SetOutputPadding(int64_t outputPadD, int64_t outputPadH, int64_t outputPadW)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| outputPadD | Input | Padding value of the output in the Depth dimension. |
| outputPadH | Input | Padding value of the output in the Height dimension. |
| outputPadW | Input | Padding value of the output in the Width dimension. |

## Return Value

Returns **true** on success; otherwise, returns **false**.

## Constraints

None

## Examples

```
auto ascendcPlatform = platform_ascendc::PlatformAscendCManager::GetInstance();
ConvBackpropApi::Conv3DBpInputTiling conv3DBpDxTiling(*ascendcPlatform);
conv3DBpDxTiling.SetOutputPadding(outputPadD, outputPadH, outputPadW);
```

