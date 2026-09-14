# GetBaseN

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-09T07:13:28.973Z pushedAt=2026-09-12T09:55:18.169Z -->

## Description

Obtains the **baseN** value computed by Tiling. For details about the **baseN** parameter, see [Table 1](tcubetiling_structure.md#tcubetiling-struct).

## Prototype

```
int32_t GetBaseN() const
```

## Parameters

None

## Return Value

Returns the baseN value obtained through Tiling computation.

## Constraints

Call this API using the created Tiling object, and call it only after the Tiling computation is complete ([GetTiling](GetTiling.md)).

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
int baseN = tiling.GetBaseN();  // Obtain the baseN computed by Tiling.
```

