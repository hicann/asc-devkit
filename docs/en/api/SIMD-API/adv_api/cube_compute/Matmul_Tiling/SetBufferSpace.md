# SetBufferSpace

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-11-02T07:11:24.326Z pushedAt=2026-11-18T04:55:17.698Z nl=2 -->

## Description

Sets the sizes of the L1 Buffer, L0C Buffer, Unified Buffer (UB), and BiasTable Buffer available for Matmul computation, in bytes.

## Prototype

```
int32_t SetBufferSpace(int32_t l1Size = -1, int32_t l0CSize = -1, int32_t ubSize = -1, int32_t btSize = -1)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| l1Size | Input | Sets the L1 Buffer size available for Matmul computation, in bytes. The default value -1 indicates that the AI Processor L1 Buffer size is used. |
| l0CSize | Input | Sets the L0C Buffer size available for Matmul computation, in bytes. The default value -1 indicates that the AI Processor L0C Buffer size is used. |
| ubSize | Input | Sets the UB size available for Matmul computation, in bytes. The default value -1 indicates that the AI Processor UB size is used. |
| btSize | Input | Sets the BiasTable Buffer size available for Matmul computation, in bytes. The default value -1 indicates that the AI Processor BiasTable Buffer size is used. |

## Return Value

-1 indicates that the setting failed; 0 indicates that the setting succeeded.

## Constraints

None

## Examples

```
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
matmul_tiling::MatmulApiTiling tiling(ascendcPlatform);
tiling.SetBufferSpace(-1, -1, -1, -1); // Set the L1/L0C/UB/BT space sizes available for computation.
```
