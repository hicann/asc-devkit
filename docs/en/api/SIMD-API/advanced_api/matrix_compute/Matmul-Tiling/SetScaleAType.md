# SetScaleAType

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-09T07:27:01.956Z pushedAt=2026-09-12T09:55:18.206Z -->

## Description

In the MxMatmul scenario, this API sets the position, data format, and transpose attribute of the **scaleA** matrix. These settings must be consistent with those on the Kernel side. If this API is not called, the **scaleA** matrix information remains consistent with that of the A matrix set in [SetAType](SetAType.md).

## Prototype

```
int32_t SetScaleAType(TPosition scalePos, CubeFormat scaleType, bool isScaleTrans = false)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| scalePos | Input | Memory logical position of the scaleA matrix.<br><br>For Ascend 950PR/Ascend 950DT, the scaleA matrix can be set to TPosition::GM, TPosition::VECOUT, and TPosition::TSCM. |
| scaleType | Input | Physical layout format of the scaleA matrix.<br><br>For Ascend 950PR/Ascend 950DT, the scaleA matrix can be set to CubeFormat::ND and CubeFormat::NZ. |
| isScaleTrans<a name="p2934103115919"></a> | Input | Whether the scaleA matrix is transposed. Supported values are as follows:<br>true: The scaleA matrix is transposed.<br>false: Default value. The scaleA matrix is not transposed. |

## Return Value

-1 indicates that the setting fails; 0 indicates that the setting succeeds.

## Constraints

None

## Examples

```
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
matmul_tiling::MatmulApiTiling tiling(ascendcPlatform);
tiling.SetAType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
// Set the scaleA matrix with the buffer at GM, ND data format, and no transpose.
tiling.SetScaleAType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, false);
```
