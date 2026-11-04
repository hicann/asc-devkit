# Asynchronous Scenario Handling<a name="ZH-CN_TOPIC_0000002532388133"></a>

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-10-23T12:02:50.284Z pushedAt=2026-11-03T07:05:13.298Z nl=2 -->

## Feature Overview<a name="zh-cn_topic_0000002264077888_section310824820358"></a>

The [Iterate](../../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/Iterate.md) and [IterateAll](../../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/IterateAll.md) APIs of Matmul provide both synchronous and asynchronous modes in MIX scenarios (containing cube calculation and vector calculation). In cube-only scenarios (matrix-only calculation), only the synchronous mode is supported.

The synchronous mode means that during program execution, the next operation can be performed only after the current operation is complete. The asynchronous mode means that during program execution, the next operation can be performed without waiting for the current operation to complete.

-   Synchronous and asynchronous modes of Iterate&GetTensorC
    -   Synchronous: After one Iterate calculation is complete, [GetTensorC](../../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/GetTensorC.md) is called to move the cube C fragment. Only after the move is complete can the next calculation be performed. As shown in the following figure, in cube C, cube block 2 can be calculated only after cube block 1 is moved out, and cube block 3 can be calculated only after cube block 2 is moved out.

        ![](../../../../figures/sync_async.png)

        The following is a key code example of the synchronous mode of Iterate&GetTensorC:

        ```
        while (mm.Iterate()) {
            mm.GetTensorC(gm_c);
        }
        ```

    -   Asynchronous: Enable the asynchronous mode by setting the template parameter of the Iterate API. After Iterate is called, you do not need to immediately call GetTensorC to synchronously wait for the cube C fragment to be moved out. You can perform other operations first, and call GetTensorC when the result is needed. The asynchronous mode reduces synchronous waiting and improves parallelism. You can use this mode when high calculation performance is required. In the asynchronous scenario, a temporary workspace is required to cache the Iterate calculation result. Otherwise, the calculation result will be overwritten. When GetTensorC is called, the cube C fragment is obtained from this temporary workspace. The temporary workspace is set through the [SetWorkspace](../../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/SetWorkspace.md) API. The SetWorkspace API must be called before the Iterate API.

        The following is a key code example of the asynchronous mode of Iterate&GetTensorC:

        ```
        mm.SetWorkspace(workspace, size); // workspace is the physical address of the temporary workspace, and size is the size of cube C of singleCoreM * singleCoreN
        // Asynchronous mode
        mm.template Iterate<false>();
        ......// Perform other operations
        auto mIter = Ceil(singleCoreM, baseM);
        auto nIter = Ceil(singleCoreN, baseN);
        for (int i = 0; i < mIter * nIter ; ++i) {
            mm.GetTensorC<false> (gm_c);
        }
        ```

-   Synchronous and asynchronous modes of IterateAll
    -   Synchronous: Subsequent operations need to synchronously wait for IterateAll to finish execution.

        The key code example of the IterateAll synchronous mode is as follows:

        ```
        mm.SetTensorA(gm_a);    // Set left cube A
        mm.SetTensorB(gm_b);    // Set right cube B
        mm.SetBias(gm_bias);    // Set bias
        mm.IterateAll(gm_c);
        // Subsequent operations
        ...
        ```

    -   Asynchronous: Subsequent operations do not need to synchronously wait for IterateAll to finish execution. When the result of IterateAll is required, call [WaitIterateAll](../../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/WaitIterateAll.md) to wait for the IterateAll asynchronous API to return.

        The key code example of the IterateAll asynchronous mode is as follows:

        ```
        AscendC::Matmul<aType, bType, cType, biasType> mm;
        mm.SetTensorA(queryGm[tensorACoreOffset]);
        mm.SetTensorB(keyGm[tensorBCoreOffset + sInnerStart * singleProcessSInnerSize *
              tilingData->attentionScoreOffsetStrideParams.matmulHead], true);
        mm.SetTail(singleProcessSOuterSize, mmNNum);
        mm.template IterateAll<false>(workspaceGm[tmp_block_idx * mmResUbSize * sInnerLoopTimes],0, false,true);
        // Perform other operations
        mm.WaitIterateAll(); // Wait for IterateAll to complete
        DataCopy(dstUB, GM);  // Copy data from GM to Unified Buffer (UB)
        ```

## Scenarios<a name="zh-cn_topic_0000002264077888_section118051016163613"></a>

-   Synchronous mode of Iterate&GetTensorC: MIX scenario (containing cube calculation and vector calculation) and Cube-only scenario (matrix-only calculation).
-   Asynchronous mode of Iterate&GetTensorC: MIX scenario only (containing cube calculation and vector calculation).
-   Synchronous mode of IterateAll: MIX scenario (containing cube calculation and vector calculation) and Cube-only scenario (matrix-only calculation).
-   Asynchronous mode of IterateAll: MIX scenario only (containing cube calculation and vector calculation).

## Constraints<a name="zh-cn_topic_0000002264077888_section14160134220363"></a>

-   Asynchronous scenario of Iterate & GetTensorC:
    -   The address space size of the input cube C must be no smaller than baseM \* baseN.
    -   The SetWorkspace API must be called before the Iterate API.
    -   Three output modes are supported: output to UB (VECIN) only, output to Global Memory only, and output to both Global Memory and UB (VECIN).
    -   When the cube C is fetched to UB (VECIN), only the NZ data format is supported. When the cube C is fetched to GM, the ND or NZ data format is supported.

-   Asynchronous scenario of IterateAll:
    -   The address space size of the input cube C must be no smaller than singleCoreM \* singleCoreN.
    -   Only continuous output to Global Memory is supported.

## Example<a name="zh-cn_topic_0000002264077888_section15486294368"></a>

-   For the complete example of the asynchronous scenario of Iterate&GetTensorC, see [Asynchronous Scenario Example](../../../../../../../examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_fused) and [Iterate Asynchronous Scenario Example](../../../../../../../examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_async_iterate).
-   For the complete example of the asynchronous scenario of IterateAll, see [IterateAll Asynchronous Scenario Example](../../../../../../../examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_async_iterate_all).
