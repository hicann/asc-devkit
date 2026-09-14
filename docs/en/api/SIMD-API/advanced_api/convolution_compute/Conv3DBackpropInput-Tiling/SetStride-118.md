# SetStride

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-05T14:39:14.426Z -->

## Description

Set the stride information.

## Prototype

```
void SetStride(int64_t strideD, int64_t strideH, int64_t strideW)
```

## Parameters

**Table 1**  Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| strideD | Input | Stride size in the Depth direction during convolution forward. |
| strideH | Input | Stride size in the Height direction during convolution forward. |
| strideW | Input | Stride size in the Width direction during convolution forward. |

## Return Value

None

## Constraints

None

## Examples

```
auto ascendcPlatform = platform_ascendc::PlatformAscendCManager::GetInstance();
ConvBackpropApi::Conv3DBpInputTiling conv3DBpDxTiling(*ascendcPlatform);
conv3DBpDxTiling.SetStride(strideD, strideH, strideW);
```

