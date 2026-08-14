# BestPractices样例介绍

## 概述

基于Ascend C SIMT的算子开发实践，展示SIMT特性在实际算子开发中的运用。

## 样例列表

| 目录名称                                               | 功能描述                                                                                        |
| ------------------------------------------------------ | ----------------------------------------------------------------------------------------------- |
| [00_memory_optimizations](./00_memory_optimizations)   | 介绍SIMT编程方式下全局访存合并、片上缓存中转复用、bank冲突规避及短向量调优等访存优化方法。 |
| [01_execution_conf_optimizations](./01_execution_conf_optimizations)   | 介绍SIMT编程方式下执行配置最大线程块设置的优化方法。 |
| [02_control_flow](./02_control_flow)   | 介绍SIMT编程方式下Warp Divergence对性能的影响及优化方法。 |
| [03_instruction_optimizations](./03_instruction_optimizations)   | 以Histogram（直方图统计）为载体，展示原子操作性能优化方法。 |
