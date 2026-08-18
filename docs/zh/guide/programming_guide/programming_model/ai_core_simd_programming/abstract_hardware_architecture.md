# 抽象硬件架构<a name="ZH-CN_TOPIC_0000001861989988"></a>

AI Core是AI处理器的计算核心，AI处理器内部有多个AI Core。本章节将介绍AI Core的并行计算架构抽象，该抽象架构屏蔽了不同硬件之间的差异。使用Ascend C进行编程时，基于抽象硬件架构，可以简化硬件细节，显著降低开发门槛。如需了解更详细的硬件架构信息或者原理，请参考[硬件实现](../../advanced_programming/hardware_implementation/basic_architecture.md)。

## 概览<a name="section_core_components"></a>

AI Core的抽象硬件架构可以分为[**计算单元**](#section_compute_units)、[**存储单元**](#section_memory_units)、[**搬运单元**](#section_memory_units)三类核心组件。下图以NPU架构版本2201为例，展示三类组件在AI Core中的位置关系和协同方式。

**图1**  抽象硬件架构（[NPU架构版本2201](../../language_extension/simd_builtin_keywords.md#npu-arch)）<a name="fig1095152043812"></a>

<img src="../../../figures/npu_2201_2.png" alt="抽象硬件架构" width="840px">

结合前述三类组件和[图1](#fig1095152043812)，一次算子执行可以理解为三类组件协同完成数据处理的过程：Host侧下发算子任务后，算子指令序列进入Device侧的AI Core；计算单元负责控制逻辑以及标量、矢量和矩阵计算，存储单元保存输入、输出和中间数据，搬运单元负责在不同存储层级之间转移数据。

需要注意的是，上述过程并不表示各步骤完全串行执行。异构系统中存在Host侧与Device侧之间的异步并行，也存在Device内部不同执行单元之间的流水化并行。后续章节将分别介绍计算单元、存储单元和搬运单元的具体组成，并进一步说明异步执行与同步机制之间的关系。

## 计算单元<a name="section_compute_units"></a>

AI Core中的计算单元主要包括Scalar、Vector和Cube三类。

**表1** 计算单元

<a name="zh-cn_topic_0000001588832845_table14884154362"></a>
| 组件名称 | 组件功能 | 对应编程概念 |
| --- | --- | --- |
| Scalar | 执行地址计算、循环控制等标量计算工作，并把矢量计算、矩阵计算、数据搬运、同步指令发射给对应单元执行。 | 核函数（Kernel）中的控制逻辑、同步控制API。 |
| Vector | 负责执行矢量运算。 | 矢量计算API、矢量类算子。 |
| Cube | 负责执行矩阵运算。 | 矩阵计算API、矩阵类算子。 |

## 存储单元和搬运单元<a name="section_memory_units"></a>

存储单元可以从使用位置上分为Local Memory和Global Memory。

-   **Local Memory**：AI Core片内存储，用于暂存从Global Memory搬入的数据分片，并保存计算输出和中间结果。这些数据可供Vector、Cube等计算单元高效访问，继续参与片内计算，或通过搬运单元搬出到Global Memory。Local Memory包含面向不同编程方式和计算单元的片内资源，如[图2](#fig_simd_reg_memory_hierarchy)所示。
-   **Global Memory**：Device侧全局存储，是Local Memory中数据搬入和搬出的主要来源或目的位置。

**图2**  SIMD-Reg矢量计算内存层级<a name="fig_simd_reg_memory_hierarchy"></a>

<img src="../../../figures/simd_reg_2.png" alt="SIMD-Reg矢量计算内存层级" width="520px">

**DMA（Direct Memory Access）搬运单元**负责数据搬运，包括Global Memory和Local Memory之间的数据搬入、搬出，以及不同层级Local Memory之间的数据流转。常见搬运单元包括MTE1、MTE2、MTE3和FixPipe，具体介绍请参考[搬运单元](../../advanced_programming/hardware_implementation/basic_architecture.md#table288493152012)。

## 执行流程与同步机制<a name="section_programming_flow"></a>

理解抽象硬件架构时，还需要关注不同单元如何协同完成一次数据块处理。可以从**异步指令流、同步信号流、计算数据流**三个视角理解：

-   **异步指令流**：表示Scalar计算单元对其他执行单元的任务发射关系，而不是完整指令执行流程。Scalar计算单元根据算子指令序列，将计算、搬运等任务发射到Vector、Cube、DMA等单元的指令队列中，各执行单元再在各自流水上异步执行。该过程可以参考[图1](#fig1095152043812)所示的指令流。
-   **计算数据流**：表示各执行单元在指令控制下访问或更新存储资源的关系，而不是一次计算任务中完整的数据搬入、计算、搬出流程。例如，Vector/Cube等计算单元基于Local Memory中的数据执行计算，DMA搬运单元负责Local Memory和Global Memory之间的数据流转。该过程可以参考[图1](#fig1095152043812)所示的数据流。
-   **同步信号流**：表示Scalar计算单元通过同步信号约束不同执行单元任务先后关系的控制路径，而不是数据本身的流向。当不同单元的异步任务存在数据依赖或顺序依赖时，需要通过同步信号约束执行先后。该过程可以参考[图1](#fig1095152043812)所示的同步信号流。

因此，开发者不仅要关注数据如何搬入、计算和搬出，还需要理解同步机制如何保证异步任务按正确的依赖关系执行。同步控制相关接口及依赖关系在编程中的表达方式，请参考[核内同步能力概述](../../../../api/SIMD-API/basic_api/sync_control/intra_core_sync/intra_core_sync_overview.md)。

## 架构版本差异<a name="section_arch_version"></a>

**图3**  抽象硬件架构（[NPU架构版本3510](../../language_extension/simd_builtin_keywords.md#npu-arch)）<a name="fig10951520438120"></a>

<img src="../../../figures/npu_3510_2.png" alt="抽象硬件架构NPU架构版本3510" width="840px">

与NPU架构版本2201相比，NPU架构版本3510的主要差异如下：

-   新增[Regbase矢量计算方式](c_pointer_programming/reg_vector_computation.md)，支持直接使用寄存器参与计算。Membase和Regbase的差异见下表。
-   计算单元、存储单元和数据类型支持范围存在差异，具体请参考[存储单元变更](../../../cross_gen_migration_guide/3510_arch_migration/2201_to_3510_arch_changes.md#section_2201_to_3510_storage_unit_changes)和[内置数据类型](../../../../api/SIMD-API/basic_api/data_structures/builtin_data_types.md)。
-   编程方式更加丰富，除SIMD编程外，还支持SIMT编程以及SIMD与SIMT混合编程。三种编程方式的选择指导请参见[AI Core编程模型](../programming_model_overview.md#section_ai_core_programming_model)。

**表2**  Membase和Regbase差异

| 计算方式 | 数据暂存位置 | 特点 | 适用场景 |
| --- | --- | --- | --- |
| Membase | Local Memory（UB） | 每步计算结果写回UB | NPU架构版本2201 |
| Regbase | 寄存器（VF Reg） | 中间结果可暂存寄存器，减少UB读写 | NPU架构版本3510 |

>[!NOTE]说明
>- 不同产品型号和NPU架构版本的对应关系请参考[__NPU_ARCH__](../../language_extension/simd_builtin_keywords.md#npu-arch)。
>- 详细架构变更请参考[2201到3510架构变更](../../../cross_gen_migration_guide/3510_arch_migration/2201_to_3510_arch_changes.md)。
