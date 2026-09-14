# SetStride

<!-- md-trans-meta sourceCommit=e59e1725727133ce76073dba21b022a1b0c9fd56 translatedAt=2026-09-05T14:16:51.687Z -->

## Description

Sets the stride information.

## Prototype

```
void SetStride(int64_t strideD, int64_t strideH, int64_t strideW)
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **strideD** | Input | Stride size in the D direction. |
| **strideH** | Input | Stride size in the H direction. |
| **strideW** | Input | Stride size in the W direction. |

## Return Value

None

## Constraints

This API is optional and can be called before calling the **GetTiling** API. If this API is not called, the default values are **strideD**=1, **strideH**=1, and **strideW**=1.

## Examples

```
// Instantiate the Conv3D API.
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
Conv3dTilingApi::Conv3dTiling conv3dApiTiling(ascendcPlatform);
conv3dApiTiling.SetStride(strideD, strideH, strideW);
```

