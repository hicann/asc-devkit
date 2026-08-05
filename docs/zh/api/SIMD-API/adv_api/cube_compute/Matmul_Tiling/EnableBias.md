# EnableBias

## 功能说明

设置Bias是否参与运算，设置的信息必须与Kernel侧保持一致。

## 函数原型

```
int32_t EnableBias(bool isBiasIn = false)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| isBiasIn | 输入 | 设置是否有Bias参与运算。<br><br>true：有Bias参与运算。<br><br>false：无Bias参与运算。 |

## 返回值说明

-1表示设置失败；0表示设置成功。

## 约束说明

无

## 调用示例

```
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
matmul_tiling::MatmulApiTiling tiling(ascendcPlatform);

tiling.EnableBias(true); // 设置Bias是否参与运算
```
