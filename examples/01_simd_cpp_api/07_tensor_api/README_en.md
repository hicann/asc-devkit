# Tensor API Sample Introduction

## Overview

This directory contains samples that directly include `tensor_api/tensor.h` and use the Tensor API programming model. The samples cover data movement, matrix computation, convolution, register vector computation, and high-performance matrix multiplication practices.

## Sample List

| Directory Name | Description | Supported Products |
| --- | --- | --- |
| [matmul_tensor_api](./matmul_tensor_api) | Implements matrix multiplication using the Tensor API static programming model | Ascend 950PR&950DT products |
| [copy_in_tensor_api](./copy_in_tensor_api) | Implements copy-in interfaces and dynamic-shape matrix multiplication with Bias using the Tensor API, demonstrating GM-to-L1 and L1-to-L0 data movement and mmad computation | Ascend 950PR&950DT products |
| [copy_out_tensor_api](./copy_out_tensor_api) | Implements copy-out interfaces and dynamic-shape matrix multiplication with Bias using the Tensor API, demonstrating L0C-to-GM and L0C-to-UB data movement and mmad computation | Ascend 950PR&950DT products |
| [batch_matmul_tensor_api](./batch_matmul_tensor_api) | Implements Batch Matmul with Bias using the Tensor API programming model | Ascend 950PR&950DT products |
| [conv2d_forward_tensor_api](./conv2d_forward_tensor_api) | Implements Conv2D Forward using the Tensor API and supports NC1HWC0, NCHW, and NHWC GM input/output formats | Ascend 950PR&950DT products |
| [mmad_tensor_api](./mmad_tensor_api) | Implements dynamic-shape matrix multiplication and in-path quantization using the Tensor API | Ascend 950PR&950DT products |
| [matmul_mxfp4_tensor_api_high_performance](./matmul_mxfp4_tensor_api_high_performance) | High-performance MxFP4 Matmul Tensor API sample based on the static Tensor programming model | Ascend 950PR&950DT products |
| [custom_op_tensor_api](./custom_op_tensor_api) | Implements batched-input flattening and shared-weight QKV projection with a custom Tensor API Copy Operation | Ascend 950PR&950DT products |
| [bank_conflict](./bank_conflict) | Introduces bank conflicts and mitigation methods based on the Tensor API | Ascend 950PR&950DT products |
| [matmul_split_k](./matmul_split_k) | Implements multi-core matrix multiplication with K-axis splitting using the Tensor API | Ascend 950PR&950DT products |
| [matmul_bias_fusion](./matmul_bias_fusion) | Implements Cube-Vector (CV) fusion computation with Bias using the Tensor API | Ascend 950PR&950DT products |
| [abs_diff](./experimental/reg_vector_compute/abs_diff) | Computes the element-wise absolute difference of two float Tensors using the Tensor API | Ascend 950PR&950DT products |
| [add](./experimental/reg_vector_compute/add) | Implements element-wise addition of two float Tensors using the Tensor API | Ascend 950PR&950DT products |
| [cast](./experimental/reg_vector_compute/cast) | Implements element-wise data type conversion in multiple scenarios using the Tensor API | Ascend 950PR&950DT products |
| [compare](./experimental/reg_vector_compute/compare) | Implements multi-scenario data comparison using the Tensor API operator> | Ascend 950PR&950DT products |
| [copy_vector](./experimental/reg_vector_compute/copy_vector) | Implements GM-to-UB, UB-to-GM, and UB-to-UB data movement in multiple layout scenarios on the AIV using the Tensor API | Ascend 950PR&950DT products |
| [data_reorder](./experimental/reg_vector_compute/data_reorder) | Implements data reordering using the Tensor API deinterleave | Ascend 950PR&950DT products |
| [fill](./experimental/reg_vector_compute/fill) | Implements the fill operation in scalar-filling mode using the Tensor API | Ascend 950PR&950DT products |
| [fma](./experimental/reg_vector_compute/fma) | Implements element-wise fused multiply-add on three float Tensors using the Tensor API | Ascend 950PR&950DT products |
| [gather](./experimental/reg_vector_compute/gather) | Gathers half elements from a source register according to an index Tensor using the Tensor API | Ascend 950PR&950DT products |
| [histograms](./experimental/reg_vector_compute/histograms) | Implements cumulative histogram statistics over the upper half region using the Tensor API | Ascend 950PR&950DT products |
| [mull](./experimental/reg_vector_compute/mull) | Implements element-wise multiplication that outputs the high and low 32 bits of each product using the Tensor API | Ascend 950PR&950DT products |
| [reduction_compute](./experimental/reg_vector_compute/reduction_compute) | Implements reduce_sum/reduce_max/reduce_min reduction computations using the Tensor API | Ascend 950PR&950DT products |
| [reg_load_store](./experimental/reg_vector_compute/reg_load_store) | Demonstrates unaligned copying, block-wise maximum reduction, and aligned mask register transfers using the Tensor API | Ascend 950PR&950DT products |
| [shift_left](./experimental/reg_vector_compute/shift_left) | Implements element-wise left shift with a Tensor of shift amounts using the Tensor API | Ascend 950PR&950DT products |
| [trunc](./experimental/reg_vector_compute/trunc) | Implements element-wise truncation toward zero on floating-point inputs using the Tensor API | Ascend 950PR&950DT products |
