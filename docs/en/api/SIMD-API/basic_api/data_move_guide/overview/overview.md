# Overview<a name="ZH-CN_TOPIC_0000002565807407"></a>

<!-- md-trans-meta sourceCommit=4ecc2a7539ac06aecd1976e53dc28db4535af552 translatedAt=2026-11-02T05:04:51.487Z pushedAt=2026-11-18T01:25:06.872Z nl=2 -->

Data migration is one of the most fundamental and critical operations in operator development. In the hierarchical storage architecture of the Ascend AI Core, data must flow between storage units at different levels to complete computation tasks. The data migration APIs (DataCopy, DataCopyPad, Copy, LoadData (2D matrix migration), LoadData (convolution data migration), FixPipe, and others) expose the capabilities of the underlying hardware migration units (MTE1/MTE2/MTE3/FixPipe), enabling you to efficiently and flexibly transfer data between [Global Memory](../../../basic_api/data_structures/GlobalTensor/GlobalTensor_intro.md) and various levels of [Local Memory](../../../basic_api/data_structures/LocalTensor/LocalTensor_intro.md), as well as within Local Memory.

This chapter provides an overview of data migration, aiming to help you build a unified understanding of the core concepts of data migration before diving into the detailed chapters on each data path. It contains the following three subchapters:

- **[Data Migration Concepts](data_move_concept.md)**, which mainly covers the following:

  - **Migration function classification:** The concepts and applicable scenarios of basic data migration, high-dimensional segmented migration, sliced migration, in-path format conversion (ND2NZ/NZ2ND/DN2NZ), in-path quantization activation, unaligned migration, Unified Buffer (UB) internal migration (Copy), matrix fractal migration (LoadData (2D matrix migration)/LoadData (convolution data migration)), multi-dimensional data migration NDDMA, and other functions.
  - **Key parameter concepts:** The physical meanings and configuration methods of parameters such as DataBlock, blockLen, blockCount, srcStride/dstStride, and repeat.

- **[Data Paths](data_path.md)**, which mainly covers the following:

  - **Storage hierarchy and data paths:** The physical storage units inside the AI Core, the paths along which data can flow, and the hardware pipeline types on which migration depends.
  - **Mapping between logical positions (TPosition) and physical memory:** Ascend C hides hardware differences through the TPosition mechanism. You need to understand the actual physical storage corresponding to logical positions such as VECIN/VECOUT/A1/B1/CO1.

- **[Overall Constraints](overall_constraints.md)**, which mainly covers the following:

  - Address alignment requirements, data volume constraints, and synchronization rules common to all data migration APIs.







