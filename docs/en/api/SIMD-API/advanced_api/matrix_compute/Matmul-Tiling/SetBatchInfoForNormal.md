# SetBatchInfoForNormal

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-09T07:19:01.240Z pushedAt=2026-09-12T09:55:18.183Z -->

## Description

Sets the M/N/K axis information of matrices A and B, as well as the batch count of matrices A and B. In scenarios where the layout type is NORMAL, before calling [IterateBatch](../Matmul-Kernel/IterateBatch.md) or [IterateNBatch](../Matmul-Kernel/IterateNBatch.md), you need to set the M/N/K axis information of matrices A and B through this API in the Host-side Tiling implementation.

## Prototype

```
int32_t SetBatchInfoForNormal(int32_t batchA, int32_t batchB, int32_t m, int32_t n, int32_t k)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| batchA | Input | Batch number of matrix A. |
| batchB | Input | Batch number of matrix B. |
| m | Input | M-axis information of matrix A. |
| n | Input | N-axis information of matrix B. |
| k | Input | K-axis information of matrices A and B. |

## Return Value

-1 indicates that the setting fails; 0 indicates that the setting succeeds.

## Constraints

In scenarios where the **Layout** type is **NORMAL**, before calling the [IterateBatch](../Matmul-Kernel/IterateBatch.md) or [IterateNBatch](../Matmul-Kernel/IterateNBatch.md) API, you need to set the M/N/K axis information of the A/B matrices through this API in the Host-side Tiling implementation.

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
tiling.SetBatchInfoForNormal(BATCH_NUM, BATCH_NUM, M, N, K);  // Set the matrix layout.
tiling.SetBufferSpace(-1, -1, -1);

optiling::TCubeTiling tilingData;
int ret = tiling.GetTiling(tilingData);
```

