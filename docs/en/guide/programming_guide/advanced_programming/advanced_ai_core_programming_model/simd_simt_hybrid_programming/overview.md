# Overview<a name="ZH-CN_TOPIC_0000002563469514"></a>

<!-- md-trans-meta sourceCommit=4c81e54975ec65486e709c6cf9f279160b0190ce translatedAt=2026-08-26T11:29:08.985Z pushedAt=2026-09-01T03:32:03.396Z -->

To improve the programming flexibility of vector computation in scenarios such as complex control flow and scattered memory access, the mixed programming architecture that deeply integrates SIMD and SIMT adopts the following design: the cube computation unit retains the SIMD design, while the vector computation unit introduces SIMT capabilities on top of SIMD. This design establishes a mixed programming model with SIMD as the primary approach and SIMT as the auxiliary approach.

<!-- npu="950" id1 -->
>[!NOTE] Note
>This programming model applies to the following models:
>-   Ascend 950PR/Ascend 950DT
<!-- end id1 -->

## Positioning of Mixed Programming

In this model, the cube computing unit and the SIMD computing unit in vector computing jointly provide more than 90% of the computing power, delivering high performance and high computing power utilization for intensive computing. SIMT in vector computing serves as a flexibility supplement, specifically handling irregular scenarios such as complex control flow and scattered memory access, thereby improving the efficiency of algorithm development and optimization in such scenarios.  
Therefore, in SIMD and SIMT mixed programming, SIMT is not used to replace SIMD for processing continuous and intensive computing tasks, but to handle local irregular logic in operators. A typical approach is to use SIMT VF in the kernel function to process local fragments such as branch judgment, index mapping, and scattered memory access, and then let SIMD continue to process vector or matrix computing, thereby balancing flexibility and high throughput.

## Scenarios for Mixed Programming

As described above, mixed programming applies to scenarios where an operator involves both continuous and regular computation that SIMD excels at and tasks such as scattered memory access that SIMT excels at. The following lists some applicable scenarios for mixed programming:

-   For operators with complex branch judgment, SIMT is suitable for completing the computation tasks, while the input and output data movement is relatively regular and continuous, making SIMD data movement APIs suitable for data copy. For example, see the [floor_mod sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/05_simd_simt_hybrid/02_best_practices/simd_simt_high_performance).

-   In scattered memory access scenarios, SIMT is suitable for completing the computation tasks. When the amount of data to be accessed is far smaller than the available space of the Unified Buffer, SIMD data movement APIs can be used for data copy, so that SIMT programming can directly read data from the Unified Buffer and improve memory access efficiency.

-   For certain operators, in large-shape scenarios, using SIMD for parallel computation is more efficient, whereas in small-shape irregular scenarios, SIMT is suitable for completing the computation. Therefore, operator scenarios can be distinguished and the two programming approaches can be used separately to implement the operator functionality, improving the overall performance in generalized scenarios.

## Guide Overview

As an advanced programming approach, mixed programming requires a basic understanding of the [SIMD programming model](../../../programming_model/ai_core_simd_programming/ai_core_simd_programming.md) and the [SIMT programming model](../../../programming_model/ai_core_simt_programming/ai_core_simt_programming.md) before reading this section. This section introduces mixed programming from the following aspects:

-   **[Abstract hardware architecture](abstract_hardware_architecture.md)**: Describes the hardware architecture for SIMD and SIMT mixed programming scenarios, the operating mechanism of VF (Vector Function), and concepts such as SIMT VF and SIMD VF.

-   **[Kernel functions and VF functions](kernel_and_vf_function.md)**: Describes in detail the kernel function definition, call method, and execution configuration in mixed programming scenarios, as well as VF functions and the call relationships among various functions.

-   **[Memory hierarchy](memory_hierarchy.md)**: Presents the memory hierarchy in mixed programming scenarios, with a focus on the communication mode between VFs and how UB memory is allocated and managed as shared memory.

-   **[Programming examples](programming_example.md)**: Demonstrates the key syntax of mixed programming with practical examples, including `asc_vf_call` for calling SIMT VF functions and `<<<>>>` for kernel launch configuration.

It is strongly recommended that developers read the [extended syntax and constraint description for mixed programming scenarios](../../../language_extension/simd_and_simt_hybrid_programming_builtin_keyword.md) to master the basic extended syntax and usage constraints before developing custom operators.
