# Document Organization<a name="ZH-CN_TOPIC_0000001846719812"></a>

<!-- md-trans-meta sourceCommit=195cf801df3a15b430fb9afd7dda8149cb021a63 translatedAt=2026-08-26T12:56:58.606Z -->

## Prerequisites<a name="section9155515143412"></a>

This document guides developers in developing high-performance operators on Ascend AI Processors using the Ascend C programming language. Before reading this document, you need to have the following capabilities:

-   Be proficient in the C++ programming language.
-   Understand computer architecture.
-   Understand the hardware architecture of Ascend AI Processors.
-   Complete the study of Ascend C programming documents and courses.
-   Be able to set up the Ascend C development and debugging environment.
-   Be able to independently develop Ascend C operators.
-   Be proficient in using performance analysis tools to obtain performance data.

You can obtain the learning materials for the above content by clicking [here](https://www.hiascend.com/cann/ascend-c).

## Problems That Can Be Solved<a name="section235444416411"></a>

After developers complete the development of Ascend C operators (all operators mentioned later in this document refer to operators developed using Ascend C), if they need to further optimize operator performance, this document can provide effective help.

This document first introduces the hardware characteristics of heterogeneous computing and the data interaction, then introduces the approaches of debugging and tuning when programming with Ascend C as well as various performance optimization methods, and finally introduces specific performance optimization cases.

Optimizing operator performance is a continuous iterative process. The following steps are iterated repeatedly until the performance goal is achieved. In the [best practices](best_practices/best_practices.md) chapter, developers can further learn about the specific practices based on the following four steps.

**Figure 1** Operator performance optimization process<a name="fig2643134112816"></a>  
![](../figures/operator_performance_optimization_process.png "Operator performance optimization process")

This document is divided into the following chapters. The content and objectives of each chapter are as follows:

-   **[Heterogeneous compute](heterogeneous_compute.md)**: Introduces the deployment of operators on hardware and the data flow during running. The purpose is to help developers understand, at a macro level, the processes on the hardware architecture that may affect operator execution performance.
-   **[SIMD operator implementation](simd_operator_impl/overview.md)**: Introduces the Tiling and Kernel implementation of operators in three typical scenarios: vector programming, matrix programming, and fused operator programming. It is a specific application of the Ascend C programming paradigm.
-   **[SIMT operator implementation](simt_operator_impl/simt_operator_implementation.md)**: Introduces reference cases for operator development using SIMT APIs.
-   **[SIMD and SIMT hybrid operator implementation](simd_simt_hybrid_operator_impl/simd_simt_hybrid_operator_implementation.md)**: Introduces reference cases for operator development using Reg vector computation APIs and SIMT APIs for SIMD and SIMT hybrid programming.
-   **[Functional debugging](functional_debug/functional_debug.md)**: Describes some common scenarios that affect operator functionality. The purpose is to help developers quickly resolve functional issues, facilitate performance optimization, and quickly resolve functional issues that may arise during performance optimization.
-   **[Performance analysis](performance_analysis/performance_analysis.md)**: Describes the directions for analyzing operator performance data. The purpose is to help developers identify performance optimization directions by analyzing performance data. For details about how to test operator performance data and how to use performance tools, see [*msOpProf User Guide*](https://gitcode.com/Ascend/msopprof/blob/26.1.0/docs/en/user_guide/msopprof_user_guide.md). This chapter does not elaborate on these topics.
-   **[SIMD operator performance optimization](simd_operator_optimization/simd_operator_optimization.md)**: Describes the means of performance optimization. The purpose is to help developers carry out performance optimization based on operator performance bottlenecks. The main optimization suggestions are divided into data transfer optimization, memory optimization, API usage optimization, pipeline optimization, and Tiling optimization. Some optimization suggestions that comprehensively reflect the preceding categories are described in the most relevant chapters. These suggestions are classified by priority, which is determined by comprehensively considering the performance effect and scope: suggestions that bring performance benefits to most Ascend C operators have the highest priority, while means that affect only specific cases are given a lower priority. Developers do not need to be familiar with all optimization means. Instead, they can obtain the corresponding optimization means based on the operator performance bottlenecks identified through analysis, and gradually understand the full picture of the optimization strategy.
-   **[SIMD and SIMT hybrid operator performance optimization](simd_simt_hybrid_optimization/simd_simt_hybrid_operator_optimization.md)**: Describes performance optimization suggestions for SIMD and SIMT hybrid programming scenarios, including memory access optimization and computation optimization.
-   **[SIMT operator performance optimization](simt_operator_optimization/simt_operator_optimization.md)**: Describes performance optimization suggestions for SIMT operators, which currently include two types of optimization: memory access and execution configuration.
-   **[Best practices](best_practices/best_practices.md)**: Describes best practice cases for operator performance optimization. The purpose is to help developers gain a deeper understanding of the preceding content through examples, and complete operator performance optimization by referring to the optimization means and ideas in these cases, thereby transitioning from theory to practice.
