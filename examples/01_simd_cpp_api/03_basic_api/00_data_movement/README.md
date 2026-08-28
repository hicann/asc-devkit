# 数据搬运类API样例介绍

## 概述

本路径下包含了与数据搬运相关的多个API的样例。每个样例均基于Ascend C的<<<>>>直调方法，支持main函数和kernel函数在同一个.asc文件中实现。

## 样例列表

| 目录名称 | 功能描述 | 支持的产品 |
| --------------------------------------------------------- | ----------------------------------------------------------------------------------------------------------------------------| --- |
| [broadcast_ub2l0c](./broadcast_ub2l0c) |  本样例基于BroadCastVecToMM实现数据广播搬运，将位于UB（Unified Buffer）上数据进行广播并搬运到L0C Buffer | Atlas 推理系列产品AI Core |
| [copy_ub2ub](./copy_ub2ub) |  本样例基于Copy实现数据搬运，适用于VECIN，VECOUT之间的数据搬运，支持mask连续模式和counter模式。 | Ascend 950PR/Ascend 950DT<br>Atlas A3 训练系列产品/Atlas A3 推理系列产品<br>Atlas A2 训练系列产品/Atlas A2 推理系列产品 |
| [data_copy_gm2l1](./data_copy_gm2l1) |  本样例介绍如何使用DataCopy将输入数据从GM（Global Memory）搬出到L1（L1 Buffer） | Ascend 950PR/Ascend 950DT<br>Atlas A3 训练系列产品/Atlas A3 推理系列产品<br>Atlas A2 训练系列产品/Atlas A2 推理系列产品 |
| [data_copy_gm2ub_nddma](./data_copy_gm2ub_nddma) |  本样例介绍如何使用多维数据搬运接口实现GM（Global Memory）到UB（Unified Buffer）通路的数据搬运，通过自由配置搬入的维度信息以及对应的Stride，可以用于Padding、Transpose、BroadCast、Slice等多种数据变换操作 | Ascend 950PR/Ascend 950DT |
| [data_copy_gm2ub_slice](./data_copy_gm2ub_slice) |  本样例基于DataCopy实现数据切片搬运，提取多维Tensor数据的子集进行GM（Global Memory）与UB（Unified Buffer）通路之间的搬运 | Ascend 950PR/Ascend 950DT<br>Atlas A3 训练系列产品/Atlas A3 推理系列产品<br>Atlas A2 训练系列产品/Atlas A2 推理系列产品 |
| [data_copy_l0c2gm](./data_copy_l0c2gm) |  本样例在卷积场景下，基于DataCopy实现数据随路量化激活搬运 | Ascend 950PR/Ascend 950DT<br>Atlas A3 训练系列产品/Atlas A3 推理系列产品<br>Atlas A2 训练系列产品/Atlas A2 推理系列产品 |
| [data_copy_pad_gm2ub_ub2gm](./data_copy_pad_gm2ub_ub2gm) |  本样例基于DataCopyPad实现非32字节对齐数据的搬运，并进行数据填充 | Ascend 950PR/Ascend 950DT<br>Atlas A3 训练系列产品/Atlas A3 推理系列产品<br>Atlas A2 训练系列产品/Atlas A2 推理系列产品 |
| [data_copy_ub2l1](./data_copy_ub2l1) |  本样例在Mmad矩阵乘场景下，基于DataCopy实现UB（Unified Buffer）到L1（L1 Buffer）的数据搬运 | Ascend 950PR/Ascend 950DT |
| [reg_auxscalar](./reg_auxscalar) |  本样例演示使用AuxScalar方式从UB读取多个标量数据参与计算 | Ascend 950PR/Ascend 950DT |
| [reg_load_gather](./reg_load_gather) |  本样例演示使用Gather接口实现离散数据搬入，包含高维Gather（源为LocalTensor）和GatherB（按DataBlock收集）两种场景 | Ascend 950PR/Ascend 950DT |
| [reg_load_store_align](./reg_load_store_align) |  本样例实现UB(Unified Buffer)对RegTensor(Reg矢量计算基本单元)的连续和非连续的对齐数据搬运操作 | Ascend 950PR/Ascend 950DT |
| [reg_load_store_mask](./reg_load_store_mask) |  本样例实现UB(Unified Buffer)对MaskReg(掩码寄存器)的搬入搬出，以及使用mask进行掩码搬出的操作 | Ascend 950PR/Ascend 950DT |
| [reg_load_store_unalign](./reg_load_store_unalign) |  本样例实现UB(Unified Buffer)对RegTensor(Reg矢量计算基本单元)的非对齐数据搬运操作 | Ascend 950PR/Ascend 950DT |
| [reg_move](./reg_move) |  本样例演示调用MaskGenWithRegTensor接口实现将掩码从RegTensor(Reg矢量计算基本单元)搬入MaskReg(掩码寄存器)的操作 | Ascend 950PR/Ascend 950DT |
| [reg_store_scatter](./reg_store_scatter) |  本样例演示调用Scatter接口实现离散数据搬出（按元素分散到UB） | Ascend 950PR/Ascend 950DT |
