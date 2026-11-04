# Advanced Extreme Optimization

<!-- md-trans-meta sourceCommit=dd293a1c6b873d149bf289bb2b8098dadf9f5cbd translatedAt=2026-10-23T13:25:55.328Z pushedAt=2026-11-03T07:05:13.386Z nl=2 -->

This section builds on the intermediate performance optimization described above and, based on the C++ Tensor programming paradigm, further introduces advanced optimization methods for the Matmul basic API operator. Through five advanced optimization steps (bulk data transfer, replacing LoadData2D with LoadData3D, constant Tiling, UnitFlag optimization, and L2Cache optimization), it deeply explores the hardware performance limits. 

## Optimization Path

| No. | Optimization Method | Level | Positioning |
|------|---------|------|------|
| 1 | [L1/L0 Double-Buffering Ping-Pong Layout](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_basic_api_high_performance/README_en.md#1-l1l0-double-buffer-ping-pong-layout) | Intermediate | Pipeline skeleton |
| 2 | [Fine-Grained Flow Synchronization](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_basic_api_high_performance/README_en.md#2-fine-grained-pipeline-synchronization) | Intermediate | Pipeline skeleton |
| 3 | [Multi-Core Parallelism Partitioning](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_basic_api_high_performance/README_en.md#3-multi-core-parallel-splitting) | Intermediate | Parallelism strategy |
| 4 | [DataCopyIn Priority Transfer of Cube B](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_basic_api_high_performance/README_en.md#4-datacopyin-prioritizes-b-matrix-transfer) | Intermediate | Scheduling strategy |
| **5** | **[Bulk Data Transfer](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_basic_api_high_performance/README_en.md#5-large-packet-transfer)** | **Advanced** | **Pipeline skeleton** |
| **6** | **[LoadData3D Replacing LoadData2D to Reduce Instruction Queue Occupancy](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_basic_api_high_performance/README_en.md#6-loaddata3d-replacing-loaddata2d--reducing-instruction-queue-usage)** | **Advanced** | **Instruction queue optimization** |
| **7** | **[Constant Tiling](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_basic_api_high_performance/README_en.md#7-constant-tiling)** | **Advanced** | **Compile-time optimization** |
| **8** | **[UnitFlag Optimization](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_basic_api_high_performance/README_en.md#8-unitflag-optimization)** | **Advanced** | **MMAD-FIXPIPE pipeline overlap** |
| **9** | **[L2Cache Optimization](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_basic_api_high_performance/README_en.md#9-l2cache-optimization)** | **Advanced** | **MTE2 bandwidth squeezing** |

## From Intermediate to Advanced

[Intermediate performance optimization](./intermediate_optimization.md) builds the basic pipeline skeleton and parallel computing framework through four steps: L1/L0 double buffering, fine-grained flow synchronization, multi-core parallelism partitioning, and priority transfer of cube B via DataCopyIn. It implements pipeline parallelism across the four hardware units MTE2, MTE1, Cube, and Fixpipe.

On this basis, the following **performance bottlenecks** still have room for further optimization, which is also the entry point for advanced optimization:

| Bottleneck | Remaining issue after intermediate optimization | Corresponding advanced optimization |
|------|---------------------|---------------|
| **Small MTE2 transfer granularity** | Although a three-level pipeline has been established, each DataCopyIn transfers only one `baseM×baseK` block, resulting in a high number of MTE2 instruction issues | Bulk data transfer → transfer `stepKa` blocks at a time to reduce the number of issues |
| **High MTE1 instruction queue pressure** | LoadData2D must issue multiple instructions in a loop at the `CUBE_BLOCK` (16B) granularity. The MTE1 queue depth is only 32, making it prone to becoming a bottleneck | LoadData3D → a single instruction completes the transfer of the entire base slice |
| **Scalar runtime overhead** | Tiling parameters are dynamically computed using Scalar within the kernel, increasing pipeline stalls | Constant Tiling → determined at compile time via template parameters, eliminating Scalar overhead |
| **Serial Cube/Fixpipe execution** | Mmad and Fixpipe are synchronized at the instruction level. Cube must wait for all results to be written back before starting the next round of computation | UnitFlag → MMAD-FIXPIPE pipeline overlap at 512B granularity |
| **MTE2 bandwidth bottleneck** | After multi-core parallelism in intermediate optimization, cube B is repeatedly read from GM by multiple cores, making MTE2 a performance bottleneck | L2Cache optimization → cube B resides in L2Cache across rounds, reducing GM access |

## Details

For details, see [Matmul Basic API Best Practices Sample](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_basic_api_high_performance), where advanced extreme polishing covers five core steps:

- 5. **[Bulk Data Transfer](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_basic_api_high_performance/README_en.md#5-large-packet-transfer)**: Packs multiple basic blocks into a single DataCopyIn through the `stepKa`/`stepKb` parameters, reducing the number of MTE2 instruction issues.
- 6. **[Replacing LoadData2D with LoadData3D](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_basic_api_high_performance/README_en.md#6-loaddata3d-replacing-loaddata2d--reducing-instruction-queue-usage)**: Uses a single `LoadData3DParamsV2` instruction to replace the multiple loop issues of LoadData2D, reducing MTE1 instruction queue occupancy.
- 7. **[Constant Tiling](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_basic_api_high_performance/README_en.md#7-constant-tiling)**: All Tiling parameters are determined at compile time through template parameters, eliminating the runtime Scalar dynamic computation overhead.
- 8. **[UnitFlag Optimization](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_basic_api_high_performance/README_en.md#8-unitflag-optimization)**: Enables UnitFlag to implement MMAD-FIXPIPE 512B granularity pipeline parallelism, overlapping Cube computation with result write-back.
- 9. **[L2Cache Optimization](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_basic_api_high_performance/README_en.md#9-l2cache-optimization)**: Uses M-axis round-robin scheduling to keep cube B resident in L2Cache across rounds, alleviating MTE2 bandwidth pressure.

Each optimization step includes a principle description and a data flow description, along with methods for functional debugging (printf, DumpTensor) and performance debugging (msOpProf).

> **Note:** For intermediate optimization, see [Intermediate Performance Optimization](./intermediate_optimization.md). The optimization steps 5-9 above belong to the advanced extreme polishing category and are recommended to be applied incrementally after intermediate optimization is completed. For performance data and theoretical analysis, see the [Performance Data Analysis](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_basic_api_high_performance/README_en.md#performance-data-analysis) and [Theoretical Performance Analysis](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_basic_api_high_performance/README_en.md#theoretical-performance-analysis) sections in the README.
