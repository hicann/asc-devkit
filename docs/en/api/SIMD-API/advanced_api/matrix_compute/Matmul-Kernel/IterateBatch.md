# IterateBatch

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-09T06:39:55.745Z pushedAt=2026-09-12T09:55:18.115Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported

- Atlas A3 training products/Atlas A3 inference products: Supported

- Atlas A2 training products/Atlas A2 inference products: Supported

- Atlas 200I/500 A2 inference products: Not supported

- AI Core of Atlas inference products: Supported

- Vector Core of Atlas inference products: Not supported

- Atlas training products: Not supported

## Description

When the shape processed by a single Matmul computation is small, performance may be affected because each computation involves internal communication. This API provides the capability to process Matmul in batches. By calling **IterateBatch** once, you can compute multiple C matrices of **singleCoreM** \* **singleCoreN** size.

Before using this API, you need to understand some essential data layout formats:

-   **General data format (NORMAL)**: The BMNK data layout format. B: Batch, the batch size. M, N, and K are the matrix dimensions of the matrix multiplication \[M, K\]\*\[K, N\]. The data layout format is as follows:

    ![](../../../../figures/250925154552953.png)

-   **BSH/SBH**: B: Batch, the batch size. S: sequence length. H = N \* D, where N is the number of heads and D is the head size. The layout format is shown in the following figure:

    ![](../../../../figures/jump_write_mode_diagram.png)

    ![](../../../../figures/jump_write_mode_diagram_46.png)

-   **BSNGD**: The shape obtained by reshaping the original BSH shape. S and D are the M axis (or N axis) and K axis of the matrix multiplication for a single batch, and one SD is the computation data of one batch. The layout format is shown in the following figure:

    ![](../../../../figures/250925161748040.png)

-   **SBNGD**: The shape obtained after reshaping the original SBH shape. S and D are the M axis (or N axis) and K axis of the matrix multiplication, and one SD is the computation data of one batch. The layout format is as follows:

    ![](../../../../figures/SBNGD.png)

-   **BNGS1S2**: Generally the output of matrix multiplication for the preceding two Layouts. S1S2 data is stored contiguously, and one S1S2 is the computation data of one batch. The layout format is as follows:

    ![](../../../../figures/BNGS1S2.png)

When instantiating Matmul, you need to set the input and output layout formats through MatmulType. Currently, four Layout types are supported: BSNGD, SBNGD, BNGS1S2, and NORMAL (the BMNK data layout format is represented by NORMAL).

For the BSNGD, SBNGD, and BNGS1S2 layout formats, before calling this API, you need to use [SetALayout](../Matmul-Tiling/SetALayout.md), [SetBLayout](../Matmul-Tiling/SetBLayout.md), [SetCLayout](../Matmul-Tiling/SetCLayout.md), and [SetBatchNum](../Matmul-Tiling/SetBatchNum-88.md) in the host Tiling implementation to set the Layout axis information and the maximum BatchNum of A/B/C. For the NORMAL layout format, you need to use [SetBatchInfoForNormal](../Matmul-Tiling/SetBatchInfoForNormal.md) to set the M/N/K axis information of A/B/C and the BatchNum of the A/B matrices.

<a name="row_width"></a>When using ND format input, whether to enable intrinsicsCheck in MatmulConfig depends on the inner axis length of the source matrix of the A or B matrix within a single BMM. This inner axis can be understood as the contiguous span of one row in the source ND data, that is, the source row width used by one ND2NZ transfer. The value rules under different Layouts are as follows: Under the NORMAL Layout, it is determined by the inner axis of an ordinary matrix. Under the BNGS1S2 Layout, it is determined by the actual span of one row of the source ND matrix. Under the BSNGD Layout, the source matrix inner axes of the A and B matrices are ALayoutInfoN \* ALayoutInfoG \* ALayoutInfoD and BLayoutInfoN \* BLayoutInfoG \* BLayoutInfoD, respectively. Under the SBNGD Layout, the source matrix inner axes of the A and B matrices are ALayoutInfoB \* ALayoutInfoN \* ALayoutInfoG \* ALayoutInfoD and BLayoutInfoB \* BLayoutInfoN \* BLayoutInfoG \* BLayoutInfoD, respectively.

The iteration order of a single matrix multiplication can be adjusted through the tiling parameter iterateOrder.

## Prototype

-   Mix mode

    -   Output to GM

        ```
        template <bool sync = true, bool waitIterateBatch = false>
        __aicore__ inline void IterateBatch(const GlobalTensor<DstT>& gm, uint32_t batchA, uint32_t batchB, bool enSequentialWrite, const uint32_t matrixStrideA = 0, const uint32_t matrixStrideB = 0, const uint32_t matrixStrideC = 0, const bool enPartialSum = false, const uint8_t enAtomic = 0)
        ```

    -   Output to VECIN

        ```
        template <bool sync = true>
        __aicore__ inline void IterateBatch(const LocalTensor<DstT>& ubCmatrix, uint32_t batchA, uint32_t batchB, bool enSequentialWrite, const uint32_t matrixStrideA = 0, const uint32_t matrixStrideB = 0, const uint32_t matrixStrideC = 0, const bool enPartialSum = false, const uint8_t enAtomic = 0)
        ```

-   Cube only mode

    Before use, call the [SetBatchNum](SetBatchNum.md) API to set the sizes of batchA and batchB.

    -   Output to GM

        ```
        __aicore__ inline void IterateBatch(const GlobalTensor<DstT>& gm, bool enPartialSum, uint8_t enAtomic, bool enSequentialWrite, const uint32_t matrixStrideA = 0, const uint32_t matrixStrideB = 0, const uint32_t matrixStrideC = 0)
        ```

    -   Output to VECIN

        ```
        __aicore__ inline void IterateBatch(const LocalTensor<DstT>& ubCmatrix, bool enPartialSum, uint8_t enAtomic, bool enSequentialWrite, const uint32_t matrixStrideA = 0, const uint32_t matrixStrideB = 0, const uint32_t matrixStrideC = 0)
        ```

## Parameters

**Table 1** Template parameters

| Parameter | Description |
| --- | --- |
| sync | The process of obtaining the C matrix is divided into synchronous and asynchronous modes:<br>Synchronous: You need to synchronously wait for **IterateBatch** execution to complete.<br>Asynchronous: You do not need to synchronously wait for **IterateBatch** execution to complete.<br><br>Set the synchronous or asynchronous mode through this parameter: set it to **true** for synchronous mode and **false** for asynchronous mode. The default is synchronous mode. In asynchronous scenarios, use it together with the [WaitIterateBatch](WaitIterateBatch.md) API. |
| waitIterateBatch | Whether to wait for **IterateBatch** execution to complete through the [WaitIterateBatch](WaitIterateBatch.md) API. Used only in asynchronous scenarios. The default is **false**.<br><br>**true**: Wait for **IterateBatch** execution to complete through the **WaitIterateBatch** API.<br><br>**false**: Do not wait for **IterateBatch** execution to complete through the **WaitIterateBatch** API. You handle the process of waiting for **IterateBatch** execution to complete on your own. |

**Table 2** API parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| gm | Output | C matrix. The type is [GlobalTensor](../../../basic_api/data_structures/GlobalTensor/GlobalTensor.md). For the data types supported by different models, see [Supported Data Types](#li12616155731722). 
| ubCmatrix | Output | C matrix. The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md). For the data types supported by different models, see [Supported Data Types](#li12616155731722). |
| batchA | Input | Number of batches of the left matrix. |
| batchB | Input | Number of batches of the right matrix. When batchA and batchB are different, a broadcast operation is performed by default.<br><br>Multi-batch computation supports input broadcast and output reduce on the G axis. The G axis dimensions of the left matrix and right matrix must be in an integer multiple relationship. |
| enSequentialWrite | Input | Whether the output stores data [contiguously](GetTensorC.md#sequential-write), that is, whether to enable sequential write mode (sequential write writes to [baseM, baseN]; non-sequential write writes to the corresponding position in [singleCoreM, singleCoreN]).<br>If the storage location of the left matrix, right matrix, and output matrix is Unified Buffer, set the enSequentialWrite parameter to true;<br>If the storage location of the output matrix is GM, set the enSequentialWrite parameter to false. |
| matrixStrideA | Input | Offset between the start addresses of adjacent nd matrices of the A matrix source operand, in elements. The default value is 0. |
| matrixStrideB | Input | Offset between the start addresses of adjacent nd matrices of the B matrix source operand, in elements. The default value is 0. |
| matrixStrideC | Input | This parameter is reserved. Keep the default value 0. |
| enPartialSum | Input | Whether to accumulate the matrix multiplication result into the existing CO1 data. The default value is false. When accumulating in L0C, it only supports the case where the output C matrix specification of A matrix multiplied by B matrix is singleM==baseM && singleN==baseN. |
| enAtomic | Input | Whether to enable the Atomic operation. The default value is 0.<br><br>Parameter values:<br><br>0: Disable the Atomic operation<br><br>1: Enable the AtomicAdd accumulation operation<br><br>2: Enable the AtomicMax maximum operation<br><br>3: Enable the AtomicMin minimum operation |

## Return Value

None.

## Constraints

-   This API only supports the Norm template, that is, BatchMatmul only supports the Norm template.

-   When using this API, the Layout format of matrices A and B must be the same.

-   For the BSNGD, SBNGD, and BNGS1S2 Layout formats, the total size of multi-batch data after fractal alignment of input matrices A and B must be smaller than the size of the L1 Buffer. For the NORMAL Layout format, there is no such restriction, but the relationship between the multi-batch data size of input matrices A and B and the size of the L1 Buffer must be configured through MatmulConfig;

-   For the BSNGD, SBNGD, and BNGS1S2 Layout formats, let the G axes of the left matrix and the right matrix be ALayoutInfoG and BLayoutInfoG respectively; then ALayoutInfoG / batchA = BLayoutInfoG / batchB. For the NORMAL Layout format, batchA and batchB must satisfy a multiple relationship.

-   If the API outputs to Unified Buffer, the output C matrix size BaseM\*BaseN must be smaller than the allocated Unified Buffer memory size.

-   If the API outputs to the Unified Buffer and the N-direction size singleCoreN computed by a single core is not 32-byte aligned, the CubeFormat of matrix C only supports the ND\_ALIGN format. When the matrix C tile is output, the data in the singleCoreN direction is automatically padded to 32 bytes.

-   For the BSNGD and SBNGD layout formats, the input and output only support ND-format data. For the BNGS1S2 and NORMAL layout formats, the input supports ND/NZ-format data.

-   For the BSNGD and SBNGD layout formats, the continuous write mode is not supported.

-   This API does not support the quantization mode, that is, it does not support the SetQuantScalar and SetQuantVector APIs.

-   In the BSNGD scenario, computing multiple SD rows at a time is not supported. The computation must be performed in a loop within the operator program, that is, \(ALayoutInfoN \* ALayoutInfoG\) / batchA and \(BLayoutInfoN \* BLayoutInfoG\) / batchB must both be integers.

-   **Asynchronous mode** does not support IterateBatch moving data to UB.

-   When the MixDualMaster (dual-master mode) scenario is enabled, that is, when the template parameter [enableMixDualMaster](MatmulConfig.md#matmulconfig-params) is set to true, this API is not supported.

-   On Ascend 950PR/Ascend 950DT, inputs whose matrix memory logical position of A and B is TPosition::VECOUT are not supported. The function prototype that outputs to VECIN currently only supports the BSNGD layout format.

-   On the AI Core of Atlas inference products, only the NORMAL layout format is supported.

-   On the AI Core of Atlas inference products, inputs whose matrix memory logical position of A and B is TPosition::TSCM are not supported.

-   On Ascend 950PR/Ascend 950DT, when the matrix memory logical position of A and B is TPosition::TSCM, only the NORMAL layout format is supported.

-   On the AI Core of Atlas inference products, Bias cannot be reused, and the shape size of Bias must be Batch \* N.

-   When using this API, matrix A and matrix B do not support int4b\_t type inputs, that is, BatchMatmul does not support int4b\_t type matrix inputs.

-   Supported data types<a id="li12616155731722"></a>

    Ascend 950PR/Ascend 950DT supports the following data types: half, bfloat16_t, int32_t, and float.

    Atlas A3 training products/Atlas A3 inference products support the following data types: half, bfloat16_t, int32_t, and float.

    Atlas A2 training products/Atlas A2 inference products support the following data types: half, bfloat16_t, int32_t, and float.

    AI Core of Atlas inference products support the following data types: half, bfloat16_t, int32_t, and float.

## Examples

-   For the function call example in cube only mode, see [Examples](SetBatchNum.md#examples).

-   This example performs matrix multiplication on aGM and bGM and saves the result to cGM. The layout format of the aGM, bGM, and cGM data is NORMAL. For each computation, the left matrix processes **batchA** MK data records, and the right matrix processes **batchB** KN data records.

 ```
    // Define the matmul type.
    typedef AscendC::MatmulType <AscendC::TPosition::GM, CubeFormat::ND, half, false, LayoutMode::NORMAL> aType;
    typedef AscendC::MatmulType <AscendC::TPosition::GM, CubeFormat::ND, half, true, LayoutMode::NORMAL> bType;
    typedef AscendC::MatmulType <AscendC::TPosition::GM, CubeFormat::ND, float, false, LayoutMode::NORMAL> cType;
    typedef AscendC::MatmulType <AscendC::TPosition::GM, CubeFormat::ND, float> biasType;
    // Create a Matmul instance.
    constexpr static MatmulConfig MM_CFG = GetNormalConfig(false, false, false, BatchMode::BATCH_LESS_THAN_L1);
    AscendC::Matmul<aType, bType, cType, biasType, MM_CFG> mm1;
    REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm1);
    mm1.Init(&tiling);
    mm1.SetTensorA(gm_a, isTransposeAIn);
    mm1.SetTensorB(gm_b, isTransposeBIn);
    if(tiling.isBias) {
        mm1.SetBias(gm_bias);
    }
    // Multi=batch Matmul computation
    mm1.IterateBatch(gm_c, batchA, batchB, false);
    ```

-   This example performs matrix multiplication on aGM and bGM and saves the result to cGM. The layout format of the aGM data is BSNGD, the layout format of the bGM data is BSNGD, and the layout format of the cGM is BNGS1S2. For each computation, the left matrix processes **batchA** SD data records, and the right matrix processes **batchB** SD data records.

    ```
    // Define the matmul type.
    typedef AscendC::MatmulType <AscendC::TPosition::GM, CubeFormat::ND, half, false, LayoutMode::NORMAL> aType;
    typedef AscendC::MatmulType <AscendC::TPosition::GM, CubeFormat::ND, half, true, LayoutMode::NORMAL> bType;
    typedef AscendC::MatmulType <AscendC::TPosition::GM, CubeFormat::ND, float, false, LayoutMode::NORMAL> cType;
    typedef AscendC::MatmulType <AscendC::TPosition::GM, CubeFormat::ND, float> biasType;
    // Create a Matmul instance.
    constexpr static MatmulConfig MM_CFG = GetNormalConfig(false, false, false, BatchMode::BATCH_LESS_THAN_L1);
    AscendC::Matmul<aType, bType, cType, biasType, MM_CFG> mm1;
    REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm1);
    mm1.Init(&tiling);
    mm1.SetTensorA(gm_a, isTransposeAIn);
    mm1.SetTensorB(gm_b, isTransposeBIn);
    if(tiling.isBias) {
        mm1.SetBias(gm_bias);
    }
    // Perform multi-batch Matmul computation.
    mm1.IterateBatch(gm_c, batchA, batchB, false);
    ```