# Matmul Advanced API Enabling UnitFlag<a name="ZH-CN_TOPIC_0000002353876489"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-26T13:24:08.487Z -->

## Case Introduction<a name="section17413194624510"></a>

This case demonstrates the performance improvement achieved by enabling the UnitFlag feature when using the Matmul advanced API to perform Cube computation in a matrix multiplication operator scenario. The UnitFlag feature provides fine-grained memory-access-based synchronization between the MMAD computation instructions and the FIXPIPE data movement instructions in the AIC core, enabling the computation and movement pipelines to run in parallel. To enable the UnitFlag feature, set the enUnitFlag parameter in MatmulConfig to true. For details about the enUnitFlag parameter, see [MatmulConfig](../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Kernel/MatmulConfig.md).

-   Scenarios for enabling UnitFlag

    The MMAD pipeline and the FIXPIPE pipeline of the operator are executed serially. FIXPIPE waits for the MMAD computation to complete before moving the result out, and the time spent waiting for this instruction synchronization accounts for a high proportion of the overall operator execution time. In this scenario, you can enable the UnitFlag feature to gain the performance benefit of running the MMAD and FIXPIPE pipelines in parallel. If the original MMAD and FIXPIPE pipelines of the operator can be masked by other pipelines (for example, MTE2 Bound), the overall benefit of enabling the UnitFlag feature is minimal.

-   Constraints on enabling UnitFlag
    -   The UnitFlag feature supports only the Norm, IBShare, and MDL templates.
    -   When the UnitFlag feature is enabled, the operator cannot contain both the CO1 (L0C) to Global Memory movement-out pipeline and the A1 (L1) to Global Memory movement-out pipeline at the same time.
    -   When the UnitFlag feature is enabled, if the [L0C accumulation](../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Kernel/Iterate.md) feature is also enabled, multiple [Iterate](../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Kernel/Iterate.md) computations followed by a single [GetTensorC](../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Kernel/GetTensorC.md) output are not supported.

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
<tr id="row226312045217"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p161761325185216"><a name="p161761325185216"></a><a name="p161761325185216"></a>b</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p4176112555211"><a name="p4176112555211"></a><a name="p4176112555211"></a>64, 30720</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p1176132515212"><a name="p1176132515212"></a><a name="p1176132515212"></a>float16</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p817692575212"><a name="p817692575212"></a><a name="p817692575212"></a>ND</p></td>
</tr>
</tbody>
</table>

The AI processor used in this case has 20 cores in total, and each core contains one AIC core and two AIV cores.

The Tiling parameters of the operator are as follows:

-   Original shape: M=128, N=30720, K=64.
-   Single-core shape: split across 20 AIC cores, with singleCoreM=128, singleCoreN=1536, and singleCoreK=64.

    For matrix B, it is split along the N axis into 20 singleCoreN segments, and each core processes data of size K \* SingleCoreN. For matrix A, the M axis is not split, that is, singleCoreM=M, and each core processes data of size singleCoreM \* K. A total of 20 cores participate in the computation.

-   Basic block shape: baseM=128, baseN=256, baseK=64.
-   L1-related Tiling parameters: stepM=1, stepN=1, stepKa=4, stepKb=4, depthA1=8, depthB1=8.

## Obtaining Performance Data<a name="section851404010469"></a>

Use the msOpProf tool to obtain the [operator simulation pipeline diagram](../../performance_analysis/obtain_performance_data.md#section17259539153513) and [on-board profiling](../../performance_analysis/obtain_performance_data.md#section17953123893415) data. Because the UnitFlag feature mainly optimizes the serial execution issue between the MMAD and FIXPIPE pipelines, after performance acquisition, focus on analyzing the pipeline status of Cube and FIXPIPE.

## Analyzing the Main Bottleneck<a name="section221431704714"></a>

-   The pipeline diagram before optimization is as follows. As shown in the red box in the following figure, each round of MMAD computation pipeline and FIXPIPE data movement out pipeline are executed serially between pipelines. FIXPIPE data movement out starts only after MMAD computation is complete. Consider implementing pipeline parallelism between MMAD and FIXPIPE to optimize operator performance.

    ![](../../../figures/zh-cn_image_0000002413944550.png)

-   The Profiling data before optimization is as follows. As shown by the aic_time data in column C, the maximum operator execution time among multiple cores is 37.39 us.

    ![](../../../figures/zh-cn_image_0000002414101622.png)

## Designing an Optimization Solution<a name="section990382124919"></a>

As shown in the following figure, when the UnitFlag feature is not enabled, MMAD and FIXPIPE are synchronized at the instruction level. The FIXPIPE instruction must wait for the MMAD instruction to complete before moving the result out, so the pipelines between MMAD and FIXPIPE run serially.

**Figure 1**  UnitFlag feature not enabled<a name="fig108452036134412"></a>  
![](../../../figures/unitflag_feature_not_enabled.png "UnitFlag feature not enabled")

As shown in the following figure, when the UnitFlag feature is enabled, the MMAD and FIXPIPE instructions are synchronized at a fine granularity of 512B. During the execution of an MMAD instruction, each time the computation of a 512B data result is completed, FIXPIPE immediately starts moving out that 512B of data, thereby implementing pipeline parallelism between MMAD and FIXPIPE and improving operator performance.

**Figure 2**  UnitFlag feature enabled<a name="fig58335714416"></a>  
![](../../../figures/unitflag_feature_enabled.png "UnitFlag feature enabled")

For the complete sample of enabling the UnitFlag feature in the Matmul API, see [Matmul sample with the UnitFlag feature and MDL template enabled](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_unitflag). The main steps for enabling the UnitFlag feature are as follows:

1.  Customize the MatmulConfig template parameters and set the enUnitFlag parameter to true to enable the UnitFlag feature.

    ```
    __aicore__ inline constexpr MatmulConfig GetCustomMDLCFG()
    {
        auto mmCfg = CFG_MDL;
        mmCfg.enUnitFlag = true;
        return mmCfg;
    }
    constexpr static MatmulConfig CUSTOM_CFG_MDL = GetCustomMDLCFG();
    ```

2.  Create a Matmul object based on the customized MatmulConfig template parameters.

    ```
    using A_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, AType>;
    using B_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, BType>;
    using C_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, CType>;
    using BIAS_TYPE =  AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, BiasType>;
    AscendC::Matmul<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, CUSTOM_CFG_MDL > matmulObj;
    ```

## Verifying the Performance Gains of the Optimization Solution<a name="section19022397498"></a>

-   The optimized pipeline diagram is as follows. Pipeline parallelism is achieved between the MMAD computation pipeline and the FIXPIPE data movement out pipeline.

    ![](../../../figures/zh-cn_image_0000002413945226.png)

-   The optimized profiling data is as follows. As shown by the aic\_time data in column C, the maximum operator execution time across multiple cores is 34.66 us, which is about 7.3% faster than the 37.39 us before optimization.

    ![](../../../figures/zh-cn_image_0000002447501485.png)

## Summary<a name="section8281219125011"></a>

When the MMAD computation pipeline and the FIXPIPE data movement out pipeline of an operator run serially and are not masked by other pipelines (for example, MTE2 Bound), consider enabling the UnitFlag feature to implement pipeline parallelism between the MMAD computation pipeline and the FIXPIPE data movement out pipeline, thereby improving operator performance.
