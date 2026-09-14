# Matmul Advanced API Enabling Multi-Core K Splitting<a name="ZH-CN_TOPIC_0000002353756705"></a>

<!-- md-trans-meta sourceCommit=e4a5905334db2bef02e668063a0b99d64a2e7709 translatedAt=2026-08-26T13:11:11.705Z -->

## Case Introduction<a name="section17413194624510"></a>

This case demonstrates the performance improvement achieved by using the Matmul advanced API for matrix multiplication and enabling multi-core K-splitting in a matrix multiplication operator scenario. To execute the operator in parallel across multiple cores and improve computational efficiency, the matrix data must be split, and the resulting data blocks are assigned to different cores for processing. Typically, when splitting matrix data, only the M and N axes are split, while the K axis is not. If M and N are small, splitting the M and N axes is difficult, in which case K-axis splitting must be considered. After enabling multi-core K-splitting, the K axis of the matrix can be split in this scenario, allowing the operator to execute in parallel across multiple cores. Because the K axis is large, failing to split the K axis in this scenario usually causes the input data volume of a single core to be excessively large. After enabling K-axis splitting, the splitting strategy can more effectively balance the output bandwidth and input bandwidth.

-   Scenarios for enabling multi-core K-splitting
    -   When the K axis of the matrix is large and the M and N axes are relatively small compared with the K axis, the K axis can be split to increase the number of cores on which the operator executes in parallel.
    -   When the M, N, and K axes of the matrix are all large, the K axis can be split so that the splitting strategy better balances the input and output bandwidth.

-   Constraints on enabling multi-core K-splitting
    -   In the scenario where multi-core K-splitting is enabled, when obtaining the result of matrix C, only output to Global Memory is supported.
    -   In the scenario where multi-core K-splitting is enabled, before the result of the C matrix tile is written to Global Memory for the first time in the Kernel-side code, Global Memory must be cleared first. When obtaining the result of the C matrix tile, enable AtomicAdd accumulation. If Global Memory is not cleared in advance, precision issues may arise from accumulating the original invalid data in Global Memory.
    -   In the scenario where multi-core K-splitting is enabled, Bias is not supported in the matrix multiplication computation.

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
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p141751425165213"><a name="p141751425165213"></a><a name="p141751425165213"></a>16, 1024</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p1517522515218"><a name="p1517522515218"></a><a name="p1517522515218"></a>float16</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p16176725105210"><a name="p16176725105210"></a><a name="p16176725105210"></a>ND</p></td>
</tr>
<tr id="row10176102512525"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p161761325185216"><a name="p161761325185216"></a><a name="p161761325185216"></a>b</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p4176112555211"><a name="p4176112555211"></a><a name="p4176112555211"></a>1024, 16</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p1176132515212"><a name="p1176132515212"></a><a name="p1176132515212"></a>float16</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p817692575212"><a name="p817692575212"></a><a name="p817692575212"></a>ND</p></td>
</tr>
</tbody>
</table>

The AI processor used in this case has 24 cores in total. The operator enables the pure Cube mode of the Matmul advanced API. The Tiling parameters are as follows:

-   Original shape: M=16, N=16, K=1024.
-   Single-core shape: when multi-core K splitting is not enabled, singleCoreM=16, singleCoreN=16, singleCoreK=1024; after multi-core K splitting is enabled, singleCoreM=16, singleCoreN=16, singleCoreK=512.

## Obtaining Performance Data<a name="section851404010469"></a>

Use the msOpProf tool to obtain the [operator simulation pipeline diagram](../../performance_analysis/obtain_performance_data.md#section17259539153513) and [on-board Profiling](../../performance_analysis/obtain_performance_data.md#section17953123893415) data.

## Analyzing the Main Bottleneck<a name="section221431704714"></a>

-   The following pipeline diagram shows the state before optimization. Because multi-core K-splitting is not enabled and M and N are very small, the original matrix data is not split, and all data is computed on a single core. ![](../../../figures/zh-cn_image_0000002375339736.png)
-   The following profiling data shows the state before optimization. The operator runs on only a single core, with an aic\_time of about 19.60us. The average aic\_mte2\_time is about 13.72us, and the aic\_mte2\_ratio is relatively high.

    ![](../../../figures/zh-cn_image_0000002375340188.png)

## Designing an Optimization Solution<a name="section33901368431"></a>

After multi-core K-splitting is enabled, the data along the K dimension of the matrix can be split. As shown in the following figure, the R matrix block in matrix C is obtained by accumulating A1\*B1+A2\*B2+A3\*B3, where A1\*B1, A2\*B2, and A3\*B3 can be computed in parallel on multiple cores.

**Figure 1** Enabling multi-core K-splitting<a name="fig194841921104410"></a>  
![](../../../figures/enable_multi_core_split_k.png "Enabling multi-core K-splitting")

To enable multi-core K-splitting, call the EnableMultiCoreSplitK API before the GetTiling API to enable multi-core K-splitting, and in the kernel implementation, clear the Global Memory address of matrix C to zero before enabling AtomicAdd. For a complete example of enabling multi-core K-splitting, see the [operator example in the multi-core K-splitting scenario](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_splitk). The specific steps are as follows:

-   Tiling implementation

    Before obtaining the TCubeTiling structure through the GetTiling API, call the EnableMultiCoreSplitK API with the input parameter set to true to enable multi-core K-splitting.

    ```
    cubeTiling.SetOrgShape(M, N, K);
    cubeTiling.SetShape(M, N, K);
    cubeTiling.EnableBias(isBias);
    cubeTiling.SetBufferSpace(-1, -1, -1);
    // tiling enable split K
    cubeTiling.EnableMultiCoreSplitK(true);
    if (cubeTiling.GetTiling(tilingData) == -1) {
        std::cout << "gen tiling failed." << std::endl;
        return {};
    }
    ```

-   Kernel implementation

    Call the Fill API to clear the Global Memory address of matrix C to zero.

    ```
    cGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ cType*>(c), tiling.M * tiling.N);
    // clear gm
    Fill(cGlobal, tiling.M * tiling.N, (cType)0);
    ```

    Call the IterateAll API to enable AtomicAdd accumulation and complete the matrix multiplication operation.

    ```
    // set AtomicAdd
    uint8_t enAtomic = 1;
    matmulObj.IterateAll(cGlobal, enAtomic);
    ```

## Verifying the Performance Gains of the Optimization Solution<a name="section19022397498"></a>

-   The optimized pipeline diagram is as follows. After multi-core K-splitting is enabled, the K direction of the original matrix is split, and the amount of data processed by a single core along the K direction is reduced from 1024 to 512. The amount of data processed by a single core is halved, and the MTE2 pipeline becomes shorter.

    ![](../../../figures/zh-cn_image_0000002408742265.png)

-   The optimized profiling data is as follows. It shows that the operator runs on two cores, and the average aic\_time is about 13.70 us, a significant improvement over the 19.60 us before optimization. ![](../../../figures/zh-cn_image_0000002375184084.png)

## Summary<a name="section8281219125011"></a>

When an operator uses the Matmul API to perform matrix computation, and the M and N dimensions of the original matrix cannot be effectively split, and the result is output to Global Memory, you can enable multi-core K-splitting to implement multi-core parallelism and improve computation efficiency.
