# Intermediate Performance Optimization<a name="ZH-CN_TOPIC_000000_GELU_MID"></a>

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-11-09T10:44:02.911Z pushedAt=2026-11-19T06:32:19.843Z nl=2 -->

This section describes the intermediate performance optimization methods for the GELU operator, building on the basic functionality implementation.

## Performance Bottleneck Analysis

Based on the Profiling data of the entry-level code, the core performance bottlenecks are as follows:

| Metric | Value | Proportion |
|:---|:---|:---|
| Task Duration | 351.525 μs | - |
| aiv_vec_time | 147.895 μs | 42.2% |
| aiv_scalar_time | 9.513 μs | 2.7% |
| aiv_mte2_time | 320.283 μs | 91.3% |
| aiv_mte3_time | 303.647 μs | 86.6% |

**Core bottleneck interpretation**:

- **Pain point 1: high proportion of vector computation overhead**: Each basic API vector computation internally performs load → compute → store operations, requiring data to be repeatedly exchanged between the Unified Buffer (UB) and registers. Eight vector computations generate eight Load/Store overheads.
- **Pain point 2: low instruction-level parallelism**: Each basic API vector instruction requires inserting `PipeBarrier<PIPE_V>()` for synchronization, which prevents leveraging the dual-issue feature of VF fusion.

## Optimization Method: RegBase API + VF Fusion

**MemBase vs RegBase comparison**:

| Dimension | MemBase (Basic API) | RegBase (VF Fusion API) |
|:---|:---|:---|
| Programming API | Compute API (such as `Mul(yLocal, xLocal, xLocal, n)`) | Reg API (such as `Reg::Mul(yReg, xReg, xReg, mask)`) |
| Data residency | Each instruction implicitly loads/stores, and intermediate results are written back to UB | Data is explicitly loaded into registers and stays resident, with intermediate results temporarily stored in registers |
| Synchronization | `PipeBarrier<PIPE_V>()` synchronization is required between instructions | No explicit synchronization is required within VF functions; hardware automatically manages dependencies |
| Applicable scenarios | Simple computation, single-step or few-step operations | Multi-step fused computation, reducing UB read/write operations |
| Performance characteristics | High per-step Load/Store overhead and low IPC | Only one Load and one Store, supports dual-issue, high IPC |

**Principle analysis**: After data is loaded from UB into registers within a VF function, all intermediate computations are completed in registers, requiring only one **LoadAlign** and one **StoreAlign**, which eliminates the Load/Store overhead of intermediate results. At the same time, it supports the VF dual-issue feature, and the parallelism of conventional computation instructions can reach 512 bytes/cycle.

```
Basic API mode:                    VF fusion mode:
Load → Mul → Store              LoadAlign → Mul
Load → Mul → Store                   → Mul
Load → Muls → Store                   → Muls
Load → Add → Store                    → Add
Load → Muls → Store                   → Muls
Load → Exp → Store                    → Exp
Load → Adds → Store                   → Adds
Load → Div → Store                    → Div → StoreAlign
(8 Load/Store operations)          (1 Load/1 Store)
```

## Optimization Results

| Metric | Basic Version | Intermediate Optimization Version | Improvement |
|:---|:---:|:---:|:---:|
| Task Duration (μs) | 351.525 | 348.868 | 0.76% |
| aiv_vec_time (μs) | 147.895 | 66.277 | **55.2%** |
| aiv_scalar_time (μs) | 9.513 | 3.03 | **68.2%** |
| IPC | - | 1.20 | - |

> This sample is MTE2 bound, with the data transfer proportion exceeding 90%. Therefore, the end-to-end gain is not obvious, but the optimization effect on vector computation time is significant.

For details, see the [Gelu performance tuning sample case 1](../../../../../../../examples/01_simd_cpp_api/05_best_practices/02_reg_compute/gelu_high_performance/README_en.md#case-1-enable-regbase-api-and-vf-fusion).
