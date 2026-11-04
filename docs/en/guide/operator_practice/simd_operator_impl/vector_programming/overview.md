# Overview<a name="ZH-CN_TOPIC_0000002500468230"></a>

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-10-23T12:24:22.743Z pushedAt=2026-11-03T07:05:13.314Z nl=2 -->

This section uses the Add operator as an example to guide you through quickly building an Ascend C vector operator program and learning the typical scenarios and handling methods of vector operator development. The scenarios covered include:

-   [Basic Vector Operator](basic_vector_operator.md): Develop a simple Add vector operator.
-   [Using TBuf](tbuf_usage.md): Use temporary space to store intermediate results during operator computation.
-   [Multi-Core Tiling](multi_core_tiling/overview.md): When an operator requires multi-core parallel computation or supports dynamic shapes, split and block the input data, and distribute different data blocks to multiple AI Cores for processing.
    -   [Main Block Even Split](multi_core_tiling/main_block_even_split.md): The operator runs on multiple cores of the AI processor, and all cores process an equal amount of data with 32-byte alignment.
    -   [Tail Block Even Split](multi_core_tiling/tail_block_even_split.md): The operator runs on multiple cores of the AI processor, and all cores process an equal amount of data. On each core, except for the last data block (tail block), the remaining data blocks are of equal size, and each core needs to process the tail block data.
    -   [Tail Core Split](multi_core_tiling/tail_core_split.md): The operator runs on multiple cores of the AI processor, and the data cannot be evenly distributed to each core. All cores are divided into multiple full cores and multiple tail cores. The full cores process an equal amount of data, and the tail cores process an equal amount of data.
    -   [Tail Core Tail Block Split](multi_core_tiling/tail_core_tail_block_split.md): The operator runs on multiple cores of the AI processor, and the data cannot be evenly distributed to each core. Meanwhile, the data within each core cannot be evenly split. Except for the last data block (tail block), the remaining data blocks are of equal size, and each core needs to process the tail block data separately.
-   [Double Buffer Scenario](double_buffer_scenario.md): Enable double buffer so that multiple pipelines in the operator execute in parallel.
-   [Broadcast Scenario](broadcast_scenario.md): The shapes of the two inputs in the operator are not equal. The shape of one input must be broadcast before the computation is performed.
-   [Unaligned Scenario](unaligned_scenario.md): Solutions for more scenarios where data is not 32-byte aligned.

>[!NOTE] Note
>When performing data movement and Vector computation, the following alignment requirements apply to the length of the moved data and the start address of the operands:
>- When using the DataCopy interface for data movement, the length of the moved data and the start address of the operands in the Unified Buffer (UB) must be 32-byte aligned.
>- Generally, when performing Vector computation, the start address of the operands must be 32-byte aligned, and the basic unit of computation is 32 bytes.
