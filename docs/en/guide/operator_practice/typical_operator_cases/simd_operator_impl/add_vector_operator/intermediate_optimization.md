# Intermediate Performance Optimization<a name="ZH-CN_TOPIC_000000_ADD_MID"></a>

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-10-23T13:20:21.465Z pushedAt=2026-11-03T07:05:13.380Z nl=2 -->

Based on the implementation of the entry-level features, this section describes the intermediate performance optimization methods for the Add operator.

## Performance Bottleneck Analysis

Based on the profiling data of the entry-level code (using [8192×8192 half](../../../../../../../examples/01_simd_cpp_api/05_best_practices/00_vector_compute/add_high_performance/README_en.md#case-0-single-core-scalar-version-baseline-program) as the baseline), the core performance bottlenecks are as follows:

> **Note:** The entry-level implementation uses the float data type to simplify understanding. Starting from the intermediate performance optimization version, the half data type is used to achieve higher vector computation performance and data transfer efficiency. The following profiling data and optimization results are all based on half-type tests.

| Metric | Case 0 (Scalar Baseline) | Case 1 (Single-Core Vector) | Case 2 (Multi-Core Partitioning) |
|:---|:---:|:---:|:---:|
| Task Duration (μs) | 1239689.1 | 6909.6 | 306.58 |
| aiv_vec_ratio | 0 | 0.11 | 0.053 |
| aiv_scalar_ratio | 0.995 | 0.033 | 0.028 |
| aiv_mte2_ratio | 0.005 | 0.873 | 0.725 |
| aiv_mte3_ratio | 0.002 | 0.378 | 0.183 |

**Core Bottleneck Interpretation:**

- **Pain Point 1: Scalar Computation Bottleneck** (Case 0): Scalar instructions account for 99.5% of the time, executing element by element serially, which fails to leverage the hardware parallel capability.
- **Pain Point 2: Data Transfer Bottleneck** (Case 1): In the single-core vector version, MTE2 accounts for 87.3%, making data transfer the primary bottleneck. The insufficient number of transfer requests from a single core results in underutilized bandwidth.
- **Pain Point 3: Small Transfer Granularity** (Case 2): After multi-core partitioning, MTE2 accounts for 72.5%. Small-block transfer (8KB) does not fully utilize the bandwidth.

## Optimization Method Analysis and Implementation

### Optimization Strategy 1: Scalar to Vector Instructions

**Principle analysis**: Scalar operations process only one data element at a time and require element-by-element loops. Vector instructions (such as [AscendC::Add](../../../../../api/SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Add.md)) can process multiple data elements in a single operation. By leveraging the parallel computing capability of the vector unit, they can deliver a performance improvement of more than 100 times.

**Code refactoring points**: Replace the scalar operations that combine a `for` loop with `GetValue/SetValue` with vector APIs:

```cpp
// Scalar method (Case 0)
for (uint32_t i = 0; i < curLen; i++) {
    float xVal = (float)xLocal.GetValue(i);
    float yVal = (float)yLocal.GetValue(i);
    zLocal.SetValue(i, (half)(xVal + yVal));
}

// Vector method (Case 1)
AscendC::Add(zLocal, xLocal, yLocal, curLen);
```

Performance improvement: The end-to-end overhead is reduced from 1239689.1 μs to 6909.6 μs, an improvement of **99.4%**.

### Optimization Strategy 2: Multi-core Uniform Partitioning + Bulk Data Transfer

**Principle analysis:**
- **Multi-core parallelism:** Evenly partition the data across multiple AIV Cores to implement parallel computation. Use a uniform partitioning strategy (the remainder is assigned to the first few cores) to ensure load balancing.
- **Bulk data transfer:** Increase `dataCopyLen` within the space allowed by the Unified Buffer (UB) to reduce the number of transfers, amortize the startup overhead, and better utilize the memory bandwidth.

**Code refactoring points:**

```cpp
// Uniform partitioning: calculate the number of rows processed by each core
uint32_t baseCoreM = totalM / splitM;
uint32_t remainderM = totalM % splitM;
if (blockIdxM < remainderM) {
    actualCoreM = baseCoreM + 1;  // Evenly distribute the remainder
}

// Increase the transfer granularity: increase dataCopyLen from 4096 to 16384 (Case 2 to Case 3)
```

Performance improvement: From Case 2 to Case 3, the end-to-end overhead decreases from 306.58 μs to 268.5 μs, a reduction of **12.5%**.

### Optimization Strategy 3: Dual-Buffer Pipelined Parallelism

**Principle analysis**: The Ping-Pong double-buffering technique is adopted. While the Ping buffer performs computation, the Pong buffer performs data transfer, and they alternate to achieve pipelined parallelism between computation and transfer.

<img src="../../../../../../../examples/01_simd_cpp_api/05_best_practices/00_vector_compute/add_high_performance/figures/DoubleBuffer.png" width="50%">

**Code refactoring points**:

```cpp
// Allocate addresses for the Ping-Pong double buffers
static constexpr uint32_t xAddrPing = 0;
static constexpr uint32_t yAddrPing = MAX_DATA_COPY_LEN * sizeof(half);
static constexpr uint32_t zAddrPing = yAddrPing + MAX_DATA_COPY_LEN * sizeof(half);
static constexpr uint32_t xAddrPong = zAddrPing + MAX_DATA_COPY_LEN * sizeof(half);
// ...

// Dual-buffer pipeline: use the two buffer groups alternately
for (uint32_t loopIdx = 0; loopIdx < totalBlocks; loopIdx++) {
    int32_t eventID = (loopIdx % 2 == 0 ? EVENT_ID0 : EVENT_ID1);
    // ... Perform data transfer and computation, and synchronize with the corresponding eventID
}
```

Performance improvement: Case 3 to Case 4, the end-to-end overhead decreases from 268.5μs to 264.02μs, a reduction of **1.7%**. The limited benefit of double buffering indicates that the bottleneck lies in the transfer speed itself.

## Optimization Effect Evaluation

| Metric | Case 0 (entry-level baseline) | Case 4 (intermediate optimized version) | Improvement |
|:---|:---:|:---:|:---:|
| Task Duration(μs) | 1239689.1 | 264.02 | **4695x** |
| aiv_vec_time(μs) | 0.015 | 12.846 | - |
| aiv_scalar_time(μs) | 1233742.494 | 2.796 | **99.8%** |
| aiv_mte2_ratio | 0.005 | 0.973 | MTE2 utilization significantly improved |

**Conclusion of this stage**: After four levels of optimization, namely scalar-to-vector, multi-core partitioning, bulk data transfer, and double buffering, the end-to-end performance improved from 1.24 seconds to 264 μs, a 4695x improvement. The current bottleneck lies in the transfer speed itself, and the MTE2 utilization has reached 97.3%. Further optimization needs to start from the L2 Cache and UB memory layout.

For details, see [Add performance tuning example Case 1-4](../../../../../../../examples/01_simd_cpp_api/05_best_practices/00_vector_compute/add_high_performance/README_en.md#intermediate-performance-optimization).
