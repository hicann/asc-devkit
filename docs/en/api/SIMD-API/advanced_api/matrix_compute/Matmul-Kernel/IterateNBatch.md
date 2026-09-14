# IterateNBatch

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-09T06:40:32.964Z pushedAt=2026-09-12T09:55:18.112Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- AI Core of Atlas inference products: Not supported
- Vector Core of Atlas inference products: Not supported
- Atlas training products: Not supported

## Description

Calling IterateNBatch once performs N IterateBatch computations, producing N multi-Batch C matrices of size singleCoreM \* singleCoreN. Before calling this API, set the [isNBatch](MatmulConfig.md#matmulconfig-params) parameter in MatmulConfig to true to enable multi-Batch input and multi-Batch output, and call the [SetWorkspace](SetWorkspace.md) API to request temporary space for caching computation results. That is, the results of IterateNBatch are output to the Global Memory specified by [SetWorkspace](SetWorkspace.md).

For the BSNGD, SBNGD, and BNGS1S2 Layout formats, before calling this API, use SetALayout/SetBLayout/SetCLayout/SetBatchNum in tiling to set the Layout axis information of A/B/C and the maximum BatchNum. For the Normal data format, use [SetBatchInfoForNormal](../Matmul-Tiling/SetBatchInfoForNormal.md) to set the M/N/K axis information of A/B/C and the BatchNum of the A/B matrices. When instantiating Matmul, set the Layout type through MatmulType. Currently three Layout types are supported: BSNGD, SBNGD, and BNGS1S2.

## Prototype

```
template <bool sync = true, bool waitIterateBatch = false>
__aicore__ inline void IterateNBatch(const uint32_t batchLoop, uint32_t batchA, uint32_t batchB, bool enSequentialWrite, const uint32_t matrixStrideA = 0, const uint32_t matrixStrideB = 0, const uint32_t matrixStrideC = 0)
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| sync | The process of obtaining the C matrix is divided into synchronous and asynchronous modes:<br>Synchronous: You need to synchronously wait for the execution completion of IterateNBatch, and then obtain the computation results output to Global Memory by yourself.<br>Asynchronous: You do not need to synchronously wait for the execution completion of IterateNBatch.<br><br>Use this parameter to set the synchronous or asynchronous mode: set to **true** for the synchronous mode and **false** for the asynchronous mode. The default is the synchronous mode. |
| waitIterateBatch | Whether to wait for the execution completion of IterateNBatch through the [WaitIterateBatch](WaitIterateBatch.md) API. This parameter is used only in asynchronous scenarios. The default is **false**.<br><br>**true**: Wait for the execution completion of IterateNBatch through the WaitIterateBatch API, and then obtain the computation results output to Global Memory by yourself.<br><br>**false**: Do not wait for the execution completion of IterateNBatch through the WaitIterateBatch API. After calling this API, call the [GetBatchTensorC](GetBatchTensorC.md) API to obtain the C matrix, or handle the process of waiting for the execution completion of IterateNBatch by yourself. |

**Table 2** API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| batchLoop | Input | Number of BMMs computed in the current computation. |
| batchA | Input | Number of batches of the left matrix computed in the current single BMM call. |
| batchB | Input | Number of batches of the right matrix computed in the current single BMM call. In brc scenarios, batchA and batchB are different. |
| enSequentialWrite | Input | Whether the output data is stored contiguously. |
| matrixStrideA | Input | Offset between the start addresses of adjacent nd matrices of the A matrix source operand. The default value is 0. |
| matrixStrideB | Input | Offset between the start addresses of adjacent nd matrices of the B matrix source operand. The default value is 0. |
| matrixStrideC | Input | This parameter is reserved. Keep the default value 0. |

## Return Value

None

## Constraints

-   Computation within a single BMM follows the constraints described earlier.
-   For the BSNGD, SBNGD, and BNGS1S2 layout formats, the total size of the multi-batch data of input matrices A and B must be smaller than the L1 Buffer size.
-   When MixDualMaster (dual-master mode) is enabled, that is, when the template parameter [enableMixDualMaster](MatmulConfig.md#matmulconfig-params) is set to true, this API is not supported.
-   When ND-format input is used and the [source matrix inner axis](IterateBatch.md#row_width) of matrix A or B within a single BMM is greater than 65535, set intrinsicsCheck to true in MatmulConfig. After this option is enabled, the API splits a single ND2NZ transfer from Global Memory to L1 Buffer into multiple loop transfers, thereby avoiding the parameter limit of a single transfer instruction.

## Examples

This example performs matrix multiplication on **aGM** and **bGM** and stores the result in **cGm**. The layout format of **aGM** is BSNGD, the layout format of **bGM** is BSNGD, and the layout format of **cGM** is BNGS1S2. The left matrix computes **batchA** SD data per iteration, and the right matrix computes **batchB** SD data per iteration.

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
