# Kernel Launch Configuration Performance Tuning Example

## Overview

This example uses the Gather operator to demonstrate the impact of different thread block configuration strategies on operator performance under different data volume scenarios, and provides corresponding optimization guidance.

## Supported Products

- Ascend 950PR/Ascend 950DT

## Supported CANN Software Version

- \>= CANN 9.1.0

## Directory Structure

```text
├── grid_dim_config
│   ├── figures                // Image resources for README
│   ├── CMakeLists.txt         // Build project file
│   └── grid_config.asc        // Ascend C operator implementation & invocation example
```

## Example Description

- Computation Formula:

  $$
  output[i][j] = input[index[i][j]]
  $$

  - input is the input tensor with shape [2100000] and data type float
  - index is the index tensor with data type uint32, index range [0,2100000); large data scenario is [1024,2048], small data scenario is [8,2048]
  - output is the output tensor with data type float; large data scenario is [1024,2048], small data scenario is [8,2048]
  - Computation process: collect data from the corresponding position in input based on index[i][j], and write to the output[i][j] position

- Example Specifications:

  <table>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center">Gather</td></tr>
  <tr><td rowspan="3" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">input</td><td align="center">[2100000]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">index</td><td align="center">Large data scenario: [1024,2048]<br>Small data scenario: [8,2048]</td><td align="center">uint32</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">output</td><td align="center">Large data scenario: [1024,2048]<br>Small data scenario: [8,2048]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">empty_kernel / gather_naive_kernel / gather_strided_kernel</td></tr>
  </table>

## Example Implementation

### Case Implementation Description

This example is divided into 3 types of cases. First, an empty kernel function is used to detect launch and scheduling overhead under different thread block counts. Then, thread block count selection is observed in both large and small data scenarios.

| Case   | Data Volume / Scale                      | Kernel Function                                  | Comparison Purpose                                  |
| ------ | --------------------------------- | --------------------------------------- | ----------------------------------------- |
| Case 0 | Large data scenario, [1024,2048]=2097152      | empty_kernel                            | Compare empty kernel scheduling overhead with thread block counts of 1 / 32 / 64 / 128 / 1024 |
| Case 1 | Large data scenario, [1024,2048]=2097152      | gather_naive_kernel / gather_strided_kernel | Compare performance with thread block counts of 1024 / 64 / 32 |
| Case 2 | Small data scenario, [8,2048]=16384           | gather_strided_kernel                   | Compare performance with thread block counts of 4 / 8 / 16 / 32 / 64 |

### Performance Metrics Description

| Metric                | Description                                                                              |
| ------------------- | ----------------------------------------------------------------------------------------- |
| Task Duration (us)   | Total task duration, including scheduling time to the accelerator, execution time on the accelerator, and response completion time                  |

### Case 0: Empty Kernel Launch Scheduling Overhead

**Example Goal**: Use an empty kernel function to detect launch scheduling overhead under different thread block counts.

**Core Implementation**:

- The kernel function `empty_kernel` has an empty function body
- Control `<<<...>>>` to set five thread block count configurations through `SCENARIO_NUM` values 1~5: 1 / 32 / 64 / 128 / 1024

**Key Code**:

```cpp
__global__ __launch_bounds__(MAX_THREAD_NUM) void empty_kernel(
    float* input, uint32_t* index, float* output,
    uint32_t input_total_length, uint32_t index_total_length)
{
}

// Thread block count selected by SCENARIO_NUM: 1 / 32 / 64 / 128 / 1024, threads per block fixed at 2048
empty_kernel<<<blocks_per_grid, MAX_THREAD_NUM, 0, stream>>>(
    input_device, index_device, output_device,
    input_total_length, index_total_length);
```

**Performance Data**:

<img src="figures/fig0_case0_empty_curve.png" alt="Case 0 empty kernel scheduling baseline Task Duration vs thread block count" width="600" />


**Analysis**:

From the figure above, in the empty kernel scenario, as the thread block count increases from 1 to 1024, Task Duration also increases: with 1 thread block, Task Duration is 1.221us; increasing to 32 gives 1.557us; increasing to 64 gives 2.173us; increasing to 128 gives 3.141us; increasing to 1024 further increases to 16.422us.

**Conclusion**:

Since the empty kernel performs almost no computation, this latency mainly reflects the fixed overhead of launching and scheduling thread blocks. Therefore, the more thread blocks there are, the greater the fixed overhead of launching and scheduling thread blocks, and the overall Task Duration increases accordingly.

---

### Case 1: Large Data Scenario, Gather Operator with Different Thread Block Counts

**Example Goal**: Compare the performance differences of executing the Gather operator with different thread block counts under the same large data scenario of `1024*2048`, that is, 2097152 elements.

#### Single Thread Single Data

**Core Implementation**:

- The Gather operator computation logic is relatively simple, mainly affected by memory access bandwidth, classified as Memory Bound type, so the thread count per thread block can be configured as 2048
- In this scenario, since each thread processes 1 element, the thread block count computation logic is: total elements 2097152, each thread block contains 2048 threads, so thread block count is `2097152 / 2048 = 1024`
- Control `<<<...>>>` to set thread block count to 1024 through `SCENARIO_NUM` value 6, executing the single thread single data scenario

**Key Code**:

```cpp
// Basic approach: each thread processes only 1 element, no for loop
uint32_t global_idx = blockIdx.x * blockDim.x + threadIdx.x;

uint32_t gather_idx = index[global_idx];
if (gather_idx >= input_total_length) { gather_idx = 0; }
output[global_idx] = input[gather_idx];
```

**Performance Data**:

| Task Duration (us) |
| :---------------: |
|      78.289       |

#### Single Thread Multiple Data

**Optimization Goal**: Under the premise of unchanged semantics, **each thread processes multiple elements (single thread multiple data)**, reducing scheduling overhead by reducing the thread block count.

**Core Implementation**:

- Control `<<<...>>>` to set two thread block count configurations through `SCENARIO_NUM` values 7 or 8: 64 / 32, executing the single thread multiple data scenario. The kernel function `gather_strided_kernel` has each thread start from global index `global_idx` and loop with total thread count as stride to process multiple elements
- Thread count per thread block is set to 2048; thread block count is dynamically configured:
  - Thread block count 64: total threads 131072, that is `64 * 2048`, each thread processes 16 elements
  - Thread block count 32: total threads 65536, that is `32 * 2048`, each thread processes 32 elements

**Key Code**:

```cpp
uint32_t global_idx = blockIdx.x * blockDim.x + threadIdx.x;
uint32_t stride = gridDim.x * blockDim.x;

for (uint32_t i = global_idx; i < index_total_length; i += stride) {
    uint32_t gather_idx = index[i];
    if (gather_idx >= input_total_length) { gather_idx = 0; }
    output[i] = input[gather_idx];
}
```

**Performance Data**:

| Thread Block Count | Task Duration (us) |
| :-----: | :---------------: |
|   64    |      58.583       |
|   32    |      110.540      |

#### Case 1 Three Configuration Performance Comparison

| Approach | Thread Block Count | Task Duration (us) |
| :--: | :-----: | :---------------: |
| Single thread multiple data | 32 | 110.540 |
| **Single thread multiple data** | **64** | **58.583** | 
| Single thread single data | 1024 | 78.289 |

The figure below intuitively shows the Task Duration trend with thread block count from the table above:

<img src="figures/fig1_case1_large_curve.png" alt="Case 1 large data scenario (2097152) Task Duration vs thread block count" width="600" />

**Analysis**:
- With 64 thread blocks configured, compared to the single thread single data approach with 1024 thread blocks, Task Duration decreases from 78.289us to 58.583us, a reduction of approximately 25.2%, with performance improvement of approximately 1.34x. The total Gather read/write volume and thread count per block remain unchanged. The improvement mainly comes from the significant reduction in the number of thread blocks to launch and schedule.
- With 64 thread blocks configured, compared to 32 thread blocks, Task Duration decreases from 110.540us to 58.583us, a reduction of approximately 47.0%, with performance improvement of approximately 1.89x. Under the same single thread multiple data approach, 64 thread blocks have higher parallelism, with less workload per thread block. Although more thread blocks bring more launch and scheduling overhead, this overhead is less than the benefit from increased parallelism, so the overall latency is lower.


**Conclusion**:
- Limited by hardware resources, the number of physical cores that can actually execute in parallel has an upper limit. The physical core count in this test environment is 64, which can be queried using the `query_aiv_core_num` interface in `grid_config.asc`. Since one physical core can only host and execute one thread block at a time, when the thread block count is 1024, thread blocks exceeding the physical core count must wait for preceding thread blocks to complete before being scheduled, and the additional fixed launch and scheduling overhead increases significantly. When the thread block count is 32, only half the physical cores are used, with many idle physical cores and insufficient parallelism.
- Therefore, in large data scenarios, prioritize matching the thread block count to the actual physical core count. In this example, a thread block count of 64 avoids the large thread block scheduling overhead of 1024 thread blocks, and avoids the insufficient parallelism and excessive per-block workload of 32 thread blocks, making it the optimal configuration in Case 1.

---

### Case 2: Small Data Scenario, Gather Operator with Different Thread Block Counts

**Example Goal**: Select a small data scenario of `8*2048`, that is, 16384 elements, and compare the performance differences of executing the Gather operator with different thread block counts.

**Core Implementation**:

- Control `<<<...>>>` to set five thread block count configurations through `SCENARIO_NUM` values 9/10/11/12/13: 4 / 8 / 16 / 32 / 64. The kernel function is identical to the single thread multiple data approach in Case 1, with only the data volume adjusted to `8*2048`, that is, 16384 elements
- Elements per core = 16384 / thread block count: 4 cores 4096, 8 cores 2048, 16 cores 1024, 32 cores 512, 64 cores 256
- Threads per thread block adapt to the actual element count per core: for 4 / 8 cores, elements per core >= 2048, thread count takes the upper limit of 2048; for 16 / 32 / 64 cores, elements per core are less than 2048, thread count adaptively decreases to the actual element count per core (1024 / 512 / 256), eliminating idle threads. The specific configuration is as follows:

  | Thread Block Count | Threads per Thread Block | SCENARIO_NUM |
  | :----------: | :----------------: | :----------: |
  |      4       |        2048        |      9       |
  |      8       |        2048        |      10      |
  |      16      |        1024        |      11      |
  |      32      |        512         |      12      |
  |      64      |        256         |      13      |

**Performance Data**:

| Thread Block Count | Task Duration (us) |
| :--------: | :---------------: |
|     4      |      10.831       |
|     8      |       6.569       |
|     16     |       4.500       |
|   **32**   |     **3.696**     |
|     64     |       4.133       |

The figure below intuitively shows the Task Duration trend with thread block count from the table above:

<img src="figures/fig2_case2_16384_curve.png" alt="Case 2 small data scenario (16384) Task Duration vs thread block count" width="600" />


**Analysis**:

- The curve shows a "decrease then increase" trend. **The measured optimal thread block count in this example is 32**, not 64 which uses all physical cores. The value 32 is only the measured result for this data volume and is not a universal value for small data scenarios.
- As the thread block count increases from 4 to 8, 16, and 32, Task Duration continues to decrease (10.831 -> 6.569 -> 4.500 -> 3.696). This is because threads in a thread block do not all execute in parallel, but are scheduled in warp units. When there are too many warps on a single core, queuing occurs. When the thread block count is small, each thread block needs to launch more threads, and wait time is more significant. Increasing the thread block count reduces the thread count per block and increases the number of parallel cores, so the benefit is significant.
- When the thread block count further doubles from 32 to 64, Task Duration increases from 3.696us to 4.133us, **an increase of approximately 11.8%**. At this point, the workload per core is already small and the effective computation time is short. The scheduling overhead from adding new thread blocks exceeds the parallelism benefit.
- Compared with the Case 1 large data scenario: with the same kernel, after the data volume shrinks, the optimal thread block count changes accordingly, indicating that there is no universal "optimal thread block count" value and it must be measured.

**Conclusion**: Small data scenarios cannot directly follow the experience of "using all physical cores" and must test with multiple thread block counts as in this Case.

---

## Performance Comparison Summary

The performance data for each Case is presented in the corresponding tables above. This section summarizes cross-Case horizontal patterns by large/small data scenarios:

This example distinguishes large and small data volumes by the ratio of data volume to hardware parallel capability. The reference magnitude is "physical core count x maximum threads per core". The current test environment has 64 physical cores with a maximum of 2048 threads per core, giving a reference magnitude of 131072 elements. Case 1 has a data volume of 2097152, far exceeding this reference magnitude, so it serves as the large data scenario. Case 2 has a data volume of 16384, less than this reference magnitude, so it serves as the small data scenario. This partition is only used to illustrate the tuning phenomena in this example. Actual business scenarios should be determined based on the target hardware, operator characteristics, and measured results.

### Large Data Scenario

- **Conclusion**: In large data scenarios, prioritize matching the thread block count to the physical core count. In this example, `1024*2048` corresponds to 2097152 elements. With a thread block count of 64, Task Duration is 58.583us, lower than 78.289us with 1024 thread blocks and 110.540us with 32 thread blocks. Therefore, 64 thread blocks is the optimal configuration in this Case.
- **Reason**: When the thread block count far exceeds the physical core count (such as 1024), there are many thread blocks to launch and schedule, and the fixed launch and scheduling overhead accumulates significantly (Case 0: scheduling baseline with 1024 thread blocks is 16.422us, approximately 7.6x the 2.173us with 64 thread blocks). When the thread block count is too small (such as 32), only half the physical cores are used, with insufficient parallel core count and doubled per-block workload. With 64 thread blocks, a good balance is achieved between physical core utilization and scheduling overhead.

### Small Data Scenario

- **Conclusion**: The optimal thread block count in small data scenarios varies with data volume and cannot directly follow the experience of "using all physical cores". Multiple thread block counts must be tested. In this example, `8*2048` corresponds to 16384 elements, and the measured optimal thread block count is 32, not the physical core count of 64, and this value is only valid for this data volume.
- **Reason**: When the data volume is small, the workload assigned to each core is very small and the effective computation time is short. Continuing to increase the thread block count at this point (such as from 32 to 64, with latency increasing approximately 11.8%) causes the fixed launch and scheduling overhead from new thread blocks to exceed the parallelism benefit.

## Tuning Recommendations

1. **In large data scenarios, prioritize matching the thread block count to the physical core count**: Too few thread blocks cause insufficient parallel core count and heavier per-block workload; too many thread blocks introduce more thread block launch and scheduling overhead.
2. **In small data scenarios, test thread block count empirically**: When the data volume is small and the per-core workload is very small, the fixed scheduling overhead from launching more thread blocks exceeds the benefit, and the optimal thread block count may not be the physical core count. Use the msopprof tool to measure performance and select the thread block count configuration with lower Task Duration.

## Build and Run

Run the following steps in the root directory of this example to build and execute the example.

- Configure Environment Variables
   Configure environment variables based on the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the Example
  Run the following commands in this example directory.
  ```bash
  SCENARIO_NUM=7                                                                     # Select the execution scenario, options 1-13
  mkdir build && cd build;                                                           # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j;  # Build the project
  ./grid_config                                                                      # Run the example
  ```

- Build Options Description

  | Option                       | Values      | Description                                                          |
  | -------------------------- | ----------- | ------------------------------------------------------------- |
  | `CMAKE_ASC_ARCHITECTURES`  | `dav-3510`  | NPU architecture: this example only supports dav-3510 (Ascend 950PR/Ascend 950DT)    |
  | `SCENARIO_NUM`             | `1`-`13`    | Example type, default is 1                                             |

  The following output indicates that the accuracy verification is successful.

  ```text
  [Success] Case accuracy is verification passed.
  ```

## Performance Analysis

Use the `msOpProf` tool to collect detailed performance data:

```bash
msopprof ./grid_config   # Analyze case performance
```

After the command completes, a folder named "OPPROF_{timestamp}_XXX" is generated in the default directory. The performance data folder structure example is as follows:

```text
├──dump                       # Raw performance data, no user attention needed
├──ArithmeticUtilization.csv  # cube/vector instruction cycle ratio
├──L2Cache.csv                # L2 Cache hit rate
├──Memory.csv                 # UB, L1, and main memory read/write bandwidth
├──MemoryL0.csv               # L0A, L0B, and L0C read/write bandwidth
├──MemoryUB.csv               # Vector and Scalar to UB read/write bandwidth
├──OpBasicInfo.csv            # Operator basic information
├──PipeUtilization.csv        # Compute unit and transfer unit duration and ratio
├──ResourceConflictRatio.csv  # UB bank group, bank conflict, and resource conflict ratio across all instructions
└──visualize_data.bin         # MindStudio Insight presentation file
```
