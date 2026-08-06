# Atomic Operation样例介绍

## 概述

基于Ascend C编程的基础样例，展示SIMT原子操作接口的使用方法。

## 样例列表

|  目录名称                                                   |  功能描述                                              |
| ------------------------------------------------------------ | ---------------------------------------------------- |
| [histogram](./histogram) | 基于SIMT编程方式实现Histogram样例，展示[`asc_atomic_add()`](../../../../../docs/zh/api/SIMT-API/atomic_operations/asc_atomic_add.md)接口的使用方法。|
| [atomic_add_perf](./atomic_add_perf) | 以[`asc_atomic_add()`](../../../../../docs/zh/api/SIMT-API/atomic_operations/asc_atomic_add.md)为例，通过五组对照实验展示内存层级、原子操作组织方式、是否使用返回值、数据类型对原子加性能的影响，并给出优化指导。|
| [atomic_intrinsics](./atomic_intrinsics) | 展示Ascend C SIMT原子操作接口（加、减、交换、比较交换、自增、自减、按位与/或/异或等）的使用方法。|
