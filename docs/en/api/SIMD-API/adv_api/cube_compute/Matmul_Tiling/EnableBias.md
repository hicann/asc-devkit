# EnableBias

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-11-02T07:00:22.936Z pushedAt=2026-11-18T04:54:32.115Z nl=2 -->

## Description

Sets whether **Bias** participates in the operation. The setting must be consistent with the kernel function (Kernel) side.

## Prototype

```
int32_t EnableBias(bool isBiasIn = false)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| isBiasIn | Input | Sets whether **Bias** participates in the operation.<br><br>true: **Bias** participates in the operation.<br><br>false: **Bias** does not participate in the operation. |

## Return Value

-1 indicates that the setting failed; 0 indicates that the setting succeeded.

## Constraints

None

## Examples

```
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
matmul_tiling::MatmulApiTiling tiling(ascendcPlatform);

tiling.EnableBias(true); // Set whether Bias participates in the operation.
```
