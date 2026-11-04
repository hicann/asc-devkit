# Matmul Operator Optimization Tiling Strategy<a name="ZH-CN_TOPIC_0000001893038953"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-23T11:40:49.622Z pushedAt=2026-11-03T07:05:13.280Z nl=2 -->

## Case Introduction<a name="section1532131415369"></a>

This case performs performance analysis and optimization on the Matmul operator. The Matmul operator implements cube multiplication, which mainly includes data load and store pipelines and the cube compute pipeline.

Taking cube dimensions M = 4096, N = 5120, K = 4096, input data type half, output data type float, and output format ND as an example, with the Atlas A2 products as the performance verification platform, this case introduces the optimization methods for the Matmul operator, including optimizing the core partitioning logic and base blocks as well as enabling large package moving.

-   Optimizing core partitioning logic: Enable as many cube cores as possible to implement parallel compute.
-   Optimizing base blocks: Select the optimal **baseM**, **baseN**, and **baseK** parameters, where **baseM**, **baseN**, and **baseK** are parameters in Matmul Tiling.
-   Enabling large package moving: When moving data from GM to L1, for cube A, load depthA1 base blocks at a time, with a base block size of baseM \* baseK; for cube B, load depthB1 base blocks at a time, with a base block size of baseN \* baseK. After enabling large package moving, the data volume loaded at one time increases, thereby improving the MTE2 moving efficiency.

## Obtaining Performance Data<a name="section327528143615"></a>

Use the msOpProf tool to obtain the Profiling data of the operator, focusing on analyzing the pipeline status of the MTE2, Cube, and Scalar pipelines.

## Analyzing the Main Bottleneck<a name="section788333916369"></a>

**Figure 1**  Profiling data before optimization<a name="fig136871118111317"></a>  
![](../../../figures/prof_before.png "Profiling data before optimization")

From the preceding Profiling data, it can be seen that MTE2 accounts for a large proportion of the time, and the current performance bottleneck lies in the MTE2 pipeline.

-   The **Block Dim** in the Profiling data shows that the cores are not fully split, so optimization of the core partitioning logic should be considered. Let **CurrentCore** be the number of cube cores before core partitioning optimization, and **MaxCore** be the maximum number of cube cores. When all cores are enabled to compute the current shape data volume in parallel, the estimated performance benefit is a multiple of **MaxCore** / **CurrentCore**.
-   Optimizing the base block splitting will affect the efficiency of moving data. The total data volume moved by the operator is the sum of the data volumes of the left cube and the right cube moved. In the scenario where the K direction cannot be fully loaded in Matmul computation, according to the cube multiplication algorithm, the number of times the left cube is moved is N / baseN, and the number of times the right cube is moved is M / baseM. That is, the total moved data volume totalCnt = \(N / baseN\) \* M \* K + \(M / baseM\) \* K \* N. The estimated performance benefit is the ratio of the moved data volumes, that is, the moved data volume before optimization totalCnt0 / the moved data volume after optimization totalCnt1. After simplification, the result is \(1 / baseM0 + 1 / baseN0\) / \(1 / baseM1 + 1 / baseN1\), where baseM0 and baseN0 are the base block parameters before optimization, and baseM1 and baseN1 are the base block parameters after optimization.

-   After enabling large package moving, factors such as changes in the number of instructions and address alignment will affect performance. Based on experience, for scenarios where MTE2 is the performance bottleneck, there will be an MTE2 performance benefit of more than 20%.

## Designing the Optimization Solution<a name="section12959175753610"></a>

-   Optimization point 1: Optimize the core partitioning logic.

    The Profiling data shows that the number of split cores is 4. Starting more cores to compute simultaneously can improve the compute parallelism. The AI processor used in this case has 20 cores in total, and each core contains one cube core and two vector cores. In the NPU calling program, set **numBlocks** to **20**, which is the actual number of cores used.

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

    Since Matmul APIs are all initiated from the vector side, and the ratio of cube cores to vector cores in the AI processor used in this case is 1 : 2, the Matmul tiling computation needs to split data by twice the **numBlocks**, that is, by the number of vector cores. The actual number of running cores set in the NPU calling program is 20, so the Tiling code sets the Tiling API to split data by 40 cores, as shown in the following code.

    ```
    int usedCoreNum = 40; // Before optimization, usedCoreNum is 8
    int runMode = 1;
    int32_t baseM = 64; // 64
    int32_t baseN = 64; // 64
    optiling::TCubeTiling tilingData;
    MultiCoreMatmulTiling tilingApi;
    tilingApi.SetDim(usedCoreNum);
    ```

    **Figure 2**  Profiling data after optimizing the core partitioning logic<a name="fig3811154518136"></a>  
    ![](../../../figures/opt_split_prof.png "Profiling data after optimizing the core partitioning logic")

    After the code is modified, the operator execution time drops from 12045 us to 2532 us, which is approximately equal to \(20 cores / 4 cores \) = a 5x performance improvement.

-   Optimization point 2: Optimize the base block.

    The base block set in the current Tiling is \[baseM, baseN, baseK\] = \[64, 64, 256\]. This base block has few cube compute cycles and a low compute-memory access ratio (the ratio of the compute volume to the required data volume). The base block for moving out one Matmul result to GM is 64 \* 64. Since the output format is ND and the data type is float, the start address for moving out the next Matmul result needs to be offset by the size of one baseN, that is, 64 \* 4 = 256 bytes. As a result, the GM address is not 512-byte aligned when fixpipe moves out data, so a better base block needs to be set.

    For the current scenario with a large shape, the principle for selecting the base block is to maximize the compute-memory access ratio, that is, to minimize the data volume accessed while maximizing the Cube compute volume. When the input is of the fp16 type, the Cube execution unit can compute 16 \* 16 \* 16 numbers in one cycle. Based on experience, both \[baseM, baseN, baseK\] = \[128, 256, 64\] and \[128, 128, 128\] splitting schemes satisfy [512-byte GM address alignment](../../simd_operator_optimization/memory_access/gm_address_512b_alignment.md) when moving out data (each time a Matmul result is moved out, the address is offset by 256 \* 4 bytes and 128 \* 4 bytes, respectively), and the number of Cube compute cycles is the same, which is \(128 \* 64 \* 256\) / \(16 \* 16 \* 16\) = \(128 \* 128 \* 128\) / \(16 \* 16 \* 16\) = 512 cycles. For \[baseM, baseN, baseK\] = \[128, 256, 64\], the compute-memory access ratio is 512 cycles / \(128 \* 64 \* 2 + 256 \* 64 \* 2\) = 512 cycles / 48 KB. For \[baseM, baseN, baseK\] = \[128, 128, 128\], the compute-memory access ratio is 512 cycles / \(128 \* 128 \* 2 + 128 \* 128 \* 2\) = 512 cycles / 64 KB. It can be seen that the \[128, 256, 64\] base block scheme has a higher compute-memory access ratio and higher compute density. For the same compute volume, it requires the minimum data volume, maximizing the compute volume of the Cube unit.

    Modify the Tiling code to set **baseM** and **baseN** through the **SetFixSplit\(\)** API. The tiling function automatically calculates the optimal **baseK**, which is 64 here.

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

    After using this set of base blocks, the MTE2 time (corresponding to **aic\_mte2\_time**) decreases from 2452 us to 808 us, improving MTE2 performance by 3 times.

    **Figure 3**  Profiling data after base block optimization<a name="fig1012052281415"></a>  
    ![](../../../figures/opt_bb_prof.png "Profiling data after base block optimization")

-   Optimization point 3: Enable large copy.

    The current bandwidth utilization is: totalSize / mte2Time = totalCnt \* dtype / mte2Time, which is calculated as 2491 GB/s by substituting the data. When large copy is not enabled, only one base block is moved from GM to L1 at a time. Enable large copy through a template parameter to move multiple base blocks at a time, improving the MTE2 bandwidth utilization.

    ```
     // Original matmul object definition:
      Matmul<AscendC::MatmulType<TPosition::GM, CubeFormat::ND, A_T>,
             AscendC::MatmulType<TPosition::GM, CubeFormat::ND, B_T>,
             AscendC::MatmulType<TPosition::GM, CubeFormat::ND, C_T>,
             AscendC::MatmulType<TPosition::GM, CubeFormat::ND, BiasT>>>
          mm;
     // Enable the large package move feature by adding the CFG_MDL parameter to the template parameters of the matmul object definition:
      Matmul<AscendC::MatmulType<TPosition::GM, CubeFormat::ND, A_T>,
             AscendC::MatmulType<TPosition::GM, CubeFormat::ND, B_T>,
             AscendC::MatmulType<TPosition::GM, CubeFormat::ND, C_T>,
             AscendC::MatmulType<TPosition::GM, CubeFormat::ND, BiasT>, CFG_MDL>>
          mm;
    ```

    As shown in the following figure, after the large package move is enabled, the MTE2 time decreases from 808 us to 591 us, and the bandwidth utilization, calculated by substituting the data, is 3406 GB/s, an improvement of more than 36%. The cube utilization reaches more than 80%.

    **Figure 4** Profiling data after the large package move is enabled<a name="fig13648142015171"></a>  
    ![](../../../figures/large_copy_prof.png "Profiling data after the large package move is enabled")

## Verifying the Performance Benefits of the Optimization Solution<a name="section225561133715"></a>

-   Optimizing core partitioning logic: the actual benefit is 4.75x, approximately equal to \(20 cores / 4 cores \) = 5x. Considering the core startup overhead, the benefits can be regarded as consistent.
-   Optimizing base blocks: the actual benefit is about 3x. Substituting the theoretical evaluation into the analysis formula above, the benefit is \(1 / 64 + 1 / 64\) / \(1 / 128 + 1 / 256\), approximately equal to 2.7x. Considering the impact of cache, the benefits can be regarded as consistent.
-   Enabling large package moving: the actual benefit is 25%+, consistent with the empirical value.

## Summary<a name="section7965192813710"></a>

The applicable scenarios of optimization point 1 and optimization point 2 require a sufficiently large shape and a sufficiently large data volume to fully split the cores and enable the optimal base blocks. In large-shape scenarios, MTE2 Bound operators can refer to the optimization methods in this case.
