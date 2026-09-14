# SetDilation

<!-- md-trans-meta sourceCommit=e59e1725727133ce76073dba21b022a1b0c9fd56 translatedAt=2026-09-05T14:13:18.728Z -->

## Description

Set the dilation information.

## Prototype

```
void SetDilation(int64_t dilationD, int64_t dilationH, int64_t dilationW)
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **dilationD** | Input | Dilation size in the D direction. |
| **dilationH** | Input | Dilation size in the H direction. |
| **dilationW** | Input | Dilation size in the W direction. |

## Return Value

None

## Constraints

This API is optional before calling the **GetTiling** API. If this API is not called, the default values are **dilationD**=1, **dilationH**=1, and **dilationW**=1.

## Examples

```
// Instantiate the Conv3D API.
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
Conv3dTilingApi::Conv3dTiling conv3dApiTiling(ascendcPlatform);
conv3dApiTiling.SetDilation(dilationD, dilationH, dilationW);
```

