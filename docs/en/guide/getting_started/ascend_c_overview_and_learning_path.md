# Ascend C Overview

<!-- md-trans-meta sourceCommit=e5451c7feba11f9e259f4fbc19c4bf82f8c0496c translatedAt=2026-08-26T12:51:54.216Z pushedAt=2026-09-11T08:40:46.251Z -->

## CANN Overview
Artificial Intelligence (AI) chips are specialized processors designed to accelerate AI computing tasks. With highly parallel hardware architectures, they efficiently handle large-scale tensor operations in deep learning scenarios. As a typical representative of AI chips, the Neural-Network Processing Unit (NPU) excels at efficiently executing neural network tasks. The Ascend AI Processor, as a dedicated NPU, integrates a large number of high-performance computing cores, further strengthening this hardware advantage and providing solid support for the efficient execution of AI tasks.

In a neural network model, an operator is the fundamental computing unit that constitutes the model. It encapsulates specific mathematical operation logic for tensor data, such as convolution, matrix multiplication, and activation functions. The inference and training processes of a neural network model are essentially a series of operators executing in an orderly manner according to preset logic to collaboratively complete computing tasks. To fully unleash the hardware computing power of the Ascend NPU, a software platform is required that can efficiently bridge upper-layer AI frameworks and underlying hardware. Compute Architecture for Neural Networks (CANN) is exactly the full-stack software solution that Huawei has built for the Ascend NPU.

CANN is built on a layered architecture that seamlessly connects upper-layer applications with underlying hardware. Upward, it is compatible with mainstream AI frameworks such as PyTorch, ensuring compatibility and ease of use at the application layer. Downward, it deeply adapts to the Ascend AI Processor, fully leveraging the hardware's computing potential, thereby establishing a heterogeneous computing system that deeply integrates the CPU and NPU. Within this system, CANN drives efficient collaborative scheduling between the CPU and NPU through the Runtime interface: the CPU completes data preparation, task decomposition, and scheduling, then dispatches operator tasks to the NPU for accelerated execution. After computation is complete, the results are retrieved by the CPU or further processed according to business requirements. This collaboration mechanism spans all layers of operators, models, and applications, effectively achieving an optimal balance between computing performance and execution efficiency, and providing core software support for fully unleashing the computing power of the Ascend AI Processor.

## Ascend C Overview
In the era of large models, the demand for computing power is surging at an unprecedented rate. How to develop high-performance operators and fully unleash every bit of hardware potential has become the key to the deployment of AI applications. To help developers efficiently build high-performance operators that can run directly on the NPU, Ascend C was created. Ascend C is a dedicated operator development language for Ascend AI processors within the CANN ecosystem, natively compatible with the C and C++ standard specifications. In addition, Ascend C adheres to the design philosophy of **opening up complete chip programming capabilities to support ultimate performance**, and builds a multi-level API system to meet operator development requirements in various scenarios. This allows you to flexibly balance development efficiency and runtime performance based on project requirements, team skills, and performance goals, achieving an optimal trade-off.

To meet the requirements of different scenarios, Ascend C builds a multi-level API system ranging from single-instruction abstraction and single-core common algorithms to multi-core operators. Its overall logical architecture is as follows:

<img src="../../figures/architecture.png" alt="Architecture" width="850px" height="580px">

- **Language extension layer C API**: A pure C interface that provides array memory allocation and pointer-based computation interfaces, continuing the familiar C programming experience in the industry while fully opening up chip capabilities. Ascend 950PR/Ascend 950DT add SIMT and SIMD/SIMT hybrid programming capabilities.
- **Basic API**: A C++ class library abstracted at the single-instruction level, generally based on Tensor programming, and gradually improving the Tensor programming experience through Layout.
- **Advanced API**: Abstracts and encapsulates common single-core algorithms, providing out-of-the-box common algorithm implementations.
- **Operator template library**: Provides complete operator implementation references based on templates, reducing the complexity of Tiling development and supporting user-defined extensions.
- **Python frontend PyAsc**: Based on native Python interfaces (see [PyAsc](https://gitcode.com/cann/pyasc)), it provides complete low-level chip programming capabilities and will gradually introduce Layout-based Tensor programming, SIMT programming, and more, enabling high-performance operator development in Python.

> 💡 For how to choose among the multi-level APIs, see the [Ascend C multi-level API selection guide](../../asc_how_to_choose_api.md).

> [!NOTE] Note
> Ascend C supports the following AI processor models:
> - Ascend 950PR/Ascend 950DT
> - Atlas A3 training products/Atlas A3 inference products
> - Atlas A2 training products/Atlas A2 inference products
> - Atlas 200I/500 A2 inference product
> - Atlas inference products
> - Atlas training products


# Ascend C Learning Path

<div style="display: flex; gap: 20px; margin: 30px 0; flex-wrap: wrap;">

<div style="flex: 1; min-width: 300px; border: 2px solid #e5e7eb; border-radius: 10px; padding: 8px; background: linear-gradient(135deg, #e9edff 0%, #ffffff 100%);">
<h2 style="color: #1a1a1a; margin-top: 0; border-bottom: 2px solid #e5e7eb; padding-bottom: 10px;">🚀 Quick Start: Get Hands-on Quickly and Build a Full Picture</h2>
<p style="color: #555; font-size: 14px; line-height: 1.6; min-height: 65px; margin-bottom: 10px;">The goal of the beginner stage is to help you run your first operator in the shortest possible time and understand the basic working mechanism of Ascend C.</p>
<div style="margin: 5px 0; padding: 8px; background: #fff; border-left: 3px solid #3b82f6; border-radius: 4px;">
<strong style="color: #1a1a1a;">🔧 Environment Setup</strong><br/>
<a href="./environment_setup.md" style="color: #3b82f6; font-size: 14px;">CANN Environment Installation</a>
</div>
<div style="margin: 5px 0; padding: 8px; background: #fff; border-left: 3px solid #3b82f6; border-radius: 4px;">
<strong style="color: #1a1a1a;">📖 Programming Model Overview</strong><br/>
<a href="./quick_start/heterogeneous_system_and_programming_model.md" style="color: #3b82f6; font-size: 14px;">Introduction to Heterogeneous Systems and SIMD/SIMT Programming Models</a>
</div>
<div style="margin: 5px 0; padding: 8px; background: #fff; border-left: 3px solid #3b82f6; border-radius: 4px;">
<strong style="color: #1a1a1a;"><span style="color: #3b82f6;">💻</span> Hands-on Practice</strong><br/><span style="color: #666; font-size: 14px;">• 
<a href="./quick_start/simd_programming/simd_programming.md" style="color: #3b82f6; font-size: 14px;">SIMD Operator Quick Start</a><br/><span style="color: #666; font-size: 14px;">• 
<a href="./quick_start/simt_programming/simt_programming.md" style="color: #3b82f6; font-size: 14px;">SIMT Operator Quick Start</a>
</div>
</div>

<div style="flex: 1; min-width: 300px; border: 2px solid #e5e7eb; border-radius: 10px; padding: 8px; background: linear-gradient(135deg, #caeaf8 0%, #ffffff 100%);">
<h2 style="color: #1a1a1a; margin-top: 0; border-bottom: 2px solid #e5e7eb; padding-bottom: 10px;">⚡ Advanced Programming: Master the Programming Model for Custom Development</h2>
<p style="color: #555; font-size: 14px; line-height: 1.6; min-height: 65px; margin-bottom: 10px;">This stage focuses on understanding and mastering the SIMD and SIMT programming models of Ascend C, enabling you to independently develop vector and matrix operators to meet the basic performance requirements of common scenarios.</p>

<div style=" margin: 5px 0; padding: 8px; background: #fff; border-left: 3px solid #3b82f6; border-radius: 4px;">
<strong style="color: #1a1a1a;"><a href="../programming_guide/programming_model/programming_model_overview.md" style="color: #3b82f6; text-decoration: none;">📖 Programming Model</a></strong><br/>
<div style="display: flex; gap: 15px; margin-top: 10px;">
<div style="flex: 1; background: #f8f9fa; padding: 12px; border-radius: 8px; border: 1px solid #e5e7eb;">
<a href="../programming_guide/programming_model/ai_core_simd_programming/overview.md" style="color: #3b82f6; font-size: 14px; font-weight: bold;">SIMD Programming</a><br/>
<span style="color: #666; font-size: 14px;">• <a href="../programming_guide/programming_model/ai_core_simd_programming/c_pointer_programming/language_extension_c_api_programming.md" style="color: #3b82f6; font-size: 13px;">Pointer-based C Programming</a></span><br/>
<span style="color: #666; font-size: 14px;">• <a href="../programming_guide/programming_model/ai_core_simd_programming/cpp_tensor_programming/static_tensor_programming.md" style="color: #3b82f6; font-size: 13px;">Tensor-based C++ Programming</a></span><br/>
<span style="color: #666; font-size: 14px;">•  <a href="../programming_guide/programming_model/ai_core_simd_programming/tpipe_tque_programming/tpipe_tque_framework_programming_principles.md" style="color: #3b82f6; font-size: 13px;">TPipe-TQue Framework-based Programming</a></span>
</div>
<div style="flex: 1; display: flex; flex-direction: column; gap: 10px;">
<div style="background: #f8f9fa; padding: 12px; border-radius: 8px; border: 1px solid #e5e7eb;">
<a href="../programming_guide/programming_model/ai_core_simt_programming/ai_core_simt_programming.md" style="color: #3b82f6; font-size: 14px; font-weight: bold;">SIMT Programming</a>
</div>
<div style="background: #f8f9fa; padding: 12px; border-radius: 8px; border: 1px solid #e5e7eb;">
<a href="../programming_guide/language_extension/SIMD-BuiltIn_keyword.md" style="color: #3b82f6; font-size: 14px; font-weight: bold;">Language Extension Layer Overview</a>
</div>
</div>
</div>
</div>
<div style=" margin: 5px 0; padding: 8px; background: #fff; border-left: 3px solid #3b82f6; border-radius: 4px;">
<strong style="color: #1a1a1a;">🔧 Compilation and Execution</strong><br/>
<a href="../programming_guide/compilation_and_execution/asynchronous_execution.md" style="color: #3b82f6; font-size: 14px;">Asynchronous Execution</a> | <a href="../programming_guide/compilation_and_execution/operator_compilation/bisheng_compiler.md" style="color: #3b82f6; font-size: 14px;">Operator Compilation</a>
</div>
<div style=" margin: 5px 0; padding: 8px; background: #fff; border-left: 3px solid #3b82f6; border-radius: 4px;">
<strong style="color: #1a1a1a;"><a href="../programming_guide/debug_and_tuning/overview.md" style="color: #3b82f6; text-decoration: none;">🔧 Debugging and Tuning</a></strong><br/>
<a href="../programming_guide/debug_and_tuning/functional_debug/cpu_domain_twin_debugging.md" style="color: #3b82f6; font-size: 14px;">Functional Debugging</a> <span style="color: #999; font-size: 13px;">(<a href="../programming_guide/debug_and_tuning/functional_debug/cpu_domain_twin_debugging.md" style="color: #3b82f6; font-size: 13px;">CPU Domain Twin Debugging</a> | <a href="../programming_guide/debug_and_tuning/functional_debug/npu_domain_twin_debugging.md" style="color: #3b82f6; font-size: 13px;">NPU Domain Twin Debugging</a>)</span> | <a href="../programming_guide/debug_and_tuning/performance_tuning.md" style="color: #3b82f6; font-size: 14px;">Performance Tuning</a>
</div>
<div style=" margin: 5px 0; padding: 8px; background: #fff; border-left: 3px solid #3b82f6; border-radius: 4px;">
<strong style="color: #1a1a1a;">📦 Operator Deployment</strong><br/>
<a href="../programming_guide/advanced_programming/ai_framework_adaptation/pytorch_framework.md" style="color: #3b82f6; font-size: 14px;">PyTorch Framework Adaptation</a>
</div>
<div style=" margin: 5px 0; padding: 8px; background: #fff; border-left: 3px solid #3b82f6; border-radius: 4px;">
<strong style="color: #1a1a1a;"><span style="color: #3b82f6;">💻</span> Practice Consolidation</strong><br/>
<span style="color: #666; font-size: 14px;">• <a href="../operator_practice/simd_operator_impl/vector_programming/overview.md" style="color: #3b82f6; font-size: 14px;">Typical SIMD Vector Operator Development</a></span><br/>
<span style="color: #666; font-size: 14px;">• <a href="../operator_practice/simd_operator_impl/vector_programming/matrix_basic_api/decoupled_mode.md" style="color: #3b82f6; font-size: 14px;">Typical SIMD Matrix Operator Development</a></span><br/>
<span style="color: #666; font-size: 14px;">• <a href="../operator_practice/simt_operator_impl/operator_implementation.md" style="color: #3b82f6; font-size: 14px;">Typical SIMT Operator Development</a></span>
</div>

</div>

<div style="flex: 1; min-width: 300px; border: 2px solid #e5e7eb; border-radius: 10px; padding: 8px; background: linear-gradient(135deg, #eae8fb 0%, #ffffff 100%);">
<h2 style="color: #1a1a1a; margin-top: 0; border-bottom: 2px solid #e5e7eb; padding-bottom: 10px;">🏆 Advanced Programming: Unlock Extreme Performance and Master Complex Scenarios</h2>
<p style="color: #555; font-size: 14px; line-height: 1.6; min-height: 65px; margin-bottom: 10px;">This stage focuses on gaining an in-depth understanding of the underlying hardware details and fully mastering advanced programming features to support the goal of achieving extreme operator performance.</p>

<div style=" margin: 5px 0; padding: 8px; background: #fff; border-left: 3px solid #3b82f6; border-radius: 4px;">
<strong style="color: #1a1a1a;">📖 Advanced Programming</strong><br/><span style="color: #666; font-size: 14px;">• 
<a href="../programming_guide/advanced_programming/hardware_implementation/basic_architecture.md" style="color: #3b82f6; font-size: 14px;">Hardware Implementation</a><br/>
<span style="color: #666; font-size: 14px;">• <a href="../programming_guide/advanced_programming/advanced_ai_core_programming_model/simd_simt_hybrid_programming/overview.md" style="color: #3b82f6; font-size: 14px;">SIMD & SIMT Hybrid Programming</a></span><br/>
<span style="color: #666; font-size: 14px;">• <a href="../programming_guide/advanced_programming/aclnn_operator_development/overview.md" style="color: #3b82f6; font-size: 14px;">Aclnn Operator Engineering Development</a></span><br/>
<span style="color: #666; font-size: 14px;">• <a href="../programming_guide/advanced_programming/operator_graph_development/overview.md" style="color: #3b82f6; font-size: 14px;">Operator Graph Integration Development</a></span><br/>
<span style="color: #666; font-size: 14px;">• <a href="../programming_guide/advanced_programming/ai_framework_adaptation/overview.md" style="color: #3b82f6; font-size: 14px;">AI Framework Operator Adaptation</a></span>
</div>
<div style=" margin: 5px 0; padding: 8px; background: #fff; border-left: 3px solid #3b82f6; border-radius: 4px;">
<strong style="color: #1a1a1a;"><span style="color: #3b82f6;">💻</span> Practice Consolidation</strong><br/><span style="color: #666; font-size: 14px;">• 
<a href="../operator_practice/simd_simt_hybrid_operator_impl/operator_implementation.md" style="color: #3b82f6; font-size: 14px;">Typical SIMD & SIMT Hybrid Operator Development</a><br/><span style="color: #666; font-size: 14px;">• 
<a href="https://gitcode.com/cann/cann-samples" style="color: #3b82f6; font-size: 14px;">Ascend C Operator High-Performance Practice Sample Repository</a>
</div>
<div style=" margin: 5px 0; padding: 8px; background: #fff; border-left: 3px solid #3b82f6; border-radius: 4px;">
<strong style="color: #1a1a1a;">🔄 Cross-generation Migration</strong><br/>
<a href="../cross_gen_migration_guide/overview.md" style="color: #3b82f6; font-size: 14px;">Cross-generation Migration Compatibility Guide</a>
</div>

</div>

</div>

<div style="border: 2px solid #e5e7eb; border-radius: 10px; padding: 20px; margin: 30px 0; background: linear-gradient(135deg, #e0e5ff 0%, #ffffff 100%);">
<h2 style="color: #1a1a1a; margin-top: 0; border-bottom: 2px solid #e5e7eb; padding-bottom: 10px;">📚 Technical Appendix</h2>
<div style="display: flex; gap: 20px; flex-wrap: wrap; margin-top: 15px;">
<div style="flex: 1; min-width: 200px;">
<a href="../technical_appendix/concepts_and_terms/glossary.md" style="display: block; padding: 10px; background: #fff; border-radius: 5px; color: #3b82f6; text-decoration: none; margin-bottom: 10px; font-size: 14px;">📖 Basic Concepts and Terms</a>
<a href="../technical_appendix/cpp_standard_support/overview.md" style="display: block; padding: 10px; background: #fff; border-radius: 5px; color: #3b82f6; text-decoration: none; margin-bottom: 10px; font-size: 14px;">🔧 C++ Language Standard Support</a>
</div>
<div style="flex: 1; min-width: 200px;">
<a href="../../api/api_list.md" style="display: block; padding: 10px; background: #fff; border-radius: 5px; color: #3b82f6; text-decoration: none; margin-bottom: 10px; font-size: 14px;">📖 Ascend C API Reference</a>
<a href="https://hiascend.com/document/redirect/CannCommunityadev" style="display: block; padding: 10px; background: #fff; border-radius: 5px; color: #3b82f6; text-decoration: none; margin-bottom: 10px; font-size: 14px;">🚀 CANN Runtime APIs</a>
</div>
</div>
</div>
