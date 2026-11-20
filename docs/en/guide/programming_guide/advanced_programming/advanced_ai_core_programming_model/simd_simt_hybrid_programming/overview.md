# Overview<a name="ZH-CN_TOPIC_0000002563469514"></a>

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-09T10:49:33.007Z pushedAt=2026-11-19T06:32:19.876Z nl=2 -->

To improve the programming flexibility of vector computation in scenarios such as complex control flow and scattered memory access, the hybrid programming architecture deeply integrates SIMD and SIMT: the cube computation unit retains the SIMD design, while the vector computation unit introduces SIMT capability on top of SIMD. This design establishes a hybrid programming model in which SIMD is primary and SIMT is supplementary.

> [!NOTE] Note 
> This programming model applies to the following models:
     <!-- npu="950" id1 -->
> -   Ascend 950PR&950DT products
     <!-- end id1 -->
<!-- @ref: asc-devkit/res/docs/zh/guide/programming_guide/advanced_programming/advanced_ai_core_programming_model/simd_simt_hybrid_programming/overview_res.md#id1 -->

## Positioning of Hybrid Programming

Under this model, the cube computing unit and the SIMD computing unit in vector computing together provide more than 90% of the compute power, delivering high performance and high compute utilization for intensive computation. The SIMT capability in vector computing serves as a flexibility supplement, specifically handling irregular scenarios such as complex control flow and scattered memory access, thereby improving the efficiency of algorithm development and optimization in such scenarios.  
Therefore, in SIMD and SIMT hybrid programming, SIMT is not used to replace SIMD for processing continuous, intensive computation tasks. Instead, it is used to process local irregular logic within an operator. A typical approach is to use SIMT VF in the kernel function to process local fragments such as branch judgment, index mapping, and scattered memory access, and then let SIMD continue to process vector or cube computation, thereby balancing flexibility and high throughput.

## Scenarios for Hybrid Programming

As described above, hybrid programming is suitable for scenarios where an operator involves both continuous and regular computation that SIMD excels at and tasks such as scattered memory access that SIMT excels at. The following lists some scenarios where hybrid programming is applicable:

-   For operators with complex branch judgment, SIMT is suitable for completing the computation tasks, while the data transfer of inputs and outputs is regular and continuous, making SIMD transfer APIs suitable for data copy. For example, see the [floor_mod sample](../../../../../../../examples/05_simd_simt_hybrid/02_best_practices/simd_simt_high_performance).

-   In scattered memory access scenarios, SIMT is suitable for completing the computation tasks. When the amount of data to be accessed is far smaller than the available space of the Unified Buffer (UB), SIMD transfer APIs can be used for data copy, so that SIMT programming can directly read data from the UB and improve memory access efficiency.

-   For certain operators, in large-shape scenarios, SIMD parallel computation is more efficient, while in small-shape irregular scenarios, SIMT is suitable for completing the computation. Therefore, you can distinguish operator scenarios and use the two programming approaches separately to implement the operator functions, improving the overall performance in generalized scenarios.

## Guide Overview

As an advanced programming approach, hybrid programming requires a basic understanding of the [SIMD programming model](../../../programming_model/ai_core_simd_programming/ai_core_simd_programming.md) and the [SIMT programming model](../../../programming_model/ai_core_simt_programming/ai_core_simt_programming.md) before reading this chapter. This chapter introduces hybrid programming from the following aspects:

-   **[Abstract Hardware Architecture](abstract_hardware_architecture.md)**: Describes the hardware architecture for SIMD and SIMT hybrid programming scenarios, and introduces the VF (Vector Function) running mechanism and concepts such as SIMT VF and SIMD VF.

-   **[Kernel and VF Functions](kernel_and_vf_functions.md)**: Describes in detail the kernel function definition, invocation method, and execution configuration in hybrid programming scenarios, as well as VF functions and the invocation relationships among various functions.

-   **[Memory Hierarchy](memory_hierarchy.md)**: Illustrates the memory hierarchy in hybrid programming scenarios, focusing on the communication methods between VFs and how UB memory is allocated and managed as shared memory.

-   **[Programming Examples](programming_examples.md)**: Demonstrates the key syntax of hybrid programming with practical examples, including `asc_vf_call` for calling SIMT VF functions and `<<<>>>` for kernel function launch configuration.

It is strongly recommended that developers read [Extended Syntax and Constraint Description for Hybrid Programming Scenarios](../../../language_extension/simd_simt_hybrid_builtin_keywords.md) to master the basic extended syntax and usage constraints before developing custom operators.
