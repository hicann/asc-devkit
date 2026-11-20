---
okftype: reference.concept
description: "Explains that the cube computing unit uses 16x16 block computation. Under a linear row-major order or column-major order layout, reading a rectangular data block requires collecting addresses row by row across non-contiguous locations, resulting in low memory access efficiency. Therefore, a fractal storage format is introduced to make each computation block contiguous in physical memory. It also defines a naming convention in which the capital Y represents the arrangement order between fractals and the lowercase x represents the element order within a fractal, providing a terminology foundation for the detailed format descriptions that follow."
---
# Background and Core Concepts

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T09:58:59.952Z pushedAt=2026-11-18T08:15:34.209Z nl=2 -->

The cube computing unit adopts block-based computation logic. For example, it can process 16×16 cube tiles in parallel. In traditional linear storage layouts (such as row-major order or column-major order), data is stored contiguously by row or by column. When a 16×16 rectangular data block needs to be read, if it is fetched directly from memory, multiple non-contiguous memory addresses must be accessed to "gather" the data row by row. Such non-contiguous access significantly degrades memory efficiency.

To address the above problem, the cube fractal storage format is introduced. This format rearranges data at the software layer so that each 16×16 computation block is stored contiguously in physical memory. The hardware only needs to issue a single contiguous read to load the entire data block into on-chip registers, avoiding the overhead of multi-address addressing, thereby greatly improving the data throughput of the cube computing unit.

The cube computing unit of the Ascend AI Processor adopts a proprietary hardware architecture design to pursue extremely high data throughput and compute density. Unlike the contiguous linear storage commonly used by general-purpose CPUs, this unit requires input and output data to conform to a specific fractal storage format. When developing operators using the asc_mmad class interfaces of Ascend C, a thorough understanding of these data formats is crucial for implementing correct data movement, tiling, and performance optimization.

For ease of understanding and memorization, this document adopts the intuitive "big Y, small x" naming convention:

- Big Y (Z/N): indicates the arrangement order between fractal cubes (Z is row-major order, N is column-major order).
- Small x (z/n): indicates the arrangement order of elements within a fractal cube (z is row-major order, n is column-major order).

For the specific layout of cubes A, B, and C at each storage level, see [Key Fractal Format Details](key_fractal_format_details.md); for auxiliary data such as Bias, Scale, and sparse indexes, see [Auxiliary Cube Fractal Format Details](aux_cube_fractal_format_details.md).
