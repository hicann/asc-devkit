# Enabling the NBuffer33 Template for the Matmul Advanced API<a name="ZH-CN_TOPIC_0000002327075450"></a>

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-10-23T11:36:48.452Z pushedAt=2026-11-03T07:05:13.278Z nl=2 -->

## Case Introduction<a name="section17413194624510"></a>

This case demonstrates the performance improvement achieved by using the Matmul advanced API for cube multiplication in a cube multiplication operator scenario and enabling the NBuffer33 template. The NBuffer33 template is implemented by splitting the Cube A of single-core computation into 3x3 basic blocks. These 3x3 basic blocks of the cube A are fully loaded and kept in the L1 Buffer, and each time cube multiplication is performed with 3x1 basic blocks of the cube B. Meanwhile, DoubleBuffer loads the 3x1 basic blocks of the cube B required for the next computation in parallel, until the cube multiplication in the singleCoreN direction is completed. For MTE2 Bound scenarios, the NBuffer33 algorithm splits the data to stagger the data movement pipeline, reduce the amount of data moved in a single operation, and balance the data traffic between MTE2 and FixPipe so that the bandwidth of both is evenly distributed. For details about the NBuffer33 template, see [MatmulPolicy](../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/MatmulPolicy.md).

-   Scenarios for enabling the NBuffer33 template

    In MTE2 Bound scenarios, the NBuffer33 template can be enabled when the Tiling parameters meet the constraints.

-   Constraints for enabling the NBuffer33 template
    -   Only the MDL template is supported for MatmulConfig.
    -   The logical memory positions of the A and B cubes support only TPosition::GM.
    -   Only the cube-only mode (cube computation only) is supported. The MIX mode (including both cube computation and vector computation) is not supported.
    -   The cube C result of Matmul can be obtained only through the IterateAll API.
    -   stepM, stepKa, and stepKb must be less than or equal to 3, and the following condition must be met: stepKa=stepKb=Ceil\(singleCoreK/baseK\).
    -   The sum of the basic block size for fully loading cube A and the basic block size for loading cube B does not exceed the L1 Buffer size.

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

The AI processor used in this case has 24 cores. The cube-only mode of the advanced Matmul API is enabled in the operator, using the MDL template. The Tiling parameters are as follows:

-   Original shape: M=256, N=512, K=192.
-   Single-core shape: singleCoreM=256, singleCoreN=256, singleCoreK=192.
-   Basic block shape: baseM=128, baseN=256, baseK=64.
-   L1 cache-related Tiling parameters: stepM=2, stepN=1, stepKa=3, stepKb=3.

## Obtaining Performance Data<a name="section851404010469"></a>

Use the msOpProf tool to obtain the [operator simulation pipeline diagram](../../performance_analysis/collect_performance_data.md#section17259539153513) and [on-board profiling](../../performance_analysis/collect_performance_data.md#section17953123893415) data, focusing on the pipeline status of Cube and Fixpipe.

## Analyzing the Main Bottleneck<a name="section221431704714"></a>

-   The pipeline diagram before optimization is as follows. Under the default template of **MatmulPolicy**, cubes A and B are fully loaded, and both cubes are moved only once. In this case, MTE2 takes a long time to execute, and the entire pipeline runs serially.

    ![](../../../figures/zh-cn_image_0000002354202658.png)

-   The Profiling data before optimization is as follows, **aic\_time** with an average duration of 34.01 us. ![](../../../figures/zh-cn_image_0000002355276604.png)

## Designing an Optimization Solution<a name="section33901368431"></a>

Enable the NBuffer33 template: Before calling the **GetTiling** API, call the **SetMatmulConfigParams** API to enable the NBuffer33 mode so that the obtained Tiling meets the requirements. On the kernel side, enable the NBuffer33 template when creating the Matmul object. For a complete sample of enabling the NBuffer33 template, see [Sample of Enabling the NBuffer33 Template Policy](../../../../../../examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_nbuffer33). The specific steps are as follows:

-   Tiling implementation

    Enable the NBuffer33 mode before calling the **GetTiling** API to obtain the **TCubeTiling** structure.

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

    Set the template parameter **MatmulPolicy** to the NBuffer33 template policy and create the Matmul object.

    ```
    AscendC::Matmul<
        AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, AType, IS_TRANS_A>,
        AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, BType, IS_TRANS_B>,
        AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, CType>,
        AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, BiasType>, CFG_MDL,
        AscendC::MatmulCallBackFunc<nullptr, nullptr, nullptr>, AscendC::Impl::Detail::NBuffer33MatmulPolicy>
        matmulObj;
    ```

## Verifying the Performance Gains of the Optimization Solution<a name="section19022397498"></a>

-   The optimized pipeline is shown in the following figure. The Tiling parameters remain unchanged, but because stepM is 2, the NBuffer33 mode splits the data transfer of the left cube into two parts. As shown, after the first MTE2 completes, the computation process (including MTE1, MMAD, and FIXPIPE) can run in parallel with the second MTE2. Transferring data in blocks reduces part of the head overhead caused by a single data transfer, thereby optimizing the data loading performance.

    ![](../../../figures/zh-cn_image_0000002447772589.png)

-   The optimized Profiling data is as follows. The average duration (**aic\_time**) is 32.66 us, which is improved compared with 34.01 us before optimization. ![](../../../figures/zh-cn_image_0000002388916821.png)

## Summary<a name="section8281219125011"></a>

In MTE2-bound scenarios, when the Tiling parameters satisfy the condition that stepM, stepKa, and stepKb are all less than or equal to 3, you can consider enabling the NBuffer33 template to split the cube and stagger the data movement pipeline, thereby reducing the amount of data moved in a single transfer and balancing the data traffic between MTE2 and FixPipe.
