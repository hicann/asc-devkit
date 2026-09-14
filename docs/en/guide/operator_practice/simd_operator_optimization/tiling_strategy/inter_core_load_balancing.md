# Inter-Core Load Balancing<a name="ZH-CN_TOPIC_0000001892919393"></a>

<!-- md-trans-meta sourceCommit=d0ede7621d11bdc88cdafb221560ec6e0bfe996d translatedAt=2026-08-26T14:36:39.234Z -->

[Priority] Medium

[Description] The number of physical cores in an AI processor is fixed. After L2 Cache is split, some cores may experience a computation tail. That is, the total computation amount of all cores divided by the amount of data processed by each core cannot be evenly divided by the number of cores, so some tail cores are required to compute the tail data block at the end. While the tail cores are computing, some cores remain idle, which degrades the overall performance of the operator. As shown in [Figure 1](#fig26395321518), assume that the total data amount is TotalSize. After L2 Cache is split, the data is divided into two parts of TotalSize / 2, and the computation amount of each core per pass is TotalSize / 2 / 25, which means 25 cores are required for processing. Since the AI processor has 20 cores, cores 1 to 5 each need to compute one extra data block per pass, resulting in a computation tail.

[Negative Example]

**Figure 1** Computation tail diagram<a name="fig26395321518"></a>  
![](../../../figures/computation_tail_diagram.png "Computation tail diagram")

[Positive Example]

For the preceding splitting strategy, adjusting the positions of the tail cores can achieve globally optimal load balancing, as shown in [Figure 2](#fig133658471154). When all computation is complete, cores 1 to 10 compute one extra data block, achieving globally optimal load balancing.

**Figure 2** Inter-core load balancing diagram<a name="fig133658471154"></a>  
![](../../../figures/inter_core_load_balancing_diagram.png "Inter-core load balancing diagram")
