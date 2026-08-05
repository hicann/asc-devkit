# SetWeightType

## 功能说明

设置Weight在内存上的位置、数据格式和数据类型。

## 函数原型

```
void SetWeightType(const ConvCommonApi::TPosition pos, const ConvCommonApi::ConvFormat format, const ConvCommonApi::ConvDtype dtype)
```

## 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| pos | 输入 | Weight在内存上的[位置](../../../通用说明和约束.md#table07372185712)。当前仅支持TPosition::GM。 |
| format | 输入 | Weight的数据格式。当前仅支持ConvFormat::FRACTAL_Z_3D。 |
| dtype | 输入 | Weight的数据类型。当前仅支持ConvDtype::FLOAT16、ConvDtype::BF16。 |

## 返回值说明

无

## 约束说明

在调用GetTiling接口前，本接口可选调用。若未调用本接口，默认Weight为pos=TPosition::GM，format=ConvFormat::FRACTAL\_Z\_3D，dtype=ConvDtype::FLOAT16。

## 调用示例

```
// 实例化Conv3D Api
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
Conv3dTilingApi::Conv3dTiling conv3dApiTiling(ascendcPlatform);
conv3dApiTiling.SetWeightType(
    ConvCommonApi::TPosition::GM, ConvCommonApi::ConvFormat::FRACTAL_Z_3D, ConvCommonApi::ConvDtype::BF16);
```

