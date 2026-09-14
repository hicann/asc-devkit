# SetBatchNum

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-09T06:54:50.998Z pushedAt=2026-09-12T09:55:18.129Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- AI Core of Atlas inference products: Supported
- Vector Core of Atlas inference products: Not supported
- Atlas training products: Not supported

## Description

Resets the batch number for multi-batch computation without changing the Tiling.

## Prototype

```
__aicore__ inline void SetBatchNum(int32_t batchA, int32_t batchB)
```

## Parameters

**Table 1** Parameters
 <a name="table9646134355611"></a>

| Parameter | Input/Output | Description |
| --- | --- | --- |
| batchA | Input | Number of batches of matrix A set for a single computation. |
| batchB | Input | Number of batches of matrix B set for a single computation. |

## Return Value

None

## Constraints

-   When MixDualMaster (dual-master mode) is enabled, that is, when the template parameter [enableMixDualMaster](MatmulConfig.md#matmulconfig-params) is set to true, this API is not supported.
-   This API can only be called in pure Cube mode (matrix computation only).

## Examples

```
//  Pure Cube mode
#define ASCENDC_CUBE_ONLY
typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, AType, false, LayoutMode::NORMAL> aType;
typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, BType, false, LayoutMode::NORMAL> bType;
typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, CType, false, LayoutMode::NORMAL> cType;
typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, BiasType> biasType;
AscendC::Matmul<aType, bType, cType, biasType> mm1;
mm1.SetTensorA(gm_a, isTransposeAIn);
mm1.SetTensorB(gm_b, isTransposeBIn);
if(tiling.isBias) {
    mm1.SetBias(gm_bias);
}
mm1.SetBatchNum(batchA, batchB);
// Multi-batch Matmul computation
mm1.IterateBatch(gm_c, false, 0, false);
```
