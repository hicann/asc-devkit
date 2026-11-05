# End

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-08T12:15:43.475Z pushedAt=2026-10-27T08:18:04.302Z nl=2 -->

## Applicable Products

<!-- npu="950" id3 -->
- Ascend 950PR&950DT products: Supported
<!-- end id3 -->
<!-- npu="A3" id4 -->
- Atlas A3 products: Supported
<!-- end id4 -->
<!-- npu="910b" id5 -->
- Atlas A2 products: Supported
<!-- end id5 -->
<!-- npu="310b" id6 -->
- Atlas 200I/500 A2 inference products: Supported
<!-- end id6 -->
<!-- npu="310p" id7 -->
- Atlas inference products AI Core: Supported
- Vector Core of Atlas inference products: Not supported
<!-- end id7 -->
<!-- npu="910" id8 -->
- Atlas training products: Not supported
<!-- end id8 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/End_res.md#id1 -->

## Description

When switching computation between multiple **Matmul** objects, the **End** function must be called once to release the Matmul computation resources and prevent computation resource conflicts among multiple Matmul objects.

## Prototype

```
__aicore__ inline void End()
```

## Parameters

None

## Return Value

None

## Constraints

None

## Examples

```
mm1.IterateAll(gm_c1);
mm1.End();
mm2.IterateAll(gm_c2);
mm2.End();
```
