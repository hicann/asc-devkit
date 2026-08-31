# 数据搬运类C API样例介绍

## 概述

本路径下包含多个与数据搬运相关的Ascend C C API样例。每个样例均使用Ascend C C API的`<<<>>>`直调方式，在同一个`.asc`文件中实现主函数和核函数。

## 样例列表

| 目录名称 | 功能描述 | 支持的产品 |
| --- | --- | --- |
| [data_copy_gm2l1](./data_copy_gm2l1) | 本样例展示将输入矩阵数据从GM（Global Memory）搬运到L1（L1 Buffer）的过程，包含格式转换、矩阵乘计算以及通过Fixpipe进行的量化结果搬出。 | Ascend 950PR/Ascend 950DT |
| [data_copy_l0c2gm](./data_copy_l0c2gm) | 本样例展示通过Fixpipe将L0C（L0C Buffer）中的矩阵乘计算结果搬运到GM（Global Memory），并在搬运过程中完成标量和Vector量化、ReLU激活以及Nz格式到ND格式的转换。 | Ascend 950PR/Ascend 950DT |
| [data_copy_ub2l1](./data_copy_ub2l1) | 本样例在原有Nz输入和ND输入两种UB（Unified Buffer）到L1（L1 Buffer）矩阵乘场景基础上使用非对齐shape，并新增GM->UB->L1->UB->GM双向通路场景。 | Ascend 950PR/Ascend 950DT |
| [reg_load_gather](./reg_load_gather) | 本样例演示离散数据搬入的过程，包含按元素索引的`asc_gather`和按数据块（DataBlock）索引的`asc_gather_datablock`两种场景。 | Ascend 950PR/Ascend 950DT |
| [reg_load_store_align](./reg_load_store_align) | 本样例展示使用Reg矢量计算接口实现UB（Unified Buffer）与矢量数据寄存器之间的连续和非连续的对齐数据搬运。 | Ascend 950PR/Ascend 950DT |
| [reg_load_store_mask](./reg_load_store_mask) | 本样例展示使用Reg矢量计算接口实现UB（Unified Buffer）与掩码寄存器之间的加载和存储，以及基于掩码寄存器进行数据选择的操作。 | Ascend 950PR/Ascend 950DT |
