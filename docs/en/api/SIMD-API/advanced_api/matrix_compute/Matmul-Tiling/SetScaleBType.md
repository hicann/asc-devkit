# SetScaleBType

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-09T07:27:33.428Z pushedAt=2026-09-12T09:55:18.208Z -->

## Description

In the MxMatmul scenario, this API sets the position, data format, and transpose attribute of the scaleB matrix. These settings must be consistent with those on the Kernel side. If this API is not called, the scaleB matrix information remains consistent with the B matrix information set in [SetBType](SetBType.md).

## Prototype

```
int32_t SetScaleBType(TPosition scalePos, CubeFormat scaleType, bool isScaleTrans = true)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **scalePos** | Input | Logical memory position of the scaleB matrix.<br><br>For Ascend 950PR/Ascend 950DT, the scaleB matrix can be set to **TPosition::GM**, **TPosition::VECOUT**, or **TPosition::TSCM**. |
| **scaleType** | Input | Physical layout format of the scaleB matrix.<br><br>For Ascend 950PR/Ascend 950DT, the scaleB matrix can be set to **CubeFormat::ND** or **CubeFormat::NZ**. |
| **isScaleTrans**<a name="p2934103115919"></a> | Input | Whether the scaleB matrix is transposed. Supported values are as follows:<br>**true**: Default value, indicating that the scaleB matrix is transposed.<br>**false**: The scaleB matrix is not transposed. |

## Return Value

Returns **-1** if the setting fails; returns **0** if the setting succeeds.

## Constraints

None

## Examples

```
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
matmul_tiling::MatmulApiTiling tiling(ascendcPlatform);
tiling.SetAType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);

// Set the scaleB matrix with the buffer at GM, data format ND, and transposition enabled.
tiling.SetScaleBType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, true);
```
