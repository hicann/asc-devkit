# SetBatchInfoForNormal

<!-- md-trans-meta sourceCommit=ae444a8b0a91a144a05d68a7cd8d1d0e84475d04 translatedAt=2026-11-02T07:08:19.568Z pushedAt=2026-11-18T04:55:05.394Z nl=2 -->

## Description

Sets the M/N/K axis information of cubes A and B, as well as the batch counts of cubes A and B. In scenarios where the layout type is NORMAL, before calling [IterateBatch](../Matmul_Kernel/IterateBatch.md) or [IterateNBatch](../Matmul_Kernel/IterateNBatch.md), you need to set the M/N/K axis information of cubes A and B through this API in the Host-side Tiling implementation.

## Prototype

```
int32_t SetBatchInfoForNormal(int32_t batchA, int32_t batchB, int32_t m, int32_t n, int32_t k)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| batchA | Input | Batch count of cube A. |
| batchB | Input | Batch count of cube B. |
| m | Input | M-axis information of cube A. |
| n | Input | N-axis information of cube B. |
| k | Input | K-axis information of cubes A and B. |

## Return Value

-1 indicates that the setting failed; 0 indicates that the setting succeeded.

## Constraints

In scenarios where the layout type is NORMAL, before calling [IterateBatch](../Matmul_Kernel/IterateBatch.md) or [IterateNBatch](../Matmul_Kernel/IterateNBatch.md), you need to set the M/N/K axis information of cubes A and B through this API in the Host-side Tiling implementation.

## Examples

```
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
matmul_tiling::MultiCoreMatmulTiling tiling(ascendcPlatform);
int32_t M = 32;
int32_t N = 256;
int32_t K = 64;
tiling.SetDim(1);
tiling.SetAType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
tiling.SetBType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
tiling.SetCType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
tiling.SetBiasType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
tiling.SetShape(M, N, K);
tiling.SetOrgShape(M, N, K);
tiling.SetBias(true);
tiling.SetBufferSpace(-1, -1, -1);

constexpr int32_t BATCH_NUM = 3;
tiling.SetBatchInfoForNormal(BATCH_NUM, BATCH_NUM, M, N, K); // Set the cube layout.
tiling.SetBufferSpace(-1, -1, -1);

optiling::TCubeTiling tilingData;
int ret = tiling.GetTiling(tilingData);
```
