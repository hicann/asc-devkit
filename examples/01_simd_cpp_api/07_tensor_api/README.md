# Tensor API样例介绍

## 概述

本目录收录直接包含`tensor_api/tensor.h`并使用Tensor API编程方式实现的样例。目录按照Tensor构造、Layout处理、矩阵计算和实验性接口进行规划，各分类目录负责维护该类别下的具体样例索引。

## 目录规划

| 目录名称 | 功能描述 |
| --- | --- |
| [tensor](./tensor) | 用于存放Tensor构造、访问、切分等基础能力的样例。 |
| [layout](./layout) | 用于存放Layout构造、变换和布局模式相关的样例。 |
| [matrix_compute](./matrix_compute) | 包含矩阵乘、批量矩阵乘、卷积、数据搬入搬出和高性能MxFP4矩阵乘等矩阵计算样例。 |
| [experimental/reg_vector_compute](./experimental/reg_vector_compute) | 包含实验性Tensor API样例，当前提供基于寄存器Tensor接口实现的向量计算样例。 |
