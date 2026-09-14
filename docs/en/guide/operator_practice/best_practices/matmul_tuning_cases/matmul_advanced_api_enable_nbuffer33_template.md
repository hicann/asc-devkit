# Matmul Advanced API Enabling NBuffer33 Template<a name="ZH-CN_TOPIC_0000002327075450"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-26T13:21:36.600Z -->

## Case Introduction<a name="section17413194624510"></a>

This case demonstrates the performance improvement of the operator achieved by using the Matmul advanced API for matrix multiplication computation and enabling the NBuffer33 template in the matrix multiplication operator scenario. The NBuffer33 template is implemented by splitting matrix A for single-core computation into 3x3 basic blocks. The 3x3 basic blocks of matrix A are fully loaded and kept in the L1 Buffer. Each time, matrix multiplication is performed with 3x1 basic blocks of matrix B, while DoubleBuffer loads the 3x1 basic blocks of matrix B required for the next computation in parallel, until the matrix multiplication computation in the singleCoreN direction is complete. For MTE2 Bound scenarios, the NBuffer33 algorithm splits the data to stagger the transfer pipeline, reduce the data volume transferred at a time, and balance the data traffic between MTE2 and FixPipe, so that the bandwidth of the two is evenly distributed. For details about the NBuffer33 template, see [MatmulPolicy](../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Kernel/MatmulPolicy.md).

-   Scenarios for enabling the NBuffer33 template

    In MTE2 Bound scenarios, when the Tiling parameters meet the constraints, you can enable the NBuffer33 template.

-   Constraints for enabling the NBuffer33 template
    -   Only the MDL template is supported for MatmulConfig.
    -   The memory logical positions of matrices A and B support only TPosition::GM.
    -   Only the pure Cube mode (matrix computation only) is supported. The MIX mode (including matrix computation and vector computation) is not supported yet.
    -   The C matrix of the Matmul computation result can be obtained only through the IterateAll API.
    -   stepM, stepKa, and stepKb are less than or equal to 3, and stepKa = stepKb = Ceil\(singleCoreK/baseK\).
    -   The sum of the basic block size for loading the full A matrix and the basic block size for loading the B matrix does not exceed the L1 Buffer size.

The operator specifications in this case are as follows:

**Table 1**  Operator specifications

<a name="table101751125175213"></a>
<table><thead align="left"><tr id="row8175525185219"><th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.1"><p id="p1417582516529"><a name="p1417582516529"></a><a name="p1417582516529"></a>Input</p></th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.2"><p id="p417532575212"><a name="p417532575212"></a><a name="p417532575212"></a>Shape</p></th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.3"><p id="p1017582585214"><a name="p1017582585214"></a><a name="p1017582585214"></a>Data Type</p></th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.4"><p id="p317502512522"><a name="p317502512522"></a><a name="p317502512522"></a>Format</p></th>
</tr>
</thead>
<tbody><tr id="row217562525215"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p517515253529"><a name="p517515253529"></a><a name="p517515253529"></a>a</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p141751425165213"><a name="p141751425165213"></a><a name="p141751425165213"></a>256, 192</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p1517522515218"><a name="p1517522515218"></a><a name="p1517522515218"></a>float16</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p16176725105210"><a name="p16176725105210"></a><a name="p16176725105210"></a>ND</p></td>
</tr>
<tr id="row10176102512525"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p161761325185216"><a name="p161761325185216"></a><a name="p161761325185216"></a>b</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p4176112555211"><a name="p4176112555211"></a><a name="p4176112555211"></a>192, 512</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p1176132515212"><a name="p1176132515212"></a><a name="p1176132515212"></a>float16</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p817692575212"><a name="p817692575212"></a><a name="p817692575212"></a>ND</p></td>
</tr>
</tbody>
</table>

The AI processor used in this case has 24 cores. The operator enables the pure Cube mode of the advanced Matmul API and uses the MDL template. The Tiling parameters are as follows:

-   Original shape: M=256, N=512, K=192.
-   Single-core shape: singleCoreM=256, singleCoreN=256, singleCoreK=192.
-   Basic block shape: baseM=128, baseN=256, baseK=64.
-   L1 cache-related Tiling parameters: stepM=2, stepN=1, stepKa=3, stepKb=3.

## Obtaining Performance Data<a name="section851404010469"></a>

Use the msOpProf tool to obtain the [operator simulation pipeline diagram](../../performance_analysis/obtain_performance_data.md#section17259539153513) and [on-board profiling](../../performance_analysis/obtain_performance_data.md#section17953123893415) data, focusing on analyzing the pipeline status of Cube and Fixpipe.

## Analyzing the Main Bottleneck<a name="section221431704714"></a>

-   The following figure shows the pipeline before optimization. Under the default template of MatmulPolicy, matrices A and B are fully loaded, and each of them is moved only once. In this case, MTE2 takes a long time to execute, and the entire pipeline runs serially.

    ![](../../../figures/zh-cn_image_0000002354202658.png)

-   The following Profiling data is obtained before optimization. The average aic\_time is 34.01us. ![](../../../figures/zh-cn_image_0000002355276604.png)

## Designing an Optimization Solution<a name="section33901368431"></a>

Enable the NBuffer33 template: Before calling the GetTiling API, call the SetMatmulConfigParams API to enable the NBuffer33 mode so that the obtained Tiling meets the requirements. On the Kernel side, enable the NBuffer33 template when creating the Matmul object. For a complete sample of enabling the NBuffer33 template, see the [sample of enabling the NBuffer33 template policy](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_nbuffer33). The specific steps are as follows:

-   Tiling implementation

    Enable the NBuffer33 mode before calling the GetTiling API to obtain the TCubeTiling structure.

    ```
    matmul_tiling::MatmulConfigParams matmulConfigParams(1, false,
        matmul_tiling::ScheduleType::N_BUFFER_33, /* NBuffer33 mode */
        matmul_tiling::MatrixTraverse::NOSET, false);
    cubeTiling.SetMatmulConfigParams(matmulConfigParams);
    if (cubeTiling.GetTiling(tilingData) == -1) {
        std::cout << "Generate tiling failed." << std::endl;
        return {};
    }
    ```

-   Kernel implementation

    Set the template parameter MatmulPolicy to the NBuffer33 template policy and create the Matmul object.

    ```
    AscendC::Matmul<AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, AType, IS_TRANS_A>,
                    AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, BType, IS_TRANS_B>,
                    AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, CType>,
                    AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, BiasType>, CFG_MDL,
                    AscendC::MatmulCallBackFunc<nullptr, nullptr, nullptr>,
                    AscendC::Impl::Detail::NBuffer33MatmulPolicy>
        matmulObj;
    ```

## Verifying the Performance Gains of the Optimization Solution<a name="section19022397498"></a>

-   The optimized pipeline is shown in the following figure. The tiling parameters remain unchanged, but because stepM is 2, the NBuffer33 mode splits the transfer of the left matrix data into two parts. As shown, the computation process after the first MTE2 completes (including MTE1, MMAD, and FIXPIPE) can run in parallel with the second MTE2. Transferring data in blocks reduces part of the header overhead caused by a single transfer, thereby optimizing the data loading performance.

    ![](../../../figures/zh-cn_image_0000002447772589.png)

-   The optimized profiling data is shown in the following figure. The average aic\_time is 32.66us, an improvement over the 34.01us before optimization.![](../../../figures/zh-cn_image_0000002388916821.png)

## Summary<a name="section8281219125011"></a>

In MTE2-bound scenarios, when the Tiling parameters satisfy the condition that stepM, stepKa, and stepKb are all less than or equal to 3, you can consider enabling the NBuffer33 template. Splitting the matrix staggers the data transfer pipeline, reduces the amount of data transferred in a single operation, and balances the data traffic between MTE2 and FixPipe.
