# SetStride

<!-- md-trans-meta sourceCommit=ae444a8b0a91a144a05d68a7cd8d1d0e84475d04 translatedAt=2026-11-10T06:24:11.390Z pushedAt=2026-11-18T11:07:26.571Z nl=2 -->

## Description

Sets the stride information.

## Prototype

```
void SetStride(int64_t strideD, int64_t strideH, int64_t strideW)
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| strideD | Input | Stride size in the D direction. |
| strideH | Input | Stride size in the H direction. |
| strideW | Input | Stride size in the W direction. |

## Return Value

None

## Constraints

This API is optional and can be called before calling the GetTiling API. If this API is not called, strideD=1, strideH=1, and strideW=1 by default.

## Examples

```
// Instantiate the Conv3D API.
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
Conv3dTilingApi::Conv3dTiling conv3dApiTiling(ascendcPlatform);
conv3dApiTiling.SetStride(strideD, strideH, strideW);
```

