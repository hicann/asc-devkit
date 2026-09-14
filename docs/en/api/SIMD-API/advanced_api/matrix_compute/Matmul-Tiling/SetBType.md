# SetBType

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-09T07:21:27.265Z pushedAt=2026-09-12T09:55:18.190Z -->

## Description

Sets the position, data format, data type, and transposition of matrix B. These settings must be consistent with those on the kernel side.

## Prototype

```
int32_t SetBType(TPosition pos, CubeFormat type, DataType dataType, bool isTrans = false)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| pos | Input | Buffer position of matrix B. The value can be **TPosition::GM**, **TPosition::VECOUT**, or **TPosition::TSCM**. |
| type | Input | Data format of matrix B. The value can be **CubeFormat::ND** or **CubeFormat::NZ**. |
| dataType | Input | Data type of matrix B. The value can be **DataType::DT_FLOAT**, **DataType::DT_FLOAT16**, **DataType::DT_BFLOAT16**, **DataType::DT_INT8**, **DataType::DT_INT4**, **DataType::DT_FLOAT8_E4M3FN**, **DataType::DT_FLOAT8_E5M2**, or **DataType::DT_HIFLOAT8**. |
| isTrans<a name="p47369411111"></a> | Input | Whether matrix B is transposed.<br><br>Value range:<br>**true**: Matrix B is transposed; **false**: Matrix B is not transposed. |

## Return Value

-1 indicates that the setting fails; 0 indicates that the setting succeeds.

## Constraints

None

## Examples

```
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
matmul_tiling::MatmulApiTiling tiling(ascendcPlatform);
// Set matrix B with the buffer at GM, data format ND, data type bfloat16, and no transposition by default.
tiling.SetBType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
```
