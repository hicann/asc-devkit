# Overview<a name="ZH-CN_TOPIC_0000002500468232"></a>

<!-- md-trans-meta sourceCommit=d0ede7621d11bdc88cdafb221560ec6e0bfe996d translatedAt=2026-08-26T14:06:01.111Z -->

An Ascend C kernel function is a processing function that runs on a single core. The [basic vector operator](../basic_vector_operator.md) and [TBuf usage](../tbuf_usage.md) samples described above are both operators that run on a single core and do not involve Host-side Tiling implementation. The composition of a vector operator implementation is shown in the following figure.

To improve the execution efficiency of an operator, multi-core parallel computing is usually implemented in the operator, that is, the input data is split and different data blocks are assigned to different cores for processing. In addition, because the internal Local Memory on a single core is limited in size, there are scenarios where the input and output data of an operator cannot be fully accommodated at one time. Therefore, a portion of the input needs to be moved in for computation and then moved out, and the next portion of the input is moved in for computation, until the final complete result is obtained. This process of data splitting and block-by-block computation is called **Tiling**. The algorithm for splitting data is called the Tiling algorithm or Tiling strategy. The computation program that determines the parameters related to the data splitting algorithm (such as the block size moved each time and the total number of loops) based on information such as the operator shape is called the **Tiling implementation**, also known as the Tiling Function. Because all the computations completed in the Tiling implementation are scalar computations, which AI Core is not good at, they are separated out and executed on the Host-side CPU. Inside the kernel function, the Tiling information is obtained by parsing the Tiling structure passed in from the Host side, and the process of moving data into and out of Local Memory is controlled based on the Tiling information. The operator logic is implemented by calling the computation, data movement, memory management, and task synchronization APIs.

**Figure 1** Operator implementation composition<a name="zh-cn_topic_0000002256238457_fig2052955220511"></a>  
![](../../../../figures/operator_implementation_composition.png "Operator implementation composition")

Due to hardware limitations, the following principles should be followed when splitting input data:

1.  Due to the physical limitations of the Unified Buffer on AI Core, the data storage space on the Unified Buffer must maintain 32-byte alignment.
    -   When the input data does not satisfy 32-byte alignment, the length of the input data rounded up to 32 bytes should be taken as the total length of the input data.
    -   When performing Tiling-related computations, 32 bytes should be used as the minimum unit.

2.  Make the best possible use of the Unified Buffer space.

    Performance overhead is incurred when AI Core interacts with external storage, and frequent data movement can cause performance bottlenecks. Therefore, the Unified Buffer space should be fully utilized as much as possible to reduce the number of times data is moved from Global Memory.

3.  An AI processor contains multiple AI Cores. The multi-core computing capability should be fully and evenly utilized, and computation should be evenly distributed across multiple AI Cores.

This chapter describes several typical scenarios based on the preceding principles.

**Figure 2**  Multi-core and Tiling schematic diagram<a name="zh-cn_topic_0000002256238457_fig188456591607"></a>  
![](../../../../figures/multi_core_and_tiling_diagram.png "Multi-core and Tiling schematic diagram")

The data splitting is shown in the preceding figure. The operator input with a length of TOTAL\_LENGTH is distributed to multiple cores for computation, and the length of the data computed on each core is BLOCK\_LENGTH. The data computed on each core is further split based on the size of the Local Memory. The number of split data blocks is TILE\_NUM, and the length of each data block is TILE\_LENGTH.

Depending on whether the amount of data computed on each core is the same and whether the amount of data in each data block within a core is the same, the splitting strategies may fall into the following scenarios:

1.  Even split across cores and even split within a core: The amount of data processed by each core is the same, and the amount of data in each data block within a core is the same. In this scenario, data is evenly distributed to each core for execution through [Multi-core Tiling](multi_core_tiling.md), and the length of the data computed each time on each core is the same.
2.  Even split across cores and uneven split within a core: The amount of data processed by each core is the same, but the amount of data in each data block within a core is not exactly the same. This scenario is based on multi-core Tiling. The data within a core cannot be split into multiple data blocks of the same size that are 32-byte aligned, so [Tail Block Tiling](tail_block_tiling.md) is required to handle the computation of the tail block data.
3.  Uneven split across cores and even split within a core: The amount of data processed by each core is different, but the amount of data in each data block within a core is the same. In this scenario, [Tail Core Tiling](tail_core_tiling.md) is used to resolve the issue that data cannot be evenly distributed across cores.
4.  Uneven split across cores and uneven split within a core: The amount of data processed by each core is different, and the amount of data in each data block within a core is not exactly the same. In this scenario, both [Tail Core & Tail Block](tail_core_tail_block.md) must be considered to handle the proper splitting of data across multiple cores and within each core.

