# SIMD and SIMT Hybrid Programming Samples

## Overview

This directory provides best-practice samples for SIMD and SIMT hybrid programming, covering flexible branch logic implementation using SIMT, memory coalescing optimization, UB-based discrete memory access optimization, and gridDim configuration optimization.

## Sample List

| Directory Name | Description | Supported Products |
| ------------------------------------------------------------ | ---------------------------------------------------- | --- |
| [simd_simt_adaptive_max_pool3d_grad](./simd_simt_adaptive_max_pool3d_grad) |  This sample uses adaptive_max_pool3d_grad as an example to introduce the advantages of hybrid programming over pure SIMD in scenarios with discrete writes. | Ascend 950PR/Ascend 950DT |
| [simd_simt_matrix_transpose](./simd_simt_matrix_transpose) |  This sample uses matrix transpose as an example to introduce memory access tuning in Ascend C SIMD and SIMT hybrid programming scenarios, demonstrating a complete optimization path that uses MTE movement and UB staging to optimize GM access patterns, fixes the Thread Block count to reduce scheduling overhead, adds UB padding to reduce bank conflicts, and uses double buffering to run pipelines in parallel. | Ascend 950PR/Ascend 950DT |
| [simd_simt_high_performance](./simd_simt_high_performance) |  This sample uses FloorMod as an example to introduce performance tuning in SIMD and SIMT hybrid programming scenarios, showing the performance differences among direct SIMT access to GM, SIMD Reg vector computation, SIMT access to UB, and adjusting the thread mapping so that adjacent threads within a warp access UB contiguously. | Ascend 950PR/Ascend 950DT |
| [simd_simt_hash_table_mte_queue](./simd_simt_hash_table_mte_queue) |  This sample uses HashTable insert-or-update key-value operations as an example to introduce MTE task queue optimization for moving value vectors in SIMD and SIMT hybrid programming scenarios. | Ascend 950PR/Ascend 950DT |
| [simd_simt_grid_dim_config](./simd_simt_grid_dim_config) |  This sample uses Gather as an example to introduce gridDim configuration and vf function call optimization strategies in Ascend C SIMD and SIMT hybrid programming scenarios. | Ascend 950PR/Ascend 950DT |
| [simd_simt_integer_fast_div](./simd_simt_integer_fast_div) |  This sample uses integer division as an example to demonstrate instruction optimization in SIMD and SIMT hybrid programming scenarios by replacing ordinary division with multiplication and shift operations for a fixed divisor. | Ascend 950PR/Ascend 950DT |
