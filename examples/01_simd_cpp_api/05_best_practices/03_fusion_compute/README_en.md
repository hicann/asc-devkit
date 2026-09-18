# Fusion Compute Practices Sample Introduction

## Overview

This directory provides best-practice samples for fusion compute, covering SSBuffer communication, QuantGroupMatmul grouped quantized matrix multiplication, and Cube-Vector fusion for Matmul+GELU.

## Sample List

| Directory Name | Description | Supported Products |
| ------------------------------------------------------------ | ---------------------------------------------------- | --- |
| [matmul_gelu_high_performance](./matmul_gelu_high_performance) |  This sample demonstrates a high-performance Cube-Vector fusion implementation that fuses Matmul matrix multiplication and the GELU activation function in the same AI Core for parallel execution. | Ascend 950PR/Ascend 950DT<br>Atlas A3 Training Series Products/Atlas A3 Inference Series Products<br>Atlas A2 Training Series Products/Atlas A2 Inference Series Products |
| [quant_group_matmul_high_performance](./quant_group_matmul_high_performance) |  This sample introduces the high-performance implementation of the QuantGroupMatmul operator, supports per-token quantized grouped matrix multiplication and GELU activation computation, and demonstrates performance tuning for Vector-bound scenarios in CV fusion. | Ascend 950PR/Ascend 950DT<br>Atlas A3 Training Series Products/Atlas A3 Inference Series Products<br>Atlas A2 Training Series Products/Atlas A2 Inference Series Products |
| [ssbuf_aiv_aic_comm](./ssbuf_aiv_aic_comm) | This sample implements AIV-to-AIC communication using an SSBuffer ring queue. | Ascend 950PR/Ascend 950DT |
