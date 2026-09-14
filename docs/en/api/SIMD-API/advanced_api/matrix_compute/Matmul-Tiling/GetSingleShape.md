# GetSingleShape

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-09T07:14:25.185Z pushedAt=2026-09-12T09:55:18.174Z -->

## Description

Obtains the computed **singleCoreM**/**singleCoreN**/**singleCoreK** values.

## Prototype

```
int32_t GetSingleShape(int32_t &shapeM, int32_t &shapeN, int32_t &shapeK)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| shapeM | Output | Obtains the **singleCoreM** value obtained from multi-core Tiling computation. |
| shapeN | Output | Obtains the **singleCoreN** value obtained from multi-core Tiling computation. |
| shapeK | Output | Obtains the **singleCoreK** value obtained from multi-core Tiling computation. |

## Return Value

-1 indicates a setting failure; 0 indicates a setting success.

## Constraints

Call this API using the created **Tiling** object, and call it only after the Tiling computation ([GetTiling](GetTiling.md)) is complete.

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

// Obtain the computed singleCoreM/singleCoreN/singleCoreK.
int32_t singleM, singleN, singleK;
int ret1 = tiling.GetSingleShape(singleM, singleN, singleK);
```

