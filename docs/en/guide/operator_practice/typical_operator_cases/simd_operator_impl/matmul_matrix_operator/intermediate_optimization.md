# Intermediate Performance Optimization

<!-- md-trans-meta sourceCommit=12a14b50a4f8b34df0b2db2cb19ae1e957b3bde2 translatedAt=2026-11-09T10:44:40.131Z pushedAt=2026-11-19T06:32:19.849Z nl=2 -->

This section describes the performance tuning method for the Matmul basic API operator based on the C++ tensor programming paradigm. Through four progressive optimization steps (L1/L0 double buffering, fine-grained flow synchronization, multi-core parallel partitioning, and **DataCopyIn** priority transfer of cube B), it presents the complete tuning path from basic implementation to high-performance optimization.

## Optimization Path

| No. | Optimization Method | Level | Positioning |
|------|---------|------|------|
| **1** | **[L1/L0 Double-Buffering Ping-Pong Layout](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_basic_api_high_performance/README_en.md#1-l1l0-double-buffer-ping-pong-layout)** | **Intermediate** | **Pipeline skeleton** |
| **2** | **[Fine-Grained Pipeline Synchronization](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_basic_api_high_performance/README_en.md#2-fine-grained-pipeline-synchronization)** | **Intermediate** | **Pipeline skeleton** |
| **3** | **[Multi-Core Parallel Splitting](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_basic_api_high_performance/README_en.md#3-multi-core-parallel-splitting)** | **Intermediate** | **Parallel strategy** |
| **4** | **[DataCopyIn Prioritizes Transfer of Cube B](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_basic_api_high_performance/README_en.md#4-datacopyin-prioritizes-b-matrix-transfer)** | **Intermediate** | **Scheduling strategy** |
| 5 | [Large-Packet Transfer](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_basic_api_high_performance/README_en.md#5-large-packet-transfer) | Advanced | Pipeline skeleton |
| 6 | [LoadData3D Replacing LoadData2D to Reduce Instruction Queue Occupancy](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_basic_api_high_performance/README_en.md#6-loaddata3d-replacing-loaddata2d--reducing-instruction-queue-usage) | Advanced | Instruction queue optimization |
| 7 | [Constant Tiling](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_basic_api_high_performance/README_en.md#7-constant-tiling) | Advanced | Compile-time optimization |
| 8 | [UnitFlag Optimization](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_basic_api_high_performance/README_en.md#8-unitflag-optimization) | Advanced | MMAD-FIXPIPE pipeline overlap |
| 9 | [L2Cache Optimization](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_basic_api_high_performance/README_en.md#9-l2cache-optimization) | Advanced | MTE2 bandwidth squeezing |

## From Getting Started to Optimization

[Getting Started](./getting_started.md) implements the basic functionality of the Matmul operator: the A/B cubes are loaded from GM into L1 through MTE2's `DataCopy` (ND→NZ conversion), loaded into L0A/L0B through MTE1's `LoadData` (NZ→ZZ/ZN conversion), the Cube unit executes `Mmad`, and the result is written back from L0C to GM through `Fixpipe`.

The getting-started implementation has the following **performance bottlenecks**, which are also the starting point of intermediate optimization:

| Bottleneck | Problem in the Getting-Started Implementation | Corresponding Intermediate Optimization |
|------|---------------|---------------|
| **L1/L0 single buffer** | The A and B cubes each have only one buffer in L1 and L0, so the next round of data cannot be loaded in until the previous round is fully consumed | Switch to Ping-Pong double buffering to overlap data transfer and computation |
| **Serial pipeline execution** | The current implementation uses PipeBarrier and SetFlag/WaitFlag for pipeline synchronization, but does not adopt a double buffering mechanism, so the MTE2, MTE1, M, and FIX hardware units cannot fully run in parallel | Use double buffering and hardware event synchronization to run the four units in parallel, significantly improving throughput |
| **Fixed multi-core partitioning** | The core count allocation is hardcoded as `M / singleCoreM`, which only applies to fixed cube sizes and core counts | A general partitioning strategy that supports automatic allocation for any core count and cube size |
| **Cube B transfer wait** | A and B are transferred sequentially, and the late arrival of B data causes L1 buffer underutilization | Transfer B first → schedule B before A |
| Small granularity of a single MTE2 transfer instruction | DataCopyIn loads one `baseM×baseK` block at a time, resulting in many MTE2 instruction issues | Large-block transfer → load `stepKa` blocks at a time |
| High MTE1 instruction queue pressure | LoadData2D can only transfer one 16×K slice at a time, requiring multiple issues in a for loop | LoadData3D → complete the transfer with a single instruction |
| Runtime computation of tiling parameters | All parameters are computed with Scalar inside the kernel, increasing pipeline stalls | Constant tiling → determined at compile time |
| L0C write-back blocking | `Fixpipe` and Mmad run serially, and Cube waits for the write-back to complete before computing | UnitFlag → 512B-granularity MMAD-FIXPIPE overlap |
| L2Cache underutilization | In the current implementation, the cube B is read from GM every time, and repeated GM access by multiple cores puts high pressure on MTE2 bandwidth; the pure read bandwidth of L2Cache is about 3 to 4 times that of GM, but the current implementation does not leverage this feature | Partition the Cube A along the M direction so that the cube B stays in L2Cache across rounds, reducing GM access and effectively lowering MTE2 time |

## Details

For details, see the [Matmul basic API best-practice sample](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_basic_api_high_performance), which covers the four core steps of intermediate performance optimization:

- **[L1/L0 Double-Buffering Ping-Pong Layout](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_basic_api_high_performance/README_en.md#1-l1l0-double-buffer-ping-pong-layout)**: DataCopyIn (GM to L1), DataLoad (L1 to L0), and Compute form a three-level pipeline
- **[Fine-Grained Pipeline Synchronization](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_basic_api_high_performance/README_en.md#2-fine-grained-pipeline-synchronization)**: Four types of hardware events (MTE2_MTE1, MTE1_MTE2, MTE1_M, M_MTE1) precisely control the pipeline
- **[Multi-Core Parallel Splitting](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_basic_api_high_performance/README_en.md#3-multi-core-parallel-splitting)**: Uniform splitting in the M/N directions supports automatic allocation for any number of cores and cube sizes
- **[DataCopyIn Prioritizes Transfer of Cube B](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_basic_api_high_performance/README_en.md#4-datacopyin-prioritizes-b-matrix-transfer)**: B-before-A scheduling alleviates L1 buffer underload

Each optimization step includes a principle description and a data flow description, along with functional debugging (printf, DumpTensor) and performance debugging (msOpProf) methods.

> **Note:** The L2Cache partitioning performance analysis in this sample belongs to advanced extreme optimization content. For its implementation principles and performance data, see [Advanced Extreme Optimization](./advanced_extreme_optimization.md), which is not elaborated in this intermediate performance optimization document.
