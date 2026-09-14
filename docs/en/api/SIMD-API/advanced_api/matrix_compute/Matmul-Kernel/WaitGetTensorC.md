# WaitGetTensorC

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-09T07:08:05.253Z pushedAt=2026-09-12T09:55:18.157Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference product: Not supported
- AI Core of Atlas inference products: Not supported
- Vector Core of Atlas inference products: Not supported
- Atlas training products: Not supported

## Description

When the asynchronous **GetTensorC** API is used to copy the result matrix from GM to UB, and Vector computation is subsequently required on UB, call **WaitGetTensorC** for synchronization.

## Function Prototype

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
    // Vector operations.
}
```
