# Histogram Atomic Operation Performance Tuning Sample

## Overview

A histogram is a statistical method used to count the frequency of each value in input data. In a histogram, each value corresponds to a bin, and each bin maintains a counter that is incremented when the value appears in the input. This sample uses Histogram (frequency counting) as a vehicle to demonstrate atomic operation performance optimization methods.

<img src="figures/introduction.png" alt="intro" style="width: 60%; height: auto;">

## Supported Products

- Ascend 950PR/Ascend 950DT

## Supported CANN Software Version

- \>= CANN 9.1.0

## Directory Structure

```text
├── atomic_histogram
│   ├── figures                // Image resources for README
│   ├── CMakeLists.txt         // Sample build script
│   ├── histogram.asc          // Ascend C SIMT kernel implementation & Host invocation
│   ├── README.md
│   └── README_en.md
```

## Sample Description

This sample counts the input byte stream, tallying the frequency of each byte value (0-255) and outputting count results for 256 bins.

- Formula:

  ```text
  histogram[input[i]] += 1,  i = 0, 1, ..., byte_count - 1
  ```

  - input is the input byte stream, value range 0-255
  - histogram is the histogram, each element corresponds to the occurrence count of that byte value
- Sample Specifications:

  <table>
  <tr><td align="center">Sample Type (OpType)</td><td colspan="4" align="center">Atomic Optimization</td></tr>
  <tr><td rowspan="2" align="center">Sample Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">input</td><td align="center">[262144]</td><td align="center">uint8</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Sample Output</td><td align="center">histogram</td><td align="center">[256]</td><td align="center">uint32</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Name</td><td colspan="4" align="center">histogram_gm_global / histogram_ub_block_local</td></tr>
  </table>

## Sample Implementation

### Case Implementation Overview

This sample uses 2 Cases to form an optimization comparison. The kernel and count location for each Case are shown in the table below.

| Case | Kernel | Count Location |
|---|---|---|
| 0 | `histogram_gm_global` | GM |
| 1 | `histogram_ub_block_local` | UB |

Both Cases use the same number of thread blocks and threads, with identical input read method and total atomic update count; only the storage hierarchy of the atomic target differs.

### Performance Metric

| Metric | Description |
| --- | --- |
| Task Duration (μs) | Total Task time, including scheduling to the accelerator, execution time on the accelerator, and response completion time |

---

### Case 0: GM Global Histogram Counting

**Sample Goal**: All threads directly perform atomic accumulation on the global histogram in GM, completing the frequency counting of the input byte stream.

**Scenario Configuration**:

| Case | Count Location | Data Type | Scale (Block×Thread) |
|:---:|:---:|:---:|:---:|
| 0 | GM | uint32_t | 64×2048 |

**Core Implementation**: Launches 64 thread blocks, each with 2048 threads. Each thread traverses input bytes with a fixed stride across the grid, incrementing the corresponding bin of the GM histogram via [asc_atomic_add()](../../../../../docs/zh/api/SIMT-API/atomic_operations/asc_atomic_add.md). All 262144 atomic updates are concentrated on 256 GM addresses.

In Case 0, each thread directly performs atomic accumulation on the global histogram in GM. The execution flow is shown in the figure below.

<img src="figures/case0_gm_global.png" alt="intro" style="width: 60%; height: auto;">

The corresponding kernel implementation is as follows.

```cpp
__global__ __launch_bounds__(THREADS_PER_BLOCK) void histogram_gm_global(
    uint32_t* histogram, const uint8_t* input, uint32_t byte_count)
{
    uint32_t index = blockIdx.x * blockDim.x + threadIdx.x;
    const uint32_t stride = gridDim.x * blockDim.x;
    for (; index < byte_count; index += stride) {
        asc_atomic_add(&histogram[input[index]], 1U);
    }
}
```

**Performance Data**:

| Case | Count Location | Task Duration (μs) |
|:---:|:---:|:---:|
| 0 | GM | 424.22 |

Case 0 latency is approximately 424.22μs, resulting from two compounding principles:

- **Atomic operations on the same address can only execute serially.** An atomic add must complete "read old value - compute new value - write back new value" as an indivisible whole. If multiple threads read the same old value, the later write-back will overwrite the earlier one, resulting in lost increments. To guarantee the indivisibility of these three steps, multiple atomic operations on the same address cannot execute in parallel. In this Case, 262144 atomic updates are concentrated on 256 GM addresses, averaging 1024 serial atomic adds per address.
- **Each atomic operation on GM carries a high per-operation overhead.** GM resides outside the AI Core, with a long access path and high latency. This per-operation overhead accumulates across the serial queue, ultimately reflected in the high total latency.

For details on the above two principles, see the [atomic_add_perf sample](../../../02_features/01_api_features/02_atomic_operation/atomic_add_perf/README_en.md).

---

### Case 1: Two-Level Atomic Counting with Block-Partitioned Accumulation and Merge

**Sample Goal**: Split histogram counting into two levels — "intra-block UB atomic accumulation" and "inter-block GM atomic merge" — to validate the performance benefit of the block-partitioned accumulation strategy in high-contention atomic scenarios.

**Scenario Configuration**:

| Case | Count Location | Data Type | Scale (Block×Thread) |
|:---:|:---:|:---:|:---:|
| 1 | UB block-local accumulation + GM merge | uint32_t | 64×2048 |

**Core Implementation**: Launches 64 thread blocks, each with 2048 threads. Each thread block maintains a 256-bin histogram in UB. First, initialize all bins of the UB histogram to 0 collaboratively and synchronize via `asc_syncthreads()` to ensure all threads complete the zeroing before accumulation begins; then each thread traverses input bytes with a fixed stride across the grid, performing [asc_atomic_add()](../../../../../docs/zh/api/SIMT-API/atomic_operations/asc_atomic_add.md) on the Block-private UB histogram for atomic increment, and synchronize again via `asc_syncthreads()` after accumulation to ensure the UB count results are complete before merging to GM; finally, the first 256 threads atomically accumulate the UB histogram results to the final GM histogram.

In Case 1, each thread block maintains an independent histogram in UB and merges the results to GM at the end. The execution flow is shown in the figure below.

<img src="figures/case1_ub_local.png" alt="intro" style="width: 60%; height: auto;">

The corresponding kernel implementation is as follows.

```cpp
__global__ __launch_bounds__(THREADS_PER_BLOCK) void histogram_ub_block_local(
    uint32_t* histogram, const uint8_t* input, uint32_t byte_count)
{
    __ubuf__ uint32_t block_histogram[HISTOGRAM_BIN_COUNT];

    // Threads within the Block collaboratively zero the UB histogram
    for (uint32_t bin_index = threadIdx.x; bin_index < HISTOGRAM_BIN_COUNT; bin_index += blockDim.x) {
        block_histogram[bin_index] = 0;
    }
    asc_syncthreads();

    // grid-stride traversal, atomic accumulation on the Block-private UB histogram
    uint32_t index = blockIdx.x * blockDim.x + threadIdx.x;
    const uint32_t stride = gridDim.x * blockDim.x;
    for (; index < byte_count; index += stride) {
        asc_atomic_add(&block_histogram[input[index]], 1U);
    }
    asc_syncthreads();

    // The first 256 threads each handle one bin, accumulating Block-local results to the final GM histogram
    for (uint32_t bin_index = threadIdx.x; bin_index < HISTOGRAM_BIN_COUNT; bin_index += blockDim.x) {
        asc_atomic_add(histogram + bin_index, block_histogram[bin_index]);
    }
}
```

**Performance Data**:

| Case | Count Location | Task Duration (μs) |
|:---:|:---:|:---:|
| 1 | UB block-local accumulation + GM merge | 7.95 |

**Analysis**:

The Task Duration of Case 1 is 7.95μs, reduced to approximately 1/53 of Case 0's 424.22μs. Both Cases have the same total atomic update count (262144 accumulations) and the same input read method; the only change is the storage hierarchy of the atomic target and the scope of contention. The following analysis explains this phenomenon.

The gain comes from two compounding factors:

- **The atomic target moves from GM to UB, reducing per-operation overhead.** GM resides outside the AI Core with long access paths; UB is inside the AI Core with shorter access paths and lower latency. Atomic operations on the same address can only execute serially, so the per-operation overhead difference accumulates with each serial execution.
- **The block-partitioned layout narrows contention from global to Block-level, significantly shortening the serial queue length.** In Case 0, 256 GM addresses are shared by all threads, averaging 1024 operations per address; in Case 1, each Block owns an independent 256-bin UB histogram, and contention is limited to the 2048 threads within that Block (averaging 16 operations per address). Furthermore, the 64 Blocks' UB atomic operations do not compete with each other and can execute in parallel. Only the final `64 × 256 = 16384` GM merge operations remain — a drastic reduction from Case 0's 262144.

These two factors work together: the vast majority of atomic operations not only move to the lower-overhead UB but are also split into per-Block parallel tasks. The combined benefit far exceeds the overhead of UB initialization, `asc_syncthreads` synchronization, and GM merge operations.

**Conclusion**: Moving high-contention atomic targets from GM to UB not only reduces per-operation atomic overhead but also makes the UB atomic operations of each thread block non-competing and executable in parallel, significantly shortening serial overhead.

---

## Performance Summary

| Case | Count Location | Task Duration (μs) | Relative to Baseline |
|:---:|:---:|:---:|:---:|
| 0 | GM | 424.22 | 1× |
| 1 | UB block-local accumulation + GM merge | 7.95 | **~53×** |

## Tuning Recommendations

**Prefer UB for atomic accumulation, then merge to GM**: UB resides inside the AI Core with lower per-operation atomic overhead; UBs of each thread block are independent, with non-competing atomic operations that can execute in parallel — only the final merge of each Block's result to GM is needed.

## Build and Run

Run the following steps in the root directory of this sample to build and execute the sample.

- Configure Environment Variables

  Configure environment variables based on the [installation method](../../../../../docs/zh/quick_start.md#prepare&install) of the CANN development kit in the current environment.

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the Sample

  Run the following commands in this sample directory.

  ```bash
  mkdir -p build && cd build                                # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..               # Configure the project
  make -j                                                   # Build the sample
  ./histogram 0                                             # Run Case 0 (baseline)
  ./histogram 1                                             # Run Case 1 (optimization)
  ```

  Build Options Description:

  | Option | Values | Description |
  |------|--------|------|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture: this sample only supports dav-3510 (Ascend 950PR/Ascend 950DT) |
  | `SKIP_VALIDATION` | `ON`/`OFF` | Whether to skip result validation, default `OFF`. Recommended to set to `ON` when collecting performance with `msopprof` |

  The following output indicates that the accuracy verification is successful.

  ```text
  Case 0 (GM global): 262144 bytes, 64 blocks, 2048 threads per block
  [Success] Case accuracy is verification passed.
  ```

## Performance Analysis

Use the `msopprof` tool to obtain detailed performance data:

```bash
msopprof ./histogram 0    # Collect Case 0 performance
msopprof ./histogram 1    # Collect Case 1 performance
```

> **Regarding Validation failed during performance collection:** The sample writes the histogram result to GM, where the GM counter is zeroed only once at host-side allocation; the kernel only accumulates and does not clear it at entry. The `msopprof` warmup+replay re-executes the kernel on the same GM memory, causing the counter to be accumulated multiple times. Therefore, in strict validation mode, `Validation failed` is reported (the count is an integer multiple of the expected value). This is an inherent conflict between the replay mechanism and the validation logic. When collecting performance, it is recommended to rebuild with `-DSKIP_VALIDATION=ON` before running `msopprof` to skip validation.

After the command completes, a folder named "OPPROF_{timestamp}_XXX" is generated in the default directory. The performance data folder structure is as follows:

```text
├──dump                       # Raw performance data, users do not need to focus on this
├──ArithmeticUtilization.csv  # cube/vector instruction cycle ratio
├──L2Cache.csv                # L2 Cache hit rate
├──Memory.csv                 # UB, L1, and main memory read/write bandwidth rates
├──MemoryL0.csv               # L0A, L0B, and L0C read/write bandwidth rates
├──MemoryUB.csv               # Vector and Scalar to UB read/write bandwidth rates
├──OpBasicInfo.csv            # Operator basic information
├──PipeUtilization.csv        # Compute unit and transfer unit time consumption and ratio
├──ResourceConflictRatio.csv  # UB bank group, bank conflict, and resource conflict ratio across all instructions
└──visualize_data.bin         # MindStudio Insight presentation file
```
