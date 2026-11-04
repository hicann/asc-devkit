# MC² Operator Performance Tuning Case<a name="ZH-CN_TOPIC_0000002076345610"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-23T11:22:42.153Z pushedAt=2026-11-03T07:05:13.271Z nl=2 -->

## Case Introduction<a name="section144912211504"></a>

The performance benefit of the MC<sup>2</sup> computation-communication fusion operator mainly comes from the parallel execution of communication and computation. Specifically, the input data is split into multiple sub-blocks, and the computation and communication tasks of the sub-blocks form two pipelines. By executing the tasks on the two pipelines in parallel, pipeline masking is achieved, thereby improving operator performance. As shown in the figure below, in the scenario where the MC<sup>2</sup> operator performs Matmul computation first and then communication, the input cube is split into two blocks along the M axis. The Matmul computation of the second data block and the communication of the first data block can be executed in parallel, thereby masking the computation and communication time. In all figures in this section, MM represents Matmul computation, and hcom represents the communication task.

![](../../figures/fig_1_84.png)

This case describes how to analyze the performance benefit of the computation-communication fusion operator and how to design a proper data tiling strategy. For complete examples of the MC<sup>2</sup> operator, see [MatmulAllReduce example](https://gitcode.com/cann/ops-transformer/tree/9.2.0/mc2/matmul_all_reduce), [MatmulReduceScatter example](https://gitcode.com/cann/ops-transformer/tree/9.2.0/mc2/matmul_reduce_scatter_v2), and [AllGatherMatmul example](https://gitcode.com/cann/ops-transformer/tree/9.2.0/mc2/all_gather_matmul_v2).

## Obtaining Performance Data<a name="section4647105095111"></a>

Obtain the operator performance data by using the msOpProf operator tuning tool:

-   Obtain the [performance data collected in the real environment](../performance_analysis/collect_performance_data.md#section17953123893415), including the proportion of each pipeline stage;
-   Obtain the simulated performance data (instruction pipeline diagram), including the occupied intervals of each pipeline stage, so that the dependencies between pipeline stages can be observed and the parallel efficiency can be optimized.

## Analyzing the Main Bottleneck<a name="section371410542511"></a>

The operator performance benefit formula of MC<sup>2</sup> is as follows:

Serial time consumption of operators before fusion = computation operator time consumption before fusion + communication operator time consumption before fusion

MC<sup>2</sup> operator benefit = \( serial time consumption of operators before fusion - MC<sup>2</sup> operator time consumption after fusion \) / serial time consumption of operators before fusion

The execution time of the fused MC<sup>2</sup> operator is constrained by the following factors, which in turn affect the operator performance benefit.

-   Factor 1: Difference in execution time between computation and communication

    If the execution time of the computation and communication tasks differs little, after fusion, the computation and communication of the MC<sup>2</sup> operator are executed in parallel, achieving pipeline masking and yielding a large performance benefit.

    If the execution time of the computation and communication tasks differs greatly, after fusion, the computation and communication within the MC<sup>2</sup> operator are executed in parallel, but only a small amount of time can be masked. The overall execution time of the operator is close to that of the unsplit serial execution, in which case a large performance benefit cannot be obtained.

    ![](../../figures/fig_3.png)

-   Factor 2: Expansion of computation or communication execution time caused by data tiling

    After the input data is tiled, the original whole data block is split into several small data blocks, and Matmul computation or communication tasks are performed on these small data blocks respectively. Compared with the case before tiling, the execution time of the computation or communication task may expand (that is, the execution time becomes longer). The causes of this expansion include: the tiled data blocks are too small, which reduces the efficiency of computation or communication; too many tiled data blocks increase additional scheduling overhead; and after parallel execution, computation and communication conflict in accessing the L2 cache or device memory. Taking Matmul computation as an example, the following briefly describes the possible expansion of execution time after data tiling.

    -   No expansion:

        Before data tiling, the Matmul execution time is 200us. The Matmul input is evenly split into two blocks. Assume that after splitting, the Matmul execution time of each data block is 100us. Through parallel execution of computation, the actual performance benefit is 100us, as shown in the figure below.

        ![](../../figures/fig_4.png)

    -   Moderate expansion:

        Before data tiling, the Matmul execution time is 200us. The Matmul input is evenly split into two blocks. Assume that after splitting, the Matmul execution time of each data block is 150us. Through parallel execution of computation, the actual performance benefit is 50us, as shown in the figure below.

        ![](../../figures/fig_5.png)

    -   Severe expansion:

        Before data tiling, the Matmul execution time is 200us. The Matmul input is evenly split into two blocks. Assume that after splitting, the Matmul execution time of each data block is 200us. Through parallel execution of computation, the actual performance benefit is a degradation of 50us, as shown in the figure below.

        ![](../../figures/fig_5_85.png)

Based on the preceding analysis, scenarios where computation and communication execution times are relatively balanced achieve better pipeline masking and performance benefits. Meanwhile, the performance benefit is also affected by the execution time expansion caused by data tiling. The following sections describe how to formulate a data tiling strategy to achieve the optimal pipeline masking effect.

## Designing an Optimization Solution<a name="section7611135813517"></a>

Take the MatmulAllReduce operator on the Atlas A2 products as an example, where the input data format is ND and the data type is half. In this operator, computation execution comes first and the communication task comes second. Assume that in the Matmul computation, the shape of the left cube is \[M, K\], the shape of the right cube is \[K, N\], and the communication object in the operator is the output cube of Matmul. Then the input shape of the communication task is \[M, N\]. Because the K axis exists only in the Matmul computation, when the K axis is large, the computation workload is large and the computation execution time is greater than the communication execution time, in which case computation is the bottleneck (bound) of the operator. Conversely, when the K axis is small, the computation workload is small and the computation execution time is less than the communication execution time, in which case communication is the bottleneck of the operator. Before formulating the data tiling strategy, perform the computation and communication tasks on the original cube separately, and determine the bound scenario based on the execution time of the two tasks.

The data tiling strategy of this operator must meet the following requirements:

-   Only split the M axis. Because the Hccl API called by the communication task requires the tiled data memory to be contiguous, if the N axis is split, each row of data is cut off, causing the memory of the communication data to be non-contiguous and failing to meet the communication requirement. If the M axis is split, each row of data is memory-contiguous and meets the communication requirement.
-   If A represents a long block and B represents a short block, only the form of A or B arranged continuously can be produced, for example, AAAB, BAAA, and so on.

As described above, the goal of data tiling is to achieve as much pipeline masking as possible. Based on the difference in execution time between the computation and communication tasks, the actual scenario can be decomposed into the following two specific scenarios, each of which has its own subdivided tiling goal.

-   Computation bound:

    For the same tiled data block, the computation execution time is greater than the communication execution time. In this case, **computation is continuous**, and **the tail block of communication must be short**, as shown in the figure below.

    **Figure 1** Computation bound schematic diagram<a name="fig718114619319"></a>  
    ![](../../figures/calc_bound.png "Computation bound schematic diagram")

-   Communication bound:

    For the same data block after splitting, the communication execution time is longer than the computation execution time. In this case, **communication is continuous**, and **the head block of computation must be short**, as shown in the figure below.

    **Figure 2** Communication-bound schematic diagram<a name="fig19667165810312"></a>  
    ![](../../figures/comm_bound.png "Communication-bound schematic diagram")

**Prerequisites:**

Before the final data tiling, the following prerequisites must be completed: determine the bound scenario, and fit the formulas for the relationship between the data volume and the execution time of Matmul computation and AllReduce communication respectively. The specific steps are as follows.

1.  Execute the Matmul computation and AllReduce communication tasks on the input data separately, collect the execution time using the msOpProf tool, and determine the task with the longer time as the corresponding bound scenario. For example, if the communication execution time is longer than the computation execution time, it is a [communication-bound scenario](#fig19667165810312).
2.  Split the input data along the M axis into several data blocks with M-axis lengths of 256, 512, 768, 1024, and 2048 respectively. The block size can be adjusted based on the actual situation.
3.  Perform AllReduce communication on the data blocks obtained in step 2 separately, collect the execution time using the msOpProf tool, and obtain the execution time t<sub>1</sub>, t<sub>2</sub>, ..., t<sub>n</sub> of each data block. Then, plot and analyze the relationship between the data volume and the corresponding execution time, and fit the formula t = CostComm\(m\), where m indicates the M-axis length of the data block, t indicates the communication execution time of the data block, and CostComm indicates the mapping relationship between m and t obtained by fitting. This mapping relationship is generally linear. If it does not satisfy the linear relationship, piecewise fitting can be used. An example is as follows:

    ![](../../figures/mc2excel1.png)

    ![](../../figures/mc2excel2.png)

    The data volume x = m \* N \* sizeof\(dataType\), in bytes. The fitted formula is expressed as:

    -   <a name="li1188918472441"></a>When x is less than 8 MB: t = -0.9698202 \* x \* x + 27.0622573 \* x + 14.769, in us.
    -   When x is greater than or equal to 8 MB: t = 13.58491263 \* x + 61.508333, in us.

4.  Perform Matmul computation on each data block obtained by splitting in step 2, collect the computation execution time t of each data block in the same way as in step 3, and fit the formula t = CostMM\(m\) for the relationship between the M-axis length and the computation execution time. CostMM is the mapping relationship between m and t obtained by fitting.

**Splitting algorithm steps:**

1.  Based on the shape of the input cube, that is, M, K, and N, set an appropriate short block length for splitting along the M-axis according to empirical values. In the following expressions, a, b, and c represent the candidate short block lengths given by empirical values. After substituting K and N into the following three expressions, take the minimum value m0 of a, b, and c as the selected short block length.
    -   a \* K \* N \>= 4 \* 1024 \* 1024 \* 1024, where a takes the minimum value of the inequality
    -   b \* K \* N / 1024 + b \* N \>= 6 \* 1024 \* 1024, where b takes the minimum value of the inequality
    -   c \>= 3 \* 128, where c takes the minimum value of the inequality
    -   m0 = min\(a, b, c\)

2.  Based on the short block length m0 and the fitting formula, obtain the computation execution time t0 = CostMM\(m0\) \* 1.15 and the communication execution time t1 = CostComm\(m0\) \* 1.15.

    Note: When communication and computation are executed in parallel, memory bandwidth contention may occur, increasing the execution time. Based on experience, a coefficient of 1.15 is generally multiplied in the fitting formula. You can adjust this coefficient based on actual measurements.

3.  Based on the short block length, balance computation and communication according to [Figure 1](#fig718114619319) or [Figure 2](#fig19667165810312) to obtain the long block length. Align the long block length to 128 elements as much as possible to ensure computation affinity. In this example, which is a [communication-bound scenario](#fig19667165810312), balancing means matching the communication time of the short block to the computation time of the long block: use t1 as the computation execution time of the long block, substitute it into the formula t1 = CostMM\(m1\), and calculate m1, which is the long block length.
4.  Based on the short block length m0, the long block length m1, and the original M-axis length M, obtain the number of blocks count = \(M - m0\) / m1. This formula generally does not divide evenly, in which case the following processing is required:
    -   Discard the fractional part of the result and keep the integer part as the number of blocks.
    -   Because the fractional part is discarded, the M-axis length has a remainder. Therefore, adjust the long block length m1 = \(M - m0\) / count.
    -   To maintain computation affinity, adjust the long block length m1 to 128-byte alignment, that is, round down to an integer multiple of 128, and update the long block length m1.
    -   Because the M-axis length has a remainder after m1 is adjusted, adjust the short block length m0 = M - \(m1 \* count\).
    -   Finally, obtain the short block length m0, the long block length m1, and the number of long blocks count.

## Verifying the Performance Benefit of the Optimization Solution<a name="section8934151165215"></a>

-   Develop a splitting strategy and verify the performance benefit

    In this MatmulAllReduce example, the input cube shape is M=4096, K=3072, N=8192, the data type is half, and the number of cores is 8. According to data collected by the msOpProf tool before fusion, the Matmul computation execution time of this input is 803us, the AllReduce communication execution time is 1071us, and the total time consumption is 1874us, which is a communication-bound scenario. According to the splitting algorithm described above, the specific splitting of this example is as follows:

    1.  Based on empirical values, the M-direction length m0 of the short block (in a bound scenario, the short block is the head block, that is, the first data block after splitting) is selected as 384. Then the communication data volume x is: 384 \* 8192 \* 2 / 1024 / 1024 = 6MB. According to the [communication fitting formula](#li1188918472441), the communication execution time is estimated to be 143us. Considering that memory bandwidth conflicts may occur, a coefficient of 1.15 is multiplied, giving a communication execution time of 164us.
    2.  Calculate the M-direction length of the long block. Based on the short block communication time, the computation execution time of the long block is balanced to be the same 164us, and the length m1 is estimated to be 768 according to the computation fitting formula.
    3.  Based on M=4096, m0=384, and m1=768, calculate the number of long blocks: \(4096 - 384\) / 768 = 4.83, which is rounded down to 4.
    4.  Based on the short block length m0=384 and the number of long blocks 4, adjust the long block length m1: \(4096 - 384\) / 4 = 928, which is aligned downward to 128, adjusting m1 to 896.
    5.  Based on the long block length m1=896 and the number of long blocks 4, adjust the short block length m0: 4096 - 896 \* 4 = 512.
    6.  Finally, the original input cube is split into 5 data blocks with lengths of \{512, 896, 896, 896, and 896\}.

    As shown in the following code, the defined splitting strategy is set in the Tiling code of the operator. When tested with this splitting strategy, the execution time of the operator after fusion is 1262us, so the performance benefit of the fused operator is \(1874 - 1262\) / 1874 = 32.7%.

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

-   Adjust for splitting expansion

    As mentioned above, data tiling causes expansion of the computation or communication execution time, making the measured results deviate from the theoretical values. For example, when the number of blocks is large, the expansion of the execution time has a greater impact on performance, which may reduce the performance benefit or cause performance degradation. Therefore, the tiling strategy ultimately needs to be adjusted based on the theoretical tiling strategy described above, combined with actual measurements.

## Summary<a name="section15200958526"></a>

The MC<sup>2</sup> operator achieves performance benefits through the parallel execution of computation and communication after data tiling, but it is affected by the expansion of execution time after data tiling. The primary approach to tuning the performance of the MC<sup>2</sup> operator is to formulate a data tiling strategy. Developers need to derive an ideal tiling strategy through theoretical analysis, then adjust it based on measured results, and ultimately find the optimal tiling strategy.
