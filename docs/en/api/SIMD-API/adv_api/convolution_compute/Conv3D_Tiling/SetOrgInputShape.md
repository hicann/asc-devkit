# SetOrgInputShape

<!-- md-trans-meta sourceCommit=ae444a8b0a91a144a05d68a7cd8d1d0e84475d04 translatedAt=2026-11-10T06:23:22.788Z pushedAt=2026-11-18T11:07:26.549Z nl=2 -->

## Description

Sets the original shape of the feature matrix Input.

## Prototype

```
void SetOrgInputShape(int64_t orgCi, int64_t orgDi, int64_t orgHi, int64_t orgWi)
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| orgCi | Input | Size of the original input channel. |
| orgDi | Input | Original Input D dimension size. |
| orgHi | Input | Original Input H dimension size. |
| orgWi | Input | Original Input W dimension size. |

## Return Value

None

## Constraints

None

## Examples

```
// Instantiate the Conv3D API.
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
Conv3dTilingApi::Conv3dTiling conv3dApiTiling(ascendcPlatform);
conv3dApiTiling.SetOrgInputShape(orgCi, orgDi, orgHi, orgWi);
```

