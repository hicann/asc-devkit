# SetWeightType

<!-- md-trans-meta sourceCommit=6b81d2e6322744fc9adc8665d94bfafab36b3550 translatedAt=2026-11-10T06:24:23.841Z pushedAt=2026-11-18T11:07:26.578Z nl=2 -->

## Description

Sets the position, data format, and data type of Weight in memory.

## Prototype

```
void SetWeightType(const ConvCommonApi::TPosition pos, const ConvCommonApi::ConvFormat format, const ConvCommonApi::ConvDtype dtype)
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| pos | Input | [Position](../../../general_description_and_constraints.md#table07372185712) of Weight in memory. Currently, only TPosition::GM is supported. |
| format | Input | Data format of Weight. Currently, only ConvFormat::FRACTAL_Z_3D is supported. |
| dtype | Input | Data type of Weight. Currently, only ConvDtype::FLOAT16 and ConvDtype::BF16 are supported. |

## Return Value

None

## Constraints

This API is optional and can be called before the GetTiling API. If this API is not called, Weight defaults to pos=TPosition::GM, format=ConvFormat::FRACTAL\_Z\_3D, and dtype=ConvDtype::FLOAT16.

## Examples

```
// Instantiate the Conv3D API.
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
Conv3dTilingApi::Conv3dTiling conv3dApiTiling(ascendcPlatform);
conv3dApiTiling.SetWeightType(
    ConvCommonApi::TPosition::GM, ConvCommonApi::ConvFormat::FRACTAL_Z_3D, ConvCommonApi::ConvDtype::BF16);
```

