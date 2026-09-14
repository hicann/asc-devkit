# SetDoubleBuffer

<!-- md-trans-meta sourceCommit=e9bec8b34ada6c37c20391d212da5ebf088e2e8b translatedAt=2026-09-09T07:25:29.411Z pushedAt=2026-09-12T09:55:18.204Z -->

## Description

Sets whether to enable the double buffer feature for A/B/C/Bias and whether ND2NZ or NZ2ND conversion is required. It is mainly used for internal tuning of the Tiling function.

**This API is reserved and not supported in the current version.**

## Prototype

```
int32_t SetDoubleBuffer(bool a, bool b, bool c, bool bias, bool transND2NZ = true, bool transNZ2ND = true)
```

## Parameters

**Table 1**  Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| a | Input | Sets whether to enable double buffer for the A matrix. |
| b | Input | Sets whether to enable double buffer for the B matrix. |
| c | Input | Sets whether to enable double buffer for the C matrix. |
| bias | Input | Sets whether to enable double buffer for the Bias matrix. |
| transND2NZ | Input | Sets whether [ND2NZ](../../../basic_api/cube_compute_ISASI/cube_compute_load/DataCopy_GMToL1_ND2NZ.md) is required. |
| transNZ2ND | Input | Sets whether [NZ2ND](../../../basic_api/cube_compute_ISASI/cube_compute_store/DataCopy_L0CToGM.md) is required. |

## Return Value

-1 indicates a setting failure; 0 indicates a successful setting.

## Constraints

None
