# GetTiling

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-09T07:15:26.685Z pushedAt=2026-09-12T09:55:18.175Z -->

## Description

Obtains the **Tiling** parameters.

## Prototype

```
int64_t GetTiling(optiling::TCubeTiling &tiling)
```

```
int64_t GetTiling(AscendC::tiling::TCubeTiling &tiling)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| tiling | Output | Tiling structure that stores the final tiling result. For details about the TCubeTiling structure, see [Table 1](tcubetiling_structure.md#tcubetiling-struct).<br>optiling::TCubeTiling: TCubeTiling structure with the optiling namespace, which is the Matmul TilingData defined on the host side.<br>AscendC::tiling::TCubeTiling: TCubeTiling structure with the AscendC::tiling namespace, which is the Matmul TilingData defined on the kernel side and is used together with the development method that defines the TilingData structure using standard C++ syntax. |

## Return Value

If the return value is not -1, the **Tiling** calculation succeeds, and you can use the values in the **Tiling** structure. If the return value is -1, it indicates a **Tiling** calculation failure, and the **Tiling** result cannot be used.

## Constraints

In the scenario of Tiling calculation failure, to view the cause of the Tiling calculation failure, set the log level to WARNING and search for the keyword "MatmulApi Tiling" in the logs. In the scenario of successful Tiling calculation, to view the parameter values of the Tiling structure, set the log level to INFO and search for the keyword "MatmulTiling" in the logs.

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
int ret = tiling.GetTiling(tilingData);  // Obtain the Tiling parameters.
```
