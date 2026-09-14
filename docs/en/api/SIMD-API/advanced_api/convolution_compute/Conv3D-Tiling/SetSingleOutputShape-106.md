# SetSingleOutputShape

<!-- md-trans-meta sourceCommit=e59e1725727133ce76073dba21b022a1b0c9fd56 translatedAt=2026-09-05T14:16:10.001Z -->

## Description

Sets the shape of the result matrix **Output** on a single core.

## Prototype

```
void SetSingleOutputShape(int64_t singleCo, int64_t singleDo, int64_t singleM)
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **singleCo** | Input | Size of the output channel on a single core. |
| **singleDo** | Input | Size of the Output D dimension on a single core. |
| **singleM** | Input | Size of the Output M dimension on a single core. |

## Return Value

None

## Constraints

None

## Examples

```
// Instantiate the Conv3D API.
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
Conv3dTilingApi::Conv3dTiling conv3dApiTiling(ascendcPlatform);
conv3dApiTiling.SetSingleOutputShape(singleCo, singleDo, singleM);
```

