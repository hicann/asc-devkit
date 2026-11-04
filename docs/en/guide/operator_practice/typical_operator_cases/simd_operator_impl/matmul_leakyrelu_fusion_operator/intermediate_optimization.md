# Intermediate Performance Optimization<a name="ZH-CN_TOPIC_000000_MATMUL_LEAKYRELU_MID"></a>

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-10-23T13:24:59.436Z pushedAt=2026-11-03T07:05:13.384Z nl=2 -->

This section describes the intermediate performance optimization techniques for the Matmul fused operator, building on the basic function implementation.

> **Note:** The basic function implementation uses the Matmul+LeakyRelu fused operator as the example, while the intermediate and advanced extreme tuning use the Matmul+Gelu fused operator as the example. To better demonstrate the performance optimization techniques of VF fusion, this chapter uses the Matmul+Gelu fused operator as the example. Compared with LeakyRelu, GELU has more complex computation logic, which can more fully reflect the effects of various optimization techniques.

## Optimization Technique Overview

| ID | Optimization Technique | Description | Positioning |
|------|---------|------|------|
| 1 | L1/L0 double-buffer Ping-Pong layout | MTE2 transfer and MTE1/Mmad computation run in parallel within the Cube core | Pipeline skeleton |
| 2 | Large-packet transfer | Increase the transfer granularity to reduce the number of transfers | Pipeline skeleton |
| 3 | Fine-grained pipeline synchronization | SetFlag/WaitFlag replaces PipeBarrier to implement finer-grained synchronization | Pipeline skeleton |
| 4 | LoadData3D replaces LoadData2D | Reduce the MTE1 instruction queue occupancy | Instruction queue optimization |
| 5 | Multi-core parallel partitioning | Partition along the M/N dimensions to multiple cores for parallel computation | Parallel strategy |
| 6 | Constant tiling | Tiling parameters are determined at compile time, reducing runtime Scalar computation overhead | Compile-time optimization |
| 7 | UnitFlag optimization | Fine-grained pipeline overlap between Mmad and Fixpipe | MMAD-FIXPIPE pipeline overlap |

## CV Fusion: Scenario 1 — CV Fusion-GM Relay

The intermediate performance optimization uses the CV fusion-GM relay approach (supported by A2, A3, and Ascend 950PR). On the AIC side, the L0C computation results are written to GM through Fixpipe. On the AIV side, data is read from GM into the Unified Buffer (UB), GELU computation is performed, and the results are written back to GM.

Data flow path:
```
GM ──(MTE2)──> L1 ──(MTE1)──> L0A/L0B ──(Cube)──> L0C ──(Fixpipe)──> GM ──(MTE2)──> UB ──(Gelu)──> UB ──(MTE3)──> GM
```

### Matmul Optimization Techniques

| Case | Optimization Technique | Description |
|------|---------|------|
| [Case 1](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_high_performance/README_en.md#case-1-single-core-tiling-optimization-single_core_tiling) | Single-core tiling optimization | Optimize the base block parameters to reduce the memory-access-to-compute ratio. |
| [Case 2](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_high_performance/README_en.md#case-2-multi-core-split-2x12-multi_core_split_2_12) | Multi-core partitioning 2x12 | Introduce multi-core parallel computing to balance the workload distribution. |
| [Case 3](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_high_performance/README_en.md#case-3-multi-core-split-4x6-multi_core_split_4_6) | Multi-core partitioning 4x6 | Optimize the multi-core partitioning strategy with 512B address alignment. |
| [Case 4](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_high_performance/README_en.md#case-4-multi-core-using-mdl-template-multi_core_mdl) | Multi-core using MDL template | Enable L1 multi-block caching for large-packet data transfer. |

### Gelu Optimization Techniques

| Case | Optimization Technique | Description |
|------|---------|------|
| [Case 1](../../../../../../../examples/01_simd_cpp_api/05_best_practices/02_reg_compute/gelu_high_performance/README_en.md#case-1-enable-regbase-api-and-vf-fusion) | RegBase API + VF fusion | Register-level computation reduces intermediate Load/Store and improves IPC by leveraging the dual-issue feature |

### Performance Data

The following uses the Ascend 950PR chip as an example:
- A standalone Gelu task takes 348.868 μs, and a standalone Matmul task takes 2601.311 μs.
- After fusion in Scenario 1, the total time is 2606.91 μs, which is basically the same as the standalone Matmul execution time.

This indicates that when the CV fusion approach is used, the Vector computation time of the AIV is almost completely covered by the Cube computation time of the AIC, achieving efficient pipeline parallelism.

For details, see [Matmul+Gelu Intermediate Performance Optimization](../../../../../../../examples/01_simd_cpp_api/05_best_practices/03_fusion_compute/matmul_gelu_high_performance/README_en.md).
