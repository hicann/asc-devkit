# Tensor API样例介绍

## 概述

本目录收录直接包含`tensor_api/tensor.h`并使用Tensor API编程方式实现的样例，涵盖数据搬运、矩阵计算、卷积计算和高性能矩阵乘实践。

## 样例列表

| 目录名称 | 功能描述 | 支持的产品 |
| --- | --- | --- |
| [matmul_tensor_api](./matmul_tensor_api) | 基于Tensor API静态编程范式实现矩阵乘计算 | Ascend 950PR/Ascend 950DT |
| [copy_in_tensor_api](./copy_in_tensor_api) | 基于Tensor API实现copy搬入接口与带Bias的动态Shape矩阵乘法，展示GM到L1、L1到L0的数据搬运以及mmad矩阵乘加计算 | Ascend 950PR/Ascend 950DT |
| [copy_out_tensor_api](./copy_out_tensor_api) | 基于Tensor API实现copy搬出接口与带Bias的动态Shape矩阵乘法，展示L0C到GM、L0C到UB的数据搬运以及mmad矩阵乘加计算 | Ascend 950PR/Ascend 950DT |
| [batch_matmul_tensor_api](./batch_matmul_tensor_api) | 基于Tensor API编程方式实现带Bias的Batch Matmul计算 | Ascend 950PR/Ascend 950DT |
| [conv2d_forward_tensor_api](./conv2d_forward_tensor_api) | 基于Tensor API实现Conv2D Forward计算，支持NC1HWC0、NCHW和NHWC三种GM输入/输出格式 | Ascend 950PR/Ascend 950DT |
| [mmad_tensor_api](./mmad_tensor_api) | 基于Tensor API实现动态Shape矩阵乘法和随路量化计算 | Ascend 950PR/Ascend 950DT |
| [matmul_mxfp4_tensor_api_high_performance](./matmul_mxfp4_tensor_api_high_performance) | MxFP4 Matmul Tensor API高性能样例，展示基于静态Tensor编程的高性能实现路径 | Ascend 950PR/Ascend 950DT |
