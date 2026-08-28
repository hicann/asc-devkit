# Matrix Compute Practices 样例介绍

## 概述

基于 Matrix Compute API 的矩阵计算优化样例，通过 `<<<>>>` 直调方式，介绍 Matmul 与 MxFP4 Matmul 在高阶 API 和基础 API 场景下的高性能实践。

## 样例列表

| 目录名称 | 功能描述 | 支持的产品 |
| --- | --- | --- |
| [matmul_basic_api_high_performance](./matmul_basic_api_high_performance) |  Matmul 基础 API 最佳实践样例，基于静态 Tensor 编程展示基础 API 下的高性能实现细节。 | Ascend 950PR/Ascend 950DT<br>Atlas A3 训练系列产品/Atlas A3 推理系列产品<br>Atlas A2 训练系列产品/Atlas A2 推理系列产品 |
| [matmul_high_performance](./matmul_high_performance) |  Matmul 高阶 API 递进式性能优化样例，展示多核切分、MDL、L1/L2 Cache、常量 tiling、UnitFlag 等优化方法。 | Ascend 950PR/Ascend 950DT<br>Atlas A3 训练系列产品/Atlas A3 推理系列产品<br>Atlas A2 训练系列产品/Atlas A2 推理系列产品 |
| [matmul_mxfp4_basic_api_high_performance](./matmul_mxfp4_basic_api_high_performance) |  MxFP4 Matmul 基础 API 高性能样例，基于静态 Tensor 编程展示已验证的基础 API 实现路径。 | Ascend 950PR / Ascend 950DT |
| [matmul_mxfp4_high_performance](./matmul_mxfp4_high_performance) |  MxFP4 Matmul 高阶 API 性能调优样例，展示常量化 tiling 与 scale 数据搬运优化方法。 | Ascend 950PR / Ascend 950DT |
