# SetSingleRange

<!-- md-trans-meta sourceCommit=ae444a8b0a91a144a05d68a7cd8d1d0e84475d04 translatedAt=2026-11-02T07:18:04.813Z pushedAt=2026-11-18T04:55:45.781Z nl=2 -->

## Description

Sets the maximum and minimum values of singleCoreM/singleCoreN/singleCoreK.

## Prototype

```
int32_t SetSingleRange(int32_t maxM = -1, int32_t maxN = -1, int32_t maxK = -1, int32_t minM = -1, int32_t minN = -1, int32_t minK = -1)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| maxM | Input | Sets the maximum singleCoreM value. The default value is -1, which indicates that the specified singleCoreM maximum value is not set; the value is calculated by the Tiling function itself. |
| maxN | Input | Sets the maximum singleCoreN value. The default value is -1, which indicates that the specified singleCoreN maximum value is not set; the value is calculated by the Tiling function itself. |
| maxK | Input | Sets the maximum singleCoreK value. The default value is -1, which indicates that the specified singleCoreK maximum value is not set; the value is calculated by the Tiling function itself. |
| minM | Input | Sets the minimum singleCoreM value. The default value is -1, which indicates that the specified singleCoreM minimum value is not set; the value is calculated by the Tiling function itself. |
| minN | Input | Sets the minimum singleCoreN value. The default value is -1, which indicates that the specified singleCoreN minimum value is not set; the value is calculated by the Tiling function itself. |
| minK | Input | Sets the minimum singleCoreK value. The default value is -1, which indicates that the specified singleCoreK minimum value is not set; the value is calculated by the Tiling function itself. |

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
tiling.SetSingleRange(1024, 1024, 1024, 1024, 1024, 1024); // Set the maximum and minimum values of singleCoreM/singleCoreN/singleCoreK.
tiling.SetOrgShape(1024, 1024, 1024);
tiling.SetBias(true);
tiling.SetBufferSpace(-1, -1, -1);
optiling::TCubeTiling tilingData;
int ret = tiling.GetTiling(tilingData);
```
