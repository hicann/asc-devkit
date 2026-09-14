# SetAlignSplit

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-09T07:16:55.349Z pushedAt=2026-09-12T09:55:18.179Z -->

## Description

During multi-core splitting, set the alignment value of **singleCoreM**/**singleCoreN**/**singleCoreK**. For example, if the alignment value of **singleCoreM** is set to 64 (in elements), the split **singleCoreM** is a multiple of 64.

## Prototype

```
int32_t SetAlignSplit(int32_t alignM, int32_t alignN, int32_t alignK)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| alignM | Input | Alignment value of **singleCoreM**. If -1 or 0 is passed, it indicates not set the specified alignment value of **singleCoreM**, and the value is computed by the **Tiling** function itself. |
| alignN | Input | Alignment value of **singleCoreN**. If -1 or 0 is passed, it indicates not set the specified alignment value of **singleCoreN**, and the value is computed by the **Tiling** function itself. |
| alignK | Input | Alignment value of **singleCoreK**. If -1 or 0 is passed, it indicates not set the specified alignment value of **singleCoreK**, and the value is computed by the **Tiling** function itself. |

## Return Value

-1 indicates a setting failure; 0 indicates a setting success.

## Constraints

None

## Examples

```
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
matmul_tiling::MultiCoreMatmulTiling tiling(ascendcPlatform);
tiling.SetDim(1);
tiling.SetAType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
tiling.SetBType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
tiling.SetCType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
tiling.SetBiasType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
tiling.SetShape(1024, 1024, 1024);
tiling.SetAlignSplit(-1, 64, -1);  // Set the alignment values of singleCoreM/singleCoreN/singleCoreK.
tiling.SetOrgShape(1024, 1024, 1024);
tiling.SetBias(true);
tiling.SetBufferSpace(-1, -1, -1);
optiling::TCubeTiling tilingData;
int ret = tiling.GetTiling(tilingData);
```

