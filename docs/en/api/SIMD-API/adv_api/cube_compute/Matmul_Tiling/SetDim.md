# SetDim

<!-- md-trans-meta sourceCommit=ae444a8b0a91a144a05d68a7cd8d1d0e84475d04 translatedAt=2026-11-02T07:13:26.543Z pushedAt=2026-11-18T04:55:27.149Z nl=2 -->

## Description

Sets the number of cores involved in the computation for multi-core Matmul. The setting rules in different scenarios are as follows:

-   Cube-only mode (cube computation only)

    SetDim sets the number of cores available on the current AI Processor. The number of cores actually used for Matmul computation is obtained through [Tiling calculation](GetTiling.md), and the number of cores actually used is less than or equal to the number of cores available on the AI Processor. SetSimdNumBlocks is configured by the user based on the number of cores actually used, and all cores loaded by SetSimdNumBlocks are used for Matmul API computation.

-   MIX mode (including cube computation and vector computation)
    -   Separated mode: Matmul APIs are all initiated from the AIV side. When Iterate is called for computation, it only serves as a notification on the AIV side, notifying the AIC to perform cube computation. After the computation is complete, the AIC notifies the AIV that the computation is finished. From the developer's perspective, what is perceived is the number of AIV cores. SetDim is set to the number of AIV cores available on the current AI Processor, and the number of AIV cores actually used is obtained through [Tiling calculation](GetTiling.md). SetSimdNumBlocks is set to the number of AI Cores (AIC and AIV combinations) actually used. For example, if SetDim is set to 40, it means that 40 AIV cores can be used to initiate multi-core Matmul operations, and the number of AIV cores actually used obtained through [Tiling calculation](GetTiling.md) is 20. If the AIC:AIV ratio of the current AI Processor is 1:2, then SetSimdNumBlocks is set to 10, meaning that 10 AI Cores (AIC and AIV combinations) are actually used.
    -   Coupled mode: SetDim sets the number of cores available on the current AI Processor. The number of cores actually used is obtained through [Tiling calculation](GetTiling.md), and the number of cores actually used is less than or equal to the number of cores available on the AI Processor. SetSimdNumBlocks is configured by the user based on the number of cores actually used, and all cores loaded by SetSimdNumBlocks are used for Matmul API computation.

## Prototype

```
int32_t SetDim(int32_t dim)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dim | Input | Number of cores that can be used during multi-core Matmul tiling computation. Note that in MIX mode, the value of this parameter is less than or equal to the number of AI Core cores started in coupled mode or the number of AIV cores started in separated mode. |

## Return Value

-1 indicates that the setting failed; 0 indicates that the setting succeeded.

## Constraints

None

## Examples

```
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
matmul_tiling::MultiCoreMatmulTiling tiling(ascendcPlatform);
tiling.SetDim(1); // Set the number of cores involved in the computation.
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
