# SetBiasType

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-09T07:20:20.171Z pushedAt=2026-09-12T09:55:18.188Z -->

## Description

Sets the position, data format, and data type of the **Bias**, which must be consistent with the settings on the kernel side.

## Prototype

```
int32_t SetBiasType(TPosition pos, CubeFormat type, DataType dataType)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| pos | Input | Buffer position of the Bias matrix. It can be set to **TPosition::GM**, **TPosition::VECOUT**, or **TPosition::TSCM**. |
| type | Input | Data format of the Bias matrix. It can be set to **CubeFormat::ND**. |
| dataType | Input | Data type of the Bias matrix. It can be set to **DataType::DT_FLOAT**, **DataType::DT_FLOAT16**, **DataType::DT_INT32**, or **DataType::DT_BFLOAT16**.<br><br>The data type of Bias can be set to **int32_t** only when the data types of both A and B are **int8_t**. |

## Return Value

-1 indicates that the setting failed; 0 indicates that the setting succeeded.

## Constraints

None

## Example

```
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
matmul_tiling::MatmulApiTiling tiling(ascendcPlatform);
tiling.SetBiasType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);  // Set the Bias matrix.
```
