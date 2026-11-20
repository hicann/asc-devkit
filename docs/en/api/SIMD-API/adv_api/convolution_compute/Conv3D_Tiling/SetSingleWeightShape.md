# SetSingleWeightShape

<!-- md-trans-meta sourceCommit=ae444a8b0a91a144a05d68a7cd8d1d0e84475d04 translatedAt=2026-11-10T06:24:03.651Z pushedAt=2026-11-18T11:07:26.568Z nl=2 -->

## Description

Sets the shape of the weight matrix Weight on a single core.

## Prototype

```
void SetSingleWeightShape(int64_t singleCi, int64_t singleKd, int64_t singleKh, int64_t singleKw)
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| singleCi | Input | Size of the input channel on a single core. |
| singleKd | Input | D dimension size of Weight on a single core. |
| singleKh | Input | H dimension size of Weight on a single core. |
| singleKw | Input | W dimension size of Weight on a single core. |

## Return Value

None

## Constraints

None

## Examples

```
// Instantiate the Conv3D Api.
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
Conv3dTilingApi::Conv3dTiling conv3dApiTiling(ascendcPlatform);
conv3dApiTiling.SetSingleWeightShape(singleCi, singleKd, singleKh, singleKw);
```

