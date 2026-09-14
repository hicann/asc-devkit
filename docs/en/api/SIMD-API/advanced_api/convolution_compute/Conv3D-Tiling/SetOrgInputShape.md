# SetOrgInputShape

<!-- md-trans-meta sourceCommit=e59e1725727133ce76073dba21b022a1b0c9fd56 translatedAt=2026-09-05T14:14:33.766Z -->

## Description

Sets the original shape of the feature matrix Input.

## Prototype

```
void SetOrgInputShape(int64_t orgCi, int64_t orgDi, int64_t orgHi, int64_t orgWi)
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| orgCi | Input | Original input channel size. |
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

