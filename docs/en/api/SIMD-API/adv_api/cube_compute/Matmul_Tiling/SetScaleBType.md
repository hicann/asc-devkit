# SetScaleBType

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-02T07:16:52.567Z pushedAt=2026-11-18T04:55:42.147Z nl=2 -->

## Description

In the MxMatmul scenario, this API sets the position, data format, and transpose information of the scaleB cube. These settings must be consistent with those on the kernel function (Kernel) side. If this API is not called, the scaleB cube information remains consistent with the B cube information set in [SetBType](SetBType.md).

## Prototype

```
int32_t SetScaleBType(TPosition scalePos, CubeFormat scaleType, bool isScaleTrans = true)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| scalePos | Input | Memory logical position of the scaleB cube. <!-- npu="950" id1 --><br><br>For Ascend 950PR&950DT products, the scaleB cube can be set to **TPosition::GM**, **TPosition::VECOUT**, or **TPosition::TSCM**. <!-- end id1 --> |
| scaleType | Input | Physical layout format of the scaleB cube. <!-- npu="950" id2 --><br><br>For Ascend 950PR&950DT products, the scaleB cube can be set to **CubeFormat::ND** or **CubeFormat::NZ**. <!-- end id2 --> |
| isScaleTrans<a name="p2934103115919"></a> | Input | Whether the scaleB cube is transposed. Supported values are as follows:<br>**true**: Default value, the scaleB cube is transposed.<br>**false**: The scaleB cube is not transposed. |

## Return Value

-1 indicates that the setting failed; 0 indicates that the setting succeeded.

## Constraints

None

## Examples

```
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
matmul_tiling::MatmulApiTiling tiling(ascendcPlatform);
tiling.SetAType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);

// Set the scaleB cube with the buffer at GM, data format ND, and transposed.
tiling.SetScaleBType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, true);
```
