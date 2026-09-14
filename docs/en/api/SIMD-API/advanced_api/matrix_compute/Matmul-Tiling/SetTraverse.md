# SetTraverse

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-09T07:32:04.160Z pushedAt=2026-09-12T09:55:18.220Z -->

## Description

Sets a fixed Matmul computation direction, either M-axis first or N-axis first.

## Prototype

```
int32_t SetTraverse(MatrixTraverse traverse)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| traverse | Input | Sets the fixed Matmul computation direction. Optional values: **MatrixTraverse::FIRSTM**/**MatrixTraverse::FIRSTN**.<br><br>**FIRSTM** indicates offsetting along the M-axis direction first and then along the N-axis direction.<br><br>**FIRSTN** indicates offsetting along the N-axis direction first and then along the M-axis direction. |

## Return Value

-1 indicates a setting failure; 0 indicates a setting success.

## Constraints

None

## Examples

```
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
matmul_tiling::MatmulApiTiling tiling(ascendcPlatform);
tiling.SetTraverse(MatrixTraverse::FIRSTM);  // Set the traversal mode.
```

