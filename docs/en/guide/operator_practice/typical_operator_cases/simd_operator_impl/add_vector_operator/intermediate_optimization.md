# Intermediate Performance Optimization<a name="ZH-CN_TOPIC_000000_ADD_MID"></a>

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-11-09T10:43:17.181Z pushedAt=2026-11-19T06:32:19.839Z nl=2 -->

This section describes the intermediate performance optimization methods for the Add operator, building on the basic functionality implementation.

## Performance Bottleneck Analysis

Based on the Profiling data of the entry-level code (using [8192×8192 half](../../../../../../../examples/01_simd_cpp_api/05_best_practices/00_vector_compute/add_high_performance/README_en.md#case-0-single-core-scalar-version-baseline-program) as the baseline), the core performance bottlenecks are as follows:

> **Note:** The entry-level implementation uses the float data type to simplify understanding. Starting from the intermediate performance optimization version, the half data type is used to achieve higher vector computation performance and data movement efficiency. The following Profiling data and optimization results are all based on the half type.

| Metric | Case 0 (Scalar Baseline) | Case 1 (Single-Core Vector) | Case 2 (Multi-Core Partitioning) |
|:---|:---:|:---:|:---:|
| Task Duration (μs) | 1239689.1 | 6909.6 | 306.58 |
| aiv_vec_ratio | 0 | 0.11 | 0.053 |
| aiv_scalar_ratio | 0.995 | 0.033 | 0.028 |
| aiv_mte2_ratio | 0.005 | 0.873 | 0.725 |
| aiv_mte3_ratio | 0.002 | 0.378 | 0.183 |

**Core bottleneck interpretation**:

- **Pain point 1: scalar computation bottleneck** (case 0): Scalar instructions account for 99.5% of the duration, executing element by element serially, which fails to leverage the hardware parallel capability.
- **Pain point 2: data movement bottleneck** (case 1): In the single-core vector version, MTE2 accounts for 87.3%, making data movement the primary bottleneck. The insufficient number of movement requests from a single core leaves the bandwidth underutilized.
- **Pain point 3: small movement granularity** (case 2): After multi-core partitioning, MTE2 accounts for 72.5%. Small-block movement (8 KB) does not fully utilize the bandwidth.

## Analysis and Implementation of Optimization Methods

### Optimization Strategy 1: Scalar-to-Vector Instructions

**Principle analysis**: Scalar operations process only one data element at a time and require element-by-element loops. Vector instructions (such as [AscendC::Add](../../../../../api/SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Add.md)) can process multiple data elements in a single operation, leveraging the parallel computing capability of the vector unit to achieve a performance improvement of more than 100 times.

**Code refactoring**: Replace the scalar operations that combine a `for` loop with `GetValue/SetValue` with vector APIs:

```cpp
// Scalar method (case 0)
for (uint32_t i = 0; i < curLen; i++) {
    float xVal = (float)xLocal.GetValue(i);
    float yVal = (float)yLocal.GetValue(i);
    zLocal.SetValue(i, (half)(xVal + yVal));
}

// Vector method (case 1)
AscendC::Add(zLocal, xLocal, yLocal, curLen);
```

Performance improvement: The end-to-end latency is reduced from 1239689.1 μs to 6909.6 μs, an improvement of **99.4%**.

### Optimization Strategy 2: Multi-core Partitioning + Bulk Data Movement

**Principle analysis:**
- **Multi-core parallelism**: Evenly partition the data across multiple AIV Cores to implement parallel computation. Use an even partitioning strategy (the remainder is assigned to the first few cores) to ensure load balancing.
- **Bulk data movement**: Increase `dataCopyLen` within the space allowed by the Unified Buffer (UB) to reduce the number of data movement operations, amortize the startup overhead, and better utilize the memory bandwidth.

**Code refactoring points:**

```cpp
// Even partitioning: compute the number of rows processed by each core.
uint32_t baseCoreM = totalM / splitM;
uint32_t remainderM = totalM % splitM;
if (blockIdxM < remainderM) {
    actualCoreM = baseCoreM + 1;  // Evenly assign the remainder.
}

// Increase the data movement granularity: increase dataCopyLen from 4096 to 16384 (Case 2 -> Case 3).
```

Performance improvement: From Case 2 to Case 3, the end-to-end latency decreases from 306.58 μs to 268.5 μs, a reduction of **12.5%**.

### Optimization Strategy 3: Double-Buffered Pipeline Parallelism

**Principle analysis**: The Ping-Pong double-buffering technique is adopted. While the Ping buffer performs computation, the Pong buffer performs data movement, and the two alternate to achieve pipeline parallelism between computation and data movement.

<img src="../../../../../../../examples/01_simd_cpp_api/05_best_practices/00_vector_compute/add_high_performance/figures/DoubleBuffer.png" width="50%">

**Code refactoring points**:

```cpp
// Allocate addresses for the Ping-Pong double buffers.
static constexpr uint32_t xAddrPing = 0;
static constexpr uint32_t yAddrPing = MAX_DATA_COPY_LEN * sizeof(half);
static constexpr uint32_t zAddrPing = yAddrPing + MAX_DATA_COPY_LEN * sizeof(half);
static constexpr uint32_t xAddrPong = zAddrPing + MAX_DATA_COPY_LEN * sizeof(half);
// ...

// Double-buffered pipeline: use the two buffer groups alternately.
for (uint32_t loopIdx = 0; loopIdx < totalBlocks; loopIdx++) {
    int32_t eventID = (loopIdx % 2 == 0 ? EVENT_ID0 : EVENT_ID1);
    // ... Move data and compute, and use the corresponding eventID for synchronization.
}
```

Performance improvement: Case 3→Case 4, the end-to-end latency decreases from 268.5 μs to 264.02 μs, a reduction of **1.7%**. The limited benefit of double buffering indicates that the bottleneck lies in the data movement speed itself.

## Optimization Effect Evaluation

| Metric | Case 0 (Entry Baseline) | Case 4 (Intermediate Optimization) | Improvement |
|:---|:---:|:---:|:---:|
| Task Duration (μs) | 1239689.1 | 264.02 | **4695x** |
| aiv_vec_time (μs) | 0.015 | 12.846 | - |
| aiv_scalar_time (μs) | 1233742.494 | 2.796 | **99.8%** |
| aiv_mte2_ratio | 0.005 | 0.973 | MTE2 utilization significantly improved |

**Conclusion of this stage**: After four levels of optimization—scalar-to-vector, multi-core partitioning, bulk data movement, and double buffering—the end-to-end latency dropped from 1.24 seconds to 264 μs, a 4695x improvement. The current bottleneck lies in the data movement speed itself, with MTE2 utilization already reaching 97.3%. Further optimization requires starting from L2 Cache and UB memory layout.

For details, see [Add performance tuning sample cases 1-4](../../../../../../../examples/01_simd_cpp_api/05_best_practices/00_vector_compute/add_high_performance/README_en.md#intermediate-performance-optimization).
