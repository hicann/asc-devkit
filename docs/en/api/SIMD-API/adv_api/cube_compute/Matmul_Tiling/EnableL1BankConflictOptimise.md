# EnableL1BankConflictOptimise

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-11-02T07:01:33.972Z pushedAt=2026-11-18T04:54:33.777Z nl=2 -->

## Description

Based on the Tiling parameters calculated by the [GetTiling](GetTiling.md) API, determines whether L1 Bank conflict optimization can be enabled. If this feature can be enabled, use it together with the TilingKey mechanism: by adding a TilingKey, associating the Host-side and kernel (Kernel)-side implementations, and adding a code implementation branch on the kernel (Kernel) side, set [enableL1BankConflictOptimise](../Matmul_Kernel/MatmulConfig.md#p84588523128) in MatmulConfig to true. This optimizes Bank conflicts on L1.

## Prototype

```
bool EnableL1BankConflictOptimise()
```

## Parameters

None

## Return Value

-   false: The kernel (Kernel) side cannot enable L1 Bank conflict optimization.
-   true: The kernel (Kernel) side can enable L1 Bank conflict optimization.

## Constraints

Call this API using the created Tiling object, and call it only after the Tiling calculation ([GetTiling](GetTiling.md)) is complete.

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
// Whether L1 Bank conflict optimization can be enabled on the kernel side. It can be used together with the TilingKey mechanism.
bool enableL1BankConflictOptimise = tiling.EnableL1BankConflictOptimise();
```

