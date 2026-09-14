# GetBaseK

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-09T07:12:18.032Z pushedAt=2026-09-12T09:55:18.166Z -->

## Description

Obtains the **baseK** value computed by **Tiling**. For details about the **baseK** parameter, see [Table 1](tcubetiling_structure.md#tcubetiling-struct).

## Prototype

```
int32_t GetBaseK() const
```

## Parameters

None

## Return Value

Returns the **baseK** value obtained from Tiling computation.

## Constraints

Call this API using the created Tiling object, and call it only after the Tiling computation is completed ([GetTiling](GetTiling.md)).

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
int baseK = tiling.GetBaseK();  // Obtain the baseK computed by Tiling.
```

