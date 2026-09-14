# SetAType

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-09T07:17:43.027Z pushedAt=2026-09-12T09:55:18.182Z -->

## Description

Sets the position, data format, data type, and transpose attribute of matrix A. These settings must be consistent with those on the kernel side.

## Prototype

```
int32_t SetAType(TPosition pos, CubeFormat type, DataType dataType, bool isTrans = false)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| pos | Input | Buffer position of matrix A. It can be set to **TPosition::GM**, **TPosition::VECOUT**, or **TPosition::TSCM**. |
| type | Input | Data format of matrix A. It can be set to **CubeFormat::ND**, **CubeFormat::NZ**, or **CubeFormat::VECTOR**. |
| dataType | Input | Data type of matrix A. It can be set to **DataType::DT_FLOAT**, **DataType::DT_FLOAT16**, **DataType::DT_BFLOAT16**, **DataType::DT_INT8**, **DataType::DT_INT4**, **DataType::DT_FLOAT8_E4M3FN**, **DataType::DT_FLOAT8_E5M2**, or **DataType::DT_HIFLOAT8**. |
| isTrans | Input | Whether matrix A is transposed.<br><br>Value range:<br>**true**: Matrix A is transposed; **false**: Matrix A is not transposed.<br><br>On the AI Core of Atlas inference products, transposition is not supported when matrix A is of the **DataType::DT_INT8** data type, that is, setting this parameter to **true** is not supported. |

## Return Value

-1 indicates a setting failure; 0 indicates success.

## Constraints

None

## Examples

```
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
matmul_tiling::MatmulApiTiling tiling(ascendcPlatform);
// Set matrix A with the buffer at GM, data format ND, data type bfloat16, and no transposition by default.
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
```
