# GetBaseN

<!-- md-trans-meta sourceCommit=ae444a8b0a91a144a05d68a7cd8d1d0e84475d04 translatedAt=2026-11-02T07:03:47.042Z pushedAt=2026-11-18T04:54:41.444Z nl=2 -->

## Description

Obtains the baseN value obtained from Tiling. For details about the baseN parameter, see [Table 1](TCubeTiling_struct.md#tcubetiling-struct).

## Prototype

```
int32_t GetBaseN() const
```

## Parameters

None

## Return Value

Returns the baseN value obtained from Tiling.

## Constraints

Call this API using the created **Tiling** object, and it must be called after the Tiling calculation is completed ([GetTiling](GetTiling.md)).

## Examples

```
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
matmul_tiling::MatmulApiTiling tiling(ascendcPlatform);
tiling.SetAType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
tiling.SetBType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
tiling.SetCType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
tiling.SetBiasType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
tiling.SetShape(1024, 1024, 1024);
tiling.SetOrgShape(1024, 1024, 1024);
tiling.SetBias(true);
tiling.SetBufferSpace(-1, -1, -1);

optiling::TCubeTiling tilingData;
int ret = tiling.GetTiling(tilingData);
int baseN = tiling.GetBaseN(); // Obtain the baseN calculated by Tiling.
```

