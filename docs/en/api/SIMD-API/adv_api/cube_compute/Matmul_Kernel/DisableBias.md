# DisableBias

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-08T12:15:03.025Z pushedAt=2026-10-27T08:17:56.935Z nl=2 -->

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
- Atlas 200I/500 A2 inference products: Supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products AI Core: Supported
- Vector Core of Atlas inference products: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/DisableBias_res.md#id1 -->

## Description

Clears the Bias flag, indicating that no Bias is involved in the Matmul computation. If the **isBias** parameter in the **TCubeTiling** structure is configured during initialization to enable Bias, calling this API clears the Bias flag and disables Bias.

## Prototype

```
__aicore__ inline void DisableBias()
```

## Parameters

None

## Return Value

None

## Constraints

None

## Examples

```
REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);
mm.SetTensorA(gm_a);
mm.SetTensorB(gm_b);
mm.DisableBias();    // Clear the Bias flag in tiling.
mm.IterateAll(gm_c);
mm.End();
```
