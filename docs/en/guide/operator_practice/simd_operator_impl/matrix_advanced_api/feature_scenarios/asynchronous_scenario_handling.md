# Asynchronous Scenario Handling<a name="ZH-CN_TOPIC_0000002532388133"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-26T13:39:14.913Z -->

## Description<a name="zh-cn_topic_0000002264077888_section310824820358"></a>

The [Iterate](../../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Kernel/Iterate.md) and [IterateAll](../../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Kernel/IterateAll.md) APIs of Matmul provide both synchronous and asynchronous modes in the MIX scenario (which contains matrix computation and vector computation). In the pure Cube scenario (only matrix computation), only the synchronous mode is supported.

In synchronous mode, the program must wait for an operation to complete before proceeding to the next operation. In asynchronous mode, the program can proceed to the next operation without waiting for the current operation to complete.

-   Synchronous and asynchronous modes of Iterate&GetTensorC
    -   Synchronous: After one Iterate computation is complete, [GetTensorC](../../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Kernel/GetTensorC.md) is called to move the matrix C tile. Only after the move is complete can the next computation begin. As shown in the following figure, in matrix C, matrix block 2 can be computed only after matrix block 1 is moved out, and matrix block 3 can be computed only after matrix block 2 is moved out.

        ![](../../../../figures/synchronous_asynchronous_scenario_diagram.png)

        The key code example of the Iterate&GetTensorC synchronous mode is as follows:

        ```
        while (mm.Iterate()) {
            mm.GetTensorC(gm_c);
        }
        ```

    -   Asynchronous: Enable the asynchronous mode by setting the template parameter of the Iterate API. After Iterate is called, you do not need to immediately call GetTensorC to synchronously wait for the matrix C tile to be moved. You can perform other operations first, and call GetTensorC when the result is needed. The asynchronous mode reduces synchronous waiting and improves parallelism. It is recommended when developers have high requirements on computation performance. In the asynchronous scenario, a temporary workspace is required to cache the Iterate computation result. Otherwise, the computation result will be overwritten. When GetTensorC is called, the matrix C tile is obtained from this temporary workspace. The temporary workspace is set through the [SetWorkspace](../../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Kernel/SetWorkspace.md) API. The SetWorkspace API must be called before the Iterate API.

        The key code example of the Iterate&GetTensorC asynchronous mode is as follows:

        ```
        mm.SetWorkspace(workspace, size); // where workspace is the physical address of the temporary workspace, and size is the size of matrix C of singleCoreM * singleCoreN
        // Asynchronous mode
        mm.template Iterate<false>();
        …… // Perform other operations
        auto mIter = Ceil(singleCoreM, baseM);
        auto nIter = Ceil(singleCoreN, baseN);
        for (int i = 0; i < mIter * nIter ; ++i) {
            mm.GetTensorC<false> (gm_c);
        }
        ```

-   Synchronous and asynchronous modes of IterateAll
    -   Synchronous: Subsequent operations need to wait synchronously for IterateAll to finish execution.

        The key code example of the IterateAll synchronous mode is as follows:

        ```
        mm.SetTensorA(gm_a);    // Set left matrix A
        mm.SetTensorB(gm_b);    // Set right matrix B
        mm.SetBias(gm_bias);    // Set Bias
        mm.IterateAll(gm_c);
        // Subsequent operations
        ...
        ```

    -   Asynchronous: Subsequent operations do not need to wait synchronously for IterateAll to finish execution. When the result of IterateAll is required, call [WaitIterateAll](../../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Kernel/WaitIterateAll.md) to wait for the IterateAll asynchronous API to return.

        The key code example of the IterateAll asynchronous mode is as follows:

        ```
        AscendC::Matmul<aType, bType, cType, biasType> mm;
        mm.SetTensorA(queryGm[tensorACoreOffset]);
        mm.SetTensorB(keyGm[tensorBCoreOffset + sInnerStart * singleProcessSInnerSize *
              tilingData->attentionScoreOffsetStrideParams.matmulHead], true);
        mm.SetTail(singleProcessSOuterSize, mmNNum);
        mm.template IterateAll<false>(workspaceGm[tmp_block_idx * mmResUbSize * sInnerLoopTimes],0, false,true);
        // Execute other operations.
        mm.WaitIterateAll(); // Wait for IterateAll to complete.
        DataCopy(dstUB, GM);  // Copy data from GM to UB.
        ```

## Scenarios<a name="zh-cn_topic_0000002264077888_section118051016163613"></a>

-   Synchronous Iterate&GetTensorC: MIX scenario (contains matrix computation and vector computation) and pure Cube scenario (only matrix computation).
-   Asynchronous Iterate&GetTensorC: MIX scenario only (contains matrix computation and vector computation).
-   Synchronous IterateAll: MIX scenario (contains matrix computation and vector computation) and pure Cube scenario (only matrix computation).
-   Asynchronous IterateAll: MIX scenario only (contains matrix computation and vector computation).

## Constraints<a name="zh-cn_topic_0000002264077888_section14160134220363"></a>

-   Asynchronous scenario of Iterate&GetTensorC:
    -   The address space size of the input C matrix must be no smaller than baseM \* baseN.
    -   The SetWorkspace API must be called before the Iterate API.
    -   Three output modes are supported: output to VECIN only, output to Global Memory only, and output to both Global Memory and VECIN.
    -   When the C matrix is moved to VECIN, only the NZ format is supported. When the C matrix is moved to GM, both ND and NZ formats are supported.

-   Asynchronous scenario of IterateAll:
    -   The address space size of the input C matrix must be no smaller than singleCoreM \* singleCoreN.
    -   Only continuous output to Global Memory is supported.

## Calling Example<a name="zh-cn_topic_0000002264077888_section15486294368"></a>

-   For the complete example of the asynchronous scenario of Iterate&GetTensorC, see [Asynchronous Scenario Example](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_fused) and [Iterate Asynchronous Scenario Example](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_async_iterate).
-   For the complete example of the asynchronous scenario of IterateAll, see [IterateAll Asynchronous Scenario Example](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_async_iterate_all).
