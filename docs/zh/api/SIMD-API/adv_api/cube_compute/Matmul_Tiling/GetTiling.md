# GetTiling

## 功能说明

获取Tiling参数。

## 函数原型

```
int64_t GetTiling(optiling::TCubeTiling& tiling)
```

```
int64_t GetTiling(AscendC::tiling::TCubeTiling& tiling)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| tiling | 输出 | Tiling结构体存储最终的tiling结果。TCubeTiling结构介绍请参考[表1](TCubeTiling_struct.md#tcubetiling-struct)。<br>optiling::TCubeTiling：带有optiling命名空间的TCubeTiling结构体，该结构体为Host侧定义的Matmul TilingData。<br>AscendC::tiling::TCubeTiling：带有AscendC::tiling命名空间的TCubeTiling结构体，核函数（Kernel）侧定义的Matmul TilingData，与使用标准C++语法定义TilingData结构体的开发方式配合使用。 |

## 返回值说明

如果返回值不为-1，则代表Tiling计算成功，用户可以使用该Tiling结构的值。如果返回值为-1，则代表Tiling计算失败，该Tiling结果无法使用。

## 约束说明

在Tiling计算失败的场景，若需查看Tiling计算失败的原因，请将日志级别设置为WARNING级别，并在日志中搜索关键字“MatmulApi Tiling”。在Tiling计算成功的场景，若需查看Tiling结构体的参数值，请将日志级别设置为INFO级别，并在日志中搜索关键字“MatmulTiling”。

## 调用示例

```
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
matmul_tiling::MatmulApiTiling tiling(ascendcPlatform);
tiling.SetAType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
tiling.SetBType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
tiling.SetCType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
tiling.SetBiasType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
tiling.SetShape(1024, 1024, 1024);
tiling.SetOrgShape(1024, 1024, 1024);
tiling.SetBias(true);
tiling.SetBufferSpace(-1, -1, -1);
optiling::TCubeTiling tilingData;
int ret = tiling.GetTiling(tilingData); // 获取Tiling参数
```
