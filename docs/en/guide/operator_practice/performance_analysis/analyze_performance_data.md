# Analyzing Performance Data<a name="ZH-CN_TOPIC_0000001846719828"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-23T11:47:15.546Z pushedAt=2026-11-03T07:05:13.285Z nl=2 -->

## Theoretical Parameters<a name="section142521157183920"></a>

Theoretical performance is the ideal target for the actual performance of an operator. Different hardware platforms have different hardware specifications. Theoretical performance helps us understand the potential of the hardware and thereby set the goal for performance optimization.

-   The theoretical time of data movement pipelines (MTE1/MTE2/MTE3, etc.) = data movement volume (unit: Byte) / theoretical bandwidth. For example, the GM peak bandwidth of one of the models of AI processors is about 1.8TB/s. To move a cube of 4096 \* 4096 in float data type, the theoretical time of the data movement is sizeof\(float\) \* 4096 \* 4096 / 1.8TB/s = 37.28us (calculated with 1TB = 10<sup>12</sup>Byte).

    >[!NOTE] Note 
    >- When multiple data movement instructions exist at the same time, they share the bandwidth, and not every instruction can move data at a rate close to the theoretical bandwidth. For example, when MTE2/MTE3 perform GM read/write at the same time, the time of the data movement pipeline should be (MTE2 movement volume + MTE3 movement volume) / GM bandwidth.
    >- When moving data blocks of different sizes, the bandwidth utilization (effective bandwidth / theoretical bandwidth) differs. For the situation where the data volume of each movement is small, the measured performance cannot reach the theoretical bandwidth.

-   The theoretical time of compute pipelines (Cube/Vector/Scalar, etc.) = compute data volume (unit: Element) / theoretical computing power. For example, the vector theoretical peak computing power of one of the models of AI processors for the float data type is 11.06 TOPS. To perform a single-instruction computation of 32K float elements, the theoretical time is 32K / 11.06 TOPS = 0.003 us (calculated with 1K = 1000).

## Locating Bottlenecks<a name="section423433354011"></a>

After [collecting performance data](collect_performance_data.md), the parts that differ significantly from the theoretical values and the processes that take a long time are considered "**bottlenecks**". The following describes how to locate bottlenecks and the corresponding optimization directions based on performance data.

-   **Method one: Analyze the pipeline through on-board profiling.**

    View the **op\_summary\_\*.csv** file parsed from on-board profiling to analyze the pipeline. Note: \* indicates a timestamp.

    >[!NOTE] Note 
    >In SIMD and SIMT hybrid programming scenarios, due to the inherent characteristics of the hardware architecture, all computing tasks are scheduled and executed with VF (Vector Function) as the basic unit. Therefore, in profiling data, the overall execution time of both SIMT and SIMD VFs is counted as **aiv\_vec\_time**. In particular, the time spent reading and writing Global Memory during SIMT VF execution is also counted in the **aic\_vec\_time** metric.

    **Figure 1**  op\_summary\_\*.csv example one<a name="fig15532143763010"></a>  
    ![](../../figures/op_csv_ex1.png "op_summary_-csv example one")

    The utilization of each pipeline should ideally be 100%. A pipeline that does not reach 100% may have room for improvement. In the example in the figure above, the data is acquired on one of the models of AI processors. It can be seen that for the Cube operator MatMulV2, the utilization of the Cube pipeline **aic\_mac\_ratio** is around 80%, which preliminarily indicates that the computing power is not fully utilized. The utilization of the MTE2 pipeline **aic\_mte2\_ratio** is already around 95%, indicating that MTE2 is the longest pipeline.

    Then compare the longest pipeline with the theoretical value: the shapes of the input left and right cubes are (2048, 12288) and (12288, 6144) respectively, and the data type is bfloat16. The shape of the Bias input is (6144), and the data type is float. From this, the total amount of data to be moved can be calculated, and then through the theoretical time calculation method for the data movement pipeline introduced in [theoretical parameters](#section142521157183920), the theoretical value is calculated as \(sizeof\(bfloat16\) \* \(2048 \* 12288 + 12288 \* 6144\) + sizeof\(float\) \* 6144\) / 1.8TB/s ≈ 111.8us (calculated with 1TB = 10<sup>12</sup>Byte), which differs significantly from the actual performance data **aic\_mte2\_time**. Analysis shows that the total size of the input data has exceeded the L1 space (512KB), so MatMul computation involves repeated movement of input cube data. Whether the number of repeated movements is reasonable needs to be optimized in combination with [pipeline optimization](../simd_operator_optimization/pipeline_scheduling/pipeline_scheduling.md) and [Tiling optimization](../simd_operator_optimization/tiling_strategy/tiling_strategy.md). For further analysis, see [method three: view the simulation pipeline diagram to analyze each pipeline](#li169522115204).

    **Figure 2**  op\_summary\_\*.csv example two<a name="fig115151013124114"></a>  
    ![](../../figures/op_csv_ex2.png "op_summary_-csv example two")

    In the example in the figure above, the shape of the operator input is (8192, 8192), and the data type is float. From this, the total amount of data to be moved can be calculated, and then through the theoretical time calculation method for the data movement pipeline introduced in [theoretical parameters](#section142521157183920), the theoretical value is calculated as sizeof\(float\) \* \(8192 \* 8192\) / 0.8TB/s ≈ 335.5us (calculated with 1TB = 10<sup>12</sup>Byte; different AI processors have different theoretical bandwidths), which is consistent with the actual performance data **aiv\_mte2\_time**. It can be determined that this operator is basically an MTE2-bound (reaching the upper limit) data movement operator. In this example, the overall execution time duration is 350us, which is on par with the actual MTE2 time, indicating that the operator has been fully tuned. If there is a large gap between the MTE2 time and the overall execution time, then the next optimization direction is mainly [pipeline optimization](../simd_operator_optimization/pipeline_scheduling/pipeline_scheduling.md) combined with [Tiling optimization](../simd_operator_optimization/tiling_strategy/tiling_strategy.md), so that other pipelines are hidden in the MTE2 pipeline as much as possible. For further analysis, see [method three: view the simulation pipeline diagram to analyze each pipeline](#li169522115204).

-   **Method 2: Analyze the Tiling situation through on-board profiling**

    View the **op\_summary\_\*.csv** file parsed from on-board profiling to analyze the Tiling situation.

    **Figure 3**  op\_summary\_\*.csv example<a name="fig10442174973013"></a>  
    ![](../../figures/op_csv_ex.png "op_summary_-csv example")

    The example in the figure above shows data acquired on one of the AI processors. The hardware platform shows that this AI processor has 48 vector cores. The Mul operator is a vector-only operator, but in some scenarios not all vector cores are used (Block Dim < 48), resulting in wasted computing power. Therefore, the next optimization focus is [Tiling optimization](../simd_operator_optimization/tiling_strategy/tiling_strategy.md).

-   <a name="li169522115204"></a>**Method 3: Analyze the pipeline situation through the simulation pipeline diagram**

    **Figure 4**  Simulation pipeline diagram example<a name="fig1978415320315"></a>  
    ![](../../figures/sim_pipe_ex.png "Simulation pipeline diagram example")

    The example in the figure above shows data acquired on one of the AI processors. It can be seen that the related pipelines of the vector cores (MTE2 and MTE3 of vec0, MTE2 and MTE3 of vec1, and so on) exhibit regular pipeline interruptions. Combined with the operator logic analysis, it can be determined whether factors such as data dependencies cause the interruptions. Then the next main optimization direction is [pipeline optimization](../simd_operator_optimization/pipeline_scheduling/enable_double_buffer.md), followed by [Tiling optimization](../simd_operator_optimization/tiling_strategy/tiling_strategy.md) and [memory optimization](../simd_operator_optimization/memory_access/memory_access.md) to further improve the Vector pipeline utilization.

-   **Method 4: View the overhead through on-board profiling**

    Overhead is the latency before the operator executes computation, including core startup, core address fetch TLB MISS, same-address access (due to hardware limitations, simultaneous access to the same memory address by multiple cores causes conflicts and additional latency), and latency caused by variable resource initialization. Taking the Atlas A2 products as an example, the full-core overhead is about 20\~21 microseconds. For operators whose own latency is at the microsecond level, such as those in the inference field, overhead is an object worth optimizing.

    Through the **on-board profiling** data (the **TaskDuration** data when the kernel function is empty), the startup overhead of each core can be seen. Then through methods such as **using an appropriate number of cores and the operator kernel type**, continuous practice can be performed to find the optimal configuration. For specific optimization directions, see [overhead optimization](../simd_operator_optimization/overhead_optimization/overhead_optimization.md).
