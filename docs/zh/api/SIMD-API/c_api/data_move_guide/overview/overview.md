# 概览

数据搬运是算子开发中最基础且最关键的操作之一。在AI Core的分层存储架构中，输入数据需要从Global Memory（GM）搬入片上存储，计算结果也需要从片上存储搬回GM；矩阵计算和矢量计算之间交换中间结果时，还会涉及L1 Buffer、L0 Buffer、Unified Buffer（UB）和寄存器之间的数据流转。

NPU架构版本3510的C API通过地址空间限定符和函数参数直接描述源、目的存储单元以及搬运形态。开发者需要根据数据通路选择接口，并显式配置地址、长度、步长、Padding、随路转换和流水同步。本章节用于在阅读各通路详解前，建立以下统一认知：

- **存储层级与数据通路：** AI Core包含哪些物理存储单元，数据可沿哪些方向流动，以及各通路由哪条硬件流水执行。
- **地址空间与执行范围：** `__gm__`、`__cbuf__`、`__ca__`、`__cb__`、`__cc__`、`__ubuf__`等地址空间对应的物理存储，以及接口在AIC、AIV或Vector Function中的生效范围。
- **搬运功能分类：** 连续、高维切分、多维切片、随路格式转换、随路量化激活、非对齐、UB内部复制、矩阵分形搬运、NDDMA以及Reg与UB数据交换等能力。
- **关键参数概念：** `size`、`burst_count`、`burst_len`、`src_stride`、`dst_stride`、`src_gap`、`dst_gap`和多维循环参数的物理含义与单位差异。
- **通用约束：** 地址对齐、存储容量、有效访问范围、接口执行核、配置状态和同步规则。

## 接口分类

| 分类 | 主要执行范围 | 典型通路 | 接口目录 |
| --- | --- | --- | --- |
| 矢量数据搬运 | AIV | GM与UB、UB到L1、UB内部 | [矢量数据搬运](../../vector_datamove/vector_datamove.md) |
| 矩阵数据搬运 | AIC | GM到L1、L1到L0A/L0B、L0C搬出、L1到专用Buffer | [矩阵计算搬运](../../cube_datamove/cube_datamove.md) |
| Reg加载/存储 | AIV的Vector Function | UB与矢量数据寄存器、掩码寄存器之间的数据交换 | [Reg加载](../../reg_compute/load/reg_load_overview.md)、[Reg存储](../../reg_compute/store/reg_store_overview.md) |

## 硬件流水

| 流水 | NPU架构版本3510中的主要搬运方向 |
| --- | --- |
| PIPE_MTE1 | L1到L0A/L0B、L1到UB、L1到BiasTable |
| PIPE_MTE2 | GM到L1、GM到UB |
| PIPE_MTE3 | UB到GM、UB到L1 |
| PIPE_FIX | L0C到GM/L1/UB、L1到Fixpipe Buffer |
| PIPE_V | UB内部复制和矢量计算 |

同一流水中的指令按顺序执行，但后一条指令开始执行时，前一条指令不一定已完成全部数据读写；存在覆盖或读写依赖时，需要调用[asc_sync_pipe](../../sync/intra_core_sync/asc_sync_pipe.md)等待前序指令完成。生产者和消费者位于不同流水时，需要根据数据依赖调用[同步控制](../../sync/system_sync_overview.md)接口。接口所属流水、支持的执行核和同步方式以具体接口文档为准。

## 延伸阅读

- [数据搬运概念](data_move_concept.md)：理解不同搬运模式及其适用场景。
- [数据通路](data_path.md)：按源、目的存储单元和硬件流水查找接口。
- [总体约束说明](overall_constraints.md)：编码前检查对齐、容量、参数单位和同步要求。
