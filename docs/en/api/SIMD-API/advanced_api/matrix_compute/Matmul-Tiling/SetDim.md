# SetDim

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-09T07:23:45.022Z pushedAt=2026-09-12T09:55:18.199Z -->

## Description

Sets the number of cores involved in multi-core Matmul computation. The setting rules in different scenarios are as follows:

-   Pure Cube mode (matrix computation only)

    **SetDim** sets the number of cores available on the current AI processor. The number of cores actually used for Matmul computation is obtained through [Tiling computation](GetTiling.md), and the number of cores actually used is less than or equal to the number of cores available on the AI processor. **SetBlockDim** is configured by you based on the number of cores actually used, and all cores loaded by **SetBlockDim** are used for **Matmul API** computation.

-   MIX mode (including matrix computation and vector computation)
    -   Separate mode: **Matmul API** is always initiated from the AIV side. When **Iterate** is called for computation, it only serves as a notification on the AIV side, notifying the AIC to perform matrix computation. After the computation is complete, the AIC notifies the AIV that the computation is finished. At the developer level, what is perceived is the number of AIV cores. **SetDim** is set to the number of AIV cores available on the current AI processor, and the number of AIV cores actually used is obtained through [Tiling computation](GetTiling.md). **SetBlockDim** is set to the number of AI Cores (AIC and AIV combinations) actually used. For example, if **SetDim** is set to 40, it means that 40 AIV cores can be used to initiate multi-core Matmul computation, and [Tiling computation](GetTiling.md) obtains that the number of AIV cores actually used is 20. If the AIC:AIV ratio of the current AI processor is 1:2, **SetBlockDim** is set to 10, meaning that 10 AI Cores (AIC and AIV combinations) are actually used.
    -   Coupled mode: **SetDim** sets the number of cores available on the current AI processor. The number of cores actually used is obtained through [Tiling computation](GetTiling.md), and the number of cores actually used is less than or equal to the number of cores available on the AI processor. **SetBlockDim** is configured by you based on the number of cores actually used, and all cores loaded by **SetBlockDim** are used for **Matmul API** computation.

## Function Prototype

```
int32_t SetDim(int32_t dim)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dim | Input | Number of cores available for multi-core Matmul tiling computation. Note that in MIX mode, the value of this parameter is less than or equal to the number of AI Core cores started in coupled mode or the number of AIV cores started in separated mode. |

## Return Value

-1 indicates a setting failure; 0 indicates a setting success.

## Constraints

None

## Examples

```
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
matmul_tiling::MultiCoreMatmulTiling tiling(ascendcPlatform);
tiling.SetDim(1);  // Set the number of cores involved in the computation.
tiling.SetAType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
tiling.SetBType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
tiling.SetCType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
tiling.SetBiasType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
tiling.SetShape(1024, 1024, 1024);
tiling.SetSingleShape(1024, 1024, 1024);
tiling.SetOrgShape(1024, 1024, 1024);
tiling.SetBias(true);
tiling.SetBufferSpace(-1, -1, -1);
optiling::TCubeTiling tilingData;
int ret = tiling.GetTiling(tilingData);
```

