# WaitIterateBatch

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-09T07:09:38.657Z pushedAt=2026-09-12T09:55:18.162Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- AI Core of Atlas inference products: Not supported
- Vector Core of Atlas inference products: Not supported
- Atlas training products: Not supported

## Description

Waits for the [IterateBatch](IterateBatch.md) asynchronous interface or the [IterateNBatch](IterateNBatch.md) asynchronous interface to return, and supports continuous output to Global Memory.

## Function Prototype

```
__aicore__ inline void WaitIterateBatch()
```

## Parameter Description

None

## Return Value

None

## Constraints

-   Used together with the [IterateBatch](IterateBatch.md) or [IterateNBatch](IterateNBatch.md) asynchronous interface.
-   Only continuous output to Global Memory is supported.
-   When MixDualMaster (dual-master mode) is enabled, that is, when the template parameter [enableMixDualMaster](MatmulConfig.md#matmulconfig-params) is set to **true**, this API is not supported.

## Examples

```
AscendC::Matmul<aType, bType, cType, biasType> mm;
mm.SetTensorA(gm_a[offsetA]);
mm.SetTensorB(gm_b[offsetB]);
if (tiling.isBias) {
    mm.SetBias(gm_bias[offsetBias]);
}
mm.IterateBatch(gm_c[offsetC], batchA, batchB, false);
// do some other compute tasks
mm.WaitIterateBatch(); // Wait for IterateBatch to complete.
```
