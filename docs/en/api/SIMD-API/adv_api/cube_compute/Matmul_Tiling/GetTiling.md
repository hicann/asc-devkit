# GetTiling

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-11-02T07:04:57.716Z pushedAt=2026-11-18T04:54:49.400Z nl=2 -->

## Description

Obtains the tiling parameters.

## Prototype

```
int64_t GetTiling(optiling::TCubeTiling& tiling)
```

```
int64_t GetTiling(AscendC::tiling::TCubeTiling& tiling)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| tiling | Output | Tiling structure that stores the final tiling result. For details about the TCubeTiling structure, see [Table 1](TCubeTiling_struct.md#tcubetiling-struct).<br>optiling::TCubeTiling: TCubeTiling structure with the optiling namespace, which is the Matmul TilingData defined on the host.<br>AscendC::tiling::TCubeTiling: TCubeTiling structure with the AscendC::tiling namespace, which is the Matmul TilingData defined on the kernel side and is used together with the development method that defines the TilingData structure using standard C++ syntax. |

## Return Value

If the return value is not -1, the tiling calculation succeeds and you can use the values in this tiling structure. If the return value is -1, the tiling calculation fails and the tiling result cannot be used.

## Constraints

If the tiling calculation fails and you need to view the cause, set the log level to WARNING and search for the keyword "MatmulApi Tiling" in the logs. If the tiling calculation succeeds and you need to view the parameter values of the tiling structure, set the log level to INFO and search for the keyword "MatmulTiling" in the logs.

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
int ret = tiling.GetTiling(tilingData); // Obtain the tiling parameters.
```
