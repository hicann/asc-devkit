# SetSingleOutputShape

<!-- md-trans-meta sourceCommit=8d4511303e7599d86c4c8f565e5b0a328eb60b1e translatedAt=2026-11-10T06:24:00.496Z pushedAt=2026-11-18T11:07:26.562Z nl=2 -->

## Description

Sets the shape of the result matrix Output on a single core.

## Prototype

```
void SetSingleOutputShape(int64_t singleCo, int64_t singleDo, int64_t singleM)
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| singleCo | Input | Size of the output channel on a single core. |
| singleDo | Input | Size of the Output D dimension on a single core. |
| singleM | Input | Size of the Output M dimension on a single core. |

## Return Value

None

## Constraints

None

## Examples

```
// Instantiate Conv3D Api.
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
Conv3dTilingApi::Conv3dTiling conv3dApiTiling(ascendcPlatform);
conv3dApiTiling.SetSingleOutputShape(singleCo, singleDo, singleM);
```

