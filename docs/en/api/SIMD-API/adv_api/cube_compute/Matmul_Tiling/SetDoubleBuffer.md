# SetDoubleBuffer

<!-- md-trans-meta sourceCommit=ae444a8b0a91a144a05d68a7cd8d1d0e84475d04 translatedAt=2026-11-02T07:13:58.977Z pushedAt=2026-11-18T04:55:29.085Z nl=2 -->

## Description

Sets whether to enable the double buffer feature for the A/B/C/Bias cubes and whether to perform ND2NZ or NZ2ND conversion. It is mainly used for internal tuning of the Tiling function.

**This API is reserved and is not supported in the current version.**

## Prototype

```
int32_t SetDoubleBuffer(bool a, bool b, bool c, bool bias, bool transND2NZ = true, bool transNZ2ND = true)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| a | Input | Sets whether to enable double buffer for the A cube. |
| b | Input | Sets whether to enable double buffer for the B cube. |
| c | Input | Sets whether to enable double buffer for the C cube. |
| bias | Input | Sets whether to enable double buffer for the Bias cube. |
| transND2NZ | Input | Sets whether [ND2NZ](../../../basic_api/cube_compute_ISASI/cube_compute_load/DataCopy_GMToL1_ND2NZ.md) is required. |
| transNZ2ND | Input | Sets whether [NZ2ND](../../../basic_api/cube_compute_ISASI/cube_compute_store/DataCopy_L0CToGM.md) is required. |

## Return Value

-1 indicates that the setting failed; 0 indicates that the setting succeeded.

## Constraints

None
