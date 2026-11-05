# IterateNBatch

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-08T12:33:08.605Z pushedAt=2026-10-27T11:26:49.569Z nl=2 -->

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
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id6 -->
<!-- npu="310p" id7 -->
- Atlas inference products AI Core: Not supported
- Vector Core of Atlas inference products: Not supported
<!-- end id7 -->
<!-- npu="910" id8 -->
- Atlas training products: Not supported
<!-- end id8 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/IterateNBatch_res.md#id1 -->

## Description

Calling **IterateNBatch** once performs N **IterateBatch** computations, producing N multi-Batch C cubes of size **singleCoreM** \* **singleCoreN**. Before calling this API, set the [isNBatch](MatmulConfig.md#matmulconfig-params) parameter in **MatmulConfig** to true to enable the multi-Batch input and multi-Batch output feature, and call the [SetWorkspace](SetWorkspace.md) API to request temporary space for caching computation results. That is, the results of **IterateNBatch** are output to the Global Memory specified by [SetWorkspace](SetWorkspace.md).

For the BSNGD, SBNGD, and BNGS1S2 Layout formats, before calling this API, use **SetALayout**/**SetBLayout**/**SetCLayout**/**SetBatchNum** in tiling to set the Layout axis information of A/B/C and the maximum BatchNum. For the Normal data format, use [SetBatchInfoForNormal](../Matmul_Tiling/SetBatchInfoForNormal.md) to set the M/N/K axis information of A/B/C and the BatchNum of the A/B cubes. When instantiating **Matmul**, set the Layout type through **MatmulType**. Currently, three Layout types are supported: BSNGD, SBNGD, and BNGS1S2.

## Prototype

```
template <bool sync = true, bool waitIterateBatch = false>
__aicore__ inline void IterateNBatch(const uint32_t batchLoop, uint32_t batchA, uint32_t batchB, bool enSequentialWrite, const uint32_t matrixStrideA = 0, const uint32_t matrixStrideB = 0, const uint32_t matrixStrideC = 0)
```

## Parameters

**Table 1** Template parameters

| Parameter | Description |
| --- | --- |
| sync | The process of obtaining the C cube is divided into synchronous and asynchronous modes:<br>Synchronous: It is necessary to synchronously wait for the execution completion of IterateNBatch, and then the developer obtains the computation result written to Global Memory.<br>Asynchronous: It is not necessary to synchronously wait for the execution completion of IterateNBatch.<br><br>This parameter sets the synchronous or asynchronous mode: set to **true** for synchronous mode; set to **false** for asynchronous mode. The default is synchronous mode. |
| waitIterateBatch | Whether to wait for the execution completion of IterateNBatch through the [WaitIterateBatch](WaitIterateBatch.md) API. This parameter is used only in asynchronous scenarios. The default is **false**.<br><br>**true**: It is necessary to wait for the execution completion of IterateNBatch through the WaitIterateBatch API, and then the developer obtains the computation result written to Global Memory.<br><br>**false**: It is not necessary to wait for the execution completion of IterateNBatch through the WaitIterateBatch API. After this API is called, call the [GetBatchTensorC](GetBatchTensorC.md) API to obtain the C cube, or the developer handles the process of waiting for the execution completion of IterateNBatch. |

**Table 2** API parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| batchLoop | Input | Number of BMMs in the current computation. |
| batchA | Input | Number of batches of the left cube computed in the current single BMM call. |
| batchB | Input | Number of batches of the right cube computed in the current single BMM call. In the brc scenario, batchA and batchB are different. |
| enSequentialWrite | Input | Whether the output stores data contiguously. |
| matrixStrideA | Input | Offset between the start addresses of adjacent nd cubes of the A cube source operand. The default value is 0. |
| matrixStrideB | Input | Offset between the start addresses of adjacent nd cubes of the B cube source operand. The default value is 0. |
| matrixStrideC | Input | This parameter is reserved. Keep the default value 0. |

## Return Value

None

## Constraints

-   The computation within a single BMM follows the previous constraints.
-   For the BSNGD, SBNGD, and BNGS1S2 Layout formats, the total size of the multi-batch data of the input A and B cubes should be smaller than the size of the L1 Buffer.
-   When the MixDualMaster (dual-master mode) scenario is enabled, that is, when the template parameter [enableMixDualMaster](MatmulConfig.md#matmulconfig-params) is set to **true**, this API is not supported.
-   When ND-format input is used and the [source cube inner axis](IterateBatch.md#row_width) of the A or B cube within a single BMM is greater than 65535, set **intrinsicsCheck** to **true** in MatmulConfig. After this option is enabled, the API splits a single ND2NZ transfer operation from Global Memory to the L1 Buffer into multiple loop transfers, thereby avoiding the parameter of a single transfer instruction exceeding the limit.

<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/IterateNBatch_res.md#id2 -->

## Examples

Example function: Complete the cube multiplication of aGM and bGM, and save the result to cGm. The layout format of the aGM data is BSNGD, the layout format of the bGM data is BSNGD, and the layout format of cGM is BNGS1S2. The left cube computes batchA SD data each time, and the right cube computes batchB SD data each time.

```
// Create a Matmul instance.
AscendC::Matmul<aType, bType, cType, biasType> mm1;
AscendC::TPipe pipe;
g_cubeTPipePtr = &pipe;

REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm1);
mm1.Init(&tiling);
int g_lay = tiling.ALayoutInfoG > tiling.BLayoutInfoG ? tiling.ALayoutInfoG : tiling.BLayoutInfoG;
int for_extent = tiling.ALayoutInfoB * tiling.ALayoutInfoN * g_lay / tiling.BatchNum;
mm1.SetTensorA(gm_a[0], isTransposeAIn);
mm1.SetTensorB(gm_b[0], isTransposeBIn);
mm1.SetWorkspace(workspaceGM, 0);
if (tiling.isBias) {
    mm1.SetBias(gm_bias[0]);
}
// Perform multi-batch Matmul computation.
mm1.IterateNBatch(for_extent, batchA, batchB, false);
```
