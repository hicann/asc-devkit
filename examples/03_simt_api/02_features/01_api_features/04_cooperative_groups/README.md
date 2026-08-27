# Cooperative Groups样例介绍

## 概述

基于Ascend C编程的基础样例，展示SIMT[协作组](../../../../../docs/zh/api/SIMT-API/cooperative_groups/cooperative_groups_intro.md)的基本用法。

## 样例列表

|  目录名称                                                   |  功能描述                                              |
| ------------------------------------------------------------ | ---------------------------------------------------- |
| [reduce_sum](./reduce_sum) | 以归约求和算子为例，通过grid、thread block、warp三种粒度的协作组实现同一套归约逻辑，展示协作组在代码抽象与复用方面的优势，并对比三种粒度的性能差异。|
