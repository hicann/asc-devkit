# SetStride

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-05T14:27:25.703Z -->

## Description

Sets the stride information.

## Prototype

```
void SetStride(int64_t strideD, int64_t strideH, int64_t strideW)
```

## Parameters

**Table 1**  Parameters

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
optiling::Conv3DBackpropFilterTilingData tilingData;
auto ascendcPlatform = platform_ascendc::PlatformAscendCManager::GetInstance();
ConvBackpropApi::Conv3dBpFilterTiling conv3dBpDwTiling(*ascendcPlatform);
conv3dBpDwTiling.SetStride(strideD, strideH, strideW);
```

