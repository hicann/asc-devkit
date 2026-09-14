# MC² Operator Performance Tuning Case<a name="ZH-CN_TOPIC_0000002076345610"></a>

<!-- md-trans-meta sourceCommit=e4a5905334db2bef02e668063a0b99d64a2e7709 translatedAt=2026-08-26T13:05:15.536Z -->

## Case Introduction<a name="section144912211504"></a>

The performance gain of the MC<sup>2</sup> computation-communication fusion operator mainly comes from the parallel execution of communication and computation. Specifically, the input data is split into multiple sub-blocks, and the computation and communication tasks of the sub-blocks form two pipelines. By executing the tasks on the two pipelines in parallel, pipeline overlap is achieved, thereby improving operator performance. As shown in the following figure, in the scenario where the MC<sup>2</sup> operator performs Matmul computation first and then communication, the input matrix is split into two blocks along the M axis. The Matmul computation of the second block and the communication of the first block can be executed in parallel, so that the computation time and communication time overlap each other. In all figures in this section, MM represents Matmul computation, and hcom represents the communication task.

![](../../figures/1-84.png)

This case describes how to analyze the performance benefit of a computation-communication fusion operator and how to design a better data tiling strategy. For more complete examples of MC<sup>2</sup> operators, see the [MatmulAllReduce example](https://gitcode.com/cann/ops-transformer/tree/9.1.0/mc2/matmul_all_reduce), [MatmulReduceScatter example](https://gitcode.com/cann/ops-transformer/tree/9.1.0/mc2/matmul_reduce_scatter_v2), and [AllGatherMatmul example](https://gitcode.com/cann/ops-transformer/tree/9.1.0/mc2/all_gather_matmul_v2).

## Obtaining Performance Data<a name="section4647105095111"></a>

Obtain operator performance data using the msOpProf operator tuning tool:

-   Obtain the [performance data from real environment execution](../performance_analysis/obtain_performance_data.md#section17953123893415), including the proportion of each pipeline;
-   Obtain the simulated performance data (instruction pipeline diagram), including the occupied interval of each pipeline, so that you can observe the dependencies between pipelines and optimize parallel efficiency.

## Analyzing the Main Bottleneck<a name="section371410542511"></a>

The performance benefit formula of the MC<sup>2</sup> operator is as follows:

Serial time before fusion = computation operator time before fusion + communication operator time before fusion

MC<sup>2</sup> operator benefit = \(serial time before fusion - MC<sup>2</sup> operator time after fusion\) / serial time before fusion

The execution time of the fused MC<sup>2</sup> operator is constrained by the following factors, which in turn affect the operator performance benefit.

-   Factor 1: Difference in execution time between computation and communication

    If the execution time of the computation and communication tasks differs little, after fusion, the computation and communication of the MC<sup>2</sup> operator are executed in parallel, achieving a pipeline masking effect and yielding a large performance benefit.

    If the execution time of the computation and communication tasks differs greatly, after fusion, the computation and communication within the MC<sup>2</sup> operator are executed in parallel but can mask only a small amount of time. The overall execution time of the operator is close to that of the serial execution before tiling, and a large performance benefit cannot be obtained in this case.

    ![](../../figures/3.png)

-   Factor 2: Execution time inflation of computation or communication caused by data tiling

    After the input data is tiled, the original entire data is split into several small data blocks, and Matmul computation or communication tasks are performed on these small data blocks separately. Compared with the case before tiling, the execution time of the computation or communication task may expand (that is, the execution time becomes longer). The causes of this expansion include: the tiled data blocks being too small, which reduces the efficiency of computation or communication; too many tiled data blocks, which introduces additional scheduling overhead; and access conflicts between computation and communication on the L2 cache or device memory after parallel execution. Taking Matmul computation as an example, the following briefly describes the possible expansion of execution time after data tiling.

    -   No expansion:

        Before data tiling, the Matmul execution time is 200 us. The Matmul input is evenly split into two blocks. Assume that after tiling, the Matmul execution time of each block is 100 us. Through parallel execution of computation, the actual performance benefit is 100 us, as shown in the following figure.

        ![](../../figures/4.png)

    -   Moderate expansion:

        Before data tiling, the Matmul execution time is 200 us. The Matmul input is evenly split into two blocks. Assume that after tiling, the Matmul execution time of each block is 150 us. Through parallel execution of computation, the actual performance benefit is 50 us, as shown in the following figure.

        ![](../../figures/5.png)

    -   Severe expansion:

        Before data tiling, the Matmul execution time is 200 us. The Matmul input is evenly split into two blocks. Assume that after tiling, the Matmul execution time of each block is still 200 us. Through parallel execution of computation, the actual performance is degraded by 50 us, as shown in the following figure.

        ![](../../figures/5-85.png)

Based on the preceding analysis, scenarios where computation and communication execution times are relatively balanced achieve better pipeline masking and performance benefits. Meanwhile, the performance benefit is also affected by the execution time inflation caused by data tiling. The following describes how to formulate a data tiling strategy to achieve the optimal pipeline masking effect.

## Designing an Optimization Solution<a name="section7611135813517"></a>

Take the MatmulAllReduce operator on Atlas A2 training series products/Atlas A2 inference series products as an example, where the input data format is ND and the data type is half. In this operator, computation execution comes first and the communication task comes later. Assume that in the Matmul computation, the shape of the left matrix is \[M, K\] and the shape of the right matrix is \[K, N\], and the communication object in the operator is the output matrix of Matmul. Then the input shape of the communication task is \[M, N\]. Because the K axis exists only in the Matmul computation, when the K axis is large, the computation workload is heavy and the computation execution time is greater than the communication execution time, making computation the bottleneck (bound) of the operator. Conversely, when the K axis is small, the computation workload is light and the computation execution time is less than the communication execution time, making communication the bottleneck of the operator. Before formulating the data tiling strategy, run the computation and communication tasks on the original matrix separately, and determine the bound scenario based on the execution time of the two tasks.

The data tiling strategy of this operator must meet the following requirements:

-   Split only the M axis. The Hccl API called by the communication task requires the tiled data memory to be contiguous. If the N axis is split, each row of data is cut off, making the memory of the communication data non-contiguous and failing to meet the communication requirement. If the M axis is split, each row of data remains memory-contiguous and meets the communication requirement.
-   If A represents a long block and B represents a short block, only the form of A or B arranged contiguously can be produced, for example, AAAB, BAAA, and so on.

As described above, the goal of data tiling is to achieve as much pipeline overlap as possible. Based on the difference in execution time between the computation and communication tasks, the actual scenario can be decomposed into the following two specific scenarios, each with its own subdivided tiling goal.

-   Computation bound:

    For the same tiled data block, the computation execution time is greater than the communication execution time. In this case, **computation is continuous**, and **the tail block of communication must be short**, as shown in the following figure.

    **Figure 1** Computation bound schematic diagram<a name="fig718114619319"></a>  
    ![](../../figures/computation_bound_schematic_diagram.png "Computation bound schematic diagram")

-   Communication bound:

    For the same tiled data block, the communication execution time is longer than the computation execution time. In this case, **communication is continuous**, and **the head block of computation must be short**, as shown in the following figure.

    **Figure 2** Communication-bound schematic diagram<a name="fig19667165810312"></a>  
    ![](../../figures/communication_bound_schematic_diagram.png "Communication-bound schematic diagram")

**Prerequisites:**

Before performing the final data tiling, the following prerequisites must be completed: determine the bound scenario, and fit formulas for the relationship between data volume and execution time for Matmul computation and AllReduce communication respectively. The specific steps are as follows.

1.  Execute the Matmul computation and AllReduce communication tasks on the input data separately, collect the execution time of each using the msOpProf tool, and determine the task with the longer time as the corresponding bound scenario. For example, if the communication execution time is longer than the computation execution time, it is the [communication-bound scenario](#fig19667165810312).
2.  Split the input data along the M axis into several data blocks with M axis lengths of 256, 512, 768, 1024, and 2048. The block size can be adjusted based on the actual situation.
3.  Perform AllReduce communication on the data blocks obtained in step 2, collect the execution time using the msOpProf tool, and obtain the execution time t<sub>1</sub>, t<sub>2</sub>, ..., t<sub>n</sub> of each data block. Then, plot and analyze the relationship between data volume and the corresponding execution time, and fit the formula t = CostComm\(m\), where m indicates the M axis length of the data block, t indicates the communication execution time of the data block, and CostComm indicates the fitted mapping between m and t. This mapping is generally linear. If it is not linear, piecewise fitting can be used. An example is as follows:

    ![](../../figures/mc2excel1.png)

    ![](../../figures/mc2excel2.png)

    The data volume x = m \* N \* sizeof\(dataType\), in bytes. The fitted formula is expressed as follows:

    -   <a name="li1188918472441"></a>x is less than 8 MB: t = -0.9698202 \* x \* x + 27.0622573 \* x + 14.769, in us.
    -   x is greater than or equal to 8 MB: t = 13.58491263 \* x + 61.508333, in us.

4.  Perform Matmul computation on each data block obtained by splitting in step 2, collect the computation execution time t of each data block in the same way as in step 3, and fit the formula t = CostMM\(m\) that describes the relationship between the M-axis length and the computation execution time. CostMM is the fitted mapping between m and t.

**Splitting algorithm steps:**

1.  Based on the input matrix shape M, K, and N, set an appropriate short block length for splitting along the M axis according to empirical values. In the following expressions, a, b, and c represent candidate short block lengths given by empirical values. Substitute K and N into the following three expressions, and take the minimum value m0 among a, b, and c as the selected short block length.
    -   a \* K \* N \>= 4 \* 1024 \* 1024 \* 1024, where a takes the minimum value that satisfies the inequality
    -   b \* K \* N / 1024 + b \* N \>= 6 \* 1024 \* 1024, where b takes the minimum value that satisfies the inequality
    -   c \>= 3 \* 128, where c takes the minimum value that satisfies the inequality
    -   m0 = min\(a, b, c\)

2.  Based on the short block length m0 and the fitted formulas, obtain the computation execution time t0 = CostMM\(m0\) \* 1.15 and the communication execution time t1 = CostComm\(m0\) \* 1.15.

    Note: When communication and computation are executed in parallel, memory bandwidth contention may occur, increasing the execution time. Based on experience, a coefficient of 1.15 is generally multiplied in the fitting formula. You can adjust this coefficient according to actual measurements.

3.  Based on the short block length, balance computation and communication according to [Figure 1](#fig718114619319) or [Figure 2](#fig19667165810312) to obtain the long block length. Align the long block length to 128 elements as much as possible to ensure computation affinity. This example is the [communication-bound scenario](#fig19667165810312). Here, balancing means matching the communication time of the short block with the computation time of the long block: use t1 as the computation execution time of the long block, substitute it into the formula t1 = CostMM\(m1\), and calculate m1, which is the long block length.
4.  Based on the short block length m0, the long block length m1, and the original M-axis length M, obtain the number of long blocks count = \(M - m0\) / m1. This formula generally cannot be divided evenly. In this case, perform the following processing:
    -   Discard the fractional part of the result and keep the integer part as the number of blocks.
    -   Because the fractional part is discarded, the M-axis length has a remainder. Therefore, adjust the long block length m1 = \(M - m0\) / count.
    -   To maintain computation affinity, adjust the long block length m1 to 128-byte alignment, that is, round down to an integer multiple of 128, and update the long block length m1.
    -   Because the M-axis length has a remainder after m1 is adjusted, adjust the short block length m0 = M - \(m1 \* count\).
    -   Finally, obtain the short block length m0, the long block length m1, and the number of long blocks count.

## Verifying the Performance Gains of the Optimization Solution<a name="section8934151165215"></a>

-   Design a tiling strategy and verify the performance benefit

    In this MatmulAllReduce example, the input matrix Shape is M=4096, K=3072, N=8192, the data type is half, and the number of split cores is 8. Collected by the msOpProf tool before fusion, the Matmul computation execution time for this input is 803us, the AllReduce communication execution time is 1071us, and the total time is 1874us, which is a communication-bound scenario. According to the tiling algorithm described above, the specific tiling of this example is as follows:

    1.  Based on empirical values, select the short block (in a bound scenario, the short block is the head block, that is, the first data block after splitting) with an M-direction length m0 of 384. Then the communication data volume x is: 384 \* 8192 \* 2 / 1024 / 1024 = 6MB. According to the [communication fitting formula](#li1188918472441), the estimated communication execution time is 143us. Considering possible memory bandwidth conflicts, multiply it by a factor of 1.15, giving a communication execution time of 164us.
    2.  Calculate the M-direction length of the long block. Based on the short block communication time, balance the long block computation execution time to also be 164us, and estimate the length m1 as 768 according to the computation fitting formula.
    3.  Based on M=4096, m0=384, and m1=768, calculate the number of long blocks: \(4096 - 384\) / 768 = 4.83, rounded down to 4.
    4.  Based on the short block length m0=384 and the number of long blocks 4, adjust the long block length m1: \(4096 - 384\) / 4 = 928, aligned down to 128, adjusting m1 to 896.
    5.  Based on the long block length m1=896 and the number of long blocks 4, adjust the short block length m0: 4096 - 896 \* 4 = 512.
    6.  Finally, the original input matrix is split into 5 data blocks with lengths: \{512, 896, 896, 896, 896\}.

    As shown in the following code, set the designed tiling strategy in the Tiling code of the operator. Tested with this tiling strategy, the execution time of the fused operator is 1262us, so the performance benefit of the fused operator is \(1874 - 1262\) / 1874 = 32.7%.

    ```
    MatmulAllReduceCustomTilingData *tiling = context->GetTilingData<MatmulAllReduceCustomTilingData>();
    tiling->param.rankDim = 8;
    tiling->param.tileM = 512; // short block size
    tiling->param.tileNum = 1; // number of short blocks
    tiling->param.tailM = 896; // long block size
    tiling->param.tailNum = 4; // number of long blocks
    tiling->param.rankM = 4096;
    tiling->param.rankN = 8192;
    tiling->param.rankK = 4096;
    tiling->param.isTransposeA = 0;
    tiling->param.isTransposeB = 0;
    tiling->param.cToFloatLen = 0;
    tiling->param.nd2NzWorkLen = true;
    tiling->param.dataType = static_cast<uint8_t>(HCCL_DATA_TYPE_MAP.at(aType));
    ```

-   Adjust for tiling expansion

    As mentioned earlier, tiling the data causes expansion of the computation or communication execution time, making the measured results deviate from the theoretical values. For example, when the number of blocks is large, the expansion of the execution time has a greater impact on performance, which may reduce the performance benefit or even cause performance degradation. Therefore, you need to adjust the tiling strategy based on the theoretical tiling strategy described above and the measured results.

## Summary<a name="section15200958526"></a>

The MC<sup>2</sup> operator gains performance benefits through the parallel execution of computation and communication after data tiling, but it is affected by the expansion of execution time caused by data tiling. The primary approach to tuning the performance of the MC<sup>2</sup> operator is to formulate a data tiling strategy. Developers need to derive an ideal tiling strategy based on theoretical analysis, then adjust it according to actual measurement results, and ultimately find the optimal tiling strategy.
