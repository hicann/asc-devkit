# Background and Core Concepts<a name="ZH-CN_TOPIC_0000002569070895"></a>

<!-- md-trans-meta sourceCommit=7f211052f814e6d340cc6bf2dfc44c8393b5424f translatedAt=2026-11-02T02:48:33.252Z pushedAt=2026-11-18T01:25:06.672Z nl=2 -->

The matrix computing unit adopts a block-based computation logic. For example, it can process 16×16 matrix tiles in parallel. In traditional linear storage layouts (such as row major or column major), data is stored contiguously by row or by column. When a 16×16 rectangular data block needs to be read, obtaining it directly from memory requires accessing multiple non-contiguous memory addresses to "gather" the data row by row. Such non-contiguous access significantly degrades memory efficiency.

To address this issue, the fractal matrix storage format is introduced. This format rearranges data at the software layer so that each 16×16 computation block is stored contiguously in physical memory. The hardware only needs to issue a single contiguous read to load the entire data block into on-chip registers, avoiding the overhead of multi-address addressing and thereby greatly improving the data throughput of the matrix computing unit.

To pursue extremely high data throughput and computational density, the matrix computing unit of the Ascend AI Processor adopts a proprietary hardware architecture design. Unlike the contiguous linear storage commonly used by general-purpose CPUs, this unit requires that input and output data conform to a specific fractal storage format. When developing operators using the **Mmad**-class APIs of Ascend C, a thorough understanding of these data formats is essential for implementing correct data movement, block partitioning, and performance optimization.

For ease of understanding and memorization, this document adopts the intuitive "big Y, small x" naming convention:

- Big Y (Z/N): Indicates the arrangement order between fractal matrices (Z is row major, and N is column major).
- Small x (z/n): Indicates the arrangement order of elements within a fractal matrix (z is row major, and n is column major).
