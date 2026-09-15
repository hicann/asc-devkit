# Introduction样例介绍
 
## 概述
 
基于Ascend C的简单的示例，通过Ascend C编程语言实现了自定义算子，分别给出对应的C_API实现。
 
## 样例列表
 
|  目录名称                                                   |  功能描述                                              |
| ------------------------------------------------------------ | ---------------------------------------------------- |
| [00_quickstart](./00_quickstart) | 样例介绍了基于Ascend C的HelloWorld算子的核函数直调方法，从NPU侧运行核验证算子核函数，展示核函数从调用到执行的整体流程 |
| [01_add](./01_add) | 本样例介绍了基于Ascend C的Add自定义Vector算子的核函数直调方法，实现两个输入张量的逐元素相加，支持main函数和kernel函数在同一个cpp文件中实现 |
| [02_matrix](./02_matrix) | 使用SIMD C API实现基础矩阵乘样例。 |
| [04_reg_base_add_compute](./04_reg_base_add_compute) | 本样例介绍了基于Ascend C的Add算子核函数直调方法（RegBase场景），通过C_API实现两个输入张量的逐元素相加，展示了片上存储和寄存器层级的向量计算流程。|
