# WaitIterateAll

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-08T13:02:01.499Z pushedAt=2026-10-27T11:33:57.554Z nl=2 -->

## Applicable Products

<!-- npu="950" id2 -->
- Ascend 950PR&950DT products: Supported
<!-- end id2 -->
<!-- npu="A3" id3 -->
- Atlas A3 products: Supported
<!-- end id3 -->
<!-- npu="910b" id4 -->
- Atlas A2 products: Supported
<!-- end id4 -->
<!-- npu="310b" id5 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products AI Core: Not supported
- Vector Core of Atlas inference products: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/WaitIterateAll_res.md#id1 -->

## Description

Waits for the **IterateAll** asynchronous API to return, and supports continuous output to Global Memory.

## Prototype

```
__aicore__ inline void WaitIterateAll()
```

## Parameters

None

## Return Value

None

## Constraints

-   Used together with the [IterateAll](IterateAll.md) asynchronous API.
-   Only continuous output to Global Memory is supported.

## Examples

```
AscendC::Matmul<aType, bType, cType, biasType> mm;
mm.SetTensorA(gm_a[offsetA]);
mm.SetTensorB(gm_b[offsetB]);
if (tiling.isBias) {
    mm.SetBias(gm_bias[offsetBias]);
}
mm.template IterateAll<false>(gm_c[offsetC], 0, false, true);
// do some others compute
mm.WaitIterateAll(); // Wait for IterateAll to complete.
```
