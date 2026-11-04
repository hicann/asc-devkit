# Overview

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-10-23T12:29:25.039Z pushedAt=2026-11-03T07:05:13.318Z nl=2 -->

The Ascend C kernel function is a parallel function executed on the Device side. To improve the execution efficiency of an operator, multi-core parallel computation is usually adopted, where input data is split and then distributed to different logical cores for processing. In addition, because the Local Memory capacity of a single core is limited and cannot hold the entire input and output data of an operator at once, data must be moved in batches for computation. This process of splitting input data and computing it in blocks is called **Tiling**.

The algorithm for splitting data is called the Tiling algorithm or Tiling strategy. The program that computes split-related parameters (such as the block size of each data movement and the number of loops) based on information such as the operator shape is called the **Tiling implementation**, also known as the Tiling function. The [basic vector operator](../basic_vector_operator.md) and [TBuf usage](../tbuf_usage.md) samples both run on a single core and do not involve Host-side Tiling implementation.

Because the Tiling implementation performs only scalar computation, which AI Cores are not good at, it is usually separated out and executed on the Host side. For dynamic-shape operators, the split parameters cannot be determined at compile time and also need to be computed on the Host side.

Developers need to analyze and design Tiling parameters and define the Tiling structure. On the Host side, they obtain the input and output shape information through the context, compute the Tiling parameters based on the shape information, and set them into the corresponding Tiling structure. Then, the Tiling information is passed into the kernel function through the kernel function entry parameters. Inside the kernel function, the Tiling structure passed from the Host side is parsed, and the data movement into and out of Local Memory is controlled based on the Tiling information, and the computation logic is completed.

**Figure 1** Operator implementation components

![Operator implementation components](../../../../figures/operator_implementation_components.png "Operator implementation components")

As shown in the preceding figure, the operator implementation is composed of two parts: the kernel function implementation on the Device side and the Tiling implementation on the Host side. The kernel function implementation is responsible for completing logic such as data movement, vector computation, memory management, and task synchronization on the Device side. The Tiling implementation is used to compute, on the Host side, the split parameters required by the kernel function based on information such as the input shape before the kernel function is executed. For simple operators with fixed input and output shapes and a fixed split strategy, the Host-side Tiling implementation can be omitted. For dynamic-shape or multi-core parallel scenarios, parameters such as the amount of data processed by each core and the number of blocks within a core usually need to be computed on the Host side and then passed to the kernel function for use.

The following principles should be followed when splitting input data:

1. Use DataBlock as the smallest unit of splitting. Due to hardware limitations, when performing Unified Buffer (UB)-related data movement and vector computation, both the length of the moved data and the start address of the operand must be 32-byte aligned, that is, operations are performed in units of DataBlock, and the size of one DataBlock is 32 bytes. When the input data does not meet the DataBlock size alignment requirement, it must be aligned upward to the DataBlock size.

2. Make full use of the UB space. Performance overhead occurs when an AI Core interacts with external storage, and frequent data movement causes performance bottlenecks. Therefore, the UB space should be fully utilized as much as possible to reduce the number of times data is moved from Global Memory.

3. Make balanced use of multi-core computing capabilities. An AI processor contains multiple AI Cores, so computation should be distributed evenly across cores to fully leverage the advantages of multi-core parallelism.

This chapter is based on the [vector computation multi-core Tiling strategy sample](../../../../../../../examples/01_simd_cpp_api/02_features/02_tiling_selector/vector_tiling_strategy/README.md). It uses the Add vector operator, which adds two one-dimensional Tensors, as an example to describe typical multi-core Tiling scenarios. The sample implementation file is [vector_tiling_strategy.asc](../../../../../../../examples/01_simd_cpp_api/02_features/02_tiling_selector/vector_tiling_strategy/vector_tiling_strategy.asc).

**Figure 2**  Multi-core Tiling schematic diagram<a name="fig-2"></a>

![Multi-core Tiling schematic diagram](../../../../figures/vector_tiling_intro.png "Multi-core Tiling schematic diagram")

As shown in the preceding figure, the operator input of length `totalLength` is distributed to multiple cores for computation. Host-side Tiling first aligns the input data amount by DataBlock, then allocates the same number of main blocks to each core based on the main block length `mainTileLength`, and finally distributes the remaining data to each core as evenly as possible using DataBlock as the smallest unit. When the remaining data can be evenly divided among all cores, the amount of data processed by each core is the same; when it cannot be evenly divided, the amount of data processed by each core falls into two categories—full cores that process more data and tail cores that process less data, where the number of full cores is recorded as `formerNum`. For the computation data of a single core, the core first processes `tileNum` main blocks of length `mainTileLength`; if the length of the last remaining data is less than one main block, the tail block length is recorded by `lastTileLength`. After Tiling is complete, the preceding parameters are encapsulated into a Tiling structure and passed to the kernel function side through the kernel function entry. The kernel function side calculates the Global Memory offset of the input data to be processed by the current core based on `GetBlockIdx()` and the Tiling parameters, and loops through data loading, computation, and data writing back according to `tileNum` and `lastTileLength`.

This chapter involves the following concepts:

- Main block: the largest data block processed by a single core at a time, determined based on the available Local Memory space. The main block length is represented by `mainTileLength` and must be aligned to 32 bytes.
- Tail block: the last data block within a single core whose length is less than `mainTileLength`. The tail block length is represented by `lastTileLength`.
- Full core: a core that processes more data when data cannot be evenly divided among cores.
- Tail core: a core that processes less data when data cannot be evenly divided among cores.

Based on how data is split, there are the following scenarios:

1. [Even split of the main block](main_block_even_split.md): The input data can be evenly split by the number of cores involved in the computation and the main block length. After the main block allocation is complete, no data remains, the amount of data processed by each core is the same, and each core contains only the main block.
2. [Even split of the tail block](tail_block_even_split.md): After the main block allocation is complete, there is still remaining data, but the remaining data can be evenly distributed to each core with DataBlock as the smallest unit. The amount of data processed by each core is the same, and in addition to the main block, each core also contains a tail block of equal length.
3. [Tail core split](tail_core_split.md): After the main block allocation is complete, there is still remaining data, but the remaining data is not enough to allocate one DataBlock to each core, so it can only be allocated to some cores with DataBlock as the smallest unit. The cores that obtain the extra DataBlock are full cores, and the rest are tail cores; a tail core contains only the main block.
4. [Tail core and tail block split](tail_core_tail_block_split.md): After the main block allocation is complete, there is still remaining data. Each core can be allocated several DataBlocks, but a small number of DataBlocks can still only be allocated to some cores. In this case, there are full cores that process a larger amount of data, and tail cores also need to process tail blocks.
