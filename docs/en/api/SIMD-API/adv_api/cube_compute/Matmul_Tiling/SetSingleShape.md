# SetSingleShape

<!-- md-trans-meta sourceCommit=6a51d0b167dd8f5555456a0ebb783ef0427e366e translatedAt=2026-11-02T07:18:55.238Z pushedAt=2026-11-18T04:55:47.846Z nl=2 -->

## Description

Sets the shapes **singleMIn**, **singleNIn**, and **singleKIn** for single-core Matmul computation, in elements.

## Prototype

```
int32_t SetSingleShape(int32_t singleMIn = -1, int32_t singleNIn = -1, int32_t singleKIn = -1)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| singleMIn | Input | Size of **singleMIn** to set, in elements, default value: -1. -1 indicates not to set the specified **singleMIn**, and this value is determined by the **tiling** function itself. |
| singleNIn | Input | Size of **singleNIn** to set, in elements, default value: -1. -1 indicates not to set the specified **singleNIn**, and this value is determined by the **tiling** function itself. |
| singleKIn | Input | Size of **singleKIn** to set, in elements, default value: -1. -1 indicates not to set the specified **singleKIn**, and this value is determined by the **tiling** function itself. |

## Return Value

-1 indicates that the setting failed; 0 indicates that the setting succeeded.

## Constraints

<!-- npu="950" id1 -->
-   In the MxMatmul scenario, if the positions of cubes A and B are both GM, there is no special restriction on **singleKIn**. In this case, if the K-direction sizes of **scaleA** and **scaleB** (that is, Ceil\(singleKIn, 32\)) are odd, you need to pad zeros in the K direction of **scaleA** and **scaleB** to make them even. For example, when **singleKIn** is 30, Ceil\(singleKIn, 32\) is 1, and you need to pad zeros in the K direction of **scaleA** and **scaleB** to make the K direction even. For other combinations of the logical positions of cubes A and B, that is, when the positions of cubes A and B are not both GM, the value of **singleKIn** aligned upward to 32 elements must be an even multiple of 32.
-   In the MxMatmul scenario, when the input data type is fp4x2\_e2m1\_t or fp4x2\_e1m2\_t, the inner axis must be even.
<!-- end id1 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Tiling/SetSingleShape_res.md#id1 -->

## Examples

```
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
matmul_tiling::MultiCoreMatmulTiling tiling(ascendcPlatform);

tiling.SetShape(1024, 1024, 1024);       // Set the shape for a single Matmul computation.
tiling.SetSingleShape(1024, 1024, 1024); // Set the shape for single-core computation.
tiling.SetOrgShape(1024, 1024, 1024);
```
