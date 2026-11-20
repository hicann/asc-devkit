# WaitGetTensorC

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-02T06:59:05.819Z pushedAt=2026-11-18T05:00:35.140Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- AI Core of Atlas inference products: Not supported
- Vector Core of Atlas inference products: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/WaitGetTensorC_res.md#id1 -->

## Description

When the **GetTensorC** asynchronous API is used to copy the result cube from GM to the Unified Buffer (UB), and the UB subsequently requires Vector computation, **WaitGetTensorC** must be called for synchronization.

## Prototype

```
__aicore__ inline void WaitGetTensorC()
```

## Parameters

None

## Return Value

None

## Constraints

When MixDualMaster (dual-master mode) is enabled, that is, when the template parameter [enableMixDualMaster](MatmulConfig.md#matmulconfig-params) is set to true, this API is not supported.

## Examples

```
// Asynchronous mode example.
mm.template Iterate<false>();
// Other operations.
for (int i = 0; i < singleCoreM/baseM * singleCoreN/baseN; ++i) {
    mm.template GetTensorC<false>(ubCmatrix);
    mm.WaitGetTensorC();
    // Vector operation.
}
```
