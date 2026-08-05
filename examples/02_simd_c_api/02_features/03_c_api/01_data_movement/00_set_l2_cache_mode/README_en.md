# L2 Cache Mode Best Practices Sample

## Overview

When MTE2 moves data from Global Memory (GM) to Unified Buffer (UB), the `l2_cache_mode` parameter of the `asc_copy_gm2ub_align` interface (this sample uses [pointer-based C programming](../../../../../../docs/zh/guide/编程指南/编程模型/AI-Core-SIMD编程/基于指针的C语言编程)) can explicitly configure the L2 Cache management strategy. This sample describes how to select the appropriate L2 Cache mode to optimize MTE2 data movement performance for **reuse data** and **streaming data** scenarios, and how to improve the L2 Cache hit rate through tiling strategies when L2 Cache is enabled.

- **Reuse Data Scenario (data needs to be read multiple times)**
  Case1: Repeat the entire block 4 times, `l2_cache_mode = asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM` — the entire block far exceeds the L2 capacity, hit rate is extremely low, demonstrating the performance bottleneck without tiling.
  Case2: Split into 4 tiles along the N direction, repeat each tile 4 times, `l2_cache_mode = asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM` — after tiling, the working set per tile fits within the L2 capacity, improving the hit rate.

- **Streaming Data Scenario (data is read only once)**
  Case3: Add + double buffer, `l2_cache_mode = asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM` (baseline).
  Case4: Add + double buffer, `l2_cache_mode = asc_load_l2_cache_mode::NOTALLOC_KEEP` bypassing L2 Cache — compare with Case3.

## Supported Products and CANN Software Versions

| Product | CANN Software Version |
|---------|----------------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |

## Directory Structure

```text
├── set_l2_cache_mode
│   ├── scripts
│   │   └── gen_data.py              // Script for generating input data and ground truth data
│   ├── CMakeLists.txt               // Build configuration file
│   ├── set_l2_cache_mode.asc        // Sample entry (kernel call + main function)
│   ├── set_l2_cache_mode.h          // Kernel implementation (DataCopyRepeat + Add)
│   ├── data_utils.h                 // Data read and write utility functions
│   ├── README.md                    // Sample documentation (Chinese)
│   └── README_en.md                 // Sample documentation (English)
```

## Sample Description

The C-API GM→UB data movement interface [asc_copy_gm2ub_align](../../../../../../docs/zh/api/SIMD-API/c_api/vector_data_move/asc_copy_gm2ub_align/asc_copy_gm2ub_align_arch_3510.md) uses the `l2_cache_mode` parameter to control how the data being moved is managed in the L2 Cache. For details on the values and their meanings, see the  enum class [asc_load_l2_cache_mode](../../../../../../docs/zh/api/SIMD-API/c_api/) documentation.

Based on the two scenarios described above, this sample designs 4 Cases for comparison and verification:

- **Data reuse scenario (Case1-2)**
  This group uses a half-type 2D matrix with shape [12288, 12288] as input. `asc_copy_gm2ub_align` moves data from GM to UB. By comparing the L2 Cache hit rate under different data movement strategies, observe the performance difference between whole-block repetition and tiled repetition. This group does not include computation logic and only performs data movement operations.

- **Streaming data scenario (Case3-4)**
  This group implements the addition of two half-type matrices (z = x + y) with shape [8192, 8192]. It uses double buffering (Ping-Pong) to pipeline data movement and vector computation. By comparing `l2_cache_mode = asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM` vs `l2_cache_mode = asc_load_l2_cache_mode::NOTALLOC_KEEP`, demonstrate the optimization effect of bypassing L2 Cache for streaming data.

## Sample Implementation

### Performance Metrics Description

The performance data in this section is collected on Ascend 950 series products. Ascend 950PR/Ascend 950DT is referred to as Ascend 950 series for short.

Different performance collection commands are used to obtain different performance metrics:

- `msopprof ./demo` — Collects AI Core instruction-level latency and ratios (MTE2/MTE3/Vector), corresponding to **Table 1**.
- `msopprof --ai-core=on --aic-metrics=L2Cache ./demo` — Collects L2 Cache read/write hit and miss counts, corresponding to **Table 2**.

**Table 1** AI Core Performance Metrics Description

| Field | Description |
|:---:|:---|
| Task Duration(μs) | Total execution time of the entire task. Operator execution time is measured by this parameter. |
| aiv_total_cycles | Total cycles for task execution. |
| aiv_mte2_time(μs) | Execution time of MTE2 type instructions (GM to AI Core data movement), in μs. |
| aiv_mte2_ratio | Ratio of MTE2 instruction cycles to total cycles. |
| aiv_mte3_time(μs) | Execution time of MTE3 type instructions (UB to GM data movement), in μs. |
| aiv_mte3_ratio | Ratio of MTE3 instruction cycles to total cycles. |
| aiv_vec_time(μs) | Execution time of vec type instructions (vector computation), in μs. |
| aiv_vec_ratio | Ratio of vec instruction cycles to total cycles. |

**Table 2** L2 Cache Performance Metrics Description

| Field | Description |
|:---:|:---|
| Task Duration(μs) | Total execution time of the entire task. |
| aiv_total_cycles | Total cycles for task execution. |
| aiv_write_cache_hit | Number of write cache hits. |
| aiv_write_cache_miss_allocate | Number of write cache misses that trigger cache reallocation. |
| aiv_read_local_l2_hit | Number of read cache hits. |
| aiv_read_local_l2_miss | Number of read cache misses. |
| aiv_read_local_l2_victim | Number of read cache misses that caused cache data eviction. |

L2 Cache hit rate = `aiv_read_local_l2_hit / (aiv_read_local_l2_hit + aiv_read_local_l2_miss + aiv_read_local_l2_victim)`

### Data Reuse Pattern Diagram

The data movement patterns between GM and UB for Case1 and Case2 are shown below.

**Case1: Repeat the entire matrix 4 times**

```text
GM Matrix: [M, N]
┌─────────────────────────────── N ───────────────────────────────┐
│                    All columns moved at once                    │
└─────────────────────────────────────────────────────────────────┘

All cores start and move the entire matrix along the same path:
Round 1: All cores read the entire matrix from GM to UB
Round 2: All cores read the entire matrix again to UB
Round 3: All cores read the entire matrix again to UB
Round 4: All cores read the entire matrix again to UB
Note: The working set in each round is the entire matrix, making it difficult for L2 Cache to retain data from the previous round.
```

**Case2: Split into 4 tiles along the N direction, repeat each tile 4 times**

```text
GM Matrix: [M, N]
┌─────── N/4 ────────┬─────── N/4 ────────┬─────── N/4 ────────┬─────── N/4 ────────┐
│       Tile 0       │       Tile 1       │       Tile 2       │       Tile 3       │
└────────────────────┴────────────────────┴────────────────────┴────────────────────┘

All cores start, move Tile 0 data 4 times consecutively, then process the next tile:
Tile 0: Round 1 reads from GM, Rounds 2-4 preferentially read from L2 Cache
Tile 1: Round 1 reads from GM, Rounds 2-4 preferentially read from L2 Cache
Tile 2: Round 1 reads from GM, Rounds 2-4 preferentially read from L2 Cache
Tile 3: Round 1 reads from GM, Rounds 2-4 preferentially read from L2 Cache
Note: The working set per tile is smaller, making it easier to retain in L2 Cache during consecutive repeated accesses.
```

### Case1: Repeat the entire matrix 4 times + `l2_cache_mode = asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM`

**Design intent**: The entire matrix (301.99MB) is larger than the L2 Cache capacity (approximately 128MB). Each full matrix movement causes old data to be evicted, resulting in a low L2 hit rate. Compare with Case2 (tiled repetition) to demonstrate the importance of controlling the working set size through tiling — even with L2 Cache enabled, if the single working set exceeds the L2 capacity, tiling is necessary to control the working set size.

**Sample configuration**:

- Repeat the entire matrix along the same path 4 times.
- Total data moved: 301.99MB x 4 = 1207.96MB.
- Tile: [64, 1024], each `asc_copy_gm2ub_align` moves 131,072B.

**L2 strategy**: `l2_cache_mode = asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM`

**Performance data**:

| Arch | Scenario | Configuration | Task Duration(μs) | aiv_total_cycles | aiv_time(μs) | aiv_read_local_l2_hit | aiv_read_local_l2_miss | aiv_read_local_l2_victim | Description | L2Cache Hit Rate |
|:---|:---|:---|:---:|:---:|:---:|:---:|:---:|:---:|:---|:---:|
| Ascend 950 series | Case1 | Tensor=[12288,12288]<br>Tile=[64,1024]<br>Block Num=64 | 690.38 | 72348218 | 689.48 | 731534 | 523177 | 7672752 | Entire matrix repeated 4 times along the same path | 8.2% |

**Optimization analysis**:
- The L2 Cache hit rate is only **8.2%**, meaning most repeated accesses still need to be read from GM.
- The entire matrix (302MB) far exceeds the L2 Cache capacity (approximately 128MB), making L2 Cache practically ineffective.
- The victim count is as high as 7,672,752, indicating that almost all cached data from the previous round has been evicted.

**Principle**:
- When moving the entire matrix in each round, the data cached in L2 from the previous round is almost entirely evicted (victim count as high as 7,672,752). In the next round, repeated accesses still need to be read from GM again.
- Each miss triggers Cache Line allocation and old data eviction. These ineffective cache management operations consume additional L2 controller management bandwidth, reducing MTE2 data movement efficiency.

**Performance optimization suggestions**:
- When the single working set far exceeds the L2 capacity, enabling L2 Cache alone cannot provide reuse benefits. Use tiling to control the working set size within the L2 capacity.

### Case2: Split into 4 tiles along N + repeat each tile 4 times + `l2_cache_mode = asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM`

**Design intent**: For data that needs to be read multiple times, enabling L2 Cache caches the data read in the first access. Subsequent repeated accesses read directly from L2, significantly reducing MTE2 data movement latency. The input matrix (301.99MB) exceeds the L2 capacity (approximately 128MB), so the entire matrix cannot be fully cached. Splitting into 4 tiles along the N direction results in each tile being approximately 75.50MB, allowing each tile to achieve L2 hit rate improvements within a certain range.

**Sample configuration**:

- Split into 4 tiles along the N direction. Repeat each tile 4 times.
- Total data moved: 301.99MB x 4 = 1207.96MB.
- Tile: [64, 1024], each `asc_copy_gm2ub_align` moves 131,072B.

**L2 strategy**: `l2_cache_mode = asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM`

**Performance data**:

| Arch | Scenario | Configuration | Task Duration(μs) | aiv_total_cycles | aiv_time(μs) | aiv_read_local_l2_hit | aiv_read_local_l2_miss | aiv_read_local_l2_victim | Description | L2Cache Hit Rate |
|:---|:---|:---|:---:|:---:|:---:|:---:|:---:|:---:|:---|:---:|
| Ascend 950 series | Case2 | Tensor=[12288,12288]<br>Tile=[64,1024]<br>Block Num=64 | 350.95 | 36233475 | 350.03 | 6668774 | 528676 | 1720984 | 4 tiles along N, each tile repeated 4 times | 74.8% |

**Optimization analysis**:
- The L2 Cache hit rate increased from **8.2%** (Case1) to **74.8%**, close to the theoretical limit of 75% (in 4 rounds of data movement, the first round always misses, and rounds 2-4 ideally hit every time).
- Task Duration decreased from 690.38μs to 350.95μs, a reduction of **49.2%**.
- The victim count decreased from 7,672,752 to 1,720,984, indicating significantly reduced cache thrashing.

**Principle**:
- The entire matrix (302MB) far exceeds the L2 capacity (128MB), so it cannot be fully cached. After splitting into 4 tiles along the N direction, each tile is approximately 75.5MB and can be effectively cached within the L2 capacity.
- When consecutively accessing the same tile, data from rounds 2-4 hits directly in L2, greatly reducing GM read operations.

**Performance optimization suggestions**:
- For repeated accesses to data exceeding the L2 capacity, first use tiling to control the single working set within the L2 capacity, then enable L2 Cache to obtain reuse benefits.

### Case3: Add with double buffer + `l2_cache_mode = asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM`

**Design intent**: The inputs x and y of the Add operator are both streaming data (each element is read only once). Double buffering is used for pipeline parallelism. With `l2_cache_mode = asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM`, MTE2 writes data to the L2 Cache while moving it to UB. However, the data is never accessed again, making the L2 write operations completely wasteful and consuming additional L2 controller management bandwidth.

**Sample configuration**:

- Matrix: [8192, 8192], half type (128MB per matrix).
- Evenly distributed across 64 AIV Cores.

**L2 strategy**: `l2_cache_mode = asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM`

**Performance data**:

| Task Duration(μs) | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| 240.01 | 239.13 | 83.14 | 0.348 | 1.815 | 0.008 | 233.869 | 0.978 | 121.44 | 0.507 |

**Optimization analysis**:
- MTE2 writes data to the L2 Cache while moving it, wasting L2 controller management bandwidth (aiv_mte2_time reaches **233.869μs**, accounting for **97.8%**).

**Principle**:
- The inputs x and y of the Add operator are both streaming data. Each element is accessed only once and is never reused.
- `l2_cache_mode = asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM` causes MTE2 to perform additional L2 Cache write operations during GM to UB data movement. These writes do not help performance and instead consume L2 controller management bandwidth and pollute the L2 Cache space.

**Performance optimization suggestions**:

- For streaming data, configure `l2_cache_mode = asc_load_l2_cache_mode::NOTALLOC_KEEP` to bypass L2 Cache writes, eliminating unnecessary cache management overhead.

### Case4: Add with double buffer + `l2_cache_mode = asc_load_l2_cache_mode::NOTALLOC_KEEP`

**Design intent**: The only difference from Case3 is that `asc_copy_gm2ub_align` is called with `l2_cache_mode = asc_load_l2_cache_mode::NOTALLOC_KEEP` instead of `l2_cache_mode = asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM`. For streaming data that is read only once, disabling L2 Cache allows MTE2 to move data directly from GM to UB, avoiding unnecessary cache write overhead and preventing streaming data from occupying L2 space.

**Key code**:

```cpp
// Case 3 (l2_cache_mode = asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM) and Case 4 (l2_cache_mode = asc_load_l2_cache_mode::NOTALLOC_KEEP)
// The only difference is the l2_cache_mode parameter value
__aicore__ inline void ProcessDoubleBufferImpl(asc_load_l2_cache_mode l2CacheMode)
{
    // ...
    asc_copy_gm2ub_align(xLocal, xGm + startElement,
        1, (uint32_t)(curLen * sizeof(half)),
        0, 0, 0, l2CacheMode, 0, 0);
    asc_copy_gm2ub_align(yLocal, yGm + startElement,
        1, (uint32_t)(curLen * sizeof(half)),
        0, 0, 0, l2CacheMode, 0, 0);
    // ...
}
```

**L2 strategy**: `l2_cache_mode = asc_load_l2_cache_mode::NOTALLOC_KEEP`

**Performance data**:

| Task Duration(μs) | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| 172.996 | 171.81 | 82.462 | 0.48 | 1.801 | 0.01 | 166.752 | 0.971 | 18.723 | 0.109 |

**Optimization analysis**:
- Task Duration decreased from 240.01μs to 172.996μs, a reduction of **27.9%**.
- MTE2 time decreased from 233.869μs to 166.752μs, a reduction of **28.7%**.
- MTE3 time decreased from 121.44μs to 18.723μs, a reduction of **84.6%**.
- aiv_vec_time remained nearly unchanged (83.14μs to 82.462μs, a change of **0.8%**), indicating that L2 Cache bypass does not affect vector computation time.

**Principle**:

- `l2_cache_mode = asc_load_l2_cache_mode::NOTALLOC_KEEP` bypasses L2 Cache writes. MTE2 moves data directly from GM to UB, eliminating unnecessary cache write operations and Cache Line allocation overhead.

**Performance optimization suggestions**:

- For large amounts of streaming data that are read only once (such as inputs to element-wise operators like Add and Mul), configure `l2_cache_mode = asc_load_l2_cache_mode::NOTALLOC_KEEP` in `asc_copy_gm2ub_align` to bypass unnecessary cache writes and improve MTE2 data movement efficiency.

## Optimization Summary

| Optimization Method | Core Principle | Applicable Scenario |
|:---|:---|:---|
| L2 Cache bypass | Avoid cache pollution, reduce overhead | Streaming access (read once) |
| Enable L2 Cache | Cache the data read in the first access using L2 Cache, subsequent repeated accesses read directly from L2 (L2 Cache bandwidth is larger than GM bandwidth) | Data that is read multiple times |
| Tiling with repeated access | Limit repeated access to a smaller data range to improve L2 Cache hit chances | Reusable data size exceeds L2 Cache capacity |

**L2 Cache Mode Decision Tree**:

```text
Is the data read multiple times?
  ├── Yes → Use l2_cache_mode = asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM
  │         Single working set > L2 capacity?
  │         └── Yes → Tile first, then use l2_cache_mode = asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM within each tile
  └── No  → Use l2_cache_mode = asc_load_l2_cache_mode::NOTALLOC_KEEP (bypass for streaming data)
```

## Build and Run

In the sample root directory, perform the following steps to build and run the sample.

- Set environment variables

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN installation directory.

- Run the sample

  ```bash
  SCENARIO_NUM=1 ASC_ARCH=dav-3510
  mkdir -p build && cd build
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=$ASC_ARCH ..
  make -j
  python3 ../scripts/gen_data.py -scenarioNum $SCENARIO_NUM
  ./demo
  ```

- Build options

  | Parameter | Description | Values | Default |
  |-----------|-------------|--------|---------|
  | `SCENARIO_NUM` | Scenario number | 1-4 | 1 |
  | `CMAKE_ASC_RUN_MODE` | Run mode | `npu`, `sim` | `npu` |
  | `CMAKE_ASC_ARCHITECTURES` | NPU hardware architecture | `dav-3510` (only this architecture is supported) | `dav-3510` |

- Performance profiling

  Use the `msopprof` tool to obtain detailed performance data:

  ```bash
  msopprof ./demo                                              # Collect MTE2/MTE3/Vector performance data. See Table 1 for field descriptions.
  msopprof --ai-core=on --aic-metrics=L2Cache ./demo           # Collect L2 Cache performance data. See Table 2 for field descriptions.
  ```

  After collection, a `OPPROF_{timestamp}_XXX`-prefixed directory is generated in the current directory.

  ```bash
  ├──dump
  ├──ArithmeticUtilization.csv
  ├──L2Cache.csv
  ├──Memory.csv
  ├──MemoryL0.csv
  ├──MemoryUB.csv
  ├──OpBasicInfo.csv
  ├──PipeUtilization.csv
  ├──ResourceConflictRatio.csv
  └──visualize_data.bin
  ```

  To view the specific performance analysis results:

  ```bash
  # View Task Duration and other data
  cat ./PROF_*/PipeUtilization.csv
  ```
