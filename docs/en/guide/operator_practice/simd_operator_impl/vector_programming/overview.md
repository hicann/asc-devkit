# Overview<a name="ZH-CN_TOPIC_0000002500468230"></a>

<!-- md-trans-meta sourceCommit=0d34cdb684455a0bff9d665380eb33bff08028bd translatedAt=2026-08-26T13:58:13.209Z -->

This section uses the Add operator as an example to guide you through building an Ascend C vector operator program and learning the typical scenarios and handling methods of vector operator development. The scenarios covered include:

-   [Basic Vector Operator](basic_vector_operator.md): Develop a simple Add vector operator.
-   [Using TBuf](tbuf_usage.md): Use temporary space to store intermediate results during operator computation.
-   [Multi-core Tiling](multi_core_tiling/multi_core_tiling.md): The operator runs on multiple cores of the AI processor, and all cores compute an equal amount of data that is 32-byte aligned.
-   [Tail Block Tiling](multi_core_tiling/tail_block_tiling.md): The operator runs on multiple cores of the AI processor, and all cores compute an equal amount of data. On each core, all data blocks except the last one (the tail block) have an equal amount of data, and each core needs to process the computation of the tail block data.
-   [Tail Core Tiling](multi_core_tiling/tail_core_tiling.md): The operator runs on multiple cores of the AI processor, and the data cannot be evenly distributed to each core. All cores are divided into multiple full cores and multiple tail cores. The full cores compute an equal amount of data, and the tail cores compute an equal amount of data.
-   [Tail Core & Tail Block](multi_core_tiling/tail_core_tail_block.md): The operator runs on multiple cores of the AI processor. The data cannot be evenly distributed to each core, and the data within each core cannot be evenly divided either. All data blocks except the last one (the tail block) have an equal amount of data, and each core needs to process the computation of the tail block data separately.
-   [DoubleBuffer Scenario](doublebuffer_scenario.md): Enable double buffer so that multiple pipelines in the operator execute in parallel.
-   [Broadcast Scenario](broadcast_scenario.md): The two inputs of the operator have unequal shapes. The shape of one input needs to be broadcast before the computation is performed.
-   [Non-aligned scenario](non-aligned_scenario.md): Solutions for more scenarios where data is not 32-byte aligned.

> [!NOTE] Note
> During data transfer and Vector computation, the following alignment requirements apply to the transferred data length and the start address of the operand:
> - When using the DataCopy API for data transfer, the transferred data length and the start address of the operand (on the Unified Buffer) must be 32-byte aligned.
> - Generally, during Vector computation, the start address of the operand must be 32-byte aligned, and the basic unit of computation is 32 bytes.
