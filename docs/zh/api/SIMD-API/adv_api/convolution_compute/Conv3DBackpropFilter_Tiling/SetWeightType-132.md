# SetWeightType

## 功能说明

设置权重矩阵Weight的位置、数据格式、数据类型信息，这些信息必须与Kernel侧的设置保持一致。

## 函数原型

```
void SetWeightType(ConvCommonApi::TPosition pos, ConvCommonApi::ConvFormat format, ConvCommonApi::ConvDtype dtype)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| pos | 输入 | Weight在内存上的[位置](../../../通用说明和约束.md#table07372185712)。当前仅支持TPosition::GM。 |
| format | 输入 | Weight的数据格式。当前仅支持ConvFormat::FRACTAL_Z_3D。 |
| dtype | 输入 | Weight的数据类型。当前仅支持ConvDtype::FLOAT32。 |

## 返回值说明

无

## 约束说明

无

## 调用示例

```
auto ascendcPlatform = platform_ascendc::PlatformAscendCManager::GetInstance();
ConvBackpropApi::Conv3dBpFilterTiling conv3dBpDwTiling(*ascendcPlatform);
conv3dBpDwTiling.SetWeightType(
    ConvCommonApi::TPosition::GM, ConvCommonApi::ConvFormat::FRACTAL_Z_3D, ConvCommonApi::ConvDtype::FLOAT32);
```
