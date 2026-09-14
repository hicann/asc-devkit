# SetCType

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-09T07:22:40.752Z pushedAt=2026-09-12T09:55:18.194Z -->

## Description

Sets the position, data format, data type, and other information of the C matrix. These settings must be consistent with those on the kernel side.

## Function Prototype

```
int32_t SetCType(TPosition pos, CubeFormat type, DataType dataType)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **pos** | Input | Buffer position of matrix C, which can be set to TPosition::GM and TPosition::VECIN. |
| **type** | Input | Data format of matrix C, which can be set to CubeFormat::ND, CubeFormat::NZ, and CubeFormat::ND_ALIGN. |
| **dataType** | Input | Data type of matrix C, which can be set to DataType::DT_FLOAT, DataType::DT_FLOAT16, DataType::DT_BFLOAT16, DataType::DT_INT8, DataType::DT_INT32, DataType::DT_FLOAT8_E4M3FN, and DataType::DT_HIFLOAT8. |

## Return Value

**-1** indicates that the setting fails; **0** indicates that the setting succeeds.

## Constraints

None

## Examples

```
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
matmul_tiling::MatmulApiTiling tiling(ascendcPlatform);

// Set the C matrix with the buffer at GM, data format ND, data type float, and no transpose by default.
tiling.SetCType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
```
