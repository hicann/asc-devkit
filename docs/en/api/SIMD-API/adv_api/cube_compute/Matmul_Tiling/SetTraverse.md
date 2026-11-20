# SetTraverse

<!-- md-trans-meta sourceCommit=ae444a8b0a91a144a05d68a7cd8d1d0e84475d04 translatedAt=2026-11-02T07:21:03.590Z pushedAt=2026-11-18T04:55:57.493Z nl=2 -->

## Description

Sets a fixed Matmul computation direction, prioritizing the M axis or the N axis.

## Prototype

```
int32_t SetTraverse(MatrixTraverse traverse)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| traverse | Input | Sets a fixed Matmul computation direction. Optional values: **MatrixTraverse::FIRSTM**/**MatrixTraverse::FIRSTN**.<br><br>**FIRSTM** means offsetting along the M axis first and then along the N axis.<br><br>**FIRSTN** means offsetting along the N axis first and then along the M axis. |

## Return Value

-1 indicates that the setting failed; 0 indicates that the setting succeeded.

## Constraints

None

## Examples

```
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
matmul_tiling::MatmulApiTiling tiling(ascendcPlatform);
tiling.SetTraverse(MatrixTraverse::FIRSTM); // Set the traversal mode.
```
