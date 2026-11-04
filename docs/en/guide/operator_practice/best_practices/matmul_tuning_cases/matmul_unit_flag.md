# Enabling UnitFlag for the Matmul Advanced API<a name="ZH-CN_TOPIC_0000002353876489"></a>

<!-- md-trans-meta sourceCommit=dd293a1c6b873d149bf289bb2b8098dadf9f5cbd translatedAt=2026-10-23T11:41:46.615Z pushedAt=2026-11-03T07:05:13.281Z nl=2 -->

## Case Introduction<a name="section17413194624510"></a>

This case demonstrates the performance improvement achieved by enabling the UnitFlag feature when using the Matmul advanced API for cube multiplication in a cube multiplication operator scenario. The UnitFlag feature provides fine-grained memory-access-based synchronization between the MMAD computation instructions and FixPipe data movement instructions on the AIC core, enabling pipeline parallelism between computation and data movement. To enable the UnitFlag feature, set the **enUnitFlag** parameter in **MatmulConfig** to **true**. For details about the **enUnitFlag** parameter, see [MatmulConfig](../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/MatmulConfig.md).

-   Scenarios where enabling UnitFlag is applicable

    The MMAD pipeline and FixPipe pipeline of the operator are executed serially. FixPipe waits for the MMAD computation to complete before transferring out the result, and the time consumed by this instruction-level synchronization wait accounts for a high proportion of the overall execution time of the operator. In this scenario, you can enable the UnitFlag feature to obtain the performance benefit of pipeline parallelism between MMAD and FixPipe. If the original MMAD and FixPipe pipelines of the operator can be masked by other pipelines (for example, MTE2 Bound), enabling the UnitFlag feature yields little overall benefit.

-   Constraints on enabling UnitFlag
    -   The UnitFlag feature supports only the Norm, IBShare, and MDL templates.
    -   When the UnitFlag feature is enabled, the operator cannot contain both the pipeline that copies out data from the L0C Buffer (CO1) to Global Memory and the pipeline that copies out data from the L1 Buffer (A1) to Global Memory.
    -   When the UnitFlag feature is enabled, if the [L0C accumulation](../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/Iterate.md) feature is also enabled, multiple [Iterate](../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/Iterate.md) computations followed by a single [GetTensorC](../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/GetTensorC.md) output are not supported.

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

    For cube B, it is split along the N axis into 20 singleCoreN parts, and each core processes data of size K \* singleCoreN. For cube A, the M axis is not split, that is, singleCoreM=M, and each core processes data of size singleCoreM \* K. A total of 20 cores participate in the computation.

-   Base block shape: baseM=128, baseN=256, baseK=64.
-   L1-related Tiling parameters: stepM=1, stepN=1, stepKa=4, stepKb=4, depthA1=8, depthB1=8.

## Obtaining Performance Data<a name="section851404010469"></a>

Use the msOpProf tool to obtain the [operator simulation pipeline diagram](../../performance_analysis/collect_performance_data.md#section17259539153513) and [on-board Profiling](../../performance_analysis/collect_performance_data.md#section17953123893415) data. Because the UnitFlag feature mainly optimizes the serial execution issue between the MMAD and FixPipe pipelines, focus on analyzing the pipeline status of Cube and FixPipe after obtaining the performance data.

## Analyzing the Main Bottleneck<a name="section221431704714"></a>

-   The pipeline diagram before optimization is as follows. As shown in the red box in the following figure, each round of MMAD computation pipeline and FixPipe data copy-out pipeline are executed serially between pipelines. The FixPipe data copy-out starts only after the MMAD computation is complete. Consider implementing pipeline parallelism between MMAD and FixPipe to optimize operator performance.

    ![](../../../figures/zh-cn_image_0000002413944550.png)

-   The Profiling data before optimization is as follows. As shown in the **aic\_time** data in column C, the maximum operator execution time among multiple cores is 37.39us.

    ![](../../../figures/zh-cn_image_0000002414101622.png)

## Designing an Optimization Solution<a name="section990382124919"></a>

As shown in the following figure, when the UnitFlag feature is disabled, MMAD and FixPipe are synchronized at the instruction level. The FixPipe instruction must wait for the MMAD instruction to complete before transferring out the result, so the pipelines between MMAD and FixPipe run serially.

**Figure 1** UnitFlag feature disabled<a name="fig108452036134412"></a>  
![](../../../figures/no_unitflag.png "UnitFlag feature disabled")

As shown in the following figure, when the UnitFlag feature is enabled, the MMAD and FixPipe instructions are synchronized at a fine granularity of 512B. During the execution of an MMAD instruction, each time the computation of a 512B data result is completed, FixPipe immediately starts transferring out the 512B data, thereby implementing pipeline parallelism between MMAD and FixPipe and improving operator performance.

**Figure 2** UnitFlag feature enabled<a name="fig58335714416"></a>  
![](../../../figures/unitflag.png "UnitFlag feature enabled")

For the complete sample of enabling the UnitFlag feature in the Matmul API, see [Matmul Sample with UnitFlag Feature and MDL Template Enabled](../../../../../../examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_unitflag). The main steps for enabling the UnitFlag feature are as follows:

1.  Customize the MatmulConfig template parameters and set the **enUnitFlag** parameter to **true** to enable the UnitFlag feature.

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

-   The optimized pipeline diagram is as follows. Pipeline parallelism is achieved between the MMAD computation pipeline and the FixPipe data copy-out pipeline.

    ![](../../../figures/zh-cn_image_0000002413945226.png)

-   The optimized Profiling data is as follows. As shown in the **aic\_time** data in column C, the maximum operator execution time across multiple cores is 34.66us, which is a performance improvement of about 7.3% compared with the 37.39us before optimization.

    ![](../../../figures/zh-cn_image_0000002447501485.png)

## Summary<a name="section8281219125011"></a>

When the MMAD computation pipeline and the FixPipe data copy-out pipeline of an operator run serially and are not masked by other pipelines (for example, MTE2 Bound), consider enabling the UnitFlag feature to implement pipeline parallelism between the MMAD computation pipeline and the FixPipe data copy-dout pipeline, thereby improving operator performance.
