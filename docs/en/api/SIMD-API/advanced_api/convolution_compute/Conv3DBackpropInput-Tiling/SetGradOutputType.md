# SetGradOutputType

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-05T14:37:19.547Z -->

## Description

Sets the position, data format, and data type of **GradOutput**. These settings must be consistent with those on the Kernel side.

## Prototype

```
void SetGradOutputType(ConvCommonApi::TPosition pos, ConvCommonApi::ConvFormat format, ConvCommonApi::ConvDtype dtype)
```

## Parameters

**Table 1**  Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| pos | Input | [Position](../../../general_description_and_constraints.md#table07372185712) of **GradOutput** in memory. Currently, only **TPosition::GM** is supported. |
| format | Input | Data format of **GradOutput**. Currently, only **ConvFormat::NDC1HWC0** is supported. |
| dtype | Input | Data type of **GradOutput**. Currently, only **ConvDtype::FLOAT16** and **ConvDtype::BF16** are supported. |

## Return Value

None

## Constraints

None

## Examples

```
auto ascendcPlatform = platform_ascendc::PlatformAscendCManager::GetInstance();
ConvBackpropApi::Conv3DBpInputTiling conv3DBpDxTiling(*ascendcPlatform);
conv3DBpDxTiling.SetGradOutputType(ConvCommonApi::TPosition::GM,
                                   ConvCommonApi::ConvFormat::NDC1HWC0,
                                   ConvCommonApi::ConvDtype::FLOAT16);
```

