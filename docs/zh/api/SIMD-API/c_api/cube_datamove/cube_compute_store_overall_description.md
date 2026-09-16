# 矩阵搬出总体说明

矩阵计算的搬出是Ascend C编程框架中用于数据搬运的一类核心接口，主要实现从L0C Buffer到Global Memory和L1 Buffer的数据传输。该接口系列提供了多种数据搬运模式，能够满足矩阵计算中的数据搬运需求，通常在asc_mmad接口后配合使用。

矩阵计算的搬出接口支持多种随路搬运场景，包括**随路量化、随路ReLU、随路格式转换（Nz2ND、Nz2DN）、随路通道拆分（Channel Split）、随路通道合并（Channel Merge）**。通过配置接口参数与辅助寄存器，开发者可以精确控制数据的搬运路径、起始位置、搬运长度、随路搬运能力等关键信息，充分利用硬件的数据搬运能力，最大化算子的整体执行性能。

<!-- npu="950,A3,910b" id1 -->
矩阵计算的搬出整体流程如下图所示：
<!-- end id1 -->

<!-- npu="A3,910b" id2 -->
**图1** 矩阵计算的搬出整体流程图（[NPU架构版本2201](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md)）

![](../../../figures/fixpipe_execution_flow_a2a3.png)
<!-- end id2 -->

<!-- npu="950" id3 -->
**图2** 矩阵计算的搬出整体流程图（[NPU架构版本3510](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md)）

![](../../../figures/fixpipe_execution_flow_a5.png)
<!-- end id3 -->

其中，Fixpipe Buffer不同的量化模式对应的内存排布如下：

**图3** Fixpipe Buffer内存排布图

![](../../../figures/fixpipe_buffer.png)
