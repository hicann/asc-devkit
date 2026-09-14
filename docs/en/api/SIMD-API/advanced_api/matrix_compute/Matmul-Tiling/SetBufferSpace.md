# SetBufferSpace

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-09T07:21:48.499Z pushedAt=2026-09-12T09:55:18.193Z -->

## Description

Sets the size of the L1 Buffer/L0C Buffer/Unified Buffer/BiasTable Buffer space available for Matmul computation, in bytes.

## Prototype

```
int32_t SetBufferSpace(int32_t l1Size = -1, int32_t l0CSize = -1, int32_t ubSize = -1, int32_t btSize = -1)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **l1Size** | Input | Sets the L1 Buffer size available for Matmul computation, in bytes. The default value -1 indicates that the AI processor L1 Buffer size is used. |
| **l0CSize** | Input | Sets the L0C Buffer size available for Matmul computation, in bytes. The default value -1 indicates that the AI processor L0C Buffer size is used. |
| **ubSize** | Input | Sets the UB Buffer size available for Matmul computation, in bytes. The default value -1 indicates that the AI processor UB Buffer size is used. |
| **btSize** | Input | Sets the BiasTable Buffer size available for Matmul computation, in bytes. The default value -1 indicates that the AI processor BiasTable Buffer size is used. |

## Return Value

**-1** indicates that the setting failed; **0** indicates that the setting succeeded.

## Constraints

None

## Examples

```
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
matmul_tiling::MatmulApiTiling tiling(ascendcPlatform);
tiling.SetBufferSpace(-1, -1, -1, -1);  // Set the L1/L0C/UB/BT space size available for computation.
```

