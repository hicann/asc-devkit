# Background and Core Concepts<a name="ZH-CN_TOPIC_0000002569070895"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-27T16:36:28.359Z -->

The cube computing unit adopts a block-based computation logic. For example, it can process a 16×16 cube tile in parallel. In traditional linear storage layouts (such as row major or column major), data is stored contiguously by row or by column. When a 16×16 rectangular data block needs to be read, if it is fetched directly from memory, multiple non-contiguous memory addresses must be accessed to "gather" the data row by row. Such non-contiguous access significantly degrades memory efficiency.

To address the above problem, the cube fractal storage format is introduced. This format rearranges data at the software layer so that each 16×16 computation block is stored contiguously in physical memory. The hardware only needs to initiate a single contiguous read to load the entire data block into on-chip registers, avoiding the overhead of multi-address addressing, thereby greatly improving the data throughput of the cube computing unit.

To pursue extremely high data throughput and computation density, the cube computing unit of the Ascend AI Processor adopts a proprietary hardware architecture design. Unlike the contiguous linear storage commonly used by general-purpose CPUs, this unit requires that both input and output data conform to a specific fractal storage format. When developing operators using the **Mmad**-class APIs of **Ascend C**, a thorough understanding of these data formats is essential for implementing correct data movement, tiling, and performance optimization.

For ease of understanding and memorization, this document adopts the intuitive "big Y small x" naming convention:

- Big Y (Z/N): Indicates the arrangement order among fractal matrices (Z is row major, and N is column major).
- Small x (z/n): Indicates the arrangement order of elements within a fractal cube (z is row major, and n is column major).
