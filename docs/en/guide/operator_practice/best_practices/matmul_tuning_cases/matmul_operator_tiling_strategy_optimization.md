# Matmul Operator Optimization Tiling Strategy<a name="ZH-CN_TOPIC_0000001893038953"></a>

<!-- md-trans-meta sourceCommit=e4a5905334db2bef02e668063a0b99d64a2e7709 translatedAt=2026-08-26T13:24:23.947Z -->

## Case Introduction<a name="section1532131415369"></a>

This case performs performance analysis and optimization on the Matmul operator. The Matmul operator implements matrix multiplication, which mainly includes data load-in and load-out pipelines and the Cube computation pipeline.

Taking matrix dimensions M = 4096, N = 5120, K = 4096, input data type half, output data type float, and output format ND as an example, the performance verification platform is Atlas A2 Training Series/Atlas A2 Inference Series. This case introduces the optimization methods for the Matmul operator, including optimizing core partitioning logic and basic blocks, as well as enabling large package loading.

-   Core partitioning logic: Enable as many Cube cores as possible to implement parallel computation.
-   Basic blocks: Select the optimal baseM, baseN, and baseK parameters, where baseM, baseN, and baseK are parameters in Matmul Tiling.
-   Enable large package loading: When loading data from GM to L1, for matrix A, load depthA1 basic blocks at a time, with a basic block size of baseM \* baseK; for matrix B, load depthB1 basic blocks at a time, with a basic block size of baseN \* baseK. After enabling large package loading, the data volume loaded at a time increases, thereby improving the MTE2 loading efficiency.

## Obtaining Performance Data<a name="section327528143615"></a>

Use the msOpProf tool to obtain the Profiling data of the operator, focusing on analyzing the pipeline status of the MTE2, Cube, and Scalar pipelines.

## Analyzing the Main Bottleneck<a name="section788333916369"></a>

**Figure 1** Profiling data before optimization<a name="fig136871118111317"></a>  
![](../../../figures/profiling_data_before_optimization.png "Profiling data before optimization")

From the preceding profiling data, it can be seen that MTE2 accounts for a large proportion of the time, and the current performance bottleneck lies in the MTE2 pipeline.

-   The Block Dim in the profiling data shows that the cores are not fully partitioned, so optimization of the core partitioning logic should be considered. Let CurrentCore be the number of Cube cores before core partitioning optimization, and MaxCore be the maximum number of Cube cores. When all cores are enabled to compute the current shape data volume in parallel, the estimated performance benefit is a factor of MaxCore / CurrentCore.
-   Optimizing the basic block partitioning will affect the efficiency of loading data. The total data volume loaded by the operator is the sum of the data volumes of the left matrix and the right matrix. In the scenario where the K direction cannot be fully loaded in Matmul computation, according to the matrix multiplication algorithm, the number of times the left matrix is loaded is N / baseN, and the number of times the right matrix is loaded is M / baseM. Therefore, the total loaded data volume totalCnt = \(N / baseN\) \* M \* K + \(M / baseM\) \* K \* N. The estimated performance benefit is the ratio of the loaded data volumes, that is, the data volume before optimization totalCnt0 / the data volume after optimization totalCnt1. After simplification, the result is \(1 / baseM0 + 1 / baseN0\) / \(1 / baseM1 + 1 / baseN1\), where baseM0 and baseN0 are the basic block parameters before optimization, and baseM1 and baseN1 are the basic block parameters after optimization.

-   After enabling large package loading, factors such as the change in the number of instructions and address alignment will affect performance. Based on experience, for scenarios where MTE2 is the performance bottleneck, there will be an MTE2 performance benefit of more than 20%.

## Designing an Optimization Solution<a name="section12959175753610"></a>

-   Optimization point 1: Optimize the core partitioning logic

    The Profiling data shows that the number of partitioned cores is 4. Starting more cores to compute simultaneously can improve the computing parallelism. The AI processor used in this case has 20 cores in total, and each core contains one Cube Core and two Vector Cores. In the NPU calling program, set numBlocks to 20, which is the number of cores actually used.

    ```
    // Code snippet
    uint32_t numBlocks = 20; // Before optimization, numBlocks is 4
    CHECK_ACL(aclInit(nullptr));
    int32_t deviceId = 0;
    CHECK_ACL(aclrtSetDevice(deviceId));
    aclrtStream stream = nullptr;
    CHECK_ACL(aclrtCreateStream(&stream));
    
    uint8_t *aHost;
    uint8_t *aDevice;
    CHECK_ACL(aclrtMallocHost((void **)(&aHost), aFileSize));
    CHECK_ACL(
      aclrtMalloc((void **)&aDevice, aFileSize, ACL_MEM_MALLOC_HUGE_FIRST));
    ReadFile("./input/x1_gm.bin", aFileSize, aHost, aFileSize);
    // PrintData(aHost, 16, printDataType::HALF);
    CHECK_ACL(aclrtMemcpy(aDevice, aFileSize, aHost, aFileSize,
    					ACL_MEMCPY_HOST_TO_DEVICE));
    
    uint8_t *bHost;
    uint8_t *bDevice;
    CHECK_ACL(aclrtMallocHost((void **)(&bHost), bFileSize));
    CHECK_ACL(
      aclrtMalloc((void **)&bDevice, bFileSize, ACL_MEM_MALLOC_HUGE_FIRST));
    ReadFile("./input/x2_gm.bin", bFileSize, bHost, bFileSize);
    // PrintData(bHost, 16, printDataType::HALF);
    CHECK_ACL(aclrtMemcpy(bDevice, bFileSize, bHost, bFileSize,
    					ACL_MEMCPY_HOST_TO_DEVICE));
    
    uint8_t *workspaceHost;
    uint8_t *workspaceDevice;
    CHECK_ACL(aclrtMallocHost((void **)(&workspaceHost), workspaceSize));
    CHECK_ACL(aclrtMalloc((void **)&workspaceDevice, workspaceSize,
    					ACL_MEM_MALLOC_HUGE_FIRST));
    
    uint8_t *tilingHost;
    uint8_t *tilingDevice;
    CHECK_ACL(aclrtMallocHost((void **)(&tilingHost), tilingFileSize));
    CHECK_ACL(aclrtMalloc((void **)&tilingDevice, tilingFileSize,
    					ACL_MEM_MALLOC_HUGE_FIRST));
    CHECK_ACL(aclrtMemcpy(tilingHost, tilingFileSize, GenerateTiling(),
    					tilingFileSize, ACL_MEMCPY_HOST_TO_HOST));
    // PrintData(tilingHost, 16, printDataType::UINT32_T);
    CHECK_ACL(aclrtMemcpy(tilingDevice, tilingFileSize, tilingHost,
    					tilingFileSize, ACL_MEMCPY_HOST_TO_DEVICE));
    
    uint8_t *cHost;
    uint8_t *cDevice;
    CHECK_ACL(aclrtMallocHost((void **)(&cHost), cFileSize));
    CHECK_ACL(
      aclrtMalloc((void **)&cDevice, cFileSize, ACL_MEM_MALLOC_HUGE_FIRST));
    
    // ACLRT_LAUNCH_KERNEL(matmul_custom)
    // (numBlocks, stream, aDevice, bDevice, cDevice, workspaceDevice, tilingDevice);
    matmul_custom_do(numBlocks, stream, aDevice, bDevice, cDevice, workspaceDevice, tilingDevice);
    ```

    Since the Matmul API is always initiated from the Vector side, and the ratio of Cube Cores to Vector Cores in the AI processor used in this case is 1 : 2, the Matmul tiling computation needs to partition data by twice the number of numBlocks, that is, the number of Vector Cores. The actual number of running cores set in the NPU calling program is 20, so the Tiling code sets the Tiling API to partition data by 40 cores, as shown in the following code.

    ```
    int usedCoreNum = 40; // Before optimization, usedCoreNum is 8
    int runMode = 1;
    int32_t baseM = 64; // 64
    int32_t baseN = 64; // 64
    optiling::TCubeTiling tilingData;
    MultiCoreMatmulTiling tilingApi;
    tilingApi.SetDim(usedCoreNum);
    ```

    **Figure 2** Profiling data after optimizing the core partitioning logic<a name="fig3811154518136"></a>  
    ![](../../../figures/profiling_data_after_optimizing_core_partitioning_logic.png "Profiling data after optimizing the core partitioning logic")

    After the code is modified, the operator execution time decreases from 12045 us to 2532 us, which is approximately equal to a performance improvement of (20 cores / 4 cores) = 5 times.

-   Optimization point 2: Optimize the basic blocks

    The base block set in the current Tiling is \[baseM, baseN, baseK\] = \[64, 64, 256\]. This basic block has few Cube computation cycles and a low compute-memory access ratio (the ratio of computation volume to the required data volume). The base block for loading a Matmul result to GM once is 64 \* 64. Since the output format is ND and the data type is float, the start address for loading the next Matmul result needs to be offset by the size of one baseN, that is, 64 \* 4 = 256 bytes. As a result, the GM address is not 512-byte aligned when fixpipe loads the result. Therefore, a better basic block needs to be set.

    For the current scenario with a large shape, the principle for selecting the basic block is to maximize the compute-memory access ratio, that is, to minimize the accessed data volume while maximizing the Cube computation volume. When the input is of the fp16 type, the Cube execution unit can compute 16 \* 16 \* 16 numbers in one cycle. Based on experience, the two partitioning schemes \[baseM, baseN, baseK\] = \[128, 256, 64\] and \[128, 128, 128\] both satisfy [512-byte GM address alignment](../../simd_operator_optimization/memory_access/gm_address_512b_alignment.md) when loading (each time a Matmul result is loaded, the address is offset by 256 \* 4 bytes and 128 \* 4 bytes, respectively). The Cube computation cycle counts are the same, which is \(128 \* 64 \* 256\) / \(16 \* 16 \* 16\) = \(128 \* 128 \* 128\) / \(16 \* 16 \* 16\) = 512 cycles. For \[baseM, baseN, baseK\] = \[128, 256, 64\], the compute-memory access ratio is 512 cycles / \(128 \* 64 \* 2 + 256 \* 64 \* 2\) = 512 cycles / 48 KB. For \[baseM, baseN, baseK\] = \[128, 128, 128\], the compute-memory access ratio is 512 cycles / \(128 \* 128 \* 2 + 128 \* 128 \* 2\) = 512 cycles / 64 KB. It can be seen that the \[128, 256, 64\] basic block scheme has a higher compute-memory access ratio and greater computation density. For the same computation volume, it requires the minimum data volume, maximizing the computation volume of the Cube unit.

    Modify the Tiling code and set baseM and baseN through the SetFixSplit\(\) API. The tiling function automatically calculates the optimal baseK, which is 64 here.

    ```
    int32_t baseM = 128; // baseM is 64 before optimization
    int32_t baseN = 256; // baseN is 64 before optimization
    
    optiling::TCubeTiling tilingData;
    MultiCoreMatmulTiling tilingApi;
    tilingApi.SetDim(usedCoreNum);
    tilingApi.SetAType(leftPos, leftFormat, leftDtype, bool(transposeA));
    tilingApi.SetBType(rightPos, rightFormat, rightDtype, bool(transposeB));
    tilingApi.SetCType(resPos, resFormat, resDtype);
    tilingApi.SetBiasType(biasPos, biasFormat, biasDtype);
    
    tilingApi.SetOrgShape(M, N, K);
    tilingApi.SetShape(M, N, K);
    tilingApi.SetFixSplit(baseM, baseN, -1);
    ```

    After using this set of basic blocks, the MTE2 time (corresponding to aic\_mte2\_time) decreases from 2452 us to 808 us, improving MTE2 performance by 3 times.

    **Figure 3**  Profiling data after basic block optimization<a name="fig1012052281415"></a>  
    ![](../../../figures/profiling_data_after_basic_block_optimization.png "Profiling data after basic block optimization")

-   Optimization point 3: enable large package loading

    The current bandwidth utilization is: totalSize / mte2Time = totalCnt \* dtype / mte2Time, which is calculated as 2491 GB/s by substituting the data. When large package loading is not enabled, only one basic block is loaded from GM to L1 at a time. Enable large package loading through the template parameter to load multiple basic blocks at a time, improving the MTE2 bandwidth utilization.

    ```
     // Original matmul object definition:
      Matmul<AscendC::MatmulType<TPosition::GM, CubeFormat::ND, A_T>,
             AscendC::MatmulType<TPosition::GM, CubeFormat::ND, B_T>,
             AscendC::MatmulType<TPosition::GM, CubeFormat::ND, C_T>,
             AscendC::MatmulType<TPosition::GM, CubeFormat::ND, BiasT>>>
          mm;
     // Enable large package loading by adding the CFG_MDL parameter to the template parameters when defining the matmul object:
      Matmul<AscendC::MatmulType<TPosition::GM, CubeFormat::ND, A_T>,
             AscendC::MatmulType<TPosition::GM, CubeFormat::ND, B_T>,
             AscendC::MatmulType<TPosition::GM, CubeFormat::ND, C_T>,
             AscendC::MatmulType<TPosition::GM, CubeFormat::ND, BiasT>, CFG_MDL>>
          mm;
    ```

    As shown in the following figure, after enabling large package loading, the MTE2 time decreases from 808 us to 591 us, and the bandwidth utilization, calculated by substituting the data, is 3406 GB/s, an improvement of more than 36%. The Cube utilization reaches more than 80%.

    **Figure 4** Profiling data after enabling large package movement<a name="fig13648142015171"></a>  
    ![](../../../figures/profiling_data_after_enabling_large_package_movement.png "Profiling data after enabling large package movement")

## Verifying the Performance Gains of the Optimization Solution<a name="section225561133715"></a>

-   With optimized core partitioning logic, the actual benefit is 4.75x, approximately equal to \(20 cores / 4 cores\) = 5x. Considering the core startup overhead, the benefit can be regarded as consistent.
-   With optimized basic blocks, the actual benefit is approximately 3x. Substituting the theoretical evaluation into the analysis formula above, the benefit is \(1 / 64 + 1 / 64\) / \(1 / 128 + 1 / 256\), approximately equal to 2.7x. Considering the impact of cache, the benefit can be regarded as consistent.
-   With large package loading, the actual benefit is 25%+, consistent with the empirical value.

## Summary<a name="section7965192813710"></a>

The scenarios applicable to optimization point 1 and optimization point 2 require a sufficiently large shape and a sufficiently large data volume so that the cores can be fully partitioned and the optimal basic blocks can be enabled. In large-shape scenarios, MTE2 Bound operators can refer to the optimization methods in this case.
