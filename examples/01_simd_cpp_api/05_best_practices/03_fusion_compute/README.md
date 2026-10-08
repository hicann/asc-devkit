# Fusion Compute Practices样例介绍

## 概述

本目录提供融合计算相关的最佳实践样例，覆盖SSBuffer通信以及QuantGroupMatmul分组量化矩阵乘和Matmul+GELU的Cube-Vector融合实现。

## 样例列表

| 目录名称 | 功能描述 | 支持的产品 |
| ------------------------------------------------------------ | ---------------------------------------------------- | --- |
| [matmul_gelu_high_performance](./matmul_gelu_high_performance) |  本样例展示Cube-Vector融合的高性能实现，将Matmul矩阵乘法与GELU激活函数融合到同一AI Core中并行执行。 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [quant_group_matmul_high_performance](./quant_group_matmul_high_performance) |  本样例介绍QuantGroupMatmul算子的高性能实现，支持per-token量化的分组矩阵乘法与GELU激活计算，并展示CV融合时Vector bound场景下的性能调优方法。 | Ascend 950PR&950DT系列产品<br>Atlas A3系列产品<br>Atlas A2系列产品 |
| [ssbuf_aiv_aic_comm](./ssbuf_aiv_aic_comm) | 本样例使用SSBuffer环形队列实现AIV到AIC通信。 | Ascend 950PR&950DT系列产品 |
