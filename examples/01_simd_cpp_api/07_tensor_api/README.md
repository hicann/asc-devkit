# Tensor API样例介绍

## 概述

本目录收录直接包含`tensor_api/tensor.h`并使用Tensor API编程方式实现的样例，涵盖数据搬运、矩阵计算、卷积计算、寄存器矢量计算和高性能矩阵乘实践。

## 样例列表

| 目录名称 | 功能描述 | 支持的产品 |
| --- | --- | --- |
| [matmul_tensor_api](./matmul_tensor_api) | 基于Tensor API静态编程范式实现矩阵乘计算 | Ascend 950PR&950DT系列产品 |
| [copy_in_tensor_api](./copy_in_tensor_api) | 基于Tensor API实现copy搬入接口与带Bias的动态Shape矩阵乘法，展示GM到L1、L1到L0的数据搬运以及mmad矩阵乘加计算 | Ascend 950PR&950DT系列产品 |
| [copy_out_tensor_api](./copy_out_tensor_api) | 基于Tensor API实现copy搬出接口与带Bias的动态Shape矩阵乘法，展示L0C到GM、L0C到UB的数据搬运以及mmad矩阵乘加计算 | Ascend 950PR&950DT系列产品 |
| [batch_matmul_tensor_api](./batch_matmul_tensor_api) | 基于Tensor API编程方式实现带Bias的Batch Matmul计算 | Ascend 950PR&950DT系列产品 |
| [conv2d_forward_tensor_api](./conv2d_forward_tensor_api) | 基于Tensor API实现Conv2D Forward计算，支持NC1HWC0、NCHW和NHWC三种GM输入/输出格式 | Ascend 950PR&950DT系列产品 |
| [mmad_tensor_api](./mmad_tensor_api) | 基于Tensor API实现动态Shape矩阵乘法和随路量化计算 | Ascend 950PR&950DT系列产品 |
| [matmul_mxfp4_tensor_api_high_performance](./matmul_mxfp4_tensor_api_high_performance) | MxFP4 Matmul Tensor API高性能样例，展示基于静态Tensor编程的高性能实现路径 | Ascend 950PR&950DT系列产品 |
| [custom_op_tensor_api](./custom_op_tensor_api) | 基于自定义Tensor API Copy Operation实现batch输入展平和共享权重QKV投影 | Ascend 950PR&950DT系列产品 |
| [bank_conflict](./bank_conflict) | 基于Tensor API介绍bank冲突及规避方式 | Ascend 950PR&950DT系列产品 |
| [matmul_split_k](./matmul_split_k) | 基于Tensor API实现多核K轴切分矩阵乘计算 | Ascend 950PR&950DT系列产品 |
| [matmul_bias_fusion](./matmul_bias_fusion) | 基于Tensor API实现Cube和Vector的CV融合计算 | Ascend 950PR&950DT系列产品 |
| [abs_diff](./experimental/reg_vector_compute/abs_diff) | 基于Tensor API对两组float Tensor逐元素计算差的绝对值 | Ascend 950PR&950DT系列产品 |
| [add](./experimental/reg_vector_compute/add) | 基于Tensor API对两组float Tensor执行逐元素加法 | Ascend 950PR&950DT系列产品 |
| [cast](./experimental/reg_vector_compute/cast) | 基于Tensor API实现多场景逐元素数据类型转换 | Ascend 950PR&950DT系列产品 |
| [compare](./experimental/reg_vector_compute/compare) | 基于Tensor API的operator>实现多场景数据比较 | Ascend 950PR&950DT系列产品 |
| [copy_vector](./experimental/reg_vector_compute/copy_vector) | 基于Tensor API实现AIV侧GM->UB、UB->GM和UB->UB搬运及多种分形场景 | Ascend 950PR&950DT系列产品 |
| [data_reorder](./experimental/reg_vector_compute/data_reorder) | 基于Tensor API的deinterleave实现数据重排 | Ascend 950PR&950DT系列产品 |
| [fill](./experimental/reg_vector_compute/fill) | 基于Tensor API实现fill标量填充运算 | Ascend 950PR&950DT系列产品 |
| [fma](./experimental/reg_vector_compute/fma) | 基于Tensor API对三组float Tensor执行逐元素融合乘加计算 | Ascend 950PR&950DT系列产品 |
| [gather](./experimental/reg_vector_compute/gather) | 基于Tensor API按照索引Tensor收集源寄存器中的half元素 | Ascend 950PR&950DT系列产品 |
| [histograms](./experimental/reg_vector_compute/histograms) | 基于Tensor API实现高半区累计直方图统计 | Ascend 950PR&950DT系列产品 |
| [mull](./experimental/reg_vector_compute/mull) | 基于Tensor API实现逐元素乘法并输出乘积的高32位和低32位 | Ascend 950PR&950DT系列产品 |
| [reduction_compute](./experimental/reg_vector_compute/reduction_compute) | 基于Tensor API实现reduce_sum/reduce_max/reduce_min归约计算 | Ascend 950PR&950DT系列产品 |
| [reg_load_store](./experimental/reg_vector_compute/reg_load_store) | 基于Tensor API展示非对齐复制、分块最大值归约和对齐的掩码寄存器搬运 | Ascend 950PR&950DT系列产品 |
| [shift_left](./experimental/reg_vector_compute/shift_left) | 基于Tensor API按照Tensor移位量执行逐元素左移 | Ascend 950PR&950DT系列产品 |
| [trunc](./experimental/reg_vector_compute/trunc) | 基于Tensor API对浮点输入执行逐元素向零取整 | Ascend 950PR&950DT系列产品 |
