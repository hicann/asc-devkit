# Matmul Advanced API Enabling MTE2 Preload<a name="ZH-CN_TOPIC_0000002360914053"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-26T13:19:10.145Z -->

## Case Introduction<a name="section17413194624510"></a>

This case demonstrates the performance improvement of the operator achieved by enabling MTE2 Preload when using the Matmul advanced API for matrix multiplication in the matrix multiplication operator scenario. The doMTE2Preload parameter in MatmulConfig enables the preload functionality in the M or N direction of the matrix. Preload means loading the matrix data of matrix A/B in advance during the MTE2 gap. After the preload functionality is enabled, the MTE2 gap can be reduced, thereby improving operator performance. For details about the doMTE2Preload parameter, see [MatmulConfig](../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Kernel/MatmulConfig.md).

-   Scenarios for enabling MTE2 Preload

    When the MTE2 pipeline gap is larger and the M or N value is larger.

-   Constraints for enabling MTE2 Preload
    -   MTE2 Preload takes effect only when the MDL template and SpecialMDL template are used.
    -   When the preload functionality in the M or N direction is enabled, ensure that the matrix data in the K direction is fully loaded and DoubleBuffer is enabled in the M or N direction.
    -   The condition for fully loading the matrix data in the K direction is singleK <= baseK \* stepK.
    -   The condition for enabling DoubleBuffer in the M direction is depthA1 = stepM \* stepK \* 2.
    -   The condition for enabling DoubleBuffer in the N direction is depthB1 = stepN \* stepK \* 2.

The operator specifications of this case are as follows:

**Table 1**  Operator specifications

<a name="table101751125175213"></a>
<table><thead align="left"><tr id="row8175525185219"><th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.1"><p id="p1417582516529"><a name="p1417582516529"></a><a name="p1417582516529"></a>Input</p></th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.2"><p id="p417532575212"><a name="p417532575212"></a><a name="p417532575212"></a>Shape</p></th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.3"><p id="p1017582585214"><a name="p1017582585214"></a><a name="p1017582585214"></a>Data Type</p></th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.4"><p id="p317502512522"><a name="p317502512522"></a><a name="p317502512522"></a>Format</p></th>
</tr>
</thead>
<tbody><tr id="row217562525215"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p517515253529"><a name="p517515253529"></a><a name="p517515253529"></a>a</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p141751425165213"><a name="p141751425165213"></a><a name="p141751425165213"></a>128, 512</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p1517522515218"><a name="p1517522515218"></a><a name="p1517522515218"></a>float16</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p16176725105210"><a name="p16176725105210"></a><a name="p16176725105210"></a>ND</p></td>
</tr>
<tr id="row10176102512525"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p161761325185216"><a name="p161761325185216"></a><a name="p161761325185216"></a>b</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p4176112555211"><a name="p4176112555211"></a><a name="p4176112555211"></a>512, 24576</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p1176132515212"><a name="p1176132515212"></a><a name="p1176132515212"></a>float16</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p817692575212"><a name="p817692575212"></a><a name="p817692575212"></a>ND</p></td>
</tr>
</tbody>
</table>

The AI processor used in this case has 24 cores in total, and the pure Cube mode of the advanced Matmul API is enabled in the operator. The MDL template is used, and the Tiling parameters are as follows:

-   Original shape: M=128, N=24576, K=512.
-   Single-core shape: singleCoreM=128, singleCoreN=1024, singleCoreK=512.
-   Basic block shape: baseM=128, baseN=128, baseK=64.
-   L1 cache-related Tiling parameters: stepM=1, stepN=1, stepKa=8, stepKb=8, depthA1=8, depthB1=16.

## Obtaining Performance Data<a name="section851404010469"></a>

Use the msOpProf tool to obtain the [operator simulation pipeline diagram](../../performance_analysis/obtain_performance_data.md#section17259539153513) and [on-board profiling](../../performance_analysis/obtain_performance_data.md#section17953123893415) data, focusing on analyzing the pipeline status of Cube and Fixpipe.

## Analyzing the Main Bottleneck<a name="section221431704714"></a>

-   The pipeline diagram before optimization is as follows. The M and K directions are fully loaded, so matrix A is transferred only once. Since N is larger, matrix B is transferred multiple times, and it can be seen that there are gaps between individual MTE2 operations. ![](../../../figures/zh-cn_image_0000002394102925.png)
-   The Profiling data before optimization is as follows. The average aic_time is 30.88 us. ![](../../../figures/zh-cn_image_0000002394115169.png)

## Designing an Optimization Solution<a name="section33901368431"></a>

Enable the MTE2 Preload functionality: When creating a Matmul object, enable the doMTE2Preload switch. For a complete sample of enabling MTE2 Preload, see the [M-direction preload Matmul operator sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_preload). The specific steps are as follows:

1.  Configure the MDL template parameters and set the doMTE2Preload parameter to 2 to enable the N-direction Preload functionality.

    ```
     // preloadMode = 2
    static constexpr MatmulConfig MM_CFG = GetMDLConfig(false, false, preloadMode); 
    ```

2.  Create a Matmul object based on the custom MatmulConfig template parameters.

    ```
    AscendC::Matmul<AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, aType>,
        AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, bType>,
        AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, cType>,
        AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, biasType>, MM_CFG> matmulObj;
    ```

## Verifying the Performance Gains of the Optimization Solution<a name="section19022397498"></a>

-   The optimized pipeline diagram is as follows. The tiling parameters remain unchanged. As shown, the B matrix data used in the next computation is loaded in advance, shortening the gap between MTE2 operations. ![](../../../figures/zh-cn_image_0000002394160225.png)
-   The optimized profiling data is as follows. The average aic_time is 28.50 us, down from 30.88 us before optimization. ![](../../../figures/zh-cn_image_0000002394120757.png)

## Summary<a name="section8281219125011"></a>

When the MTE2 pipeline gap is large and M or N is large, you can enable the MTE2 Preload functionality to load matrix A or matrix B data in advance.
