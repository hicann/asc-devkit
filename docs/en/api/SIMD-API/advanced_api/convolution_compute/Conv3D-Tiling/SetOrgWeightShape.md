# SetOrgWeightShape

<!-- md-trans-meta sourceCommit=e59e1725727133ce76073dba21b022a1b0c9fd56 translatedAt=2026-09-05T14:14:39.609Z -->

## Description

Sets the original shape of the weight matrix **Weight**.

## Prototype

```
void SetOrgWeightShape(int64_t orgCo, int64_t orgKd, int64_t orgKh, int64_t orgKw)
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **orgCo** | Input | Size of the original output channel. |
| **orgKd** | Input | Size of the original **Weight** D dimension. |
| **orgKh** | Input | Size of the original **Weight** H dimension. |
| **orgKw** | Input | Size of the original **Weight** W dimension. |

## Return Value

None

## Constraints

None

## Examples

```
// Instantiate the Conv3D API.
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
Conv3dTilingApi::Conv3dTiling conv3dApiTiling(ascendcPlatform);
conv3dApiTiling.SetOrgWeightShape(cout, kd, kh, kw);
```

