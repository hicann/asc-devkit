# EnableBias

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-09T07:10:57.949Z pushedAt=2026-09-12T09:55:18.160Z -->

## Description

Sets whether **Bias** participates in operations. The configured information must be consistent with that on the **Kernel** side.

## Prototype

```
int32_t EnableBias(bool isBiasIn = false)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| isBiasIn | Input | Whether Bias participates in operations.<br><br>true: Bias participates in operations.<br><br>false: Bias does not participate in operations. |

## Return Value

-1 indicates that the setting fails; 0 indicates that the setting succeeds.

## Constraints

None

## Examples

```
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
matmul_tiling::MatmulApiTiling tiling(ascendcPlatform);

tiling.EnableBias(true);  // Set whether Bias participates in operations.
```

