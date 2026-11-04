# Document Organization<a name="ZH-CN_TOPIC_0000001846719812"></a>

<!-- md-trans-meta sourceCommit=f83e857605e3844db8b9de0442007cc115278a47 translatedAt=2026-10-23T11:19:20.723Z pushedAt=2026-11-03T07:05:13.266Z nl=2 -->

## Required Knowledge<a name="section9155515143412"></a>

This document guides developers in developing high-performance operators on Ascend AI Processors using the Ascend C programming language. Before reading this document, you are expected to have the following capabilities:

-   Be proficient in the C++ programming language.
-   Understand computer architecture.
-   Understand the hardware architecture of Ascend AI Processors.
-   Have completed the documentation and courses related to Ascend C programming.
-   Be able to set up the Ascend C development and debugging environment.
-   Be able to independently develop Ascend C operators.
-   Be proficient in using performance analysis tools to obtain performance data.

You can obtain the learning materials for the above content through [LINK](https://www.hiascend.com/zh/ascend-c).

## Problems That Can Be Solved<a name="section235444416411"></a>

After developers complete the development of Ascend C operators (operators mentioned later in this document all refer to operators developed using Ascend C), if they need to further optimize operator performance, reading this document can provide effective help.

This document first introduces the hardware characteristics of heterogeneous computing and the data interaction during running, then introduces the ideas of debugging and tuning when programming with Ascend C as well as various performance optimization methods, and finally introduces specific performance optimization cases.

Optimizing operator performance is a continuous iterative process. The following steps are iterated repeatedly until the performance goal is achieved. In the [Best Practices](best_practices/best_practices.md) chapter, developers can further learn about specific practices based on the following four steps.

**Figure 1** Operator performance optimization process<a name="fig2643134112816"></a>  
![](../figures/op_opt_flow.png "Operator performance optimization process")

This document is divided into the following chapters. The content and objectives of each chapter are as follows:

-   **[Heterogeneous Computing](heterogeneous_computing.md)**: Introduces the deployment of operators on hardware and the data flow during running. The purpose is to help developers understand, at a macro level, the processes on the hardware architecture that may affect operator execution performance.
-   **[SIMD Operator Implementation](simd_operator_impl/overview.md)**: Introduces the operator Tiling and kernel function implementation in three typical scenarios: vector programming, cube programming, and fusion operator programming. It is a specific application of the Ascend C programming paradigm.
-   **[SIMT Operator Implementation](simt_operator_impl/simt_operator_impl.md)**: Introduces reference cases for operator development using the SIMT API.
-   **[SIMD and SIMT Hybrid Operator Implementation](simd_simt_hybrid_operator_impl/simd_simt_hybrid_operator_impl.md)**: Introduces reference cases for operator development using the Reg vector computation API and the SIMT API for SIMD and SIMT hybrid programming.
-   **[Functional Debugging](functional_debug/functional_debug.md)**: Describes some common scenarios that affect operator functionality. The purpose is to help developers quickly resolve functional issues, facilitate performance optimization, and quickly resolve functional issues that may arise during performance optimization.
-   **[Performance Analysis](performance_analysis/performance_analysis.md)**: Describes the directions for analyzing operator performance data. The purpose is to help developers identify optimization directions by analyzing performance data. For details about how to test operator performance data and how to use the performance tools, see [msOpp User Guide](https://gitcode.com/Ascend/msopprof/blob/26.2.0/docs/en/user_guide/msopprof_user_guide.md). This chapter does not elaborate on these topics.
-   **[SIMD Operator Performance Optimization](simd_operator_optimization/simd_operator_optimization.md)**: Describes the means of performance optimization. The purpose is to help developers carry out performance optimization based on the performance bottlenecks of operators. The main optimization suggestions are divided into data transfer optimization, memory optimization, API usage optimization, pipeline optimization, and Tiling optimization. Some optimization suggestions that comprehensively reflect the preceding categories are placed in the most relevant sections. These suggestions are classified by priority, which is set by comprehensively considering the performance effect and its scope: suggestions that bring performance benefits to most Ascend C operators have the highest priority, while means that only affect specific situations are given a lower priority. Developers do not need to be familiar with all optimization means. They can obtain the corresponding optimization means based on the operator performance bottlenecks identified through analysis, and gradually understand the full picture of the optimization strategies.
-   **[SIMD and SIMT Hybrid Operator Performance Optimization](simd_simt_hybrid_optimization/simd_simt_hybrid_optimization.md)**: Describes performance optimization suggestions for SIMD and SIMT hybrid programming scenarios, including memory access optimization and computation optimization.
-   **[SIMT Operator Performance Optimization](simt_operator_optimization/simt_operator_optimization.md)**: Describes performance optimization suggestions for SIMT operators. Currently, it covers two types of optimization: memory access and execution configuration.
-   **[Best Practices](best_practices/best_practices.md)**: Describes best practice cases for operator performance optimization. The purpose is to help developers gain a deeper understanding of the preceding content through examples, and to complete operator performance optimization by referring to the optimization means and ideas in these cases, thereby transitioning from theory to practice.
