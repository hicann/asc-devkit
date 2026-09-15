# Overview<a name="ZH-CN_TOPIC_0000002565807407"></a>

<!-- md-trans-meta sourceCommit=unknown translatedAt=2026-08-31T11:02:52.696Z -->

Data movement is one of the most fundamental and critical operations in operator development. In the hierarchical storage architecture of the Ascend AI Core, data must flow between storage units at different levels to complete computation tasks. The data movement APIs (**DataCopy**, **DataCopyPad**, **Copy**, **Load2D**, **Load3D**, **FixPipe**, and so on) expose the capabilities of the underlying hardware migration units (**MTE1**/**MTE2**/**MTE3**/**FixPipe**), enabling developers to efficiently and flexibly transfer data between [Global Memory](../../data_structures/GlobalTensor/globaltensor_introduction.md) and various levels of [Local Memory](../../data_structures/LocalTensor/localtensor_introduction.md), as well as within Local Memory.

This chapter provides introductory content on data movement, aiming to help readers establish a unified understanding of the following core concepts before delving into the detailed chapters on each data path:

- **Storage hierarchy and data paths:** What physical storage units exist inside the AI Core, between which paths data can flow, and the hardware pipeline types on which migration depends.
- **Mapping between logical positions (TPosition) and physical memory:** Ascend C hides hardware differences through the TPosition mechanism. Developers need to understand the actual physical storage corresponding to logical positions such as VECIN/VECOUT/A1/B1/CO1.
- **Classification of migration functions:** The concepts and applicable scenarios of basic data movement, high-dimensional segmented migration, sliced migration, in-path format conversion (ND2NZ/NZ2ND/DN2NZ), in-path quantization activation, unaligned migration, intra-UB migration (Copy), cube fractal migration (Load2D/Load3D), multi-dimensional data movement NDDMA, and other functions.
- **Key parameter concepts:** The physical meanings and configuration methods of parameters such as DataBlock, blockLen, blockCount, srcStride/dstStride, and repeat.
- **General constraints:** The address alignment requirements, data volume constraints, synchronization rules, and other common restrictions shared by all data movement APIs.

- **[Data Movement Concepts](data_movement_concept.md)**  

- **[Data Paths](data_path.md)**  

- **[General Constraints](general_constraint_description.md)**  
