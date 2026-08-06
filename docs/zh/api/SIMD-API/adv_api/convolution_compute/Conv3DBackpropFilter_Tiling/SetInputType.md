# SetInputType

## 功能说明

设置特征矩阵Input的位置、数据格式、数据类型信息，这些信息必须与Kernel侧的设置保持一致。

## 函数原型

```
void SetInputType(ConvCommonApi::TPosition pos, ConvCommonApi::ConvFormat format, ConvCommonApi::ConvDtype dtype)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| pos | 输入 | Input在内存上的[位置](../../../通用说明和约束.md#table07372185712)。当前仅支持TPosition::GM。 |
| format | 输入 | Input的数据格式。当前仅支持ConvFormat::NDC1HWC0。 |
| dtype | 输入 | Input的数据类型。当前仅支持ConvDtype::FLOAT16、ConvDtype::BF16。 |

## 返回值说明

无

## 约束说明

无

## 调用示例

```
optiling::Conv3DBackpropFilterTilingData tilingData;
auto ascendcPlatform = platform_ascendc::PlatformAscendCManager::GetInstance();
ConvBackpropApi::Conv3dBpFilterTiling conv3dBpDwTiling(*ascendcPlatform);
conv3dBpDwTiling.SetInputType(
    ConvCommonApi::TPosition::GM, ConvCommonApi::ConvFormat::NDC1HWC0, ConvCommonApi::ConvDtype::FLOAT16);
```
