# SetWeightType

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-05T14:28:47.691Z -->

## Description

Sets the position, data format, and data type of the weight matrix **Weight**. These settings must be consistent with those on the Kernel side.

## Prototype

```
void SetWeightType(ConvCommonApi::TPosition pos, ConvCommonApi::ConvFormat format, ConvCommonApi::ConvDtype dtype)
```

## Parameters

**Table 1**  Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| pos | Input | [Position](../../../general_description_and_constraints.md#table07372185712) of **Weight** in memory. Currently, only **TPosition::GM** is supported. |
| format | Input | Data format of **Weight**. Currently, only **ConvFormat::FRACTAL_Z_3D** is supported. |
| dtype | Input | Data type of **Weight**. Currently, only **ConvDtype::FLOAT32** is supported. |

## Return Value

None

## Constraints

None

## Examples

```
auto ascendcPlatform = platform_ascendc::PlatformAscendCManager::GetInstance();
ConvBackpropApi::Conv3dBpFilterTiling conv3dBpDwTiling(*ascendcPlatform);
conv3dBpDwTiling.SetWeightType(ConvCommonApi::TPosition::GM,
                                   ConvCommonApi::ConvFormat::FRACTAL_Z_3D,
                                   ConvCommonApi::ConvDtype::FLOAT32);
```

