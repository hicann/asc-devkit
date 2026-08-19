# Ascend C概述

## CANN概述
AI（Artificial Intelligence，人工智能）芯片是专为加速人工智能计算任务设计的专用处理器，凭借高度并行化的硬件架构，可高效应对深度学习场景中的大规模张量运算。NPU（Neural-Network Processing Unit，神经网络处理器）作为AI芯片的典型代表，其核心优势在于适配神经网络任务的高效执行；昇腾AI处理器作为专用NPU，内部集成大量高性能计算核心，进一步强化了这一硬件优势，为AI任务的高效运行提供坚实支撑。

在神经网络模型中，算子是构成模型的基础计算单元，封装了针对张量数据的特定数学操作逻辑，如卷积、矩阵乘、激活函数等。神经网络模型的推理与训练过程，本质上是一系列算子按照预设逻辑有序执行、协同完成计算任务的过程。为充分释放昇腾NPU的硬件算力，需构建一套能够高效衔接上层AI框架与底层硬件的软件平台，而CANN（Compute Architecture for Neural Networks）正是华为专为昇腾NPU打造的全栈软件解决方案。

CANN基于分层架构设计，实现了上层应用与底层硬件的无缝衔接。向上，兼容PyTorch等主流AI框架，确保应用层的兼容性与易用性；向下，深度适配昇腾AI处理器，充分发挥硬件的计算潜能，进而构建起CPU与NPU深度融合的异构计算体系。在该体系中，CANN通过Runtime运行时接口驱动CPU与NPU高效协同调度：CPU完成数据准备、任务拆解与调度后，将算子任务下发至NPU加速执行；计算完成后，结果由CPU回收或根据业务需求进行进一步处理。这一协同机制贯穿算子、模型、应用全层级，有效达成计算性能与运行效率的最优平衡，为昇腾AI处理器算力的充分释放提供核心软件支撑。

## Ascend C简介
大模型时代，算力需求正以前所未有的速度激增。如何开发高性能算子，充分释放硬件每一分潜能，已成为决定AI应用落地的关键。为帮助开发者高效构建可直接运行于NPU的高性能算子，Ascend C应运而生。Ascend C是CANN生态下，面向昇腾AI处理器推出的专用算子开发语言，原生兼容C与C++标准规范。此外，Ascend C秉持**开放芯片完备编程能力，支撑极致性能**的设计理念，构建多层级API体系，满足多场景算子开发诉求，让您能够根据项目需求、团队技能和性能目标，在开发效率与运行性能之间灵活权衡，实现最优平衡。

为满足不同场景的诉求，Ascend C构建了从单指令抽象、单核公共算法到多核算子的多层级API体系，其总体逻辑架构如下：

<img src="../figures/architecture_ascendc.png" alt="架构图" width="1000px">

- **语言扩展层C API**：纯C接口，提供数组内存分配、基于指针的计算接口，延续业界熟悉的C编程体验，并完整开放芯片能力。<!-- npu="950" id1 -->Ascend 950PR/Ascend 950DT新增SIMT、SIMD/SIMT混合编程能力。<!-- end id1 -->
- **基础API**：以单指令为抽象的C++类库，一般基于Tensor编程，并逐步通过Layout完善Tensor编程体验。
- **高阶API**：对单核常见算法进行抽象与封装，提供开箱即用的公共算法实现。
- **算子模板库**：基于模板提供算子的完整实现参考，降低Tiling开发复杂度，支持用户自定义扩展。
- **Python前端PyAsc**：基于Python原生接口（参见 [PyAsc](https://gitcode.com/cann/pyasc)），提供芯片底层完备编程能力，并将逐步引入Layout化Tensor编程、SIMT编程等，实现用Python开发高性能算子。

> 💡 如何选择多级API，可参考 [Ascend C多级API选择指南](../../asc_how_to_choose_api.md)。

> [!NOTE]说明
> Ascend C支持在如下AI处理器型号上使用：
> <!-- npu="950" id2 -->
> - Ascend 950PR/Ascend 950DT
> <!-- end id2 -->
> <!-- npu="A3" id3 -->
> - Atlas A3 训练系列产品/Atlas A3 推理系列产品
> <!-- end id3 -->
> <!-- npu="910b" id4 -->
> - Atlas A2 训练系列产品/Atlas A2 推理系列产品
> <!-- end id4 -->
> <!-- npu="310b" id5 -->
> - Atlas 200I/500 A2 推理产品
> <!-- end id5 -->
> <!-- npu="310p" id6 -->
> - Atlas 推理系列产品
> <!-- end id6 -->
> <!-- npu="910" id7 -->
> - Atlas 训练系列产品
> <!-- end id7 -->

---
# Ascend C学习路径

<div style="display: flex; gap: 20px; margin: 30px 0; flex-wrap: wrap;">

<div style="flex: 1; min-width: 300px; border: 2px solid #e5e7eb; border-radius: 10px; padding: 8px; background: linear-gradient(135deg, #e9edff 0%, #ffffff 100%);">
<h2 style="color: #1a1a1a; margin-top: 0; border-bottom: 2px solid #e5e7eb; padding-bottom: 10px;">🚀 快速入门：快速上手实操，构建全景认知</h2>
<p style="color: #555; font-size: 14px; line-height: 1.6; min-height: 65px; margin-bottom: 10px;">初级阶段的目标是让您在最短时间内跑通第一个算子，理解Ascend C的基本工作方式。</p>
<div style="margin: 5px 0; padding: 8px; background: #fff; border-left: 3px solid #3b82f6; border-radius: 4px;">
<strong style="color: #1a1a1a;">🔧 环境搭建</strong><br/>
<a href="./environment_setup.md" style="color: #3b82f6; font-size: 14px;">CANN环境安装</a>
</div>
<div style="margin: 5px 0; padding: 8px; background: #fff; border-left: 3px solid #3b82f6; border-radius: 4px;">
<strong style="color: #1a1a1a;">📖 编程模型概览</strong><br/>
<a href="./quick_start/heterogeneous_system_and_programming_model.md" style="color: #3b82f6; font-size: 14px;">异构系统与SIMD/SIMT编程模型介绍</a>
</div>
<div style="margin: 5px 0; padding: 8px; background: #fff; border-left: 3px solid #3b82f6; border-radius: 4px;">
<strong style="color: #1a1a1a;"><span style="color: #3b82f6;">💻</span> 动手实践</strong><br/><span style="color: #666; font-size: 14px;">• 
<a href="./quick_start/simd_programming/simd_programming.md" style="color: #3b82f6; font-size: 14px;">SIMD算子快速入门</a><br/><span style="color: #666; font-size: 14px;">• 
<a href="./quick_start/simt_programming/simt_programming.md" style="color: #3b82f6; font-size: 14px;">SIMT算子快速入门</a>
</div>
</div>

<div style="flex: 1; min-width: 300px; border: 2px solid #e5e7eb; border-radius: 10px; padding: 8px; background: linear-gradient(135deg, #caeaf8 0%, #ffffff 100%);">
<h2 style="color: #1a1a1a; margin-top: 0; border-bottom: 2px solid #e5e7eb; padding-bottom: 10px;">⚡ 进阶编程：掌握编程模型，实现定制开发</h2>
<p style="color: #555; font-size: 14px; line-height: 1.6; min-height: 65px; margin-bottom: 10px;">本阶段聚焦于理解并掌握Ascend C的SIMD与SIMT编程模型，使您具备自主开发矢量及矩阵类算子的能力，从而满足常规场景下的基本性能要求。</p>

<div style=" margin: 5px 0; padding: 8px; background: #fff; border-left: 3px solid #3b82f6; border-radius: 4px;">
<strong style="color: #1a1a1a;"><a href="../programming_guide/programming_model/programming_model_overview.md" style="color: #3b82f6; text-decoration: none;">📖 编程模型</a></strong><br/>
<div style="display: flex; gap: 15px; margin-top: 10px;">
<div style="flex: 1; background: #f8f9fa; padding: 12px; border-radius: 8px; border: 1px solid #e5e7eb;">
<a href="../programming_guide/programming_model/ai_core_simd_programming/overview.md" style="color: #3b82f6; font-size: 14px; font-weight: bold;">SIMD编程</a><br/>
<span style="color: #666; font-size: 14px;">• <a href="../programming_guide/programming_model/ai_core_simd_programming/c_pointer_programming/c_programming_overview.md" style="color: #3b82f6; font-size: 13px;">基于指针的C语言编程</a></span><br/>
<span style="color: #666; font-size: 14px;">• <a href="../programming_guide/programming_model/ai_core_simd_programming/cpp_tensor_programming/cpp_tensor_programming_overview.md" style="color: #3b82f6; font-size: 13px;">基于Tensor的C++编程</a></span><br/>
<span style="color: #666; font-size: 14px;">•  <a href="../programming_guide/programming_model/ai_core_simd_programming/tpipe_tque_programming/tpipe_tque_principles.md" style="color: #3b82f6; font-size: 13px;">基于TPipe-TQue框架编程</a></span>
</div>
<div style="flex: 1; display: flex; flex-direction: column; gap: 10px;">
<div style="background: #f8f9fa; padding: 12px; border-radius: 8px; border: 1px solid #e5e7eb;">
<a href="../programming_guide/programming_model/ai_core_simt_programming/ai_core_simt_programming.md" style="color: #3b82f6; font-size: 14px; font-weight: bold;">SIMT编程</a>
</div>
<div style="background: #f8f9fa; padding: 12px; border-radius: 8px; border: 1px solid #e5e7eb;">
<a href="../programming_guide/language_extension/simd_builtin_keywords.md" style="color: #3b82f6; font-size: 14px; font-weight: bold;">语言扩展层概述</a>
</div>
</div>
</div>
</div>
<div style=" margin: 5px 0; padding: 8px; background: #fff; border-left: 3px solid #3b82f6; border-radius: 4px;">
<strong style="color: #1a1a1a;">🔧 构建与运行</strong><br/>
<a href="../programming_guide/compilation_and_execution/async_execution.md" style="color: #3b82f6; font-size: 14px;">异步执行</a> | <a href="../programming_guide/compilation_and_execution/operator_compilation/bisheng_compiler.md" style="color: #3b82f6; font-size: 14px;">算子编译</a>
</div>
<div style=" margin: 5px 0; padding: 8px; background: #fff; border-left: 3px solid #3b82f6; border-radius: 4px;">
<strong style="color: #1a1a1a;"><a href="../programming_guide/debug_and_tuning/overview.md" style="color: #3b82f6; text-decoration: none;">🔧 调试与调优</a></strong><br/>
<a href="../programming_guide/debug_and_tuning/functional_debug/cpu_twin_debug.md" style="color: #3b82f6; font-size: 14px;">功能调试</a> <span style="color: #999; font-size: 13px;">（<a href="../programming_guide/debug_and_tuning/functional_debug/cpu_twin_debug.md" style="color: #3b82f6; font-size: 13px;">CPU域孪生调试</a> | <a href="../programming_guide/debug_and_tuning/functional_debug/npu_board_debug.md" style="color: #3b82f6; font-size: 13px;">NPU域上板调试</a>）</span> | <a href="../programming_guide/debug_and_tuning/performance_tuning.md" style="color: #3b82f6; font-size: 14px;">性能调试</a>
</div>
<div style=" margin: 5px 0; padding: 8px; background: #fff; border-left: 3px solid #3b82f6; border-radius: 4px;">
<strong style="color: #1a1a1a;">📦 算子部署</strong><br/>
<a href="../programming_guide/advanced_programming/ai_framework_adaptation/pytorch_framework.md" style="color: #3b82f6; font-size: 14px;">PyTorch框架适配</a>
</div>
<div style=" margin: 5px 0; padding: 8px; background: #fff; border-left: 3px solid #3b82f6; border-radius: 4px;">
<strong style="color: #1a1a1a;"><span style="color: #3b82f6;">💻</span> 实践巩固</strong><br/>
<span style="color: #666; font-size: 14px;">• <a href="../operator_practice/simd_operator_impl/vector_programming/overview.md" style="color: #3b82f6; font-size: 14px;">典型SIMD矢量算子开发</a></span><br/>
<span style="color: #666; font-size: 14px;">• <a href="../operator_practice/simd_operator_impl/matrix_basic_api/decoupled_mode.md" style="color: #3b82f6; font-size: 14px;">典型SIMD矩阵类算子开发</a></span><br/>
<span style="color: #666; font-size: 14px;">• <a href="../operator_practice/simt_operator_impl/operator_impl.md" style="color: #3b82f6; font-size: 14px;">典型SIMT算子开发</a></span>
</div>

</div>

<div style="flex: 1; min-width: 300px; border: 2px solid #e5e7eb; border-radius: 10px; padding: 8px; background: linear-gradient(135deg, #eae8fb 0%, #ffffff 100%);">
<h2 style="color: #1a1a1a; margin-top: 0; border-bottom: 2px solid #e5e7eb; padding-bottom: 10px;">🏆 高级编程：挖掘极限性能，驾驭复杂场景</h2>
<p style="color: #555; font-size: 14px; line-height: 1.6; min-height: 65px; margin-bottom: 10px;">本阶段聚焦于深入理解硬件底层细节，全面掌握高阶编程特性，以支撑实现算子的极致性能目标。</p>

<div style=" margin: 5px 0; padding: 8px; background: #fff; border-left: 3px solid #3b82f6; border-radius: 4px;">
<strong style="color: #1a1a1a;">📖 高级编程</strong><br/><span style="color: #666; font-size: 14px;">• 
<a href="../programming_guide/advanced_programming/hardware_implementation/basic_architecture.md" style="color: #3b82f6; font-size: 14px;">硬件实现</a><br/>
<span style="color: #666; font-size: 14px;">• <a href="../programming_guide/advanced_programming/advanced_ai_core_programming_model/simd_simt_hybrid_programming/overview.md" style="color: #3b82f6; font-size: 14px;">SIMD & SIMT混合编程</a></span><br/>
<span style="color: #666; font-size: 14px;">• <a href="../programming_guide/advanced_programming/aclnn_operator_development/overview.md" style="color: #3b82f6; font-size: 14px;">Aclnn算子工程化开发</a></span><br/>
<span style="color: #666; font-size: 14px;">• <a href="../programming_guide/advanced_programming/operator_graph_development/overview.md" style="color: #3b82f6; font-size: 14px;">算子入图开发</a></span><br/>
<span style="color: #666; font-size: 14px;">• <a href="../programming_guide/advanced_programming/ai_framework_adaptation/overview.md" style="color: #3b82f6; font-size: 14px;">AI框架算子适配</a></span>
</div>
<div style=" margin: 5px 0; padding: 8px; background: #fff; border-left: 3px solid #3b82f6; border-radius: 4px;">
<strong style="color: #1a1a1a;"><span style="color: #3b82f6;">💻</span> 实践巩固</strong><br/><span style="color: #666; font-size: 14px;">• 
<a href="../operator_practice/simd_simt_hybrid_operator_impl/operator_impl.md" style="color: #3b82f6; font-size: 14px;">典型SIMD & SIMT混合算子开发</a><br/><span style="color: #666; font-size: 14px;">• 
<a href="https://gitcode.com/cann/cann-samples" style="color: #3b82f6; font-size: 14px;">Ascend C算子高性能实践样例仓</a>
</div>
<div style=" margin: 5px 0; padding: 8px; background: #fff; border-left: 3px solid #3b82f6; border-radius: 4px;">
<strong style="color: #1a1a1a;">🔄 跨代迁移</strong><br/>
<a href="../cross_gen_migration_guide/overview.md" style="color: #3b82f6; font-size: 14px;">跨代迁移兼容性指南</a>
</div>

</div>

</div>

<div style="border: 2px solid #e5e7eb; border-radius: 10px; padding: 20px; margin: 30px 0; background: linear-gradient(135deg, #e0e5ff 0%, #ffffff 100%);">
<h2 style="color: #1a1a1a; margin-top: 0; border-bottom: 2px solid #e5e7eb; padding-bottom: 10px;">📚 技术附录</h2>
<div style="display: flex; gap: 20px; flex-wrap: wrap; margin-top: 15px;">
<div style="flex: 1; min-width: 200px;">
<a href="../technical_appendix/concepts_and_terms/glossary.md" style="display: block; padding: 10px; background: #fff; border-radius: 5px; color: #3b82f6; text-decoration: none; margin-bottom: 10px; font-size: 14px;">📖 基本概念与术语</a>
<a href="../technical_appendix/cpp_standard_support/overview.md" style="display: block; padding: 10px; background: #fff; border-radius: 5px; color: #3b82f6; text-decoration: none; margin-bottom: 10px; font-size: 14px;">🔧 C++语言标准支持</a>
</div>
<div style="flex: 1; min-width: 200px;">
<a href="../../api/api_list.md" style="display: block; padding: 10px; background: #fff; border-radius: 5px; color: #3b82f6; text-decoration: none; margin-bottom: 10px; font-size: 14px;">📖 Ascend C API参考</a>
<a href="https://hiascend.com/document/redirect/CannCommunityadev" style="display: block; padding: 10px; background: #fff; border-radius: 5px; color: #3b82f6; text-decoration: none; margin-bottom: 10px; font-size: 14px;">🚀 CANN Runtime接口</a>
</div>
</div>
</div>
