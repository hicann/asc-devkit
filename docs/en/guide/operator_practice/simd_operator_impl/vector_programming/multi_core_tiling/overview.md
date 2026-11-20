# Overview

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-11-09T10:32:44.730Z pushedAt=2026-11-19T06:32:19.754Z nl=2 -->

The Ascend C kernel function is a parallel function executed on the device side. To improve operator execution efficiency, multi-core parallel computation is usually adopted, where input data is split and distributed to different logical cores for processing. In addition, because the Local Memory capacity of a single core is limited and cannot hold the entire input and output data of an operator at once, data must be moved in batches for computation. This process of splitting input data and computing it block by block is called **tiling**.

The algorithm for splitting data is called the tiling algorithm or tiling strategy. The program that computes splitting-related parameters (such as the block size of each data transfer and the number of loop iterations) based on information such as the operator shape is called the **tiling implementation**, also known as the tiling function. The [basic vector operator](../basic_vector_operator.md) and [TBuf usage](../tbuf_usage.md) samples both run on a single core and do not involve host-side tiling implementation.

Because the tiling implementation performs only scalar computation, which AI Cores are not good at, it is usually separated out and executed on the host side. For dynamic-shape operators, the splitting parameters cannot be determined at compile time and therefore also need to be computed on the host side.

Developers need to analyze and design the tiling parameters and define the tiling structure. On the host side, they obtain the input and output shape information through the context, compute the tiling parameters based on the shape information, and set them into the corresponding tiling structure. The tiling information is then passed into the kernel function through the kernel function entry parameters. Inside the kernel function, the tiling structure passed from the host side is parsed, and the data transfer into and out of Local Memory is controlled based on the tiling information, completing the computation logic.

**Figure 1** Operator implementation components

![Operator implementation components](../../../../figures/operator_implementation_components.png "Operator implementation components")

As shown in the figure above, the operator implementation consists of two parts: the kernel function implementation on the device side and the tiling implementation on the host side. The kernel function implementation is responsible for completing data transfer, vector computation, memory management, and task synchronization on the device side. The tiling implementation is used to compute, on the host side, the splitting parameters required by the kernel function based on information such as the input shape before the kernel function is executed. For simple operators with fixed input and output shapes and a fixed splitting strategy, the host-side tiling implementation can be omitted. For dynamic-shape or multi-core parallel scenarios, parameters such as the data size processed by each core and the number of blocks within a core usually need to be computed on the host side and then passed to the kernel function.

The following principles should be followed when splitting input data:

1. Use DataBlock as the minimum splitting unit. Due to hardware limitations, when performing Unified Buffer (UB)-related data transfer and vector computation, both the length of the transferred data and the start address of the operands must be 32-byte aligned, that is, operations are performed in units of DataBlock, where one DataBlock is 32 bytes. If the input data does not meet the DataBlock size alignment requirement, it must be aligned upward to the DataBlock size.

2. Make full use of the UB space. Interaction between the AI Core and external storage incurs performance overhead, and frequent data transfer can cause performance bottlenecks. Therefore, the UB space should be utilized as fully as possible to reduce the number of data transfers from Global Memory.

3. Balance the use of multi-core computing capabilities. An AI processor contains multiple AI Cores. Distribute computation evenly across cores to fully leverage the advantages of multi-core parallelism.

This chapter is based on the [vector computation multi-core tiling strategy sample](../../../../../../../examples/01_simd_cpp_api/02_features/02_tiling_selector/vector_tiling_strategy/README.md) and uses the Add vector operator, which adds two one-dimensional tensors, as an example to describe typical multi-core tiling scenarios. The sample implementation file is [vector_tiling_strategy.asc](../../../../../../../examples/01_simd_cpp_api/02_features/02_tiling_selector/vector_tiling_strategy/vector_tiling_strategy.asc).

**Figure 2** Multi-core tiling schematic diagram<a name="fig-2"></a>

![Multi-core tiling schematic diagram](../../../../figures/vector_tiling_intro.png "Multi-core tiling schematic diagram")

As shown in the figure above, the operator input of length `totalLength` is distributed across multiple cores for computation. On the host side, tiling first aligns the input data size to DataBlock, then allocates the same number of main blocks to each core based on the main block length `mainTileLength`, and finally distributes the remaining data across cores as evenly as possible using DataBlock as the minimum unit. When the remaining data can be evenly divided among all cores, each core processes the same data size. When it cannot be evenly divided, the per-core data sizes fall into two categories: full cores that process more data and tail cores that process less data, where the number of full cores is recorded as `formerNum`. For the computation data of a single core, the core first processes `tileNum` main blocks of length `mainTileLength`. If the length of the last remaining data is less than one main block, the tail block length is recorded through `lastTileLength`. After tiling is complete, the preceding parameters are encapsulated into a tiling structure and passed to the kernel function through the kernel function entry. On the kernel function side, the Global Memory offset of the input data to be processed by the current core is computed based on `GetBlockIdx()` and the tiling parameters, and data loading, computation, and data writing back are completed in a loop according to `tileNum` and `lastTileLength`.

This chapter involves the following concepts:

- Main block: the maximum data block processed by a single core at a time, determined based on the available Local Memory space. The main block length is represented by `mainTileLength` and must be aligned to 32 bytes.
- Tail block: the last data block within a single core whose length is less than `mainTileLength`. The tail block length is represented by `lastTileLength`.
- Full core: a core that processes more data when data cannot be evenly divided across cores.
- Tail core: a core that processes less data when data cannot be evenly divided across cores.

Based on how the data is split, the following scenarios exist:

1. [Main block even split](main_block_even_split.md): The input data can be evenly split by the number of cores participating in the computation and the main block length. After the main block allocation is complete, there is no remaining data. Each core processes the same data size, and each core contains only the main block.
2. [Tail block even split](tail_block_even_split.md): After the main block allocation is complete, there is still remaining data, but the remaining data can be evenly distributed to each core with DataBlock as the minimum unit. Each core processes the same data size, and in addition to the main block, each core also contains a tail block of equal length.
3. [Tail core split](tail_core_split.md): After the main block allocation is complete, there is still remaining data, but the remaining data is not enough to allocate one DataBlock to each core. It can only be allocated to some cores with DataBlock as the minimum unit. The cores that receive the additional DataBlock are full cores, and the rest are tail cores. A tail core contains only the main block.
4. [Tail core and tail block split](tail_core_tail_block_split.md): After the main block allocation is complete, there is still remaining data. Each core can be allocated several DataBlocks, but a small number of DataBlocks can still only be allocated to some cores. In this case, there are full cores that process more data, and the tail cores also need to process the tail block.
