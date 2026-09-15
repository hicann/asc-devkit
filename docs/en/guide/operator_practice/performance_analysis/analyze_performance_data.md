# Analyzing Performance Data<a name="ZH-CN_TOPIC_0000001846719828"></a>

<!-- md-trans-meta sourceCommit=d0ede7621d11bdc88cdafb221560ec6e0bfe996d translatedAt=2026-08-26T13:00:53.889Z -->

## Theoretical Parameters<a name="section142521157183920"></a>

The theoretical performance is the ideal target for the actual operator performance. Different hardware platforms have different hardware specifications. The theoretical performance helps you understand the potential of the hardware and set performance optimization goals accordingly.

-   The theoretical time of data movement pipelines (MTE1/MTE2/MTE3, etc.) = data movement volume (unit: byte) / theoretical bandwidth. For example, the peak GM bandwidth of a certain AI processor is about 1.8TB/s. To move a matrix of the float data type with a size of 4096 \* 4096, the theoretical time is sizeof\(float\) \* 4096 \* 4096 / 1.8TB/s = 37.28us (calculated based on 1 TB = 10<sup>12</sup> bytes).

    >[!NOTE] Note
    >- When multiple data movement instructions exist simultaneously, they share the bandwidth, and not every instruction can move data at a rate close to the theoretical bandwidth. For example, when MTE2/MTE3 perform GM read/write at the same time, the time of the data movement pipeline should be (MTE2 data movement volume + MTE3 data movement volume) / GM bandwidth.
    >- When moving data blocks of different sizes, the bandwidth utilization (effective bandwidth / theoretical bandwidth) differs. When the amount of data moved each time is small, the measured performance cannot reach the theoretical bandwidth.

-   The theoretical time of compute pipelines (Cube/Vector/Scalar, etc.) = compute data volume (unit: Element) / theoretical computing power. For example, the theoretical peak Vector computing power of a certain AI processor for the float data type is 11.06TOPS. To perform a single-instruction computation on 32K float Elements, the theoretical time is 32K / 11.06TOPS = 0.003us (calculated based on 1K = 1000).

## Locating Bottlenecks<a name="section423433354011"></a>

After [obtaining performance data](obtain_performance_data.md), the areas that differ significantly from the theoretical values and the processes that take a long time are considered "**bottlenecks**". The following describes how to locate bottlenecks and the corresponding optimization directions based on performance data.

-   **Method 1: Analyzing the pipeline using on-board profiling**

    View the op\_summary\_\*.csv file parsed from on-board Profiling to analyze the pipeline. Note: "\*" indicates a timestamp.

    >[!NOTE] Note
    >In SIMD and SIMT hybrid programming scenarios, due to the inherent characteristics of the hardware architecture, all compute tasks are scheduled and executed with VF (Vector Function) as the basic unit. Therefore, in Profiling data, the overall execution time of both SIMT and SIMD VFs is counted as aiv\_vec\_time. In particular, the time spent reading from and writing to Global Memory during SIMT VF execution is also counted in the aic\_vec\_time metric.

    **Figure 1**  op\_summary\_\*.csv example 1<a name="fig15532143763010"></a>  
    ![](../../figures/op_summary_-csv_example_1.png "op_summary_-csv example 1")

    Ideally, the utilization of each pipeline should be 100%. A pipeline that does not reach 100% may have room for improvement. In the preceding example, the data is obtained from a certain AI processor. For the Cube operator MatMulV2, the utilization of the Cube pipeline, aic\_mac\_ratio, is about 80%, which preliminarily indicates that the computing power is not fully utilized. The utilization of the MTE2 pipeline, aic\_mte2\_ratio, is already about 95%, indicating that MTE2 is the longest pipeline.

    Then compare the longest pipeline with the theoretical value: the shapes of the input left and right matrices are (2048, 12288) and (12288, 6144) respectively, and the data type is bfloat16. The shape of the Bias input is (6144), and the data type is float. From this, the total amount of data to be moved can be calculated. Then, using the theoretical time calculation method for the data movement pipeline described in [Theoretical Parameters](#section142521157183920), the theoretical value is calculated as \(sizeof\(bfloat16\) \* \(2048 \* 12288 + 12288 \* 6144\) + sizeof\(float\) \* 6144\) / 1.8TB/s ≈ 111.8us (calculated with 1TB = 10<sup>12</sup>Byte), which differs significantly from the actual performance data aic\_mte2\_time. Analysis shows that the total size of the input data has exceeded the L1 space (512KB), so the input matrix data is moved repeatedly during MatMul computation. Whether the number of repeated data movements is reasonable needs to be optimized by combining [pipeline optimization](../simd_operator_optimization/pipeline_scheduling/pipeline_scheduling.md) and [Tiling optimization](../simd_operator_optimization/tiling_strategy/tiling_strategy.md). For further analysis, see [Method 3: Viewing the simulated pipeline diagram to analyze each pipeline](#li169522115204).

    **Figure 2**  op\_summary\_\*.csv example 2<a name="fig115151013124114"></a>  
    ![](../../figures/op_summary_-csv_example_2.png "op_summary_-csv example 2")

    In the preceding example, the shape of the operator input is (8192, 8192), and the data type is float. From this, the total amount of data to be moved can be calculated. Then, using the theoretical time calculation method for the data movement pipeline described in [Theoretical Parameters](#section142521157183920), the theoretical value is calculated as sizeof\(float\) \* \(8192 \* 8192\) / 0.8TB/s ≈ 335.5us (calculated with 1TB = 10<sup>12</sup>Byte; different AI processors have different theoretical bandwidths), which is consistent with the actual performance data aiv\_mte2\_time. It can be determined that this operator is basically an MTE2-bound operator (reaching the upper limit). In this example, the overall execution time Duration is 350us, which is equal to the actual MTE2 time, indicating that the operator has been fully optimized. If there is a large gap between the MTE2 time and the overall execution time, the next optimization direction is mainly [pipeline optimization](../simd_operator_optimization/pipeline_scheduling/pipeline_scheduling.md) combined with [Tiling optimization](../simd_operator_optimization/tiling_strategy/tiling_strategy.md), so that other pipelines are hidden in the MTE2 pipeline as much as possible. For further analysis, see [Method 3: Viewing the simulated pipeline diagram to analyze each pipeline](#li169522115204).

-   **Method 2: Analyzing the tiling status through on-board profiling**

    View the op\_summary\_\*.csv file parsed from on-board Profiling to analyze the Tiling status.

    **Figure 3**  op\_summary\_\*.csv example<a name="fig10442174973013"></a>  
    ![](../../figures/op_summary_-csv_example.png "op_summary_-csv example")

    In the example above, the data is obtained on a certain AI processor. Through the hardware platform, you can see that this AI processor has 48 Vector cores. The Mul operator is a pure Vector operator, but in some scenarios not all Vector cores are used (Block Dim < 48), resulting in wasted computing power. Therefore, the next main optimization direction is [Tiling optimization](../simd_operator_optimization/tiling_strategy/tiling_strategy.md).

-   <a name="li169522115204"></a>**Method 3: Analyzing the pipeline status through the simulated pipeline diagram**

    **Figure 4**  Simulated pipeline diagram example<a name="fig1978415320315"></a>  
    ![](../../figures/simulated_pipeline_diagram_example.png "Simulated pipeline diagram example")

    In the example above, the data is obtained on a certain AI processor. You can see that the related pipelines of the Vector cores (MTE2 and MTE3 of vec0, MTE2 and MTE3 of vec1, and so on) show regular interruptions. You can analyze the operator logic to determine whether factors such as data dependencies cause the interruptions. Therefore, the next main optimization direction is [pipeline optimization](../simd_operator_optimization/pipeline_scheduling/enable_doublebuffer.md), followed by [Tiling optimization](../simd_operator_optimization/tiling_strategy/tiling_strategy.md) and [memory optimization](../simd_operator_optimization/memory_access/memory_access.md) to further improve the Vector pipeline utilization.

-   **Method 4: Viewing the head overhead through on-board profiling**

    The head overhead is the latency generated before the operator starts computation, including core startup, core instruction fetch TLB MISS, same-address access (due to hardware limitations, simultaneous access to the same memory address by multiple cores causes conflicts and additional latency), and the latency caused by variable resource initialization. Taking Atlas A2 training products/Atlas A2 inference products as an example, the head overhead with all cores occupied is about 20 to 21 microseconds. For operators whose own latency is at the microsecond level, such as those in the inference field, the head overhead is worth optimizing.

    Through the **on-board Profiling** data (the TaskDuration data of an empty Kernel), you can see the startup overhead of each core. Then, by continuously practicing methods such as **using an appropriate number of cores and an appropriate operator Kernel Type**, you can try to find the optimal configuration. For specific optimization directions, see [head and tail overhead optimization](../simd_operator_optimization/overhead_optimization/overhead_optimization.md).

