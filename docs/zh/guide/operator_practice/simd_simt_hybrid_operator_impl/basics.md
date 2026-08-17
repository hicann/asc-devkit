# 基础知识<a name="ZH-CN_TOPIC_0000002532388147"></a>

本节内容为使用[Reg矢量计算API](../../../api/SIMD-API/basic_api/reg_vector_compute/reg_vector_compute.md)和[SIMT API](../../../api/SIMT-API/SIMT-API.md)进行SIMD与SIMT混合编程的指导。

在[Vector Core](../../programming_guide/advanced_programming/hardware_implementation/basic_architecture.md)中，SIMT单元和SIMD单元共享片上存储，因此可以利用片上存储Unified Buffer完成SIMD与SIMT混合编程，具体硬件架构的介绍请参考[NPU架构版本3510](../../programming_guide/advanced_programming/hardware_implementation/architecture_spec/npu_arch_3510.md)。在进行后续内容的学习前，请先了解SIMD与SIMT混合编程的编程模型：[SIMD与SIMT混合编程](../../programming_guide/advanced_programming/advanced_ai_core_programming_model/simd_simt_hybrid_programming/overview.md)。

[SIMD编程](../../programming_guide/programming_model/ai_core_simd_programming/cpp_tensor_programming/reg_vector_computation.md)提供了基于寄存器（Regbase）开发的Reg矢量计算API，Reg矢量计算API可以直接操作Vector Core中的SIMD寄存器，API单次处理的数据量上限等于寄存器的大小，通过[GetVecLen](../../../api/SIMD-API/basic_api/tool_interface/system_resources_and_variables/GetVecLen.md)接口获取该值。在算子实现中，需要多次调用Reg矢量计算API完成对单核数据的处理。

与SIMD编程不同的是，在SIMT编程中Global Memory上的数据可以被直接读取和使用。SIMT编程常通过组织线程的层次结构来实现数据的切分，使用threadIdx等[SIMT BuiltIn关键字](../../programming_guide/language_extension/simt_builtin_keywords.md)计算线程应处理的数据索引，完成索引对应数据的计算，从而将函数实现简化为标量计算。
