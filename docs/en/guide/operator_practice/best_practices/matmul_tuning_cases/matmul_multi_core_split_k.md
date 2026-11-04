# Enabling Multi-Core Split-K for the Matmul Advanced API<a name="ZH-CN_TOPIC_0000002353756705"></a>

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-10-23T11:34:27.450Z pushedAt=2026-11-03T07:05:13.276Z nl=2 -->

## Case Introduction<a name="section17413194624510"></a>

This case demonstrates the performance improvement of an operator achieved by using the Matmul advanced API for cube multiplication and enabling the multi-core split-K feature. To execute the operator in parallel on multiple cores and improve computing efficiency, the cube data must be split, and the split data blocks are assigned to different cores for processing. Typically, when splitting cube data, only the M and N axes are split, and the K axis is not split. If M and N are small, splitting the M and N axes is difficult, and splitting the K axis must be considered. After the multi-core split-K feature is enabled, the K axis of the cube can be split in this scenario, allowing the operator to execute in parallel on multiple cores. Because the K axis is large, not splitting the K axis in this scenario usually causes the input data volume of a single core to be excessively large. After K-axis splitting is enabled, the splitting strategy can more effectively balance the output bandwidth and input bandwidth.

-   Scenarios for enabling multi-core split-K
    -   When the K axis of the cube is large and the M and N axes are smaller than the K axis, the K axis can be split so that the operator can execute in parallel on more cores.
    -   When the M, N, and K axes of the cube are all large, the K axis can be split so that the splitting strategy can better balance the input and output bandwidth.

-   Constraints for enabling multi-core split-K
    -   In the multi-core split-K scenario, when obtaining the result of the cube C, only output to Global Memory is supported.
    -   In the multi-core split-K scenario, before the result of the cube C tile is written to Global Memory for the first time in the kernel function code, Global Memory must be cleared first. When obtaining the result of the cube C tile, enable AtomicAdd accumulation. If Global Memory is not cleared in advance, precision issues may occur because the original invalid data in Global Memory is accumulated.
    -   In the multi-core split-K scenario, Bias is not supported in cube multiplication.

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

The AI processor used in this case has 24 cores, and the cube-only mode of the advanced Matmul API is enabled in the operator. The Tiling parameters are as follows:

-   Original shape: M=16, N=16, K=1024.
-   Single-core shape: when multi-core K-splitting is not enabled, singleCoreM=16, singleCoreN=16, singleCoreK=1024; after multi-core K-splitting is enabled, singleCoreM=16, singleCoreN=16, singleCoreK=512.

## Obtaining Performance Data<a name="section851404010469"></a>

Use the msOpProf tool to obtain the [operator simulation pipeline diagram](../../performance_analysis/collect_performance_data.md#section17259539153513) and [on-board Profiling](../../performance_analysis/collect_performance_data.md#section17953123893415) data.

## Analyzing the Main Bottleneck<a name="section221431704714"></a>

-   The pipeline diagram before optimization is as follows. Because multi-core split-K is not enabled and M and N are very small, the original cube data is not split, and all data is computed on a single core. ![](../../../figures/zh-cn_image_0000002375339736.png)
-   The profiling data before optimization is as follows. It can be seen that the operator runs only on a single core, with **aic\_time** taking about 19.60us, of which the average time of **aic\_mte2\_time** is about 13.72us, and **aic\_mte2\_ratio** accounts for a relatively high proportion.

    ![](../../../figures/zh-cn_image_0000002375340188.png)

## Designing an Optimization Solution<a name="section33901368431"></a>

After multi-core split-K is enabled, the data in the K direction of the cube can be split. As shown in the following figure, the R cube block in the cube C is obtained by accumulating A1\*B1+A2\*B2+A3\*B3, where A1\*B1, A2\*B2, and A3\*B3 can be computed in parallel on multiple cores.

**Figure 1**  Enabling multi-core split-K<a name="fig194841921104410"></a>  
![](../../../figures/multi_k.png "Enabling multi-core split-K")

To enable multi-core split-K, call the **EnableMultiCoreSplitK** API before the **GetTiling** API to enable multi-core split-K, and in the kernel function implementation, clear the Global Memory address of the cube C and then enable **AtomicAdd**. For a complete sample of enabling multi-core split-K, see [Operator Sample in the Multi-Core Split-K Scenario](../../../../../../examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_splitk). The specific steps are as follows:

-   Tiling implementation

    Before obtaining the **TCubeTiling** structure through the **GetTiling** API, call the **EnableMultiCoreSplitK** API with the input parameter set to **true** to enable multi-core split-K.

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

-   Kernel function implementation

    Call the **Fill** API to clear the Global Memory address of the cube C.

    ```
    cGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ cType*>(c), tiling.M * tiling.N);
    // clear gm
    Fill(cGlobal, tiling.M * tiling.N, (cType)0);
    ```

    Call the **IterateAll** API to enable **AtomicAdd** accumulation and complete the cube multiplication operation.

    ```
    // set AtomicAdd
    uint8_t enAtomic = 1;
    matmulObj.IterateAll(cGlobal, enAtomic);
    ```

## Verifying the Performance Gains of the Optimization Solution<a name="section19022397498"></a>

-   The optimized pipeline is shown in the following figure. After multi-core K-splitting is enabled, the K axis of the original cube is split, and the amount of data processed by a single core along the K axis is reduced from 1024 to 512. The amount of data processed by a single core is halved, and the MTE2 pipeline becomes shorter.

    ![](../../../figures/zh-cn_image_0000002408742265.png)

-   The optimized profiling data is shown in the following figure. You can see that the operator is executed on two cores, and the average time consumed is about 13.70 us, which is a significant improvement over the 19.60 us before optimization. ![](../../../figures/zh-cn_image_0000002375184084.png)

## Summary<a name="section8281219125011"></a>

When an operator uses the Matmul API to perform cube computation, if the M and N dimensions of the original cube cannot be effectively split and the result is output to Global Memory, you can consider enabling the multi-core K-splitting feature to implement multi-core parallelism and improve computation efficiency.
