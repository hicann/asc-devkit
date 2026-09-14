# Matmul Advanced API Enabling Full Constantization of tiling<a name="ZH-CN_TOPIC_0000002374865629"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-26T13:21:13.208Z -->

## Case Introduction<a name="section12231144316533"></a>

This case demonstrates the performance improvement of an operator achieved by enabling full constantization of Matmul tiling when using the Matmul advanced API for matrix multiplication. The Matmul API involves a large amount of Scalar computation during initialization and iteration. The Scalar computation at Matmul initialization affects the instruction header overhead, and the Scalar computation between Matmul iterations may block the MTE2 pipeline. When calling the Matmul API to implement matrix multiplication, use the [MatmulApiStatictiling](../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Kernel/matmul_template_parameters.md) parameter instead of the TCubetiling variable parameter to move the Scalar computation to the compile time, thereby reducing the Scalar computational overhead at runtime and improving operator performance.

-   Scenarios where Matmul tiling constant applies:
    -   The Scalar computation at Matmul initialization is compute-intensive and affects the instruction header overhead.
    -   There are many scalar computations between Matmul iterations, blocking the MTE2 pipeline.

-   Matmul tiling constantization requires some tiling parameters to be determined at compile time. Depending on the parameters determined, it is divided into two scenarios: full constantization and partial constantization. To use Matmul tiling constantization, the condition of either scenario must be met:

    -   <a name="li159920538152"></a>Full constantization: the constant singleCore Shape (singleCoreM/singleCoreN/singleCoreK) and the constant base Shape (basicM/basicN/basicK, also called baseM/baseN/baseK) can be determined.

    -   <a name="li98655581816"></a>Partial constantization: the constant base Shape (basicM/basicN/basicK, also called baseM/baseN/baseK) can be determined.

    Among them, the full constantization scenario can reduce more Scalar computational overhead than the partial constantization scenario.

The operator specifications in this case are as follows:

**Table 1** Operator specifications

<a name="table101751125175213"></a>
<table><thead align="left"><tr id="row8175525185219"><th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.1"><p id="p1417582516529"><a name="p1417582516529"></a><a name="p1417582516529"></a>Input</p></th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.2"><p id="p417532575212"><a name="p417532575212"></a><a name="p417532575212"></a>Shape</p></th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.3"><p id="p1017582585214"><a name="p1017582585214"></a><a name="p1017582585214"></a>Data Type</p></th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.4"><p id="p317502512522"><a name="p317502512522"></a><a name="p317502512522"></a>Format</p></th>
</tr>
</thead>
<tbody><tr id="row217562525215"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p517515253529"><a name="p517515253529"></a><a name="p517515253529"></a>a</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p141751425165213"><a name="p141751425165213"></a><a name="p141751425165213"></a>128, 64</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p1517522515218"><a name="p1517522515218"></a><a name="p1517522515218"></a>float16</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p16176725105210"><a name="p16176725105210"></a><a name="p16176725105210"></a>ND</p></td>
</tr>
<tr id="row10176102512525"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p161761325185216"><a name="p161761325185216"></a><a name="p161761325185216"></a>b</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p4176112555211"><a name="p4176112555211"></a><a name="p4176112555211"></a>64, 30720</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p1176132515212"><a name="p1176132515212"></a><a name="p1176132515212"></a>float16</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p817692575212"><a name="p817692575212"></a><a name="p817692575212"></a>ND</p></td>
</tr>
</tbody>
</table>

The AI processor used in this case has 24 cores in total, each containing one AIC core and two AIV cores.

The tiling parameters are as follows:

-   Original shape: M=128, N=30720, K=64.
-   Single-core shape: split across 24 AIC cores, with singleCoreM=128, singleCoreN=1280, and singleCoreK=64.

    For matrix B, it is split along the N axis into 24 singleCoreN partitions, and each core processes data of size K \* singleCoreN. For matrix A, the M axis is not split, that is, singleCoreM=M, and each core processes data of size singleCoreM \* K. A total of 24 cores participate in the computation.

-   Basic block shape: baseM=128, baseN=256, baseK=64.
-   L1-related tiling parameters: stepM=1, stepN=1, stepKa=4, stepKb=4, depthA1=8, depthB1=8.

## Obtaining Performance Data<a name="section1910315281533"></a>

Use the msOpProf tool to obtain the [operator simulation pipeline diagram](../../performance_analysis/obtain_performance_data.md#section17259539153513) and [on-board Profiling](../../performance_analysis/obtain_performance_data.md#section17953123893415) data. Compared with the basic scenario, tiling constantization converts some or all tiling parameters from variables to constant values at compile time, and directly uses the constant-folded tiling parameters during operator execution, which reduces the Scalar performance overhead. Therefore, focus on analyzing the Scalar pipeline.

## Analyzing the Main Bottleneck<a name="section93975169548"></a>

-   The pipeline diagram before optimization is as follows. tiling constantization is disabled by default, so the tiling parameters need to be copied from the Host side to the Kernel side. This causes a large amount of Scalar computation during Matmul initialization. The first MTE2 instruction starts at about 3.536us, and the instruction header overhead before MTE2 accounts for a large proportion of the entire operator pipeline. Therefore, the Scalar computation needs to be optimized.

    ![](../../../figures/zh-cn_image_0000002411422713.png)

-   The Profiling data before optimization is as follows. From the aic_time data in column C, the maximum operator execution time among multiple cores is 10.62us. From the aic_scalar_time data in column G, the average Scalar time is 6.32us.

    ![](../../../figures/zh-cn_image_0000002411018580.png)

## Designing an Optimization Solution<a name="section10569929145417"></a>

As shown in the following figure, when tiling constantization is disabled by default, the developer creates a tiling object on the host side and automatically obtains tiling parameters by calling the API. The tiling parameters are then passed from the host side to the kernel side and passed in at initialization on the kernel side. During operator execution, the tiling variable parameters are used to complete the matrix multiplication operation.

**Figure 1**  Matmul computation flow with tiling constantization disabled by default<a name="fig1911911419426"></a>  
![](../../../figures/matmul_computation_flow_with_tiling_constant_disabled_by_default.png "Matmul computation flow with tiling constantization disabled by default")

As shown in the following figure, when tiling constantization is enabled, the developer only needs to call the GetMatmulApitiling API to obtain the constant-folded tiling information at compile time when creating the Matmul object on the kernel side, thereby completing tiling constantization. During operator execution, the constant-folded tiling parameters are used to complete the matrix multiplication operation, reducing the Scalar computational overhead.

**Figure 2**  Matmul computation flow with tiling constantization enabled<a name="fig146371949194314"></a>  
![](../../../figures/matmul_computation_flow_with_tiling_constant_ensabled.png "Matmul computation flow with tiling constantization enabled")

For the complete sample of enabling full tiling constantization for the Matmul API, see the [Matmul tiling constantization operator sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_constant_tiling). The procedure for enabling full tiling constantization is as follows:

1.  When calling the GetMMConfig API to obtain the MatmulConfig template, use constant values to set MatmulShapeParams to obtain the custom MatmulConfig template CUSTOM\_CFG with constant-folded parameters.

    ```
    constexpr int32_t MAX_M = 10000; // custom matmul kernel support max value of M Dim shape
    constexpr int32_t MAX_N = 10000; // custom matmul kernel support max value of N Dim shape
    constexpr int32_t MAX_K = 10000; // custom matmul kernel support max value of K Dim shape
    constexpr int32_t BASE_M = 128;  // BASE_M * BASE_K * sizeof(typeA) <=L0A size
    constexpr int32_t BASE_N = 256;  // BASE_N * BASE_K * sizeof(typeB) <=L0B size
    constexpr int32_t BASE_K = 64;   // BASE_M * BASE_N * sizeof(typeC) <=L0C size
    constexpr MatmulShapeParams shapeParams = { MAX_M,
                                                MAX_N,
                                                MAX_K,
                                                BASE_M,
                                                BASE_N,
                                                BASE_K };
    constexpr MatmulConfig CUSTOM_CFG = GetMMConfig<MatmulConfigMode::CONFIG_MDL>(shapeParams);
    ```

2.  Create the Matmul object. First call the GetMatmulApitiling API to fold the tiling information into constants and obtain the constant-folded template parameter CONSTANT\_CFG, which includes the constant-folded Matmul tiling information and the MatmulConfig template. When creating the Matmul object, use the constant-folded template parameter CONSTANT\_CFG.

    ```
    using A_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, aType>;
    using B_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, bType>;
    using C_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, cType>;
    using BIAS_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, biasType>;
    constexpr static auto CONSTANT_CFG = AscendC::GetMatmulApitiling<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE>(CUSTOM_CFG);
    AscendC::Matmul<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, CONSTANT_CFG> matmulObj;
    ```

3.  Initialize the operation. In the full constantization scenario, a null pointer can be used in place of the tiling parameter in the input of the REGIST\_MATMUL\_OBJ API. In the partial constantization scenario, tiling is still required when initializing the Matmul object using the REGIST\_MATMUL\_OBJ API on the kernel side.

    ```
    // Example of initialization in the full constantization scenario
    REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), matmulObj, (TCubetiling*)nullptr);
    
    // Example of initialization in a partial constantization scenario
    REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), matmulObj, &tiling);
    ```

## Verifying the Performance Gains of the Optimization Solution<a name="section7519174385413"></a>

-   The optimized pipeline diagram is as follows. By enabling full constantization of tiling, there is no need to copy tiling parameters from the Host side to the Kernel side. tiling constantization is completed at compile time, which reduces the Scalar computation during Matmul initialization. The time from 0 us to the initiation of the first MTE2 instruction is the Matmul initialization time, which is reduced from 3.536 us before optimization to 2.185 us, indicating a performance improvement.

    ![](../../../figures/zh-cn_image_0000002377921500.png)

-   The optimized profiling data is as follows. From the aic_time data in column C, the maximum operator execution time among multiple cores is 7.87 us, which is 25.9% faster than the 10.62 us before optimization. From the aic_scalar_time data in column G, the average Scalar time is 3.38 us, which is 46.5% faster than the 6.32 us before optimization.

    ![](../../../figures/zh-cn_image_0000002411019880.png)

## Summary<a name="section252011820413"></a>

When an operator calls the Matmul API to perform matrix multiplication, if there is a large amount of scalar computation during Matmul initialization and the instruction header overhead is affected, or if there are many scalar computations between Matmul iterations and the MTE2 pipeline is blocked, you can consider enabling tiling constantization to reduce the Scalar computational overhead and improve operator performance, provided that the tiling constantization conditions described above are met ([full constantization](#li159920538152) or [partial constantization](#li98655581816)).
