# SetPadding

<!-- md-trans-meta sourceCommit=e59e1725727133ce76073dba21b022a1b0c9fd56 translatedAt=2026-09-05T14:15:21.855Z -->

## Description

Sets the padding information.

## Prototype

```
void SetPadding(int64_t padHead, int64_t padTail, int64_t padUp, int64_t padDown, int64_t padLeft, int64_t padRight)
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| padHead | Input | Padding size at the front in the D direction. |
| padTail | Input | Padding size at the back in the D direction. |
| padUp | Input | Padding size at the top in the H direction. |
| padDown | Input | Padding size at the bottom in the H direction. |
| padLeft | Input | Padding size on the left in the W direction. |
| padRight | Input | Padding size on the right in the W direction. |

## Return Value

None

## Constraints

This API is optional and can be called before calling the **GetTiling** API. If this API is not called, the default values are **padHead**=0, **padTail**=0, **padUp**=0, **padDown**=0, **padLeft**=0, and **padRight**=0.

## Examples

```
// Instantiate the Conv3D API.
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
Conv3dTilingApi::Conv3dTiling conv3dApiTiling(ascendcPlatform);
conv3dApiTiling.SetPadding(padHead, padTail, padUp, padDown, padLeft, padRight);
```

