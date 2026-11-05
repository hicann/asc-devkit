# IterateBatch

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-08T12:32:10.915Z pushedAt=2026-10-27T11:26:22.112Z nl=2 -->

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
- Atlas inference products AI Core: Supported
- Vector Core of Atlas inference products: Not supported
<!-- end id7 -->
<!-- npu="910" id8 -->
- Atlas training products: Not supported
<!-- end id8 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/IterateBatch_res.md#id1 -->

## Description

When the shape processed by a single Matmul computation is relatively small, performance may be affected because each computation involves internal communication. This API provides the capability to process Matmul in batches. By calling **IterateBatch** once, multiple C cubes of **singleCoreM** \* **singleCoreN** size can be computed.

Before using this API, you need to understand some essential data layout formats:

-   **General data format (NORMAL)**: The data layout format of BMNK. B: Batch, the batch size. M, N, and K are the cube dimensions of cube multiplication \[M, K\]\*\[K, N\]. The data layout format is as follows:

    ![](../../../../figures/250925154552953.png)

-   **BSH/SBH**: B: Batch, the batch size. S: sequence length. H = N \* D, where N is the number of heads and D is the head size. The layout format is shown in the following figure:

    ![](../../../../figures/skip_write_mode.png)

    ![](../../../../figures/skip_write_mode_46.png)

-   **BSNGD**: The shape obtained by reshaping the original BSH shape. S and D are the M axis (or N axis) and K axis of the cube multiplication for a single batch. One SD is the computation data of one batch. The layout format is shown in the following figure:

    ![](../../../../figures/250925161748040.png)

-   **SBNGD**: The shape obtained by reshaping the original SBH shape. S and D are the M axis (or N axis) and K axis of the cube multiplication. One SD is the computation data of one batch. The layout format is shown in the following figure:

    ![](../../../../figures/SBNGD.png)

-   **BNGS1S2**: Generally the output of cube multiplication for the preceding two layouts. S1S2 data is stored contiguously. One S1S2 is the computation data of one batch. The layout format is shown in the following figure:

    ![](../../../../figures/BNGS1S2.png)

When instantiating Matmul, set the input and output Layout formats through MatmulType. Currently, four Layout types are supported: BSNGD, SBNGD, BNGS1S2, and NORMAL (the BMNK data layout format is represented by NORMAL).

For the BSNGD, SBNGD, and BNGS1S2 Layout formats, before calling this API, use [SetALayout](../Matmul_Tiling/SetALayout.md), [SetBLayout](../Matmul_Tiling/SetBLayout.md), [SetCLayout](../Matmul_Tiling/SetCLayout.md), and [SetBatchNum](../Matmul_Tiling/SetBatchNum.md) in the host Tiling implementation to set the Layout axis information of A/B/C and the maximum BatchNum. For the NORMAL Layout format, use [SetBatchInfoForNormal](../Matmul_Tiling/SetBatchInfoForNormal.md) to set the M/N/K axis information of A/B/C and the BatchNum of the A/B cubes.


<a name="row_width"></a>When using ND format input, whether to enable intrinsicsCheck in MatmulConfig depends on the inner axis length of the source cube of the A or B cube within a single BMM. This inner axis can be understood as the continuous span of one row in the source ND data, that is, the source row width used by a single ND2NZ transfer. The value rules under different Layouts are as follows: Under NORMAL Layout, it is determined by the inner axis of an ordinary cube. Under BNGS1S2 Layout, it is determined by the actual span of one row of the source ND cube. Under BSNGD Layout, the inner axes of the source cubes of A and B are ALayoutInfoN \* ALayoutInfoG \* ALayoutInfoD and BLayoutInfoN \* BLayoutInfoG \* BLayoutInfoD, respectively. Under SBNGD Layout, the inner axes of the source cubes of A and B are ALayoutInfoB \* ALayoutInfoN \* ALayoutInfoG \* ALayoutInfoD and BLayoutInfoB \* BLayoutInfoN \* BLayoutInfoG \* BLayoutInfoD, respectively.

The iteration order of a single cube multiplication can be adjusted through the tiling parameter iterateOrder.

## Prototype

-   Mix mode
    -   Output To GM

        ```
        template <bool sync = true, bool waitIterateBatch = false>
        __aicore__ inline void IterateBatch(const GlobalTensor<DstT>& gm, uint32_t batchA, uint32_t batchB, bool enSequentialWrite, const uint32_t matrixStrideA = 0, const uint32_t matrixStrideB = 0, const uint32_t matrixStrideC = 0, const bool enPartialSum = false, const uint8_t enAtomic = 0)
        ```

    -   Output To Unified Buffer (UB, VECIN)

        ```
        template <bool sync = true>
        __aicore__ inline void IterateBatch(const LocalTensor<DstT>& ubCmatrix, uint32_t batchA, uint32_t batchB, bool enSequentialWrite, const uint32_t matrixStrideA = 0, const uint32_t matrixStrideB = 0, const uint32_t matrixStrideC = 0, const bool enPartialSum = false, const uint8_t enAtomic = 0)
        ```

-   Pure cube mode

    Before use, call the [SetBatchNum](SetBatchNum.md) API to set the sizes of batchA and batchB.

    -   Output to GM.

        ```
        __aicore__ inline void IterateBatch(const GlobalTensor<DstT>& gm, bool enPartialSum, uint8_t enAtomic, bool enSequentialWrite, const uint32_t matrixStrideA = 0, const uint32_t matrixStrideB = 0, const uint32_t matrixStrideC = 0)
        ```

    -   Output to UB (VECIN).

        ```
        __aicore__ inline void IterateBatch(const LocalTensor<DstT>& ubCmatrix, bool enPartialSum, uint8_t enAtomic, bool enSequentialWrite, const uint32_t matrixStrideA = 0, const uint32_t matrixStrideB = 0, const uint32_t matrixStrideC = 0)
        ```

## Parameters

**Table 1** Template parameters

| Parameter | Description |
| --- | --- |
| sync | The process of obtaining cube C is divided into two modes: synchronous and asynchronous.<br>Synchronous: You must wait synchronously for **IterateBatch** to finish executing.<br>Asynchronous: You do not need to wait synchronously for **IterateBatch** to finish executing.<br><br>This parameter sets the synchronous or asynchronous mode: set to **true** for synchronous mode and **false** for asynchronous mode. The default is synchronous mode. In asynchronous scenarios, use it together with the [WaitIterateBatch](WaitIterateBatch.md) API. |
| waitIterateBatch | Whether to wait for the execution completion of **IterateBatch** through the [WaitIterateBatch](WaitIterateBatch.md) API. Used only in asynchronous scenarios. The default is **false**.<br><br>**true**: You must wait for **IterateBatch** to finish executing through the **WaitIterateBatch** API.<br><br>**false**: You do not need to wait for **IterateBatch** to finish executing through the **WaitIterateBatch** API; the developer handles waiting for **IterateBatch** to finish executing. |

**Table 2** API parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| gm | Output | Cube C. The type is [GlobalTensor](../../../basic_api/data_structures/GlobalTensor/GlobalTensor.md). For the data types supported by different models, see [Supported Data Types](#li12616155731722). |
| ubCmatrix | Output | Cube C. The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md). For the data types supported by different models, see [Supported Data Types](#li12616155731722). |
| batchA | Input | Number of batches of the left cube. |
| batchB | Input | Number of batches of the right cube. When **batchA** and **batchB** are different, a broadcast operation is performed by default.<br><br>Multi-batch computation supports input broadcast and output reduction on the G axis. The G-axis dimensions of the left and right cubes must have an integer multiple relationship. |
| enSequentialWrite | Input | Whether the output is [stored contiguously](GetTensorC.md#fig580415103338), that is, whether to enable the continuous write mode (in continuous write mode, data is written to [baseM, baseN]; in non-continuous write mode, data is written to the corresponding position in [singleCoreM, singleCoreN]).<br>If the storage locations of the left cube, right cube, and output cube are UB, the **enSequentialWrite** parameter should be set to **true**.<br>If the storage location of the output cube is GM, the **enSequentialWrite** parameter should be set to **false**. |
| matrixStrideA | Input | Offset between the start addresses of adjacent nd cubes in the source operand of cube A, in elements. The default value is 0. |
| matrixStrideB | Input | Offset between the start addresses of adjacent nd cubes in the source operand of cube B, in elements. The default value is 0. |
| matrixStrideC | Input | This parameter is reserved. Keep the default value 0. |
| enPartialSum | Input | Whether to accumulate the cube multiplication result into the existing L0C Buffer (CO1) data. The default value is **false**. When accumulating in L0C, only the output cube C specification with singleM==baseM && singleN==baseN is supported for the multiplication of cube A and cube B. |
| enAtomic | Input | Whether to enable the Atomic operation. The default value is 0.<br><br>Parameter values:<br><br>**0**: The Atomic operation is not enabled.<br><br>**1**: The AtomicAdd accumulation operation is enabled.<br><br>**2**: The AtomicMax maximum operation is enabled.<br><br>**3**: The AtomicMin minimum operation is enabled. |

## Return Value

None

## Constraints

-   This API only supports the Norm template, that is, **BatchMatmul** only supports the Norm template.
-   When using this API, the Layout formats of cubes A and B must be the same.
-   For the BSNGD, SBNGD, and BNGS1S2 Layout formats, the total size of the multi-batch data of input cubes A and B after fractal alignment should be smaller than the L1 Buffer size. For the NORMAL Layout format, there is no such restriction, but the relationship between the multi-batch data size of input cubes A and B and the L1 Buffer size must be configured through **MatmulConfig**.
-   For the BSNGD, SBNGD, and BNGS1S2 Layout formats, let the G axes of the left cube and right cube be **ALayoutInfoG** and **BLayoutInfoG**, respectively. Then ALayoutInfoG / batchA = BLayoutInfoG / batchB. For the NORMAL Layout format, **batchA** and **batchB** must satisfy a multiple relationship.
-   If the API outputs to UB, the size of output cube C BaseM\*BaseN should be smaller than the allocated UB memory size.
-   If the API outputs to UB and the N-direction size singleCoreN computed by a single core is not 32-byte aligned, the CubeFormat of cube C only supports the ND\_ALIGN format. When outputting cube C tiles, the data in the singleCoreN direction is automatically padded to 32 bytes.
-   For the BSNGD and SBNGD Layout formats, the input and output only support ND format data. For the BNGS1S2 and NORMAL Layout formats, the input supports ND/NZ format data.
-   For the BSNGD and SBNGD Layout formats, the continuous write mode is not supported.
-   This API does not support the quantization mode, that is, it does not support the **SetQuantScalar** and **SetQuantVector** APIs.
-   In the BSNGD scenario, computing multiple SD rows at a time is not supported. The operator program must compute them in a loop, that is, \(ALayoutInfoN \* ALayoutInfoG\) / **batchA** and \(BLayoutInfoN \* BLayoutInfoG\) / **batchB** must both be integers.
-   The **asynchronous mode** does not support moving data to UB through **IterateBatch**.
-   When the MixDualMaster (dual-master mode) scenario is enabled, that is, when the template parameter [enableMixDualMaster](MatmulConfig.md#matmulconfig-params) is set to true, this API is not supported.
<!-- npu="950" id9 -->
-   On Ascend 950PR&950DT products, inputs whose A and B cube memory logical position is TPosition::VECOUT are not supported. The function prototype that outputs to VECIN currently only supports the BSNGD Layout format.
<!-- end id9 -->
<!-- npu="310p" id10 -->
-   On the Atlas inference products AI Core, only the NORMAL Layout format is supported.
-   On the Atlas inference products AI Core, inputs whose A and B cube memory logical position is TPosition::TSCM are not supported.
<!-- end id10 -->
<!-- npu="950" id11 -->
-   On Ascend 950PR&950DT products, when the A and B cube memory logical position is TPosition::TSCM, only the NORMAL Layout format is supported.
<!-- end id11 -->
<!-- npu="310p" id12 -->
-   On the Atlas inference products AI Core, Bias does not support reuse, and the shape of Bias must be **Batch** \* **N**.
<!-- end id12 -->
-   When using this API, cube A and cube B do not support int4b\_t type input, that is, **BatchMatmul** does not support int4b\_t type cube input.
<!-- npu="950,A3,910b,310p" id17 -->
-   Supported data types<a id="li12616155731722"></a>

    <!-- npu="950" id13 -->
    For Ascend 950PR&950DT products, the supported data types are: half, bfloat16_t, int32_t, and float.
    <!-- end id13 -->

    <!-- npu="A3" id14 -->
    For Atlas A3 products, the supported data types are: half, bfloat16_t, int32_t, and float.
    <!-- end id14 -->

    <!-- npu="910b" id15 -->
    For Atlas A2 products, the supported data types are: half, bfloat16_t, int32_t, and float.
    <!-- end id15 -->

    <!-- npu="310p" id16 -->
    For the Atlas inference products AI Core, the supported data types are: half, bfloat16_t, int32_t, and float.
    <!-- end id16 -->

<!-- end id17 -->

<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/IterateBatch_res.md#id2 -->

## Examples

-   For the function call example in pure cube mode, see [Examples](SetBatchNum.md#examples).
-   This example performs cube multiplication of aGM and bGM, and saves the result to cGm, where the layout format of aGM, bGM, and cGM data is NORMAL. The left cube computes batchA MK data each time, and the right cube computes batchB KN data each time.

    ```
    // Define the matmul type.
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half, false, LayoutMode::NORMAL> aType;
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half, true, LayoutMode::NORMAL> bType;
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float, false, LayoutMode::NORMAL> cType;
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> biasType;
    // Create a Matmul instance.
    constexpr static MatmulConfig MM_CFG = GetNormalConfig(false, false, false, BatchMode::BATCH_LESS_THAN_L1);
    AscendC::Matmul<aType, bType, cType, biasType, MM_CFG> mm1;
    REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm1);
    mm1.Init(&tiling);
    mm1.SetTensorA(gm_a, isTransposeAIn);
    mm1.SetTensorB(gm_b, isTransposeBIn);
    if (tiling.isBias) {
        mm1.SetBias(gm_bias);
    }
    // Perform multi-batch Matmul computation.
    mm1.IterateBatch(gm_c, batchA, batchB, false);
    ```

-   This example performs cube multiplication of aGM and bGM, and saves the result to cGm. The layout format of aGM data is BSNGD, the layout format of bGM data is BSNGD, and the layout format of cGM is BNGS1S2. The left cube computes batchA SD data each time, and the right cube computes batchB SD data each time.

    ```
    // Define the matmul type.
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half, false, LayoutMode::BSNGD> aType;
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half, true, LayoutMode::BSNGD> bType;
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float, false, LayoutMode::BNGS1S2> cType;
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> biasType;
    // Create a Matmul instance.
    AscendC::Matmul<aType, bType, cType, biasType> mm1;
    REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm1);
    mm1.Init(&tiling);
    int batchC = batchA > batchB ? batchA : batchB;
    int g_lay = tiling.ALayoutInfoG > tiling.BLayoutInfoG ? tiling.ALayoutInfoG : tiling.BLayoutInfoG;
    // Calculate the number of loops required for multi-batch computation.
    int for_exent = tiling.ALayoutInfoB * tiling.ALayoutInfoN * g_lay / tiling.BatchNum;
    for (int i = 0; i < for_exent; ++i) {
        // Calculate the start address of the A/B cubes for each multi-batch computation.
        int batchOffsetA = i * tiling.ALayoutInfoD * batchA;
        int batchOffsetB = i * tiling.BLayoutInfoD * batchB;
        mm1.SetTensorA(gm_a[batchOffsetA], isTransposeAIn);
        mm1.SetTensorB(gm_b[batchOffsetB], isTransposeBIn);
        int idx_c = i * batchC;
        if (tiling.CLayoutInfoG == 1 && (tiling.BLayoutInfoG != 1 || tiling.ALayoutInfoG != 1)) {
            idx_c = idx_c / (tiling.BLayoutInfoG > tiling.ALayoutInfoG ? tiling.BLayoutInfoG : tiling.ALayoutInfoG);
        }
        if (tiling.isBias) {
            int batchOffsetBias = idx_c * tiling.CLayoutInfoS2;
            mm1.SetBias(gm_bias[batchOffsetBias]);
        }
        int batchOffsetC = idx_c * tiling.CLayoutInfoS2;
        if (C_TYPE::layout == LayoutMode::BNGS1S2) {
            batchOffsetC = idx_c * tiling.CLayoutInfoS2 * tiling.CLayoutInfoS1;
        }
        // Perform multi-batch Matmul computation.
        mm1.IterateBatch(gm_c[batchOffsetC], batchA, batchB, false);
    }
    ```
