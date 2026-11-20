# GetSingleShape

<!-- md-trans-meta sourceCommit=ae444a8b0a91a144a05d68a7cd8d1d0e84475d04 translatedAt=2026-11-02T07:04:36.229Z pushedAt=2026-11-18T04:54:45.262Z nl=2 -->

## Description

Obtains the calculated **singleCoreM**/**singleCoreN**/**singleCoreK**.

## Prototype

```
int32_t GetSingleShape(int32_t& shapeM, int32_t& shapeN, int32_t& shapeK)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| shapeM | Output | Get the **singleCoreM** value calculated by Multi-Core Tiling. |
| shapeN | Output | Get the **singleCoreN** value calculated by Multi-Core Tiling. |
| shapeK | Output | Get the **singleCoreK** value calculated by Multi-Core Tiling. |

## Return Value

-1 indicates that the setting failed; 0 indicates that the setting succeeded.

## Constraints

Call this API using the created **Tiling** object, and it must be called after the Tiling calculation is completed ([GetTiling](GetTiling.md)).

## Examples

```
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
matmul_tiling::MultiCoreMatmulTiling tiling(ascendcPlatform);
tiling.SetDim(1);
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

// Obtain the calculated singleCoreM/singleCoreN/singleCoreK.
int32_t singleM, singleN, singleK;
int ret1 = tiling.GetSingleShape(singleM, singleN, singleK);
```
