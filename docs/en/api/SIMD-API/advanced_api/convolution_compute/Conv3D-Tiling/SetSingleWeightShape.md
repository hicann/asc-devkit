# SetSingleWeightShape

<!-- md-trans-meta sourceCommit=e59e1725727133ce76073dba21b022a1b0c9fd56 translatedAt=2026-09-05T14:16:10.676Z -->

## Description

Sets the shape of the weight matrix **Weight** on a single core.

## Prototype

```
void SetSingleWeightShape(int64_t singleCi, int64_t singleKd, int64_t singleKh, int64_t singleKw)
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| singleCi | Input | Size of the input channel on a single core. |
| singleKd | Input | Weight D dimension size on a single core. |
| singleKh | Input | Weight H dimension size on a single core. |
| singleKw | Input | Weight W dimension size on a single core. |

## Return Value

None

## Constraints

None

## Examples

```
// Instantiate the Conv3D API.
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
Conv3dTilingApi::Conv3dTiling conv3dApiTiling(ascendcPlatform );
conv3dApiTiling.SetSingleWeightShape(singleCi, singleKd, singleKh, singleKw);
```

