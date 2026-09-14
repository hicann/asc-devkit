# SetSparse

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-09T07:29:36.244Z pushedAt=2026-09-12T09:55:18.215Z -->

## Description

Sets whether the Matmul usage scenario is a sparse Matmul scenario.

## Function Prototype

```
int32_t SetSparse(bool isSparseIn = false)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| isSparseIn | Input | Whether to set the Matmul usage scenario to the Sparse Matmul sparse scenario.<br><br>**true**: Sparse scenario.<br><br>**false**: Non-sparse scenario. |

## Return Value

-1 indicates a setting failure; 0 indicates a successful setting.

## Constraints

This API must be called before [GetTiling](GetTiling.md).

## Examples

```
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
matmul_tiling::MatmulApiTiling tiling(ascendcPlatform);
tiling.SetAType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
tiling.SetBType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
tiling.SetCType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
tiling.SetBiasType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
tiling.SetSparse(true); // Set the Sparse Matmul sparse scenario.
tiling.SetShape(1024, 1024, 1024);
tiling.SetOrgShape(1024, 1024, 1024);
tiling.SetBias(true);
tiling.SetBufferSpace(-1, -1, -1);
optiling::TCubeTiling tilingData;
int ret = tiling.GetTiling(tilingData);
```

