# SetSingleShape

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-09T07:29:13.317Z pushedAt=2026-09-12T09:55:18.212Z -->

## Description

Sets the single-core computation shape of Matmul, **singleMIn**, **singleNIn**, and **singleKIn**, in elements.

## Prototype

```
int32_t SetSingleShape(int32_t singleMIn = -1, int32_t singleNIn = -1, int32_t singleKIn = -1)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **singleMIn** | Input | Set **singleMIn** size, unit is element, default value is -1. -1 indicates not set the specified **singleMIn**, this value is computed by the tiling function itself. |
| **singleNIn** | Input | Set **singleNIn** size, unit is element, default value is -1. -1 indicates not set the specified **singleNIn**, this value is computed by the tiling function itself. |
| **singleKIn** | Input | Set **singleKIn** size, unit is element, default value is -1. -1 indicates not set the specified **singleKIn**, this value is computed by the tiling function itself. |

## Return Value

**-1** indicates a setting failure; **0** indicates success.

## Constraints

-   In the MxMatmul scenario, when both matrix A and matrix B are located in GM, there is no special restriction on **singleKIn**. In this case, if the K-direction sizes of **scaleA** and **scaleB** (that is, Ceil\(singleKIn, 32\)) are odd, you need to pad zeros in the K direction of **scaleA** and **scaleB** to make them even. For example, when **singleKIn** is 30, Ceil\(singleKIn, 32\) is 1, and you need to pad zeros in the K direction of **scaleA** and **scaleB** to make the K direction even. For other combinations of the logical positions of matrix A and matrix B, that is, when matrix A and matrix B are not both located in GM, the value of **singleKIn** aligned upward to 32 elements must be an even multiple of 32.
-   In the MxMatmul scenario, when the input data type is fp4x2\_e2m1\_t or fp4x2\_e1m2\_t, the inner axis must be even.

## Examples

```
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
matmul_tiling::MultiCoreMatmulTiling tiling(ascendcPlatform);

tiling.SetShape(1024, 1024, 1024);  // Set the shape for a single Matmul computation.
tiling.SetSingleShape(1024, 1024, 1024);  // Set the shape for single-core computation.
tiling.SetOrgShape(1024, 1024, 1024);
```

