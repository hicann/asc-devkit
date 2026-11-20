# SetGradOutputType

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-11-10T06:29:06.078Z pushedAt=2026-11-18T11:07:26.712Z nl=2 -->

## Description

Sets the position, data format, and data type of GradOutput. These settings must be consistent with those on the kernel side.

## Prototype

```
void SetGradOutputType(ConvCommonApi::TPosition pos, ConvCommonApi::ConvFormat format, ConvCommonApi::ConvDtype dtype)
```

## Parameters

**Table 1**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| pos | Input | [Position](../../../general_description_and_constraints.md#table07372185712) of GradOutput in memory. Currently, only TPosition::GM is supported. |
| format | Input | Data format of GradOutput. Currently, only ConvFormat::NDC1HWC0 is supported. |
| dtype | Input | Data type of GradOutput. Currently, only ConvDtype::FLOAT16 and ConvDtype::BF16 are supported. |

## Return Value

None

## Constraints

None

## Examples

```
auto ascendcPlatform = platform_ascendc::PlatformAscendCManager::GetInstance();
ConvBackpropApi::Conv3DBpInputTiling conv3DBpDxTiling(*ascendcPlatform);
conv3DBpDxTiling.SetGradOutputType(
    ConvCommonApi::TPosition::GM, ConvCommonApi::ConvFormat::NDC1HWC0, ConvCommonApi::ConvDtype::FLOAT16);
```
