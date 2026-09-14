# Matmul Advanced API Enabling Multi-Core K-Axis Staggered Memory Access<a name="ZH-CN_TOPIC_0000002374905793"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-26T13:14:06.225Z -->

## Case Introduction<a name="section579816421564"></a>

This case demonstrates the performance improvement achieved by using the Matmul advanced API for matrix multiplication and enabling multi-core K-axis-staggered access to the device memory in a matrix multiplication operator scenario. When Matmul computation is executed in parallel on multiple cores, if the memory location of input matrix A or B is in GM and the matrices involved in multi-core computation are the same, multiple cores access the same GM address simultaneously, causing address access conflicts and thereby degrading operator performance. If multi-core K-axis-staggered access to the device memory is enabled, the different cores corresponding to the K-axis direction of the split matrix access and move data from different GM start addresses as much as possible, mitigating address access conflicts and improving operator performance.

**Figure 1**  Access address conflict illustration<a name="fig7682191511128"></a>  
![](../../../figures/access_address_conflict_illustration.png "Access address conflict illustration")

**Figure 2**  Mitigate address conflict illustration<a name="fig1499117817383"></a>  
![](../../../figures/mitigate_address_conflict_illustration.png "Mitigate address conflict illustration")

-   Applicable scenarios for enabling multi-core K-axis-staggered memory access:

    Matmul is executed on multiple cores, and the K-axis of the input matrix is large.

-   Constraints for enabling multi-core K-axis-staggered memory access:
    -   The K-axis of the input matrix is not fully loaded, which means that the data in the K direction of the matrix cannot be moved into and kept in the L1 Buffer at the same time.
    -   Only the MDL template is supported.
    -   Matmul computation is executed on multiple cores.
    -   The memory location of matrix A or matrix B is in the GM.

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

Use the msOpProf tool to obtain the [operator simulation pipeline diagram](../../performance_analysis/obtain_performance_data.md#section17259539153513) and [on-board profiling](../../performance_analysis/obtain_performance_data.md#section17953123893415) data, focusing on the MTE2 pipeline.

## Analyzing the Main Bottleneck<a name="section013514116370"></a>

The profiling data before optimization (PipeUtilization.csv) is as shown below. The average aic_mte2_ratio reaches 0.93, indicating that MTE2 accounts for a high proportion of the overall operator execution time and the operator is currently MTE2-bound. In this case, the matrix is split along the M and N directions. The single-core shape [singleCoreM, singleCoreN, singleCoreK] is [128, 512, 6144], and the basic block shape [baseM, baseN, baseK] is [128, 256, 64]. Each time the data of matrix A is loaded, multiple cores may access the same GM address simultaneously, causing address conflicts, which reduces the MTE2 transfer efficiency and increases the MTE2 execution time.

![](../../../figures/zh-cn_image_0000002414322380.png)

The MTE2 transfer efficiency can also be verified by checking its bandwidth utilization. As shown below, analysis of Memory.csv reveals that the average MTE2 bandwidth utilization is only 34.4%.

![](../../../figures/zh-cn_image_0000002414480828.png)

Check the OpBasicInfo.csv file. The overall operator execution time before optimization is 98.72 us.

## Designing an Optimization Solution<a name="section33901368431"></a>

Enable K-axis staggered memory access: when creating a Matmul object, set the enableKdimReorderLoad parameter in MatmulConfig to true. For details about the enableKdimReorderLoad parameter, see [MatmulConfig](../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Kernel/MatmulConfig.md).

For a complete example of enabling K-axis staggered memory access, see the [operator example of K-axis staggered data loading](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_k_reorder_load). The main steps to enable this feature are as follows:

1.  Configure the MDL template parameters and set the enableKdimReorderLoad parameter to true to enable multi-core K-axis staggered access to the device memory.

    ```
    constexpr MatmulConfig GetMDLKDimReorderConfig()
    {
        auto CFG = CFG_MDL;
        CFG.enableKdimReorderLoad = true;
        return CFG;
    }
    constexpr static MatmulConfig MM_CFG = GetMDLKDimReorderConfig();
    ```

2.  Create a Matmul object based on the custom MatmulConfig template parameters.

    ```
    AscendC::Matmul<AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, aType>,
        AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, bType>,
        AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, cType>,
        AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, biasType>, MM_CFG> matmulObj;
    ```

## Verifying the Performance Gains of the Optimization Solution<a name="section19022397498"></a>

With the operator tiling parameters unchanged, the optimized profiling data (PipeUtilization.csv) is as shown below. It can be seen that the MTE2 time is significantly reduced, with the average MTE2 time reduced from 90us to 69.87us and the maximum time reduced from 91.94us to 75.82us.

![](../../../figures/zh-cn_image_0000002448122873.png)

The MTE2 bandwidth utilization (Memory.csv) is as shown below, with the average bandwidth utilization increased to 41.7%.

![](../../../figures/zh-cn_image_0000002414329628.png)

Check the OpBasicInfo.csv file. After optimization, the overall operator time is 85.68us, reduced from 98.72us to 85.68us, achieving a performance improvement of 13.2%.

## Summary<a name="section8281219125011"></a>

In multi-core Matmul execution scenarios, when the K axis of the input matrix is large (generally greater than 4096), you can try using the MDL template and enable the K-axis staggered memory access feature to mitigate address access conflicts, improve MTE2 data transfer efficiency, and thereby optimize operator performance.
