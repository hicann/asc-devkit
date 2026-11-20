# SetOrgWeightShape

<!-- md-trans-meta sourceCommit=ae444a8b0a91a144a05d68a7cd8d1d0e84475d04 translatedAt=2026-11-10T06:23:33.104Z pushedAt=2026-11-18T11:07:26.552Z nl=2 -->

## Description

Sets the original shape of the weight matrix Weight.

## Prototype

```
void SetOrgWeightShape(int64_t orgCo, int64_t orgKd, int64_t orgKh, int64_t orgKw)
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| orgCo | Input | Original output channel size. |
| orgKd | Input | Original Weight D dimension size. |
| orgKh | Input | Original Weight H dimension size. |
| orgKw | Input | Original Weight W dimension size. |

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

