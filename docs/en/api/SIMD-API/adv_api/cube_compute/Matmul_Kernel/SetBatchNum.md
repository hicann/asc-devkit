# SetBatchNum

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-08T12:46:47.953Z pushedAt=2026-10-27T11:29:01.358Z nl=2 -->

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
- Atlas inference products AI Core: Supported
- Vector Core of Atlas inference products: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/SetBatchNum_res.md#id1 -->

## Description

Resets the batch count for multi-batch computation without changing the Tiling.

## Prototype

```
__aicore__ inline void SetBatchNum(int32_t batchA, int32_t batchB)
```

## Parameters

**Table 1**  Parameters
<a name="table9646134355611"></a>

| Parameter | Input/Output | Description |
| --- | --- | --- |
| batchA | Input | Batch count of cube A set for a single computation. |
| batchB | Input | Batch count of cube B set for a single computation. |

## Return Value

None

## Constraints

-   When MixDualMaster (dual-master mode) is enabled, that is, when the template parameter [enableMixDualMaster](MatmulConfig.md#matmulconfig-params) is set to true, this API is not supported.
-   This API can be called only in pure Cube mode (cube computation only).

## Examples

```
//  Pure Cube mode.
#define ASCENDC_CUBE_ONLY
typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, AType, false, LayoutMode::NORMAL> aType;
typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, BType, false, LayoutMode::NORMAL> bType;
typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, CType, false, LayoutMode::NORMAL> cType;
typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, BiasType> biasType;
AscendC::Matmul<aType, bType, cType, biasType> mm1;
mm1.SetTensorA(gm_a, isTransposeAIn);
mm1.SetTensorB(gm_b, isTransposeBIn);
if (tiling.isBias) {
    mm1.SetBias(gm_bias);
}
mm1.SetBatchNum(batchA, batchB);
// Multi-batch Matmul computation.
mm1.IterateBatch(gm_c, false, 0, false);
```
