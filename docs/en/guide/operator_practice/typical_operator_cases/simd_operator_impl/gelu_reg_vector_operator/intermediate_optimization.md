# Intermediate Performance Optimization<a name="ZH-CN_TOPIC_000000_GELU_MID"></a>

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-10-23T13:23:15.741Z pushedAt=2026-11-03T07:05:13.382Z nl=2 -->

This section introduces the intermediate performance optimization methods for the GELU operator, building on the implementation of the entry-level functionality.

## Performance Bottleneck Analysis

Based on the profiling data of the entry-level code, the core performance bottlenecks are as follows:

| Metric | Value | Proportion |
|:---|:---|:---|
| Task Duration | 351.525μs | - |
| aiv_vec_time | 147.895μs | 42.2% |
| aiv_scalar_time | 9.513μs | 2.7% |
| aiv_mte2_time | 320.283μs | 91.3% |
| aiv_mte3_time | 303.647μs | 86.6% |

**Core bottleneck interpretation**:

- **Pain Point 1: High proportion of vector computation overhead**: Each basic API vector computation internally performs load → compute → store operations. Data must be repeatedly exchanged between the Unified Buffer (UB) and registers, and eight vector computations generate eight Load/Store overheads.
- **Pain Point 2: Low instruction-level parallelism**: Each vector instruction of the basic API requires inserting `PipeBarrier<PIPE_V>()` for synchronization, which prevents leveraging the dual-issue feature of VF fusion.

## Optimization Method: RegBase API + VF Fusion

**MemBase vs. RegBase Comparison**:

| Dimension | MemBase (Basic API) | RegBase (VF Fusion API) |
|:---|:---|:---|
| Programming APIs | Compute API (e.g., `Mul(yLocal, xLocal, xLocal, n)`) | Reg API (e.g., `Reg::Mul(yReg, xReg, xReg, mask)`) |
| Data residency | Each instruction implicitly loads/stores, and intermediate results are written back to UB | Data is explicitly loaded into registers and stays resident, with intermediate results temporarily stored in registers |
| Synchronization | `PipeBarrier<PIPE_V>()` synchronization is required between instructions | No explicit synchronization is required within VF functions; hardware automatically manages dependencies |
| Application Scenario | Simple computation, single-step or few-step operations | Multi-step fused computation, reducing UB read/write operations |
| Performance characteristics | High per-step Load/Store overhead and low IPC | Only one Load plus one Store, supports dual-issue, and achieves high IPC |

**Principle Analysis**: Within a VF function, after data is loaded from UB into registers, all intermediate computations are performed in registers. Only one LoadAlign and one StoreAlign are required, eliminating the Load/Store overhead of intermediate results. In addition, the VF dual-issue feature is supported, and the parallelism of regular computation instructions can reach 512 bytes/cycle.

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

| Metric | Entry Version | Intermediate Optimization Version | Improvement |
|:---|:---:|:---:|:---:|
| Task Duration(μs) | 351.525 | 348.868 | 0.76% |
| aiv_vec_time(μs) | 147.895 | 66.277 | **55.2%** |
| aiv_scalar_time(μs) | 9.513 | 3.03 | **68.2%** |
| IPC | - | 1.20 | - |

> This sample is MTE2 bound, with the data transfer proportion exceeding 90%. Therefore, the end-to-end gain is not obvious, but the optimization of the vector computation time is significant.

For details, see [Gelu Performance Tuning Sample Case 1](../../../../../../../examples/01_simd_cpp_api/05_best_practices/02_reg_compute/gelu_high_performance/README_en.md#case-1-enable-regbase-api-and-vf-fusion).
