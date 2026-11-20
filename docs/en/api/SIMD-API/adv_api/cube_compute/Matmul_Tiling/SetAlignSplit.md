# SetAlignSplit

<!-- md-trans-meta sourceCommit=ae444a8b0a91a144a05d68a7cd8d1d0e84475d04 translatedAt=2026-11-02T07:07:47.007Z pushedAt=2026-11-18T04:54:59.865Z nl=2 -->

## Description

During multi-core splitting, sets the alignment values of **singleCoreM**/**singleCoreN**/**singleCoreK**. For example, if the alignment value of **singleCoreM** is set to 64 (in elements), the split **singleCoreM** is a multiple of 64.

## Prototype

```
int32_t SetAlignSplit(int32_t alignM, int32_t alignN, int32_t alignK)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| alignM | Input | Alignment value of **singleCoreM**. If -1 or 0 is passed in, it indicates that the specified **singleCoreM** alignment value is not set, and the value is calculated by the Tiling function itself. |
| alignN | Input | Alignment value of **singleCoreN**. If -1 or 0 is passed in, it indicates that the specified **singleCoreN** alignment value is not set, and the value is calculated by the Tiling function itself. |
| alignK | Input | Alignment value of **singleCoreK**. If -1 or 0 is passed in, it indicates that the specified **singleCoreK** alignment value is not set, and the value is calculated by the Tiling function itself. |

## Return Value

-1 indicates that the setting failed; 0 indicates that the setting succeeded.

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
tiling.SetAlignSplit(-1, 64, -1); // Set the alignment value of singleCoreM/singleCoreN/singleCoreK.
tiling.SetOrgShape(1024, 1024, 1024);
tiling.SetBias(true);
tiling.SetBufferSpace(-1, -1, -1);
optiling::TCubeTiling tilingData;
int ret = tiling.GetTiling(tilingData);
```
