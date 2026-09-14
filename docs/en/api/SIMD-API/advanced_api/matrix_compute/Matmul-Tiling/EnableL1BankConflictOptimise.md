# EnableL1BankConflictOptimise

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-09T07:11:39.273Z pushedAt=2026-09-12T09:55:18.165Z -->

## Feature Description

Based on the Tiling parameters calculated by the [GetTiling](GetTiling.md) interface, this function obtains whether the L1 Bank conflict optimization can be enabled. If this feature can be enabled, it is used in conjunction with the TilingKey mechanism: by adding a TilingKey to associate the Host-side and Kernel-side implementations, and by adding a code implementation branch on the Kernel side, set [enableL1BankConflictOptimise](../Matmul-Kernel/MatmulConfig.md#matmulconfig-params) in MatmulConfig to `true`, thereby optimizing the Bank conflicts on L1.

## Prototype

```
bool EnableL1BankConflictOptimise()
```

## Parameters

None

## Return Value

-   false: L1 Bank conflict optimization cannot be enabled on the Kernel side.
-   true: L1 Bank conflict optimization can be enabled on the Kernel side.

## Constraints

Call this API using the created Tiling object, and call it only after the Tiling computation is complete ([GetTiling](GetTiling.md)).

## Examples

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
// Whether L1 Bank conflict optimization can be enabled on the kernel side, which can be used together with the TilingKey mechanism.
bool enableL1BankConflictOptimise = tiling.EnableL1BankConflictOptimise();
```
