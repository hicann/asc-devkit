# Document Organization<a name="ZH-CN_TOPIC_0000001846719812"></a>

<!-- md-trans-meta sourceCommit=49eda737d7d7268a9ddb02fcaba4732176bfc076 translatedAt=2026-11-09T10:44:35.101Z pushedAt=2026-11-19T06:32:19.848Z nl=2 -->

## Required Knowledge<a name="section9155515143412"></a>

This document guides developers in developing high-performance operators on Ascend AI processors using the Ascend C programming language. To read this document, you need the following knowledge and skills:

-   Proficiency in the C++ programming language.
-   Understanding of computer architecture.
-   Understanding of the Ascend AI processor hardware architecture.
-   Completion of Ascend C programming documentation and courses.
-   Ability to set up the Ascend C development and debugging environment.
-   Ability to independently develop Ascend C operators.
-   Proficiency in using performance analysis tools to obtain performance data.

You can obtain the learning materials for the above content from [here](https://www.hiascend.com/zh/ascend-c).

## Problems This Document Can Solve<a name="section235444416411"></a>

After developers complete the development of Ascend C operators (all operators mentioned later in this document refer to operators developed using Ascend C), if they need to further optimize operator performance, this document can provide effective help.

This document first introduces the hardware characteristics of heterogeneous computing and the data interaction during execution, then describes the debugging and tuning approaches and various performance optimization methods when programming with Ascend C, and finally presents specific performance optimization cases.

Optimizing operator performance is a continuous iterative process. Iterate through the following steps repeatedly until the performance target is achieved. In the [Best Practices](best_practices/best_practices.md) chapter, developers can learn more about the specific practices based on the following four steps.

**Figure 1** Operator performance optimization process<a name="fig2643134112816"></a>  
<img src="../figures/op_opt_flow.png" alt="Operator performance optimization process" width="600px" height="318px">

This document is divided into the following chapters. The content and objectives of each chapter are as follows:

-   **[Heterogeneous Computing](heterogeneous_computing.md)**: Introduces the deployment of operators on hardware and the data flow during execution. The objective is to help developers understand, at a macro level, the processes on the hardware architecture that may affect operator execution performance.
-   **[SIMD Operator Implementation](simd_operator_impl/overview.md)**: Introduces operator tiling and kernel function implementation in three typical scenarios: vector programming, cube programming, and fused operator programming. It is a concrete application of the Ascend C programming paradigm.
-   **[SIMT Operator Implementation](simt_operator_impl/simt_operator_impl.md)**: Introduces reference cases for operator development using the SIMT API.
-   **[SIMD and SIMT Hybrid Operator Implementation](simd_simt_hybrid_operator_impl/simd_simt_hybrid_operator_impl.md)**: Introduces reference cases for operator development using the Reg vector computation API and the SIMT API for SIMD and SIMT hybrid programming.
-   **[Functional Debugging](functional_debug/functional_debug.md)**: Describes some common scenarios that affect operator functionality. The purpose is to help developers quickly resolve functional issues, facilitate performance optimization, and quickly resolve functional issues that may arise during performance optimization.
-   **[Performance Analysis](performance_analysis/performance_analysis.md)**: Describes the directions for analyzing operator performance data. The purpose is to help developers identify optimization directions by analyzing performance data. For details about how to test operator performance data and how to use performance tools, see [msOpProf User Guide](https://gitcode.com/Ascend/msopprof/blob/master/docs/en/user_guide/msopprof_user_guide.md). This chapter does not elaborate on these topics.
-   **[SIMD Operator Performance Optimization](simd_operator_optimization/simd_operator_optimization.md)**: Describes the means of performance optimization. The purpose is to help developers carry out performance optimization based on operator performance bottlenecks. The main optimization suggestions are divided into data movement optimization, memory optimization, API usage optimization, pipeline optimization, and tiling optimization. Some optimization suggestions reflect a combination of the above categories and are therefore placed in the most relevant chapters. These suggestions are classified by priority, which is determined by comprehensively considering the performance effect and its scope: suggestions that bring performance gains to most Ascend C operators have the highest priority, while means that affect only specific situations are given lower priority. Developers do not need to be familiar with all optimization means. Instead, they can obtain the corresponding optimization means based on the operator performance bottlenecks identified through analysis, and gradually understand the full picture of the optimization strategy.
-   **[SIMD and SIMT Hybrid Operator Performance Optimization](simd_simt_hybrid_optimization/simd_simt_hybrid_optimization.md)**: Describes performance optimization suggestions for SIMD and SIMT hybrid programming scenarios, including memory access optimization and computation optimization.
-   **[SIMT Operator Performance Optimization](simt_operator_optimization/simt_operator_optimization.md)**: Describes performance optimization suggestions for SIMT operators, currently covering two types of optimization: memory access and execution configuration.
-   **[Best Practices](best_practices/best_practices.md)**: Describes best practice cases for operator performance optimization. The purpose is to help developers gain a deeper understanding of the preceding content through examples, and to complete operator performance optimization by referring to the optimization means and ideas in these cases, thereby transitioning from theory to practice.
