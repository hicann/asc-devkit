# Inter-core Load Balancing<a name="ZH-CN_TOPIC_0000001892919393"></a>

<!-- md-trans-meta sourceCommit=dd293a1c6b873d149bf289bb2b8098dadf9f5cbd translatedAt=2026-10-23T12:52:36.182Z pushedAt=2026-11-03T07:05:13.351Z nl=2 -->

[Priority]: Medium

[Description] The number of physical cores in an AI processor is fixed. After L2 Cache tiling, some cores may experience a computation tail. That is, the total computation amount of all cores divided by the data amount processed by each core cannot be evenly divided by the number of cores, so some tail cores are required to compute the tail block data at the end. When the tail cores are computing, some cores remain idle, which degrades the overall performance of the operator. As shown in [Figure 1](#fig26395321518), assume that the total data amount is TotalSize. After L2 Cache tiling, the data is divided into two parts of TotalSize / 2, and the computation amount of each core per iteration is TotalSize / 2 / 25, meaning that 25 cores are required for processing. Since the AI processor has 20 cores, at computation time, each of cores 1 to 5 needs to compute one extra data block, resulting in a tail.

For a more detailed multi-core Tiling process, see [Multi-core Tiling](../../simd_operator_impl/vector_programming/multi_core_tiling/overview.md).

[Negative example]

**Figure 1**  Computation tail diagram<a name="fig26395321518"></a>  
![](../../../figures/calc_tail.png "Computation tail diagram")

[Positive example]

For the preceding tiling strategy, adjusting the position of the tail cores can achieve globally optimal load, as shown in [Figure 2](#fig133658471154). When all computation is complete, cores 1 to 10 compute one extra data block, achieving globally optimal load.

**Figure 2**  Inter-core load balancing diagram<a name="fig133658471154"></a>  
![](../../../figures/core_load_balance.png "Inter-core load balancing diagram")
