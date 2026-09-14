# SetOutputType

<!-- md-trans-meta sourceCommit=e59e1725727133ce76073dba21b022a1b0c9fd56 translatedAt=2026-09-05T14:15:36.330Z -->

## Description

Sets the position, data format, and data type of the result matrix **Output** in memory.

## Prototype

```
void SetOutputType(const ConvCommonApi::TPosition pos, const ConvCommonApi::ConvFormat format, const ConvCommonApi::ConvDtype dtype)
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| pos | Input | [Position](../../../general_description_and_constraints.md#table07372185712) of **Output** in memory. Currently, only **TPosition::CO1** is supported. |
| format | Input | Data format of **Output**. Currently, only **ConvFormat::NDC1HWC0** is supported. |
| dtype | Input | Data type of **Output**. Currently, only **ConvDtype::FLOAT16** and **ConvDtype::BF16** are supported. |

## Return Value

None

## Constraints

This API is optional and can be called before calling the **GetTiling** API. If this API is not called, the default values are pos=**TPosition::CO1**, format=**ConvFormat::NDC1HWC0**, and dtype=**ConvDtype::FLOAT16**.

## Examples

```
// Instantiate the Conv3D API.
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
Conv3dTilingApi::Conv3dTiling conv3dApiTiling(ascendcPlatform);
conv3dApiTiling.SetOutputType(ConvCommonApi::TPosition::CO1, ConvCommonApi::ConvFormat::NDC1HWC0, ConvCommonApi::ConvDtype::BF16);
```

