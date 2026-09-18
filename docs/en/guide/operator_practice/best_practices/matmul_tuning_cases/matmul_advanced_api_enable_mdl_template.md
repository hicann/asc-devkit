# Matmul Advanced API Enabling MDL Template<a name="ZH-CN_TOPIC_0000002374905781"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-26T13:18:32.691Z -->

## Case Introduction<a name="section5590915836"></a>

This case demonstrates the performance improvement achieved by enabling the MDL template when using the Matmul advanced API for matrix multiplication in a matrix multiplication operator scenario. In the MDL template, the MTE2 pipeline transfers data from Global Memory to A1/B1 in a single large packet, that is, one MTE2 transfer can move multiple base blocks for Matmul computation, improving bandwidth utilization. This allows the subsequent MTE1 pipeline to reuse the cached data of base blocks in A1/B1 as much as possible, reducing the transfer count of MTE2. For details about the MDL template, see [MatmulConfig](../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Kernel/MatmulConfig.md).

-   Applicable scenarios of the MDL template

    The MDL template is generally applicable to large-shape scenarios where the MTE2 loop transfer count is high. It caches the data required for multiple computations in A1/B1, avoiding frequent MTE2 transfers.

-   Constraints of the MDL template

    The TCubeTiling struct of the MDL template must meet the TCubeTiling constraints and the supplementary constraints of the MDL template. For details, see [TCubeTiling struct](../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Tiling/tcubetiling_structure.md).

The operator specifications in this case are as follows:

**Table 1** Operator specifications

<a name="table9129161439"></a>
<table><thead align="left"><tr id="row4121116114317"><th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.1"><p id="p1712111610439"><a name="p1712111610439"></a><a name="p1712111610439"></a>Input</p></th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.2"><p id="p15129162437"><a name="p15129162437"></a><a name="p15129162437"></a>Shape</p></th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.3"><p id="p31231684318"><a name="p31231684318"></a><a name="p31231684318"></a>Data Type</p></th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.4"><p id="p61211694311"><a name="p61211694311"></a><a name="p61211694311"></a>Format</p></th>
</tr>
</thead>
<tbody><tr id="row212716144319"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p61219167439"><a name="p61219167439"></a><a name="p61219167439"></a>a</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p17124161434"><a name="p17124161434"></a><a name="p17124161434"></a>128, 1024</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p312131614432"><a name="p312131614432"></a><a name="p312131614432"></a>float16</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p81213165432"><a name="p81213165432"></a><a name="p81213165432"></a>ND</p></td>
</tr>
<tr id="row49815284318"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p49845284319"><a name="p49845284319"></a><a name="p49845284319"></a>b</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p9419175534316"><a name="p9419175534316"></a><a name="p9419175534316"></a>1024, 30720</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p6419195512430"><a name="p6419195512430"></a><a name="p6419195512430"></a>float16</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p12420205594320"><a name="p12420205594320"></a><a name="p12420205594320"></a>ND</p></td>
</tr>
</tbody>
</table>

The AI processor used in this case has 24 cores in total, and each core contains one AIC core and two AIV cores.

The Tiling parameters are as follows:

-   Original shape: M=128, N=30720, K=1024.
-   Single-core shape: split across 24 AIC cores, with singleCoreM=128, singleCoreN=1280, and singleCoreK=1024.

    For matrix B, it is split along the N axis into 24 singleCoreN segments, and each core processes data of the size K \* SingleCoreN. For matrix A, the M axis is not split, that is, singleCoreM=M, and each core processes data of the size singleCoreM \* K. A total of 24 cores participate in the computation.

-   Base block shape: baseM=128, baseN=256, baseK=64.
-   L1-related Tiling parameters: stepM=1, stepN=1, stepKa=4, stepKb=4, depthA1=8, depthB1=8.

## Obtaining Performance Data<a name="section443613301436"></a>

Use the msOpProf tool to obtain the [operator simulation pipeline diagram](../../performance_analysis/obtain_performance_data.md#section17259539153513) and [on-board profiling](../../performance_analysis/obtain_performance_data.md#section17953123893415) data. Because the MDL template mainly optimizes MTE2 transfer efficiency, focus on analyzing the MTE2 pipeline.

## Analyzing the Main Bottleneck<a name="section10759361232"></a>

-   The profiling data before optimization is as follows. Matmul uses the Norm template by default. As shown by the aic\_time data in column C, the maximum operator execution time among multiple cores is 83.68us. Based on the aic\_time data in column C, the aic\_mte2\_time data in column L, and the aic\_mte2\_ratio data in column M, the average MTE2 time is 75.64us, accounting for more than 92% of the total time. Therefore, the MTE2 pipeline time needs to be optimized.

    ![](../../../figures/zh-cn_image_0000002444614021.png)

-   The pipeline diagram before optimization is as follows. MTE2 transfers base blocks from Global Memory to A1/B1 in multiple passes. Because the input matrix Shape is large, MTE2 performs many loop transfers, but each transfer moves only one base block, resulting in low bandwidth utilization and a long overall MTE2 transfer time. This in turn affects the subsequent MTE1 and MMAD pipelines, causing long synchronization wait times between pipelines. As shown in the red box, computing the first base block (baseM\*baseN) requires 16 MMAD instruction calls (singleCoreK/baseK=16). From the first MMAD instruction call on the left to the 16th MMAD instruction call on the right, the elapsed time is 10.899us, most of which is pipeline synchronization wait time.

    ![](../../../figures/zh-cn_image_0000002411385581.png)

## Designing an Optimization Solution<a name="section6251134218311"></a>

The following figure shows the Matmul computation pipeline schematic diagram of the default Norm template. MTE2 transfers base blocks from Global Memory to A1 or B1 in multiple passes, transferring only one base block at a time. The advantage of the Norm template is its low startup overhead, which allows the MTE1 pipeline to start early. The disadvantage of the Norm template is that in large-shape scenarios, MTE2 has a high transfer count and low transfer bandwidth utilization, resulting in high overall performance overhead.

**Figure 1**  Default Norm template pipeline schematic diagram<a name="fig108684561130"></a>  
![](../../../figures/default_norm_template_pipeline_diagram.png "Default Norm template pipeline schematic diagram")

The specific steps to implement the Norm template are as follows:

1.  Create a Matmul object using the default Norm template parameter CFG\_NORM.

    ```
    #define ASCENDC_CUBE_ONLY
    #include "lib/matmul_intf.h"
    
    using A_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, AType>;
    using B_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, BType>;
    using C_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, CType>;
    using BIAS_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, BiasType>;
    AscendC::Matmul<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, CFG_NORM> matmulObj; // Define a Matmul object using CFG_NORM
    ```

The following figure shows the Matmul computation pipeline schematic diagram of the MDL template. MTE2 transfers multiple base blocks from Global Memory to A1 or B1 in a single pass, transferring stepM \* stepKa base blocks to A1 or stepN \* stepKb base blocks to B1 each time. The advantage of the MDL template is that MTE2 transfers multiple base blocks in a single pass, achieving high bandwidth utilization. The subsequent MTE1 pipeline can reuse the cached data in A1 or B1 as much as possible, and MTE2 has a low repeated transfer count. The disadvantage of the MDL template is that MTE2 has a long header overhead time, and the MTE1 pipeline must wait for the MTE2 pipeline to complete before starting, so MTE1 starts late.

**Figure 2**  MDL template pipeline schematic diagram<a name="fig15563371352"></a>  
![](../../../figures/mdl_template_pipeline_diagram.png "MDL template pipeline schematic diagram")

For a complete example of enabling the MDL template for the Matmul API, see the [Matmul example with UnitFlag and MDL template enabled](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_unitflag). The main steps to enable the MDL template are as follows:

1.  Create a Matmul object using the default MDL template parameter CFG\_MDL.

    ```
    #define ASCENDC_CUBE_ONLY
    #include "lib/matmul_intf.h"
    
    using A_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, AType>;
    using B_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, BType>;
    using C_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, CType>;
    using BIAS_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, BiasType>;
    AscendC::Matmul<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, CFG_MDL> matmulObj; // Define the Matmul object using CFG_MDL.
    ```

## Verifying the Performance Gains of the Optimization Solution<a name="section36229589310"></a>

-   The optimized Profiling data is as follows. As shown by the aic\_time data in column C, the maximum operator execution time among multiple cores is 53.4us, a significant improvement over the 83.68us before optimization. As shown by the aic\_mte2\_time data in column L, the average MTE2 time decreases significantly, from 75.64us before optimization to 46.24us.

    ![](../../../figures/zh-cn_image_0000002444614873.png)

-   The optimized pipeline diagram is as follows. Compared with the default Norm template, the MDL template allows MTE2 to transfer multiple base blocks at a time, reducing the overall MTE2 transfer count. In addition, because MTE2 transfers multiple base blocks to A1/B1 at a time, the subsequent MTE1 pipeline can reuse the cached data in A1/B1 as much as possible, reducing pipeline synchronization waits and improving the overall operator performance. As shown in the red box, computing the first base block (baseM\*baseN) requires 16 MMAD instruction calls (singleCoreK/baseK=16). From the first MMAD instruction call on the left to the 16th MMAD instruction call on the right, it takes about 5.198us, a significant improvement over the 10.899us before optimization, with the pipeline synchronization wait time greatly reduced.

    ![](../../../figures/zh-cn_image_0000002411385293.png)

## Summary<a name="section1593812115413"></a>

In scenarios with large-shape input, a high MTE2 transfer count, and long synchronization wait time for the MTE1 pipeline waiting on the MTE2 pipeline, you can enable the MDL template. By having MTE2 transfer multiple base blocks from global memory to A1 or B1 at a time, the subsequent MTE1 pipeline can reuse the cached data in A1/B1 as much as possible, reducing the MTE2 transfer count and thereby improving operator performance.
