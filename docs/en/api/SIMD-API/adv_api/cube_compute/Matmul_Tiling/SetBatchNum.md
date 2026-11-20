# SetBatchNum

<!-- md-trans-meta sourceCommit=8d4511303e7599d86c4c8f565e5b0a328eb60b1e translatedAt=2026-11-02T07:09:14.136Z pushedAt=2026-11-18T04:55:07.276Z nl=2 -->

## Description

Sets the maximum batch number for multi-batch computation. The maximum batch number is the larger of [batchA](../Matmul_Kernel/SetBatchNum.md#table9646134355611) of cube A and [batchB](../Matmul_Kernel/SetBatchNum.md#table9646134355611) of cube B. Before calling the [IterateBatch](../Matmul_Kernel/IterateBatch.md) API, set the batch number for multi-batch computation through this API in the host-side tiling implementation.

## Prototype

```
int32_t SetBatchNum(int32_t batch)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| batch | Input | Batch number for multi-batch computation. The batch number is the larger of **batchA** of cube A and **batchB** of cube B. |

## Return Value

-1 indicates that the setting failed; 0 indicates that the setting succeeded.

## Constraints

Before calling the [IterateBatch](../Matmul_Kernel/IterateBatch.md) API, set the batch number for multi-batch computation through this API in the host-side tiling implementation.

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
tiling.SetBatchNum(BATCH_NUM); // Set the batch number.
tiling.SetBufferSpace(-1, -1, -1);

optiling::TCubeTiling tilingData;
int ret = tiling.GetTiling(tilingData);
```
