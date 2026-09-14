# SetGroups

<!-- md-trans-meta sourceCommit=2737c34bbaf046b0c4e915117e38247d0f621f63 translatedAt=2026-09-05T14:13:47.007Z -->

## Description

Sets the group size for grouped convolution. A group size of 1 indicates ordinary convolution. **The current high-level Conv3D API does not support grouped convolution.**

## Prototype

```
void SetGroups(int64_t groups)
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| groups | Input | Currently only the value 1 is supported, and grouped convolution is not supported yet. |

## Return Value

None

## Constraints

This API is optional and can be called before calling the GetTiling API. If this API is not called, **groups** defaults to 1. Currently, only the value 1 is supported for **groups**, and the convolution capability of group\>1 is not supported yet.

## Examples

```
// Instantiate the Conv3D API.
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
Conv3dTilingApi::Conv3dTiling conv3dApiTiling(ascendcPlatform);
conv3dApiTiling.SetGroups(groups);
```

