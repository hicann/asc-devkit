# ND2NZ Format Conversion on the AIV Core<a name="ZH-CN_TOPIC_0000002340907814"></a>

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-10-23T11:25:18.010Z pushedAt=2026-11-03T07:05:13.270Z nl=2 -->

## Case Introduction<a name="section17413194624510"></a>

This case demonstrates the performance improvement achieved by performing ND2NZ format conversion on the AIV core for input cubes whose inner axis (the inner axis is the row direction of the cube) is not 256-byte aligned, when using the advanced Matmul API for computation in a cube multiplication operator scenario. To improve the compute efficiency of the cube unit, an input cube in ND format is first converted to NZ format before cube computation is performed. For details about the ND and NZ formats, see [Data Format](../../simd_operator_impl/matrix_advanced_api/basics.md#zh-cn_topic_0000001622194138_section1453415011). The Matmul API internally uses the inline ND2NZ instruction to perform format conversion and data movement simultaneously. However, when the data is not 256-byte aligned, the [inline ND2NZ instruction](../../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/DataCopy_GMToL1_ND2NZ.md) suffers from low bandwidth utilization. Therefore, when the inner axis of the input cube is not 256-byte aligned, using the Vector compute unit on the AIV core to complete the conversion from ND format to NZ format before Matmul computation can avoid the low efficiency of inline unaligned data movement, thereby improving operator performance.

-   Applicable scenarios for ND2NZ format conversion on the AIV core

    The inner axis of the input cube is not 256-byte aligned, and the data volume is large enough to affect the efficiency of inline format conversion.

The operator specifications in this case are as follows:

**Table 1**  Operator specifications

<a name="table101751125175213"></a>
<table><thead align="left"><tr id="row8175525185219"><th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.1"><p id="p1417582516529"><a name="p1417582516529"></a><a name="p1417582516529"></a>Input</p></th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.2"><p id="p417532575212"><a name="p417532575212"></a><a name="p417532575212"></a>Shape</p></th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.3"><p id="p1017582585214"><a name="p1017582585214"></a><a name="p1017582585214"></a>Data type</p></th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.4"><p id="p317502512522"><a name="p317502512522"></a><a name="p317502512522"></a>Format</p></th>
</tr>
</thead>
<tbody><tr id="row217562525215"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p517515253529"><a name="p517515253529"></a><a name="p517515253529"></a>a</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p141751425165213"><a name="p141751425165213"></a><a name="p141751425165213"></a>1024, 1024</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p1517522515218"><a name="p1517522515218"></a><a name="p1517522515218"></a>float16</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p16176725105210"><a name="p16176725105210"></a><a name="p16176725105210"></a>ND</p></td>
</tr>
<tr id="row10176102512525"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p161761325185216"><a name="p161761325185216"></a><a name="p161761325185216"></a>b</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p4176112555211"><a name="p4176112555211"></a><a name="p4176112555211"></a>1024, 4095</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p1176132515212"><a name="p1176132515212"></a><a name="p1176132515212"></a>float16</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p817692575212"><a name="p817692575212"></a><a name="p817692575212"></a>ND</p></td>
</tr>
</tbody>
</table>

The AI processor used in this case has 24 cores in total, and the operator enables the cube-only mode of the advanced API Matmul. The MDL template is used, and the Tiling parameters are as follows:

-   Original shape: M=1024, N=4095, K=1024.
-   Single-core shape: singleCoreM=128, singleCoreN=1408, singleCoreK=1024.
-   Basic block shape: baseM=128, baseN=256, baseK=64.
-   L1 cache-related Tiling parameters: stepM=1, stepN=1, stepKa=4, stepKb=4.

## Obtaining Performance Data<a name="section851404010469"></a>

Use the msOpProf tool to obtain the [operator simulation pipeline diagram](../../performance_analysis/collect_performance_data.md#section17259539153513) and [on-board Profiling](../../performance_analysis/collect_performance_data.md#section17953123893415) data, focusing on analyzing the MTE2 pipeline.

## Analyzing the Main Bottleneck<a name="section221431704714"></a>

-   The Cube pipeline diagram before optimization is as follows. Because the inline ND2NZ instruction is used, format conversion is performed during MTE2 data transfer, resulting in a high overall MTE2 ratio. ![](../../../figures/zh-cn_image_0000002409729950.png)
-   The Profiling data before optimization is as follows. It can be seen that only the Cube unit is used for computation, **aic\_time** with a maximum duration of 149.04us, of which **aic\_mte2\_ratio** accounts for a high proportion. ![](../../../figures/zh-cn_image_0000002381161446.png)

## Designing an Optimization Solution<a name="section33901368431"></a>

For an input cube in ND format, the inline ND2NZ instruction is no longer used for format conversion. Instead, the Vector compute unit is used to perform the data format conversion. First, the **DataCopyPad** API is used to move the non-aligned cube data into the Unified Buffer (UB). Then, the **Duplicate** API is used to fill the data at the positions that need to be aligned. After that, the **Copy** API is called row by row to reorder the data from ND to NZ format, and the reordered NZ data is written to the workspace memory. Finally, the NZ data in the workspace is directly read for Matmul computation.

The main steps for implementing ND2NZ format conversion on the AIV core are as follows:

1.  When creating the Matmul object, define the format of cube B, whose inner axis is not 256-byte aligned, as NZ format.

    ```
    using A_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, ATYPE, true>;
    // Use CubeFormat::NZ to define the type information of cube B.
    using B_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, AscendC::TPosition::GM, CubeFormat::NZ, BType, true>;
    using C_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, CType>;
    using BIAS_TYPE =  AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, BiasType>;
    AscendC::Matmul<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, CFG_MDL> matmulObj;
    ```

2.  Use the Vector compute unit to implement ND2NZ format conversion. In the following code, MatrixBtoNZ is the function that converts cube B from ND format to NZ format. For the specific implementation of this function, see the complete sample code.

    ```
    // Vector ND2NZ
    if ASCEND_IS_AIV {
        pipe->InitBuffer(ubBuf, TOTAL_UB_SIZE);
        MatrixBtoNZ<typename B_TYPE::T>(tempGM, bGMNZ, tiling, isTransB, ubBuf, tiling.baseK,
            tiling.baseN); // ND2NZ format conversion function
        SyncAll();
        // CV SYNC
        NotifyEvent<PIPE_MTE3>(4);
        return;
    }
    if ASCEND_IS_AIC {
        WaitEvent(4); // Wait for the Vector to complete ND2NZ format conversion.
    }
    ```

3.  Set the left cube A, right cube B, and Bias to complete the cube multiplication operation.

    ```
    matmulObj.SetTail(tailM, tailN, shapes.k);
    matmulObj.SetTensorA(aGlobal, false);
    matmulObj.SetTensorB(bGlobal, false);
    if (shapes.isBias) {
        matmulObj.SetBias(biasGlobal);
    }
    matmulObj.IterateAll(cGlobal);
    ```

## Verifying the Performance Gains of the Optimization Solution<a name="section19022397498"></a>

-   The optimized Vector pipeline is shown in the following figure. The Vector compute unit is used to complete the format conversion of cube B. ![](../../../figures/zh-cn_image_0000002409891270.png)
-   The optimized Cube pipeline is shown in the following figure. After the inline ND2NZ instruction is no longer used to convert the format of cube B, the proportion of MTE2 decreases significantly. ![](../../../figures/zh-cn_image_0000002443251985.png)
-   The optimized Profiling data is shown below. It can be seen that the Cube unit and the Vector unit are used simultaneously, and the **aic\_time** maximum duration is 90.95 us, with the proportion of **aic\_mte2\_ratio** significantly reduced. ![](../../../figures/zh-cn_image_0000002381162386.png)![](../../../figures/zh-cn_image_0000002414561861.png)

**Table 2** End-to-end performance comparison

<a name="table135810433437"></a>
<table><thead align="left"><tr id="row65820433437"><th class="cellrowborder" valign="top" width="22.61%" id="mcps1.2.5.1.1"><p id="p75827438435"><a name="p75827438435"></a><a name="p75827438435"></a>Optimization Method</p></th>
<th class="cellrowborder" valign="top" width="21.88%" id="mcps1.2.5.1.2"><p id="p3582204311439"><a name="p3582204311439"></a><a name="p3582204311439"></a>Total Duration (us)</p></th>
<th class="cellrowborder" valign="top" width="25.729999999999997%" id="mcps1.2.5.1.3"><p id="p16582243124317"><a name="p16582243124317"></a><a name="p16582243124317"></a>Average AIC_MTE2 Duration (us)</p></th>
<th class="cellrowborder" valign="top" width="29.78%" id="mcps1.2.5.1.4"><p id="p15582144314316"><a name="p15582144314316"></a><a name="p15582144314316"></a>Average AIV_MTE2 Duration (us)</p></th>
</tr>
</thead>
<tbody><tr id="row65821543124312"><td class="cellrowborder" valign="top" width="22.61%" headers="mcps1.2.5.1.1 "><p id="p185821343184318"><a name="p185821343184318"></a><a name="p185821343184318"></a>Inline ND2NZ</p></td>
<td class="cellrowborder" valign="top" width="21.88%" headers="mcps1.2.5.1.2 "><p id="p4582743164312"><a name="p4582743164312"></a><a name="p4582743164312"></a>149.82</p></td>
<td class="cellrowborder" valign="top" width="25.729999999999997%" headers="mcps1.2.5.1.3 "><p id="p0582194314314"><a name="p0582194314314"></a><a name="p0582194314314"></a>130.77</p></td>
<td class="cellrowborder" valign="top" width="29.78%" headers="mcps1.2.5.1.4 "><p id="p1958224374312"><a name="p1958224374312"></a><a name="p1958224374312"></a>0</p></td>
</tr>
<tr id="row155821743154311"><td class="cellrowborder" valign="top" width="22.61%" headers="mcps1.2.5.1.1 "><p id="p1458210436437"><a name="p1458210436437"></a><a name="p1458210436437"></a>ND2NZ on the Vector side</p></td>
<td class="cellrowborder" valign="top" width="21.88%" headers="mcps1.2.5.1.2 "><p id="p758284324319"><a name="p758284324319"></a><a name="p758284324319"></a>93.76</p></td>
<td class="cellrowborder" valign="top" width="25.729999999999997%" headers="mcps1.2.5.1.3 "><p id="p1658234344311"><a name="p1658234344311"></a><a name="p1658234344311"></a>22.85</p></td>
<td class="cellrowborder" valign="top" width="29.78%" headers="mcps1.2.5.1.4 "><p id="p4582134310436"><a name="p4582134310436"></a><a name="p4582134310436"></a>10.31</p></td>
</tr>
</tbody>
</table>

From the execution time comparison in the preceding table, it can be seen that after the inline ND2NZ instruction is no longer used, the total time decreases significantly and the end-to-end performance improves noticeably.

## Summary<a name="section8281219125011"></a>

For scenarios where the inner axis of the input cube is not 256-byte aligned in cube multiplication, the inline ND2NZ instruction has low bandwidth utilization, which degrades operator performance. By performing ND2NZ data rearrangement on the AIV core, the overall operator performance is improved. It is worth noting that bandwidth utilization is related to the data volume. If the total cube data volume is too small, even the ND2NZ conversion performed on the AIV core cannot significantly improve the effective bandwidth. Instead, it may degrade the end-to-end operator performance due to the introduced multi-core synchronization.
