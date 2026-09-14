# SetSingleRange

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-09T07:28:28.767Z pushedAt=2026-09-12T09:55:18.209Z -->

## Description

Sets the maximum and minimum values of **singleCoreM**/**singleCoreN**/**singleCoreK**.

## Prototype

```
int32_t SetSingleRange(int32_t maxM = -1, int32_t maxN = -1, int32_t maxK = -1, int32_t minM = -1, int32_t minN = -1, int32_t minK = -1)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| maxM | Input | Sets the maximum **singleCoreM** value. The default value is -1, which means that no maximum **singleCoreM** value is specified and the value is computed by the Tiling function. |
| maxN | Input | Sets the maximum **singleCoreN** value. The default value is -1, which means that no maximum **singleCoreN** value is specified and the value is computed by the Tiling function. |
| maxK | Input | Sets the maximum **singleCoreK** value. The default value is -1, which means that no maximum **singleCoreK** value is specified and the value is computed by the Tiling function. |
| minM | Input | Sets the minimum **singleCoreM** value. The default value is -1, which means that no minimum **singleCoreM** value is specified and the value is computed by the Tiling function. |
| minN | Input | Sets the minimum **singleCoreN** value. The default value is -1, which means that no minimum **singleCoreN** value is specified and the value is computed by the Tiling function. |
| minK | Input | Sets the minimum **singleCoreK** value. The default value is -1, which means that no minimum **singleCoreK** value is specified and the value is computed by the Tiling function. |

## Return Value

Returns **-1** on failure and **0** on success.

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
tiling.SetSingleRange(1024, 1024, 1024, 1024, 1024, 1024);  // Set the maximum and minimum values of singleCoreM/singleCoreN/singleCoreK.
tiling.SetOrgShape(1024, 1024, 1024);
tiling.SetBias(true);
tiling.SetBufferSpace(-1, -1, -1);
optiling::TCubeTiling tilingData;
int ret = tiling.GetTiling(tilingData);
```

