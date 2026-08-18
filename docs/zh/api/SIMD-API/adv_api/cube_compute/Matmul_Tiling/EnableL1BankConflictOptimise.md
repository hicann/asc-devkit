# EnableL1BankConflictOptimise

## 功能说明

根据[GetTiling](GetTiling.md)接口计算出的Tiling参数，获取是否可以开启L1 Bank冲突优化功能。若可以开启该功能，则与TilingKey机制配合使用，通过增加TilingKey，关联Host侧与核函数（Kernel）侧实现，并在核函数（Kernel）侧增加代码实现分支，将MatmulConfig中的[enableL1BankConflictOptimise](../Matmul_Kernel/MatmulConfig.md#p84588523128)设置为true，即可优化L1上的Bank冲突。

## 函数原型

```
bool EnableL1BankConflictOptimise()
```

## 参数说明

无

## 返回值说明

-   false：核函数（Kernel）侧不能开启L1 Bank冲突优化。
-   true：核函数（Kernel）侧可以开启L1 Bank冲突优化。

## 约束说明

使用创建的Tiling对象调用本接口，且需在完成Tiling计算（[GetTiling](GetTiling.md)）后调用本接口。

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
int ret = tiling.GetTiling(tilingData);
// 核函数（Kernel）侧是否可以开启L1 Bank冲突优化，可与TilingKey机制结合使用
bool enableL1BankConflictOptimise = tiling.EnableL1BankConflictOptimise();
```

