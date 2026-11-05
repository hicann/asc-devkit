# WaitIterateBatch

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-08T13:03:03.305Z pushedAt=2026-10-27T11:34:09.233Z nl=2 -->

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
- Atlas inference products AI Core: Not supported
- Vector Core of Atlas inference products: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/WaitIterateBatch_res.md#id1 -->

## Description

Waits for the [IterateBatch](IterateBatch.md) asynchronous API or the [IterateNBatch](IterateNBatch.md) asynchronous API to return, and supports continuous output to Global Memory.

## Prototype

```
__aicore__ inline void WaitIterateBatch()
```

## Parameters

None

## Return Value

None

## Constraints

-   Used together with the [IterateBatch](IterateBatch.md) or [IterateNBatch](IterateNBatch.md) asynchronous API.
-   Only continuous output to Global Memory is supported.
-   When MixDualMaster (dual-master mode) is enabled, that is, when the template parameter [enableMixDualMaster](MatmulConfig.md#matmulconfig-params) is set to true, this API is not supported.

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
