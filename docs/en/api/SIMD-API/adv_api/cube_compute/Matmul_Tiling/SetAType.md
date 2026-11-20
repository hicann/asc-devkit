# SetAType

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-02T07:08:16.551Z pushedAt=2026-11-18T04:55:01.825Z nl=2 -->

## Description

Sets the position, data format, data type, and transposition flag of cube A. These settings must be consistent with those on the kernel side.

## Prototype

```
int32_t SetAType(TPosition pos, CubeFormat type, DataType dataType, bool isTrans = false)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| pos | Input | Buffer position of cube A. Can be set to **TPosition::GM**, **TPosition::VECOUT**, or **TPosition::TSCM**. |
| type | Input | Data format of cube A. Can be set to **CubeFormat::ND**, **CubeFormat::NZ**, or **CubeFormat::VECTOR**. |
| dataType | Input | Data type of cube A. Can be set to **DataType::DT_FLOAT**, **DataType::DT_FLOAT16**, **DataType::DT_BFLOAT16**, **DataType::DT_INT8**, **DataType::DT_INT4**, **DataType::DT_FLOAT8_E4M3FN**, **DataType::DT_FLOAT8_E5M2**, or **DataType::DT_HIFLOAT8**. |
| isTrans | Input | Whether cube A is transposed.<br><br>Value range:<br>**true**: Cube A is transposed; **false**: Cube A is not transposed.<!-- npu="310p" id1 --><br><br>On the AI Core of Atlas inference products, transposition is not supported when cube A is of the **DataType::DT_INT8** data type, that is, setting this parameter to **true** is not supported.<!-- end id1 --> |

## Return Value

-1 indicates that the setting failed; 0 indicates that the setting succeeded.

## Constraints

None

## Examples

```
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
matmul_tiling::MatmulApiTiling tiling(ascendcPlatform);
// Set cube A with the buffer position GM, data format ND, data type bfloat16, and no transposition by default.
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
