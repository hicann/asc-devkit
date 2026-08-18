# 概述<a name="ZH-CN_TOPIC_0000002563469514"></a>

为提升向量计算在复杂控制流、离散访存等场景下的编程灵活性，在SIMD与SIMT深度融合的混合编程架构中：矩阵计算单元延续SIMD设计，向量计算单元则在SIMD基础上引入SIMT能力。这一设计确立了以SIMD为主、SIMT为辅的混合编程模型。

<!-- npu="950" id1 -->
>[!NOTE]说明 
>该编程模型适用于如下型号：
>-   Ascend 950PR/Ascend 950DT
<!-- end id1 -->

## 混合编程的定位

在该模型下，矩阵计算单元和向量计算中的SIMD计算单元共同提供超过90%的算力，为密集计算提供高性能与高算力利用率；向量计算中的SIMT则作为灵活性补充，专门应对复杂控制流、离散访存等不规则场景，提升此类场景下算法开发与优化的效率。  
因此，在SIMD与SIMT混合编程中，SIMT不用于替代SIMD处理连续、密集的计算任务，而是用于处理算子中的局部不规则逻辑。典型方式是在核函数（Kernel）中通过SIMT VF处理分支判断、索引映射、离散访存等局部片段，再由SIMD继续处理向量或矩阵计算，从而兼顾灵活性和高吞吐。

## 混合编程的适用场景

如上文所述，混合编程适用于算子功能既包含SIMD擅长的连续规整计算，也包含SIMT擅长的离散访问等任务的场景。以下列举部分混合编程的适用场景：

-   复杂分支判断的算子，适合使用SIMT完成计算任务，而输入输出的数据搬运较规整连续，适合使用SIMD搬运接口完成数据拷贝。例如[floor_mod样例](../../../../../../../examples/05_simd_simt_hybrid/02_best_practices/simd_simt_high_performance)。

-   离散访问场景，适合使用SIMT完成计算任务，当所需访问的数据量远小于Unified Buffer（UB）可用空间时，可以使用SIMD搬运接口完成数据拷贝，使SIMT编程能够直接从UB读取数据，提高内存访问效率。

-   某些算子，在大Shape场景，使用SIMD并行计算效率更高；而在小Shape不规整场景，则适合使用SIMT完成计算。因此可以区分算子场景，分别使用两种编程方式实现算子功能，提升泛化场景整体性能。

## 指南概览

混合编程作为高级编程方式，在阅读本章节内容前需对[SIMD编程模型](../../../programming_model/ai_core_simd_programming/ai_core_simd_programming.md)和[SIMT编程模型](../../../programming_model/ai_core_simt_programming/ai_core_simt_programming.md)有一定的了解。本章节将从以下几个方面展开介绍混合编程：

-   **[抽象硬件架构](abstract_hardware_architecture.md)**：介绍了SIMD与SIMT混合编程场景的硬件架构，介绍VF（Vector Function）运行机制及SIMT VF、SIMD VF等概念。

-   **[核函数（Kernel）与VF函数](kernel_and_vf_functions.md)**：详细介绍了混合编程场景的核函数（Kernel）定义、调用方式及执行配置，VF函数及各类函数的调用关系。

-   **[内存层级](memory_hierarchy.md)**：展示混合编程场景的内存层级，重点介绍了VF之间的通信方式，UB内存作为共享内存是如何分配与管理的。

-   **[编程示例](programming_examples.md)**：以实际样例展示混合编程关键语法，包括`asc_vf_call`调用SIMT VF函数和`<<<>>>`核函数（Kernel）启动配置。

强烈建议开发者阅读[混合编程场景的扩展语法与约束说明](../../../language_extension/simd_simt_hybrid_builtin_keywords.md)，掌握基本扩展语法及使用约束后开发自定义算子。
