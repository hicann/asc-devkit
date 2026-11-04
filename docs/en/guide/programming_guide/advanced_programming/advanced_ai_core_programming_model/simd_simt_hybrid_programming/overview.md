# Overview<a name="ZH-CN_TOPIC_0000002563469514"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-23T14:07:45.309Z pushedAt=2026-11-03T07:05:13.414Z nl=2 -->

To improve the programming flexibility of vector computation in scenarios such as complex control flow and scattered memory access, in the mixed programming architecture that deeply integrates SIMD and SIMT, the cube computation unit retains the SIMD design, while the vector computation unit introduces SIMT capabilities on top of SIMD. This design establishes a mixed programming model with SIMD as the primary approach and SIMT as the auxiliary approach.

>[!NOTE] Note 
>This programming model applies to the following models:
     <!-- npu="950" id1 -->
>-   Ascend 950PR&950DT products
     <!-- end id1 -->
<!-- @ref: asc-devkit/res/docs/en/guide/programming_guide/advanced_programming/advanced_ai_core_programming_model/simd_simt_hybrid_programming/overview_res.md#id1 -->

## Positioning of Mixed Programming

In this model, the cube computing unit and the SIMD computing unit in vector computing together provide more than 90% of the computing power, delivering high performance and high computing power utilization for intensive computing. SIMT in vector computing serves as a flexibility supplement, specifically handling irregular scenarios such as complex control flow and scattered memory access, thereby improving the efficiency of algorithm development and optimization in such scenarios.  
Therefore, in SIMD and SIMT mixed programming, SIMT is not used to replace SIMD in processing continuous and intensive computing tasks, but to process local irregular logic in operators. A typical approach is to use SIMT VF in a kernel function to process local fragments such as branch judgment, index mapping, and scattered memory access, and then use SIMD to continue processing vector or cube computing, thereby balancing flexibility and high throughput.

## Scenarios for Mixed Programming

As described above, mixed programming applies to scenarios where an operator involves both continuous and regular computation that SIMD excels at and tasks such as scattered access that SIMT excels at. The following lists some applicable scenarios for mixed programming:

-   Operators with complex branch judgment are suitable for SIMT to complete computation tasks, while the input and output data movement is regular and continuous, making it suitable to use SIMD data movement APIs for data copy. For example, see [floor_mod sample](../../../../../../../examples/05_simd_simt_hybrid/02_best_practices/simd_simt_high_performance).

-   In scattered access scenarios, SIMT is suitable for completing computation tasks. When the amount of data to be accessed is far smaller than the available Unified Buffer (UB) space, SIMD data movement APIs can be used for data copy, so that SIMT programming can read data directly from the UB and improve memory access efficiency.

-   For certain operators, in large-shape scenarios, using SIMD parallel computation is more efficient, while in small-shape irregular scenarios, SIMT is suitable for completing the computation. Therefore, operator scenarios can be distinguished and the two programming approaches can be used separately to implement the operator functionality, improving the overall performance in generalized scenarios.

## Guide Overview

As an advanced programming approach, mixed programming requires a basic understanding of the [SIMD programming model](../../../programming_model/ai_core_simd_programming/ai_core_simd_programming.md) and the [SIMT programming model](../../../programming_model/ai_core_simt_programming/ai_core_simt_programming.md) before reading this chapter. This chapter introduces mixed programming from the following aspects:

-   **[Abstract Hardware Architecture](abstract_hardware_architecture.md)**: Describes the hardware architecture of SIMD and SIMT mixed programming scenarios, and introduces the VF (Vector Function) running mechanism and concepts such as SIMT VF and SIMD VF.

-   **[Kernel Function and VF Function](kernel_and_vf_functions.md)**: Describes in detail the kernel function definition, calling method, and execution configuration in mixed programming scenarios, as well as VF functions and the calling relationships between various functions.

-   **[Memory Hierarchy](memory_hierarchy.md)**: Presents the memory hierarchy of mixed programming scenarios, focusing on the communication methods between VFs and how UB memory is allocated and managed as shared memory.

-   **[Programming Examples](programming_examples.md)**: Demonstrates the key syntax of mixed programming with practical examples, including `asc_vf_call` for calling SIMT VF functions and the `<<<>>>` kernel function launch configuration.

It is strongly recommended that developers read [Extended Syntax and Constraint Description for Mixed Programming Scenarios](../../../language_extension/simd_simt_hybrid_builtin_keywords.md) to master the basic extended syntax and usage constraints before developing custom operators.
