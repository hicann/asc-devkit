# Advanced Performance Optimization<a name="ZH-CN_TOPIC_000000_ADD_ADV"></a>

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-10-23T13:19:12.090Z pushedAt=2026-11-03T07:05:13.378Z nl=2 -->

This section describes the advanced extreme optimization methods for the Add operator, building on the intermediate performance optimization.

## Residual Bottleneck Analysis

After intermediate optimization (Case 4), the end-to-end latency is 264.02 μs, the MTE2 utilization reaches 97.3%, and data transfer and computation have achieved pipelined parallelism. The residual bottlenecks are mainly as follows:

- **Bottleneck 1: L2 Cache pollution**: The Add input data is read only once (streaming access), but it passes through the L2 Cache by default, which adds unnecessary cache overhead and transfer latency.
- **Bottleneck 2: Unified Buffer (UB) Bank Conflict**: Vector instructions simultaneously read and write data within the same Bank Group, causing read/write conflicts and read-read/write-write conflicts, which serialize memory access and reduce the efficiency of vector instructions.

## Extreme Optimization

### Extreme Optimization A: L2 Cache Bypass

**Principle**: L2 Cache is the cache layer between the AI Core and HBM, suitable for data that needs to be accessed repeatedly. For streaming access scenarios such as the Add operator (where input data is read only once), passing data through L2 Cache actually occupies cache space (cache pollution) and increases transfer latency. Use [SetL2CacheHint](../../../../../api/SIMD-API/basic_api/data_structures/GlobalTensor/SetL2CacheHint.md) to set `CACHE_MODE_DISABLE`, so that data bypasses L2 Cache and is loaded directly from GM into UB.

**Optimization solution**:

```cpp
// Set L2 Cache bypass: disable L2 Cache for streaming access data.
xGm.SetL2CacheHint(AscendC::CacheMode::CACHE_MODE_DISABLE);
yGm.SetL2CacheHint(AscendC::CacheMode::CACHE_MODE_DISABLE);
```

Performance improvement: Case 4 to Case 5, the end-to-end latency decreased from 264.02 μs to 187.68 μs, a reduction of **28.9%**; the MTE2 latency decreased from 250.528 μs to 175.997 μs, a reduction of **29.8%**.

### Extreme Optimization B: Eliminating UB Bank Conflicts

**Principle**: For Atlas A2/A3 products, the UB size is 192 KB, containing 16 bank groups, each of which contains 3 banks. A vector instruction reads 256 B of data in one beat (that is, reads 8 blocks simultaneously). When the start addresses of xPing and yPing fall in the same bank group, read/write conflicts and read-read/write-write conflicts occur, causing memory access serialization.

**Optimization**: By adjusting `dataCopyLen` (16384→16256), the start addresses of xPing and yPing are offset by 256 B, ensuring that the data accessed by a vec instruction in one beat is distributed across different bank groups, effectively eliminating bank conflicts.

```cpp
// Optimized address layout (avoid bank conflicts)
static constexpr uint32_t xAddrPingBC = 0;
static constexpr uint32_t yAddrPingBC = BANK_CONFLICT_DATA_COPY_LEN * sizeof(half);  // Offset by 256 B. Note: sizeof(half)=2 B; for the float scenario, adjust to sizeof(float).
static constexpr uint32_t xAddrPongBC = MAX_DATA_COPY_LEN * sizeof(half) * 2;
// ...
```

Performance improvement: Case 5→Case 6, the vector instruction latency decreased from 12.846 μs to 6.954 μs, a reduction of **45.9%**; the end-to-end latency decreased from 187.68 μs to 184.52 μs, a reduction of **1.7%**.

## Ultimate Performance Achieved

| Metric | Case 0 (Baseline) | Case 4 (Intermediate Optimization) | Case 6 (Advanced Optimization) |
|:---|:---:|:---:|:---:|
| Task Duration (μs) | 1239689.1 | 264.02 | **184.52** |
| aiv_vec_time (μs) | 0.015 | 12.846 | **6.954** |
| aiv_mte2_ratio | 0.005 | 0.973 | **0.961** |
| End-to-end improvement | 1x | 4695x | **6711x** |

**Key verification metrics**:
- The end-to-end latency dropped from 1.24 seconds in the baseline version to **184.52μs**, a **6711x** improvement
- The vector instruction latency dropped from 12.846μs to **6.954μs**, approaching the theoretical vector latency of 5.904μs for 48 cores in the Atlas A2 training products
- The mixed read/write bandwidth of Case 6 reached **2.3456 TB/s**, approaching the hardware bandwidth limit

**Final conclusion**: After three levels of optimization (baseline → intermediate → advanced), the Add operator achieved a 6711x end-to-end performance improvement. The current bottleneck lies mainly in the transfer bandwidth itself, while vector computation has approached the theoretical limit. For Ascend 950 products, the Bank arrangement differs, so Bank Conflict optimization is unnecessary, and Case 5 is the optimal version.

For details, see [Add Performance Tuning Sample Case 5-6](../../../../../../../examples/01_simd_cpp_api/05_best_practices/00_vector_compute/add_high_performance/README_en.md#advanced-performance-refinement).
