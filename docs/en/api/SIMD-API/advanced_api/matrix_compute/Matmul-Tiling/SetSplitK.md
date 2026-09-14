# SetSplitK

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-09T07:30:41.215Z pushedAt=2026-09-12T09:55:18.216Z -->

## Description

The [EnableMultiCoreSplitK](EnableMultiCoreSplitK.md) API provides the same functionality as this API. It is recommended that you use [EnableMultiCoreSplitK](EnableMultiCoreSplitK.md).

In a multi-core scenario, this API enables splitting of the K axis. If this API is not called, the K axis is not split by default. Call this API before calling **GetTiling**.

## Function Prototype

```
void SetSplitK(bool flag)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **flag** | Input | Whether to enable the split K axis.<br><br>**true**: Enable the split K axis.<br><br>**false**: Disable the split K axis. |

## Return Value

None

## Constraints

-   If this API is used in an operator, the C matrix result can only be output to **Global Memory**.
-   If this API is used in an operator, before the C matrix tile result is written to **Global Memory** for the first time in the kernel-side code, **Global Memory** must be cleared first, and then **AtomicAdd** accumulation is enabled when the C matrix tile result is obtained. If **Global Memory** is not cleared in advance, precision issues may occur due to accumulation of the original invalid data in **Global Memory**.

## Example

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
