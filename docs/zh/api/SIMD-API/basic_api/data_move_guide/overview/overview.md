# 概览<a name="ZH-CN_TOPIC_0000002565807407"></a>

数据搬运是算子开发中最基础且最关键的操作之一。在昇腾AI Core的分层存储架构中，数据需要在不同层级的存储单元之间流转，才能完成计算任务。数据搬运类API（DataCopy、DataCopyPad、Copy、LoadData（2D矩阵搬运）、LoadData（卷积数据搬运）、FixPipe等）开放了底层硬件搬运单元（MTE1/MTE2/MTE3/FixPipe）的能力，使开发者可以高效、灵活地在[Global Memory](../../../basic_api/data_structures/GlobalTensor/GlobalTensor_intro.md)与各级[Local Memory](../../../basic_api/data_structures/LocalTensor/LocalTensor_intro.md)之间，以及Local Memory内部进行数据传输。

本章节为数据搬运的导览性内容，旨在帮助读者在深入阅读后续各通路详解章节之前，建立起对以下核心概念的统一认知：

- **存储层级与数据通路：** AI Core内部的物理存储单元有哪些，数据可以在哪些通路之间流动，以及搬运所依赖的硬件流水类型。
- **逻辑位置（TPosition）与物理内存的映射：** Ascend C通过TPosition机制隐藏硬件差异，开发者需理解VECIN/VECOUT/A1/B1/CO1等逻辑位置对应的实际物理存储。
- **搬运功能分类：** 基础数据搬运、高维切分搬运、切片搬运、随路格式转换（ND2NZ/NZ2ND/DN2NZ）、随路量化激活、非对齐搬运、Unified Buffer（UB）内部搬运（Copy）、矩阵分形搬运（LoadData（2D矩阵搬运）/LoadData（卷积数据搬运））、多维数据搬运NDDMA等功能的概念与适用场景。
- **关键参数概念：** DataBlock、blockLen、blockCount、srcStride/dstStride、repeat等参数的物理含义与配置方式。
- **通用约束：** 所有数据搬运接口共性的地址对齐要求、数据量约束、同步规则等。

- **[数据搬运概念](data_move_concept.md)**  

- **[数据通路](data_path.md)**  

- **[总体约束说明](overall_constraints.md)**  
