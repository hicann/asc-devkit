# SetCLayout

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-09T07:22:44.337Z pushedAt=2026-09-12T09:55:18.196Z -->

## Description

Sets the layout axis information of matrix C, including the [B, S, N, G, D axes](../Matmul-Kernel/IterateBatch.md). For the BSNGD, SBNGD, and BNGS1S2 layout formats, before calling the [IterateBatch](../Matmul-Kernel/IterateBatch.md) API, you need to set the layout axis information of matrix C through this API in the Host-side Tiling implementation.

## Prototype

```
int32_t SetCLayout(int32_t b, int32_t s, int32_t n, int32_t g, int32_t d)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **b** | Input | B-axis information of the C matrix layout. |
| **s** | Input | S-axis information of the C matrix layout. |
| **n** | Input | N-axis information of the C matrix layout. |
| **g** | Input | G-axis information of the C matrix layout. |
| **d** | Input | D-axis information of the C matrix layout. |

## Return Value

-1 indicates that the setting fails; 0 indicates that the setting succeeds.

## Constraints

For the BSNGD, SBNGD, and BNGS1S2 layout formats, before calling the [IterateBatch](../Matmul-Kernel/IterateBatch.md) API, you need to call this API in the Host-side Tiling implementation to set the layout axis information of the C matrix.

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
tiling.SetCLayout(C_BNUM, C_SNUM, 1, C_GNUM, C_DNUM);  // Set the C matrix layout.
tiling.SetBatchNum(BATCH_NUM);
tiling.SetBufferSpace(-1, -1, -1);

optiling::TCubeTiling tilingData;
int ret = tiling.GetTiling(tilingData);
```

