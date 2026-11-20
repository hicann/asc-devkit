# SetSparse

<!-- md-trans-meta sourceCommit=ae444a8b0a91a144a05d68a7cd8d1d0e84475d04 translatedAt=2026-11-02T07:19:09.808Z pushedAt=2026-11-18T04:55:51.619Z nl=2 -->

## Description

Sets whether the Matmul usage scenario is a Sparse Matmul scenario.

## Prototype

```
int32_t SetSparse(bool isSparseIn = false)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| isSparseIn | Input | Whether to set the Sparse Matmul sparse scenario.<br><br>**true**: Sparse scenario.<br><br>**false**: Non-sparse scenario. |

## Return Value

-1 indicates that the setting failed; 0 indicates that the setting succeeded.

## Constraints

This API must be called before the [GetTiling](GetTiling.md) API.

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
