# Matmul API Examples Introduction

## Overview

This example collection introduces typical usage of different Matmul API features and provides corresponding end-to-end implementations.

## Example List
| Directory Name | Description | Supported Products |
|------------------------------------------------------------------------------------------------------| ---------------------------------------------------- | --- |
| [batch_matmul](./batch_matmul) |  Example of batch Matmul computation | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |
| [batch_matmul_bias_reuse](./batch_matmul_bias_reuse) |  Matmul example where each Batch reuses the same Bias matrix during batch Matmul computation | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |
| [batch_matmul_iterate_n_batch](./batch_matmul_iterate_n_batch) |  Example of multiple batch Matmul computations, including implementations for both synchronous and asynchronous scenarios | Atlas A3 products<br>Atlas A2 products |
| [matmul](./matmul) |  Matmul example using Matmul API | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products<br>Atlas Inference Series Products AI Core |
| [matmul_a2b2_share](./matmul_a2b2_share) |  Matmul example with A2 (L0A Buffer) and B2 (L0B Buffer) global management enabled | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |
| [matmul_async_iterate](./matmul_async_iterate) |  Matmul example in asynchronous scenario, implemented by calling Iterate and GetTensorC to output to VECIN | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |
| [matmul_async_iterate_all](./matmul_async_iterate_all) |  Matmul example in asynchronous scenario, implemented by calling IterateAll to output to GM | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |
| [matmul_callback](./matmul_callback) |  Custom usage of Matmul API template parameter MatmulCallbackFunc | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |
| [matmul_channelsplit_output](./matmul_channelsplit_output) |  Matmul example with matrix multiplication output channel split functionality | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |
| [matmul_co1_output](./matmul_co1_output) |  Matmul example with user-managed CO1 (L0C Buffer) | Atlas A3 products<br>Atlas A2 products |
| [matmul_constant_tiling](./matmul_constant_tiling) |  Matmul example with constant Tiling, reducing runtime Scalar overhead in scenarios with fixed tiling parameters | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |
| [matmul_format_column_major](./matmul_format_column_major) |  Matmul example with input and output matrices in COLUMN_MAJOR (column-major) format | Ascend 950PR&950DT products |
| [matmul_format_gemv](./matmul_format_gemv) |  Matmul example implementing General Matrix-Vector multiplication (GEMV) | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |
| [matmul_format_nd_align](./matmul_format_nd_align) |  Matmul example with N-direction alignment enabled for matrix multiplication output when input matrix N-direction is unaligned | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |
| [matmul_fp8](./matmul_fp8) |  Matmul example with A and B matrices using hifloat8, fp8_e4m3fn, fp8_e5m2 data types as input | Ascend 950PR&950DT products |
| [matmul_fused](./matmul_fused) |  Multi-core AIC and AIV fusion programming implementation, introducing the MIX mode of Matmul high-level API, where Matmul API automatically controls inter-core synchronization between AIC and AIV | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products<br>Atlas Inference Series Products AI Core |
| [matmul_fused_manual](./matmul_fused_manual) |  Multi-core AIC and AIV fusion programming implementation, introducing the pure Cube mode of Matmul high-level API, requiring manual control of inter-core synchronization between AIC and AIV through related interfaces | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products<br>Atlas Inference Series Products AI Core |
| [matmul_ibshareAB](./matmul_ibshareAB) |  Example with IBShare enabled, reusing the same A matrix or B matrix data on L1 Buffer. This example demonstrates simultaneous reuse of both A and B matrices | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products<br>Atlas Inference Series Products AI Core |
| [matmul_ibshareB](./matmul_ibshareB) |  Example with IBShare enabled, reusing the same A matrix or B matrix data on L1 Buffer. This example demonstrates B matrix only reuse | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |
| [matmul_int4](./matmul_int4) |  Matmul example with A and B matrices using int4b_t data type as input | Atlas A3 products<br>Atlas A2 products |
| [matmul_k_reorder_load](./matmul_k_reorder_load) |  Matmul example with K-axis staggered data loading enabled, reducing the probability of multi-core Global Memory access conflicts | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |
| [matmul_l0cache](./matmul_l0cache) |  Matmul example with L0 cache enabled, reducing MTE1 repeated transfers | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |
| [matmul_l2cache](./matmul_l2cache) |  Matmul example with L2 Cache partitioning enabled, improving L2 Cache utilization | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |
| [matmul_mixdualmaster](./matmul_mixdualmaster) |  Matmul example with MixDualMaster mode enabled, where AIC and AIV run code independently without message-driven dependency, used for performance improvement | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |
| [matmul_mn_double_buffer](./matmul_mn_double_buffer) |  Matmul example with M/N-axis pipeline parallelism | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |
| [matmul_multi_core_unaligned](./matmul_multi_core_unaligned) |  Multi-core unaligned partitioning, where the actual computation of multi-core tail blocks is less than the corresponding parameters in tiling | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |
| [matmul_mx](./matmul_mx) |  Matrix multiplication with quantization coefficients in MXFP4/MXFP8 data format, known as MxMatmul example | Ascend 950PR&950DT products |
| [matmul_mx_scale_cache](./matmul_mx_scale_cache) |  MxMatmul example with quantization coefficient matrix scale having multi-buffer enabled on L1 Buffer in MXFP4/MXFP8 data format | Ascend 950PR&950DT products |
| [matmul_mx_ub_tscm_nz](./matmul_mx_ub_tscm_nz) |  MxMatmul example using user-defined TSCM and VECOUT input in MXFP4/MXFP8 data format | Ascend 950PR&950DT products |
| [matmul_nbuffer33](./matmul_nbuffer33) |  Matmul example using NBuffer33 algorithm, achieving bandwidth balance between input and output transfers to improve efficiency | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |
| [matmul_partial_output](./matmul_partial_output) |  Matmul high-level API example with Partial Output feature enabled | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |
| [matmul_preload](./matmul_preload) |  Matmul example with M/N-direction preloading, which can reduce MTE2 gaps | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |
| [matmul_quant](./matmul_quant) |  Matmul example with output dequantization, supporting both scalar dequantization mode and vector dequantization mode | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |
| [matmul_sparse](./matmul_sparse) |  Matmul example for 4:2 sparse matrix multiplication (Sparse Matmul), reducing memory usage and computation during matrix multiplication | Atlas A3 products<br>Atlas A2 products |
| [matmul_splitk](./matmul_splitk) |  Matmul example in multi-core K-split scenario, partitioning input matrices along the K-axis and distributing to multiple cores for parallel processing | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |
| [matmul_splitm](./matmul_splitm) |  Matmul example in multi-core M-split scenario, partitioning input matrices along the M-axis and distributing to multiple cores for parallel processing | Ascend 950PR&950DT products |
| [matmul_triangle](./matmul_triangle) |  Matmul examples using TrianUpperMatmulPolicy (upper triangular template policy) and TrianLowerMatmulPolicy (lower triangular template policy) | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |
| [matmul_tscm](./matmul_tscm) |  Matmul example using user-defined TSCM input with data sourced from GM, allowing developers to manage L1 Buffer for efficient hardware resource utilization | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |
| [matmul_tscm_src_vecout](./matmul_tscm_src_vecout) |  Matmul example using user-defined TSCM input with data sourced from VECOUT, allowing developers to manage L1 Buffer for efficient hardware resource utilization | Ascend 950PR&950DT products |
| [matmul_unitflag](./matmul_unitflag) |  Matmul example with UnitFlag enabled, enabling parallelism between CUBE computation pipeline and FIXPIPE data output pipeline | Ascend 950PR&950DT products<br>Atlas A3 products<br>Atlas A2 products |
| [matmul_vecout](./matmul_vecout) |  Matmul example using user-defined VECOUT input, allowing developers to manage Unified Buffer for efficient hardware resource utilization | Ascend 950PR&950DT products |