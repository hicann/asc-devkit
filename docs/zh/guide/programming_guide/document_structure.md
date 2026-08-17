# 本文档组织结构<a name="ZH-CN_TOPIC_0000002533052471"></a>

本指南为**AscendC算子开发**综合技术手册，系统涵盖SIMD/SIMT编程模型、语言扩展特性、C++类库API、编译运行机制、调试调优及高级编程等核心技术。文档兼顾入门学习与深度实践，适配不同层级研发人员。入门开发者可依托本指南完成技术体系搭建，资深算子工程师亦可借此深化技术能力，充分挖掘昇腾AI处理器硬件算力，高质量完成自定义算子的开发与性能优化。

本文档整体架构及各章节核心内容如下：
- **[编程模型](programming_model/programming_model.md)**：深入解析AscendC原生SIMD/SIMT编程模型的底层原理与应用范式。
- **[编译与运行](compilation_and_execution/compilation_and_execution.md)**：讲解算子Kernel调用机制，完整说明工程编译与程序运行全流程。
- **[语言扩展层](language_extension/language_extension.md)**：逐一介绍SIMD、SIMT编程体系的扩展语法、内置关键字及底层系统函数。
- **[C++类库API](library_api/library_api.md)**：梳理AscendC分层接口体系，包含基于Tensor编程的C++基础API、TPipe/TQue框架API、单核通用算法高阶API，以及面向多核算子开发的算子模板库。
- **[调试调优](debug_and_tuning/debug_and_tuning.md)**：介绍算子功能问题定位、性能分析、专项调优的实现方法、配套工具及工程实践经验。
- **[高级编程](advanced_programming/advanced_programming.md)**：阐述进阶开发技术，涵盖SIMD/SIMT混合编程、AI处理器硬件架构、硬件规格与使用约束，以及aclnn算子工程化、GE图接入等高阶方案。