# SetInputType

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-05T14:38:12.339Z -->

## Description

Sets the position, data format, and data type information of the feature matrix **Input**. These settings must be consistent with those on the Kernel side.

## Prototype

```
void SetInputType(ConvCommonApi::TPosition pos, ConvCommonApi::ConvFormat format, ConvCommonApi::ConvDtype dtype)
```

## Parameters

**Table 1**  Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| pos | Input | [Position](../../../general_description_and_constraints.md#table07372185712) of **Input** in memory. Currently, only **TPosition::GM** is supported. |
| format | Input | Data format of **Input**. Currently, only **ConvFormat::NDC1HWC0** is supported. |
| dtype | Input | Data type of **Input**. Currently, only **ConvDtype::FLOAT16** and **ConvDtype::BF16** are supported. |

## Return Value

None

## Constraints

None

## Examples

```
auto ascendcPlatform = platform_ascendc::PlatformAscendCManager::GetInstance();
ConvBackpropApi::Conv3DBpInputTiling conv3DBpDxTiling(*ascendcPlatform);
conv3DBpDxTiling.SetInputType(ConvCommonApi::TPosition::GM,
                                 ConvCommonApi::ConvFormat::NDC1HWC0,
                                 ConvCommonApi::ConvDtype::FLOAT16);
```

