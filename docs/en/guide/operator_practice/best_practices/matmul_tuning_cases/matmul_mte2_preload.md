# Enabling MTE2 Preload for the Matmul Advanced API<a name="ZH-CN_TOPIC_0000002360914053"></a>

<!-- md-trans-meta sourceCommit=dd293a1c6b873d149bf289bb2b8098dadf9f5cbd translatedAt=2026-10-23T11:31:15.489Z pushedAt=2026-11-03T07:05:13.274Z nl=2 -->

## Case Introduction<a name="section17413194624510"></a>

This case demonstrates the performance improvement achieved by enabling MTE2 Preload when using the Matmul advanced API for cube multiplication in a cube multiplication operator scenario. The **doMTE2Preload** parameter in **MatmulConfig** enables the preloading function in the M or N direction of the cube. Preloading means loading cube A/cube B data in advance during MTE2 gaps. After the preloading function is enabled, MTE2 gaps are reduced and operator performance is improved. For details about the **doMTE2Preload** parameter, see [MatmulConfig](../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/MatmulConfig.md).

-   Scenarios for enabling MTE2 Preload

    When the MTE2 pipeline gap is large and the M or N value is large.

-   Constraints for enabling MTE2 Preload
    -   MTE2 Preload takes effect only when the MDL template and SpecialMDL template are used.
    -   When the preloading function in the M or N direction is enabled, ensure that the K-direction data is fully loaded and DoubleBuffer is enabled in the M or N direction.
    -   The condition for fully loading K-direction data is singleK <= baseK \* stepK.
    -   The condition for enabling DoubleBuffer in the M direction is depthA1 = stepM \* stepK \* 2.
    -   The condition for enabling DoubleBuffer in the N direction is depthB1 = stepN \* stepK \* 2.

The operator specifications of this case are as follows:

**Table 1** Operator specifications

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

The AI processor used in this case has 24 cores in total, and the operator enables the cube-only mode of the advanced Matmul API. The MDL template is used, and the Tiling parameters are as follows:

-   Original shape: M=128, N=24576, K=512.
-   Single-core shape: singleCoreM=128, singleCoreN=1024, singleCoreK=512.
-   Basic block shape: baseM=128, baseN=128, baseK=64.
-   L1 cache-related Tiling parameters: stepM=1, stepN=1, stepKa=8, stepKb=8, depthA1=8, depthB1=16.

## Obtaining Performance Data<a name="section851404010469"></a>

Use the msOpProf tool to obtain the [operator simulation pipeline diagram](../../performance_analysis/collect_performance_data.md#section17259539153513) and [on-board Profiling](../../performance_analysis/collect_performance_data.md#section17953123893415) data, focusing on the pipeline status of Cube and Fixpipe.

## Analyzing the Main Bottleneck<a name="section221431704714"></a>

-   The following pipeline diagram shows the state before optimization. The M and K dimensions are fully loaded, so cube A is transferred only once. Because N is large, cube B is transferred multiple times, and gaps can be observed between individual MTE2 operations. ![](../../../figures/zh-cn_image_0000002394102925.png)
-   The following Profiling data shows the state before optimization. The average duration (**aic\_time**) is 30.88 us. ![](../../../figures/zh-cn_image_0000002394115169.png)

## Designing an Optimization Solution<a name="section33901368431"></a>

Enable the MTE2 Preload feature: when creating a Matmul object, enable the **doMTE2Preload** switch. For a complete sample of enabling MTE2 Preload, see [M-direction Preload Matmul Operator Sample](../../../../../../examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_preload). The specific steps are as follows:

1.  Configure the MDL template parameters and set the **doMTE2Preload** parameter to **2** to enable the N-direction Preload feature.

    ```
     // preloadMode = 2
    static constexpr MatmulConfig MM_CFG = GetMDLConfig(false, false, preloadMode); 
    ```

2.  Create a Matmul object based on the custom **MatmulConfig** template parameters.

    ```
    AscendC::Matmul<AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, aType>,
        AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, bType>,
        AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, cType>,
        AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, biasType>, MM_CFG> matmulObj;
    ```

## Verifying the Performance Gains of the Optimization Solution<a name="section19022397498"></a>

-   The optimized pipeline diagram is as follows. The Tiling parameters remain unchanged. As shown, the cube data of cube B used in the next computation is loaded in advance, shortening the gap between MTE2 operations. ![](../../../figures/zh-cn_image_0000002394160225.png)
-   The optimized Profiling data is as follows. The average duration (**aic\_time**) is 28.50 us, which is lower than the 30.88 us before optimization. ![](../../../figures/zh-cn_image_0000002394120757.png)

## Summary<a name="section8281219125011"></a>

When the MTE2 pipeline gap is large and M or N is large, you can enable MTE2 Preload to load cube A or cube B data in advance.
