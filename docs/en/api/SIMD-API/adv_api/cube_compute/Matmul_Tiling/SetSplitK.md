# SetSplitK

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-11-02T07:19:57.639Z pushedAt=2026-11-18T04:55:53.635Z nl=2 -->

## Description

The [EnableMultiCoreSplitK](EnableMultiCoreSplitK.md) API provides the same functionality as this API. It is recommended to use [EnableMultiCoreSplitK](EnableMultiCoreSplitK.md).

In multi-core scenarios, this API enables splitting of the K axis. If this API is not called, the K axis is not split by default. Call this API before calling the GetTiling API.

## Prototype

```
void SetSplitK(bool flag)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| flag | Input | Whether to enable splitting of the K axis.<br><br>**true**: Enables splitting of the K axis.<br><br>**false**: Does not enable splitting of the K axis. |

## Return Value

None

## Constraints

-   If this API is used in an operator, the C cube result can only be output to Global Memory when obtained.
-   If this API is used in an operator, before the result of the C cube tile is first written to Global Memory in the kernel-side code, Global Memory must be cleared first. Then, when the result of the C cube tile is obtained, AtomicAdd accumulation is enabled. If Global Memory is not cleared in advance, precision issues may occur because the original invalid data in Global Memory is accumulated.

## Examples

```
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo())
    matmul_tiling::MultiCoreMatmulTiling tiling(ascendcPlatform);
tiling.SetDim(useCoreNums);
tiling.SetAType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
tiling.SetBType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
tiling.SetCType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
tiling.SetBiasType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
tiling.SetShape(M, N, K);
tiling.SetOrgShape(M, N, K);
tiling.SetBias(true);
tiling.SetBufferSpace(-1, -1, -1);
tiling.SetSplitK(true);

optiling::TCubeTiling tilingData;
int ret = tiling.GetTiling(tilingData);
```
