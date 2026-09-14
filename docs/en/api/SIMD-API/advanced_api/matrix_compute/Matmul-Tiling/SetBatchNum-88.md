# SetBatchNum

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-09T07:19:51.735Z pushedAt=2026-09-12T09:55:18.187Z -->

## Description

Sets the maximum number of batches for multi-batch computation. The maximum number of batches is the larger of the number of batches in matrix A ([batchA](../Matmul-Kernel/SetBatchNum.md#table9646134355611)) and matrix B ([batchB](../Matmul-Kernel/SetBatchNum.md#table9646134355611)). Before calling [IterateBatch](../Matmul-Kernel/IterateBatch.md), you need to set the number of batches for multi-batch computation through this API in the host-side Tiling implementation.

## Prototype

```
int32_t SetBatchNum(int32_t batch)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| batch | Input | Batch count for multi-batch computation, which is the maximum of the batch count of matrix A (batchA) and that of matrix B (batchB). |

## Return Value

-1 indicates a setting failure; 0 indicates a setting success.

## Constraints

Before calling the [IterateBatch](../Matmul-Kernel/IterateBatch.md) API, set the number of batches for multi-batch computation through this API in the Host-side Tiling implementation.

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

constexpr int32_t A_BNUM = 2;
constexpr int32_t A_SNUM = 32;
constexpr int32_t A_GNUM = 3;
constexpr int32_t A_DNUM = 64;
constexpr int32_t B_BNUM = 2;
constexpr int32_t B_SNUM = 256;
constexpr int32_t B_GNUM = 3;
constexpr int32_t B_DNUM = 64;
constexpr int32_t C_BNUM = 2;
constexpr int32_t C_SNUM = 32;
constexpr int32_t C_GNUM = 3;
constexpr int32_t C_DNUM = 256;
constexpr int32_t BATCH_NUM = 3;
tiling.SetALayout(A_BNUM, A_SNUM, 1, A_GNUM, A_DNUM);
tiling.SetBLayout(B_BNUM, B_SNUM, 1, B_GNUM, B_DNUM);
tiling.SetCLayout(C_BNUM, C_SNUM, 1, C_GNUM, C_DNUM);
tiling.SetBatchNum(BATCH_NUM);  // Set the number of batches.
tiling.SetBufferSpace(-1, -1, -1);

optiling::TCubeTiling tilingData;
int ret = tiling.GetTiling(tilingData);
```

