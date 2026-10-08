# Memory矢量计算算子样例介绍

## 概述

本样例集介绍了Memory矢量计算算子不同特性的典型用法，给出了对应的端到端实现。

## 样例列表

| 目录名称 | 功能描述 | 支持的产品 |
|----------------------------------------------------------------------------------------------------------------------|-----------------------------------------------------------------------------| --- |
| [brcb](./brcb) |  本样例基于Brcb实现数据填充，可用于每次取输入张量中的8个数填充到结果张量的8个datablock中 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [cast](./cast) |  本样例基于Cast实现源操作数和目的操作数Tensor的数据类型及精度转换 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [compare](./compare) |  本样例基于Compare、Compares接口完成多场景下的数据比较功能，实现逐元素大小比较。 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [create_vec_index](./create_vec_index) |  本样例介绍了调用CreateVecIndex实现创建指定起始值的向量索引的方法 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [duplicate](./duplicate) |  本样例基于Duplicate实现数据填充，可用于将一个变量或立即数复制多次并填充到向量中 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [element_wise_arithmetic](./element_wise_arithmetic) |  本样例基于LeakyRelu展示了基础算术类接口的使用方法 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [element_wise_compound_compute](./element_wise_compound_compute) |  本样例介绍Ascend C矢量计算复合接口的使用方法 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [element_wise_logic](./element_wise_logic) |  本样例基于And、Ors、ShiftLeft、ShiftRight接口实现按位逻辑运算功能 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [gather](./gather) |  本样例基于GatherMask、Gather、Gatherb等接口完成多种场景模式下的数据选择功能，实现从源操作数中选取元素写入目的操作数。 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [interleave_pair](./interleave_pair) |  本样例基于Interleave和DeInterleave接口实现元素交织和解交织功能 | Ascend 950PR&950DT系列产品 |
| [mrg_sort](./mrg_sort) |  本样例基于Sort32和MrgSort基础API实现将已排好序的最多4条队列，合并成1条队列，结果按照score域由大到小排序 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [reduce](./reduce) |  本样例介绍了调用`ReduceRepeat<SUM>`和`ReduceDataBlock<SUM>`基础API实现reduce算子，实现了连续内存上数据元素的累加，返回累加结果的功能 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [reduce_data_block](./reduce_data_block) |  本样例基于`ReduceDataBlock`实现归约计算 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [reduce_repeat](./reduce_repeat) |  本样例介绍归约类接口`ReduceRepeat`在多种场景下的使用方法。 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [reduce_pair_elem](./reduce_pair_elem) |  本样例基于`ReducePairElem<SUM>`实现相邻奇偶元素对求和归约 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [reduce_computation](./reduce_computation) |  本样例基于`ReduceMax`、`ReduceMin`、`ReduceSum`接口实现归约计算 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [region_proposal_sort](./region_proposal_sort) |  本样例介绍Region Proposal相关的排序接口使用方法 | Atlas推理系列产品AI Core |
| [select](./select) |  本样例基于Select接口完成多种场景下的数据选择功能，根据掩码在两个向量或向量与标量之间选取元素写入目的向量 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [transpose](./transpose) |  本样例基于Transpose、TransDataTo5HD接口实现数据转置功能，包括16*16的二维矩阵数据块转置，[N,C,H,W]与[N,H,W,C]四维矩阵互相转换以及NCHW格式转换成NC1HWC0格式。 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
