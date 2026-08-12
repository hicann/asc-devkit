# SIMD与SIMT混合编程样例介绍

## 概述

本目录提供SIMD与SIMT混合编程相关的最佳实践样例，覆盖SIMT灵活实现分支判断逻辑、访存合并优化、使用UB提升离散内存访问效率以及gridDim配置优化等场景。

## 样例列表

| 目录名称 | 功能描述 | 支持的产品 |
| ------------------------------------------------------------ | ---------------------------------------------------- | --- |
| [simd_simt_adaptive_max_pool3d_grad](./simd_simt_adaptive_max_pool3d_grad) |  本样例以adaptive_max_pool3d_grad为例，介绍存在离散写的场景下，混合编程相对于纯SIMD的优势。 | Ascend 950PR/Ascend 950DT |
| [simd_simt_matrix_transpose](./simd_simt_matrix_transpose) |  本样例以矩阵转置为例，介绍Ascend C SIMD与SIMT混合编程场景下的访存调优方法，展示MTE搬运与UB中转优化GM访问模式、固定Thread Block数降低调度开销、UB padding降低bank冲突以及双缓冲流水并行的完整调优路径。 | Ascend 950PR/Ascend 950DT |
| [simd_simt_high_performance](./simd_simt_high_performance) |  本样例以FloorMod计算为例，介绍SIMD与SIMT混合编程场景下的性能调优方法，展示SIMT直接访问GM、SIMD Reg矢量计算、SIMT访问UB以及调整线程映射使Warp内连续访问UB优化的性能差异。 | Ascend 950PR/Ascend 950DT |
| [simd_simt_hash_table_mte_queue](./simd_simt_hash_table_mte_queue) |  本样例以HashTable插入或更新键值对场景为例，介绍SIMD与SIMT混合编程场景下使用MTE任务队列搬运value向量的性能优化方法。 | Ascend 950PR/Ascend 950DT |
| [simd_simt_grid_dim_config](./simd_simt_grid_dim_config) |  本样例以Gather计算为例，介绍Ascend C SIMD与SIMT混合编程场景下的gridDim配置和vf函数调用优化思路。 | Ascend 950PR/Ascend 950DT |
| [simd_simt_integer_fast_div](./simd_simt_integer_fast_div) |  本样例以整数除法为例，展示SIMD与SIMT混合编程场景下针对固定除数使用乘法和移位替代普通除法的指令优化方式。 | Ascend 950PR/Ascend 950DT |
