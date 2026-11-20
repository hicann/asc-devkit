# SetInputType

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-11-10T06:26:52.634Z pushedAt=2026-11-18T11:07:26.633Z nl=2 -->

## Description

Sets the position, data format, and data type of the feature matrix Input. These settings must be consistent with those on the kernel-side.

## Prototype

```
void SetInputType(ConvCommonApi::TPosition pos, ConvCommonApi::ConvFormat format, ConvCommonApi::ConvDtype dtype)
```

## Parameters

**Table 1**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| pos | Input | [Position](../../../general_description_and_constraints.md#table07372185712) of Input in memory. Currently, only TPosition::GM is supported. |
| format | Input | Data format of Input. Currently, only ConvFormat::NDC1HWC0 is supported. |
| dtype | Input | Data type of Input. Currently, only ConvDtype::FLOAT16 and ConvDtype::BF16 are supported. |

## Return Value

None

## Constraints

None

## Examples

```
optiling::Conv3DBackpropFilterTilingData tilingData;
auto ascendcPlatform = platform_ascendc::PlatformAscendCManager::GetInstance();
ConvBackpropApi::Conv3dBpFilterTiling conv3dBpDwTiling(*ascendcPlatform);
conv3dBpDwTiling.SetInputType(
    ConvCommonApi::TPosition::GM, ConvCommonApi::ConvFormat::NDC1HWC0, ConvCommonApi::ConvDtype::FLOAT16);
```
