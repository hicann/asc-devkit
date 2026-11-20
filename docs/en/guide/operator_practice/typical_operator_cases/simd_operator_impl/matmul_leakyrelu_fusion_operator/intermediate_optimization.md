# Intermediate Performance Optimization<a name="ZH-CN_TOPIC_000000_MATMUL_LEAKYRELU_MID"></a>

<!-- md-trans-meta sourceCommit=b6142b1cb0ae3dfa4b9063f8bd59db99b19fc426 translatedAt=2026-11-09T10:44:00.025Z pushedAt=2026-11-19T06:32:19.842Z nl=2 -->

This section describes the intermediate performance optimization methods for the Matmul fused operator, building on the basic functional implementation.

> **Note:** The basic functional implementation uses the Matmul+LeakyRelu fused operator as the example, while the intermediate and advanced extreme optimization use the Matmul+Gelu fused operator as the example. To better demonstrate the performance optimization techniques of VF fusion, this chapter uses the Matmul+Gelu fused operator as the example. Compared with LeakyRelu, GELU involves more complex computation logic and can more fully reflect the effects of various optimization techniques.

## Optimization Technique Overview

| No. | Optimization Method | Description | Positioning |
|------|---------|------|------|
| 1 | L1/L0 double-buffering ping-pong layout | MTE2 transfer and MTE1/Mmad computation run in parallel within the Cube core | Pipeline skeleton |
| 2 | Large-block transfer | Increase the transfer granularity to reduce the number of transfers | Pipeline skeleton |
| 3 | Fine-grained pipeline synchronization | Use SetFlag/WaitFlag instead of PipeBarrier to achieve finer-grained synchronization | Pipeline skeleton |
| 4 | LoadData3D instead of LoadData2D | Reduce the MTE1 instruction queue occupancy | Instruction queue optimization |
| 5 | Multi-core parallel partitioning | Partition along the M/N directions to multiple cores for parallel computation | Parallelism strategy |
| 6 | Constant tiling | Determine tiling parameters at compile time to reduce runtime Scalar computation overhead | Compile-time optimization |
| 7 | UnitFlag optimization | Fine-grained pipeline overlap between Mmad and Fixpipe | MMAD-FIXPIPE pipeline overlap |

## CV Fusion: Scenario 1 — CV Fusion-GM Relay

The intermediate performance optimization adopts the CV Fusion-GM Relay approach (supported by A2/A3/Ascend 950PR). On the AIC side, the L0C computation results are written to GM through Fixpipe. On the AIV side, data is read from GM into the Unified Buffer (UB), GELU computation is performed, and the results are written back to GM.

Data flow paths:

```
GM ──(MTE2)──> L1 ──(MTE1)──> L0A/L0B ──(Cube)──> L0C ──(Fixpipe)──> GM ──(MTE2)──> UB ──(Gelu)──> UB ──(MTE3)──> GM
```

### Matmul Optimization Techniques

| Case | Optimization Method | Description |
|------|---------|------|
| [Case 1](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_high_performance/README_en.md#case-1-single-core-tiling-optimization-single_core_tiling) | Single-core tiling optimization | Optimize base block parameters to reduce the memory-access-to-compute ratio |
| [Case 2](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_high_performance/README_en.md#case-2-multi-core-split-2x12-multi_core_split_2_12) | Multi-core splitting 2x12 | Introduce multi-core parallel computing to balance load distribution |
| [Case 3](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_high_performance/README_en.md#case-3-multi-core-split-4x6-multi_core_split_4_6) | Multi-core splitting 4x6 | Optimize the multi-core splitting strategy with 512B address alignment |
| [Case 4](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_high_performance/README_en.md#case-4-multi-core-using-mdl-template-multi_core_mdl) | Multi-core MDL template | Enable L1 multi-block caching and large-block transfer |

### Gelu Optimization Techniques

| Case | Optimization Method | Description |
|------|---------|------|
| [Case 1](../../../../../../../examples/01_simd_cpp_api/05_best_practices/02_reg_compute/gelu_high_performance/README_en.md#case-1-enable-regbase-api-and-vf-fusion) | RegBase API + VF Fusion | Register-level computation reduces intermediate Load/Store operations and improves IPC by leveraging the dual-issue capability |

### Performance Data

The following uses data from the Ascend 950PR chip as an example:
- The standalone Gelu task takes 348.868 μs, and the standalone Matmul task takes 2601.311 μs.
- After scenario 1 fusion, the total time is 2606.91 μs, which is basically the same as the standalone Matmul execution time.

This indicates that when the CV fusion approach is used, the vector computation time of the AIV is almost completely hidden by the cube computation time of the AIC, achieving efficient pipeline parallelism.

For details, see [Matmul+Gelu intermediate performance optimization](../../../../../../../examples/01_simd_cpp_api/05_best_practices/03_fusion_compute/matmul_gelu_high_performance/README_en.md).
