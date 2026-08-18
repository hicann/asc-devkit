# 概述

本文是基础内容，说明Aclnn算子工程化开发这组文档的范围、阅读顺序和前置条件。建议先确认环境满足要求，再按[快速入门](./aclnn_quick_start.md)跑通AddCustom示例，跑通后回到对应章节逐步补齐细节。


## 什么是工程化算子开发

### 关键术语

先介绍几个核心概念：

| 名称 | 含义 |
|------|------|
| 自定义算子工程 | 根据算子原型定义编写或者生成的工程骨架，包含原型定义、Tiling实现、核函数（Kernel）实现和CMake编译配置。 |
| 算子原型 | 描述算子的输入、输出、属性、数据类型、格式、Shape推导等信息。 |
| Host侧 | 运行在CPU侧的准备逻辑，例如Shape推导、Tiling计算、workspace大小设置。 |
| 核函数（Kernel）侧 | 运行在AI Core上的计算逻辑，是算子真正执行计算的部分。 |
| Tiling | 根据输入Shape、数据类型等信息切分计算任务，并把核函数（Kernel）需要的参数传递到核函数（Kernel）侧。 |
| aclnn单算子API | 算子工程编译后基于算子原型自动生成的C语言API，应用程序可通过该接口直接调用自定义算子。 |

### 工程化开发是什么

工程化算子开发是指基于**自定义算子工程**完成算子实现、编译部署和调用验证的开发方式，主要面向CANN相关场景对接：单算子调用场景通过aclnn API完成接入，算子入图场景通过GE图执行链路完成接入。通过工程化开发，算子原型、Host侧Tiling、核函数（Kernel）实现和编译部署配置可以在两类场景中最大化复用同一份交付件。相比手动组织编译流程和分别对接调用链路，工程化开发由msOpGen生成工程骨架，CMake管理编译，编译产物自动包含aclnn API以及入图所需的原型定义等文件，部署时只需安装.run包或者链接静态/动态库。

### 集成方式

围绕同一份算子工程，常见集成方式如下：

| 集成方式 | 说明 | 适用场景 |
|---------|------|---------|
| 单算子API调用 | 通过生成的aclnn接口直接调用算子，基于C语言API执行算子。 | 算子功能验证、单算子调用。 |
| 算子入图 | 补充Shape推导等入图适配代码后，基于GE图执行链路执行自定义算子。 | 图模式、IR构图、多算子组合。 |
| AI框架调用 | 在工程化算子基础上增加框架插件适配后，通过PyTorch/TensorFlow等框架调用自定义算子。 | 融入AI训练或推理流程。 |

## 推荐阅读路径

首次接触建议从[快速入门](./aclnn_quick_start.md)开始，先用5分钟跑通AddCustom算子的端到端流程。跑通后再按以下路径逐步补齐细节。

本指南共22个文档，按开发流程分为四个部分。首次阅读只需关注`[基础]`文档，遇到具体问题时再查阅`[扩展]`文档。

### 设计与实现

`[基础]`（按顺序阅读）：
- [算子功能设计](./design_and_implementation/operator_function_design.md) — 支持范围、场景划分、核函数（Kernel）组织。
- [算子原型定义](./design_and_implementation/operator_prototype_definition.md) — 输入输出属性定义、Shape推导。
- [Host侧Tiling实现](./design_and_implementation/host_tiling_implementation.md) — Tiling函数与数据结构、使用约束。
- [核函数（Kernel）侧算子实现](./design_and_implementation/kernel_operator_implementation.md) — 核函数（Kernel）编程与API使用。

`[扩展]`（遇到具体问题时查阅）：
- [通过TilingData传递属性信息](./design_and_implementation/tiling_data_attributes.md) — 适用于算子属性需要传递到核函数（Kernel）侧的场景。
- [多分支策略](./design_and_implementation/multi_branch_strategy.md) — 适用于算子需要按场景拆分核函数（Kernel）入口的场景。
- [使用高阶API时配套的Tiling实现](./design_and_implementation/tiling_with_advanced_api.md) — 适用于开发Matmul/Conv等复杂算子的场景。
- [开启Tiling下沉](../operator_graph_development/enable_tiling_sink.md) — 适用于需要在算子入图场景开启Tiling下沉的场景。

### 编译与部署

`[基础]`：
- [编译与部署基本流程](./compilation_and_deployment/basic_process.md) — 编译配置、算子包打包与部署。

`[扩展]`：
- [多算子包组织](./compilation_and_deployment/multi_operator_package.md) — 多算子包拆分策略。
- [算子动态库和静态库编译](./compilation_and_deployment/dynamic_static_lib_compilation.md) — 适用于需要以库形式集成到应用的场景。
- [交叉编译](./compilation_and_deployment/cross_compilation.md) — 适用于开发环境与运行环境架构不同的场景。
- [编译过程调试](./compilation_and_deployment/compilation_debug.md) — 适用于遇到编译问题的场景。
- [编译加速](./compilation_and_deployment/compilation_acceleration.md) — 适用于需要加速构建的场景。

### 调用验证

`[基础]`：
- [单算子API调用](./invocation/single_operator_api_call.md) — 通过aclnn API调用验证算子功能。

`[扩展]`：
- [运行时加载机制](./invocation/runtime_loading_mechanism.md) — 适用于需要了解二段式调用模型和代码写法如何影响运行表现的场景。

### 参考文档

以下为查阅型内容，可在开发过程中按需检索：

- [命名转换规则对照表](./appendix/naming_conversion_table.md) — 算子类型名、文件名、核函数（Kernel）名的转换规则。
- [cmake函数参考](./appendix/cmake_function_reference.md) — CMake函数的参数与用法详解。
- [外部样例链接汇总](./appendix/external_sample_links.md) — 文档中引用的外部样例与API链接汇总。
