# Deterministic Computation样例介绍

## 概述

基于Ascend C编程的基础样例，展示SIMT确定性计算相关样例。

## 样例列表

|  目录名称                                                   |  功能描述                                              |
| ------------------------------------------------------------ | ---------------------------------------------------- |
| [deterministic_reduce](./deterministic_reduce) | 基于SIMT编程方式实现归约求和对照样例，展示浮点原子累加的不确定性与两种固定顺序归约——业界通用折半规约与[`asc_shfl_down`](../../../../../docs/zh/api/SIMT-API/Warp_functions/Warp_shfl_functions/asc_shfl_down.md)蝶形归并树（实测更优）——的确定性，并通过Host同序复算golden实现逐bit验证。|
