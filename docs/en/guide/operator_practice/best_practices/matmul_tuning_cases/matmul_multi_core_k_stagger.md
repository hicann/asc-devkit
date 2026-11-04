# Enabling Multi-Core K-Axis Staggered Memory Access for the Matmul Advanced API<a name="ZH-CN_TOPIC_0000002374905793"></a>

<!-- md-trans-meta sourceCommit=dd293a1c6b873d149bf289bb2b8098dadf9f5cbd translatedAt=2026-10-23T11:34:04.633Z pushedAt=2026-11-03T07:05:13.275Z nl=2 -->

## Case Introduction<a name="section579816421564"></a>

This case demonstrates the performance improvement achieved by using the Matmul advanced API for cube multiplication and enabling multi-core K axis staggered access to Device memory in a cube multiplication operator scenario. When Matmul computation is executed in parallel across multiple cores, if the memory location of input cube A or B is in GM and the cubes involved in the multi-core computation are the same, multiple cores will access the same GM address simultaneously, causing address access conflicts and thereby degrading operator performance. If multi-core K axis staggered access to Device memory is enabled, different cores corresponding to the K axis direction of the split cube will start accessing and moving data from different GM start addresses as much as possible, mitigating address access conflicts and improving operator performance.

**Figure 1**  Address access conflict illustration<a name="fig7682191511128"></a>  
![](../../../figures/addr_conflict.png "Address access conflict illustration")

**Figure 2**  Address conflict mitigation illustration<a name="fig1499117817383"></a>  
![](../../../figures/ease_addr_conflict.png "Address conflict mitigation illustration")

-   Applicable scenarios for enabling multi-core K-axis staggered memory access:

    Matmul is executed across multiple cores, and the K axis of the input cube is large.

-   Constraints for enabling multi-core K-axis staggered memory access:
    -   The K axis of the input cube is not fully loaded, meaning that the data in the K direction of the cube cannot be moved into and retained in the L1 Buffer at the same time.
    -   Only the MDL template is supported.
    -   Matmul computation is executed across multiple cores.
    -   The memory location of cube A or cube B is in GM.

The operator specifications in this case are as follows:

**Table 1** Operator use case specifications

<a name="table15465191317123"></a>
<table><thead align="left"><tr id="row184651013131217"><th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.1"><p id="p24653132122"><a name="p24653132122"></a><a name="p24653132122"></a>Input</p></th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.2"><p id="p13465111311213"><a name="p13465111311213"></a><a name="p13465111311213"></a>Shape</p></th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.3"><p id="p14465171371212"><a name="p14465171371212"></a><a name="p14465171371212"></a>Data Type</p></th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.4"><p id="p74651713141213"><a name="p74651713141213"></a><a name="p74651713141213"></a>Format</p></th>
</tr>
</thead>
<tbody><tr id="row446561351212"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p517515253529"><a name="p517515253529"></a><a name="p517515253529"></a>a</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p141751425165213"><a name="p141751425165213"></a><a name="p141751425165213"></a>768, 6144</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p1517522515218"><a name="p1517522515218"></a><a name="p1517522515218"></a>float16</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p16176725105210"><a name="p16176725105210"></a><a name="p16176725105210"></a>ND</p></td>
</tr>
<tr id="row44651313101220"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p161761325185216"><a name="p161761325185216"></a><a name="p161761325185216"></a>b</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p4176112555211"><a name="p4176112555211"></a><a name="p4176112555211"></a>6144, 2048</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p1176132515212"><a name="p1176132515212"></a><a name="p1176132515212"></a>float16</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p817692575212"><a name="p817692575212"></a><a name="p817692575212"></a>ND</p></td>
</tr>
</tbody>
</table>

## Obtaining Performance Data<a name="section851404010469"></a>

Use the msOpProf tool to obtain the [operator simulation pipeline diagram](../../performance_analysis/collect_performance_data.md#section17259539153513) and [on-board Profiling](../../performance_analysis/collect_performance_data.md#section17953123893415) data, focusing on the MTE2 pipeline.

## Analyzing the Main Bottleneck<a name="section013514116370"></a>

The Profiling data (**PipeUtilization.csv**) before optimization is as shown below. **aic\_mte2\_ratio** reaches 0.93 on average, and MTE2 accounts for a high proportion of the overall operator execution time. The operator is currently MTE2 Bound. In this case, the cube is split along the M and N directions. The single-core shape\[singleCoreM, singleCoreN, singleCoreK\] is \[128, 512, 6144\], and the basic block shape\[baseM, baseN, baseK\] is \[128, 256, 64\]. Each time the data of cube A is loaded, multiple cores may access the same GM address simultaneously, causing an address access conflict, which reduces the MTE2 transfer efficiency and increases the MTE2 execution time.

![](../../../figures/zh-cn_image_0000002414322380.png)

The MTE2 transfer efficiency can also be verified by checking its bandwidth utilization. As shown in the following figure, by analyzing **Memory.csv**, it is found that the average bandwidth utilization of MTE2 is only 34.4%.

![](../../../figures/zh-cn_image_0000002414480828.png)

By checking the **OpBasicInfo.csv** file, the overall operator execution time before optimization is 98.72us.

## Designing an Optimization Solution<a name="section33901368431"></a>

Enable K-axis staggered memory access: When creating a Matmul object, set the **enableKdimReorderLoad** parameter in **MatmulConfig** to **true**. For details about the **enableKdimReorderLoad** parameter, see [MatmulConfig](../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/MatmulConfig.md).

For a complete example of enabling K-axis staggered memory access, see [Operator Example of K-Axis Staggered Data Loading](../../../../../../examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_k_reorder_load). The main steps to enable this feature are as follows:

1.  Configure the MDL template parameters and set the **enableKdimReorderLoad** parameter to **true** to enable multi-core K-axis staggered access to device memory.

    ```
    constexpr MatmulConfig GetMDLKDimReorderConfig()
    {
        auto CFG = CFG_MDL;
        CFG.enableKdimReorderLoad = true;
        return CFG;
    }
    constexpr static MatmulConfig MM_CFG = GetMDLKDimReorderConfig();
    ```

2.  Create a Matmul object based on the custom **MatmulConfig** template parameters.

    ```
    AscendC::Matmul<AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, aType>,
        AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, bType>,
        AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, cType>,
        AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, biasType>, MM_CFG> matmulObj;
    ```

## Verifying the Performance Gains of the Optimization Solution<a name="section19022397498"></a>

With the operator tiling parameters unchanged, the optimized profiling data (**PipeUtilization.csv**) is as shown below. It can be seen that the MTE2 duration is significantly reduced: the average MTE2 duration is reduced from 90us to 69.87us, and the maximum duration is reduced from 91.94us to 75.82us.

![](../../../figures/zh-cn_image_0000002448122873.png)

The MTE2 bandwidth utilization (**Memory.csv**) is as shown below, with the average bandwidth utilization increased to 41.7%.

![](../../../figures/zh-cn_image_0000002414329628.png)

According to the **OpBasicInfo.csv** file, the overall duration of the optimized operator is 85.68us, reduced from 98.72us to 85.68us, achieving a performance improvement of 13.2%.

## Summary<a name="section8281219125011"></a>

In multi-core Matmul execution scenarios, when the K-axis of the input cube is large (generally greater than 4096), you can try using the MDL template and enabling K-axis staggered memory access to mitigate address access conflicts, improve MTE2 transfer efficiency, and thereby optimize operator performance.
