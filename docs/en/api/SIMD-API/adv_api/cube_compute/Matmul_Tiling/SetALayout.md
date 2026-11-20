# SetALayout

<!-- md-trans-meta sourceCommit=ae444a8b0a91a144a05d68a7cd8d1d0e84475d04 translatedAt=2026-11-02T07:06:41.655Z pushedAt=2026-11-18T04:54:57.511Z nl=2 -->

## Description

Sets the Layout axis information of cube A, including the [B, S, N, G, and D axes](../Matmul_Kernel/IterateBatch.md). For the BSNGD, SBNGD, and BNGS1S2 Layout formats, before calling the [IterateBatch](../Matmul_Kernel/IterateBatch.md) API, set the Layout axis information of cube A through this API in the Host-side Tiling implementation.

## Prototype

```
int32_t SetALayout(int32_t b, int32_t s, int32_t n, int32_t g, int32_t d)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| b | Input | B-axis information of the Layout of cube A. |
| s | Input | S-axis information of the Layout of cube A. |
| n | Input | N-axis information of the Layout of cube A. |
| g | Input | G-axis information of the Layout of cube A. |
| d | Input | D-axis information of the Layout of cube A. |

## Return Value

-1 indicates that the setting failed; 0 indicates that the setting succeeded.

## Constraints

For the BSNGD, SBNGD, and BNGS1S2 Layout formats, before calling the [IterateBatch](../Matmul_Kernel/IterateBatch.md) API, set the Layout axis information of cube A through this API in the Host-side Tiling implementation.

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
tiling.SetALayout(A_BNUM, A_SNUM, 1, A_GNUM, A_DNUM); // Set the layout of cube A.
tiling.SetBLayout(B_BNUM, B_SNUM, 1, B_GNUM, B_DNUM);
tiling.SetCLayout(C_BNUM, C_SNUM, 1, C_GNUM, C_DNUM);
tiling.SetBatchNum(BATCH_NUM);
tiling.SetBufferSpace(-1, -1, -1);

optiling::TCubeTiling tilingData;
int ret = tiling.GetTiling(tilingData);
```
