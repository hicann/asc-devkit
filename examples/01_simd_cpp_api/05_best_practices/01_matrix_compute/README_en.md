# Matrix Compute Practices Sample Introduction

## Overview

Matrix computation optimization samples based on Matrix Compute API, introducing Matmul and MxFP4 Matmul high-performance practices in high-level API and basic API scenarios through `<<<>>>` direct call mode.

## Sample List

| Directory Name | Function Description | Supported Products |
| --- | --- | --- |
| [matmul_basic_api_high_performance](./matmul_basic_api_high_performance) |  Matmul basic API best practices sample, based on static Tensor programming demonstrating basic API high-performance implementation details. | Ascend 950PR/Ascend 950DT<br>Atlas A3 Training Series Products/Atlas A3 Inference Series Products<br>Atlas A2 Training Series Products/Atlas A2 Inference Series Products |
| [matmul_high_performance](./matmul_high_performance) |  Matmul high-level API progressive performance optimization sample, demonstrating multi-core splitting, MDL, L1/L2 Cache, constant tiling, UnitFlag, and other optimization methods. | Ascend 950PR/Ascend 950DT<br>Atlas A3 Training Series Products/Atlas A3 Inference Series Products<br>Atlas A2 Training Series Products/Atlas A2 Inference Series Products |
| [matmul_mxfp4_basic_api_high_performance](./matmul_mxfp4_basic_api_high_performance) |  MxFP4 Matmul basic API high-performance sample, based on static Tensor programming demonstrating verified basic API implementation paths. | Ascend 950PR/Ascend 950DT |
| [matmul_mxfp4_high_performance](./matmul_mxfp4_high_performance) |  MxFP4 Matmul high-level API performance tuning sample, demonstrating constant tiling and scale data transfer optimization methods. | Ascend 950PR/Ascend 950DT |
