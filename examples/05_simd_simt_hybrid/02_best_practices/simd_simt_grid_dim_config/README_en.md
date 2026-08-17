# Thread Block Count Configuration and VF Call Optimization Example

## Overview

This example uses the Gather computation to demonstrate thread block count configuration and VF function call optimization in Ascend C SIMD and SIMT hybrid programming scenarios. The example includes 4 cases (16 scenarios), ranging from empty kernel function scheduling overhead to thread block count configuration optimization under different data volumes.

## Supported Products

- Ascend 950PR/Ascend 950DT

## Supported CANN Versions

- \> CANN 9.0.0

## Directory Structure

```text
├── simd_simt_grid_dim_config
│   ├── figures                // Image resources for README
│   ├── CMakeLists.txt         // Build project file
│   ├── grid_config.asc        // Ascend C operator implementation & invocation example
│   └── README.md
```

## Example Description

- Computation formula:

  $$output[i][j] = input[index[i][j]]$$

  - input is the input tensor with data type float and shape [2200000]
  - index is the index tensor with data type uint32. The shape varies by case: [1024, 2048] for Case 0/1/2, [8, 2048] for Case 3. The index range is [0, input_total_length)
  - output is the output tensor with data type float and the same shape as index
  - Computation process: gather data from input at the position indicated by index[i][j] and write it to output[i][j]

- Example specifications:

  <table>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center">Gather</td></tr>
  <tr><td rowspan="3" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">input</td><td align="center">[2200000]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">index</td><td align="center">[1024,2048]/[8,2048]</td><td align="center">uint32</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">output</td><td align="center">[1024,2048]/[8,2048]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">empty_kernel / gather_kernel</td></tr>
  </table>

## Example Implementation

### Case Implementation Description

This example uses the compilation macro `SCENARIO_NUM` to control 16 test scenarios grouped into 4 cases, covering empty kernel function scheduling overhead, VF call count, and configuration choices for large and small shapes.

| Case   | SCENARIO_NUM           | Shape                      | Kernel Function                                  | Comparison Purpose                                  |
| ------ | ---------------------- | --------------------------------- | --------------------------------------- | ----------------------------------------- |
| Case 0 | 1-5     | [1024, 2048]           | empty_kernel                            | Set thread block count to 1 / 32 / 64 / 128 / 1024 and compare scheduling overhead across different thread block counts |
| Case 1 | 6-8              | [1024, 2048]              | empty_kernel                            | Set thread block count to 32 / 64 / 1024 and compare scheduling overhead between 1 VF call and 2 VF calls        |
| Case 2 | 9-11            | [1024, 2048]      | gather_kernel                      | For large shapes, set thread block count to 1024 / 64 / 32 and compare performance differences |
| Case 3 | 12-16 | [8, 2048] | gather_kernel | For small shapes, set thread block count to 4 / 8 / 16 / 32 / 64, allocate thread count based on per-core element count, and compare performance differences |

### Performance Metric Description

| Metric                | Description                                                                                      |
| ------------------- | ----------------------------------------------------------------------------------------- |
| Task Duration(μs)   | Total task latency, including scheduling time to the accelerator, execution time on the accelerator, and response completion time                  |

### Case 0: Empty Kernel Function Scheduling Overhead (SCENARIO_NUM=1-5)

**Example Objective**:

Demonstrate the fixed task startup overhead under different thread block count configurations by invoking an empty kernel function.

**Core Implementation**:

- The kernel function `empty_kernel` only calls the empty VF function `simt_empty`, which contains no actual computation logic, used to isolate and test the overhead of different thread block count configurations
- Use `SCENARIO_NUM=1~5` to control five thread block count configurations: 1 / 32 / 64 / 128 / 1024, with a fixed thread count of 2048 and 1 VF call per invocation

**Key Code**:

```cpp
__simt_vf__ __launch_bounds__(MAX_THREAD_NUM) inline void simt_empty(...)
{}

__global__ __vector__ void empty_kernel(...)
{
    for (uint32_t vf_idx = 0; vf_idx < vf_call_times; vf_idx++) {
        asc_vf_call<simt_empty>(dim3(MAX_THREAD_NUM), ...);
    }
}
```

**Performance Data**:

| SCENARIO_NUM | Thread Block Count | Task Duration(μs) |
| :----------: | :-------: | :---------------: |
| 1 | 1 | 1.455 |
| 2 | 32 | 1.589 |
| 3 | 64 | 2.160 |
| 4 | 128 | 3.373 |
| 5 | 1024 | 15.996 |

**Analysis**:

- 1→32→64→128: Task Duration increases from 1.455μs to 3.373μs, with scheduling overhead growing progressively
- 128→1024: Task Duration jumps from 3.373μs to 15.996μs, with scheduling overhead increasing significantly as thread block count grows

**Conclusion**:

Scheduling overhead increases with thread block count. The more thread blocks, the higher the fixed task startup overhead.

---

### Case 1: Impact of VF Call Count on Scheduling Overhead (SCENARIO_NUM=6-8)

**Example Objective**:

Building on Case 0, compare the impact of 1 versus 2 VF calls on Task Duration to demonstrate the relationship between VF call count and scheduling overhead.

**Core Implementation**:

- Uses the same empty kernel function `empty_kernel` as Case 0, with the only change being the VF call count increased from 1 to 2, controlled by configuring vf_call_times
- Use `SCENARIO_NUM=6-8` to launch 32, 64, and 1024 thread blocks respectively, with 2 VF calls per invocation

**Performance Data**:

| SCENARIO_NUM | Thread Block Count | VF Call Count | Task Duration(μs) |
| :----------: | :-------: | :--------: | :---------------: |
| 6 | 32 | 2 | 1.780 |
| 7 | 64 | 2 | 2.327 |
| 8 | 1024 | 2 | 16.598 |

**Comparison with Case 0**:

| Comparison | Thread Block Count | VF Count Change | Task Duration Change | Increase |
| ---- | :-------: | :--------: | :---------------: | :--: |
| SCENARIO_NUM=2→6 | 32 | 1→2 | 1.589 → 1.780 | +12.0% |
| SCENARIO_NUM=3→7 | 64 | 1→2 | 2.160 → 2.327 | +7.7% |
| SCENARIO_NUM=5→8 | 1024 | 1→2 | 15.996 → 16.598 | +3.8% |

The performance data is shown in the following figure:

<img src="figures/fig_case01_scheduling.png" alt="Case 0/1 scheduling overhead: thread block count and VF call count" width="600" />

**Analysis**:

- With 32, 64, and 1024 thread blocks, increasing from 1 to 2 VF calls increases Task Duration in all three configurations, indicating additional VF call scheduling overhead
- With 1024 thread blocks, the absolute latency increase is 0.602μs, higher than the 0.191μs and 0.167μs increases with 32 and 64 thread blocks

**Conclusion**:

VF calls also incur scheduling overhead. Minimize VF calls by processing all data in a single VF call where possible.

---

### Case 2: Performance Comparison of Different Thread Block Counts for Large-Shape Gather (SCENARIO_NUM=9-11)

**Example Objective**:

Compare performance differences when executing Gather computation with the same large shape but different thread block count configurations.

**Core Implementation**:

- All three scenarios use the kernel function `gather_kernel`, differing only in thread block count and per-thread data volume
- input shape is [2200000], index and output shape is [1024, 2048]
- The Gather operator has simple computation logic and is primarily affected by memory access bandwidth, making it a Memory Bound type. The overall computation volume in this example is small with few registers used, so the thread count per thread block can be configured as 2048
- The VF function `simt_gather` uses a grid-stride loop: starting with `blockIdx.x * blockDim.x + threadIdx.x` as the initial index and `gridDim.x * blockDim.x` as the stride to traverse the index array, each thread processes all elements assigned to its block via a for loop
- Thread count is allocated by `resolve_thread_num` based on the actual number of elements processed per core: when the per-core element count is >= 2048, use 2048; otherwise, use the actual element count

**Key Code**:

```cpp
// simt_gather: SIMT VF function -- grid-stride loop, traversing the entire index array with total thread count as stride
__simt_vf__ __launch_bounds__(MAX_THREAD_NUM) inline void simt_gather(..., uint32_t index_total_length)
{
    uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
    uint32_t stride = gridDim.x * blockDim.x;
    for (uint32_t i = idx; i < index_total_length; i += stride) {
        uint32_t gather_idx = index[i];
        if (gather_idx >= input_total_length) { gather_idx = 0; }
        output[i] = input[gather_idx];
    }
}

// gather_kernel: kernel function calls simt_gather, thread count is adaptively allocated by resolve_thread_num
__global__ __vector__ void gather_kernel(..., uint32_t thread_num)
{
    asc_vf_call<simt_gather>(dim3(thread_num), ..., index_total_length);
}
```

#### Logical Cores and Physical Cores

When launching a SIMT kernel function, the first parameter in the kernel invocation <<<>>> determines how many thread blocks to launch for the task. For this example, the thread block count can be understood as the number of **logical cores** to launch.

These logical cores must ultimately be mapped to the actual Vector Cores available on the hardware, so two concepts need to be introduced:

- **Logical cores**: The number of thread blocks launched, representing the number of parallel tasks the software side wants to launch
- **Physical cores**: The actual number of Vector Cores available on the hardware, which must be queried at runtime via [aclrtGetDeviceInfo](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/910beta1/API/runtimeapi/aclcppdevg_03_1867.html) (64 in this example environment). The code example is as follows:
  ```cpp
  // Query the number of physical Vector Cores (AIV) at runtime; return AIV_CORE_NUM on query failure
  uint32_t query_aiv_core_num(int32_t device_id)
  {
      int64_t core_num = 0;
      aclError ret = aclrtGetDeviceInfo(
          static_cast<uint32_t>(device_id), ACL_DEV_ATTR_VECTOR_CORE_NUM, &core_num);
      return (ret == ACL_SUCCESS && core_num > 0) ? static_cast<uint32_t>(core_num) : AIV_CORE_NUM;
  }
  ```

The relationship between the two affects scheduling overhead: when the logical core count <= physical core count, all logical cores can occupy physical cores simultaneously for parallel execution; when the logical core count > physical core count, the excess logical cores must wait for preceding logical cores to finish and release physical cores before being scheduled, and the waiting overhead increases as the number of excess logical cores grows.

**Performance Data**:

| SCENARIO_NUM | Thread Block Count | Elements per Core | Elements per Thread | Task Duration(μs) |
| :----------: | :-------: | :-------: | :-------: | :---------------: |
| 9 | 1024 | 2048 | 1 | 78.921 |
| **10** | **64** | **32768** | **16** | **59.109** |
| 11 | 32 | 65536 | 32 | 110.540 |

The performance data is shown in the following figure:

<img src="figures/fig_case2_large.png" alt="Case 2 Task Duration vs thread block count" width="600" />

**Analysis**:

- **Compared with a thread block count of 1024, a thread block count of 64 reduces Task Duration from 78.921μs to 59.109μs, a latency reduction of approximately 25.1% and a performance improvement of approximately 34%**. Reducing the thread block count from 1024 to 64 significantly reduces the number of thread blocks launched and scheduled, greatly lowering scheduling overhead
- **Compared with a thread block count of 32, a thread block count of 64 reduces Task Duration from 110.540μs to 59.109μs, a latency reduction of approximately 46.5% and a performance improvement of approximately 87%**. With the same grid-stride loop implementation, a thread block count of 64 provides higher parallelism; although more thread blocks increase launch and scheduling overhead, this overhead is smaller than the benefit from improved parallelism, resulting in lower overall latency
- Due to hardware resource limitations, the number of physical cores that can execute in parallel has an upper bound. Since one physical core can only host and execute one thread block at a time, when the thread block count is 1024, thread blocks exceeding the physical core count must wait for preceding thread blocks to complete before being scheduled, and the additional fixed launch and scheduling overhead increases significantly; when the thread block count is 32, only half of the physical cores are used, leaving many idle physical cores with insufficient parallelism and doubled per-thread-block workload

**Conclusion**:

For large shapes, prioritize matching the thread block count to the actual physical core count. In this example, a thread block count of 64 avoids both the large scheduling overhead from a thread block count of 1024 and the insufficient parallelism from a thread block count of 32, making it the optimal configuration in Case 2.

---

### Case 3: Performance Comparison of Different Thread Block Counts and Thread Counts for Small-Shape Gather (SCENARIO_NUM=12-16)

**Example Objective**:

Compare performance differences when executing Gather computation with the same small shape but different thread block count and thread count configurations.

**Core Implementation**:

- All five scenarios use the kernel function `gather_kernel`
- input shape is [2200000], index and output shape is [8, 2048]
- Thread count is allocated by `resolve_thread_num` based on the actual number of elements processed per core: when the per-core element count is less than 2048, reduce the thread count to eliminate idle threads

**Performance Data**:

| SCENARIO_NUM | Thread Block Count | Thread Count | Elements per Core | Elements per Thread | Task Duration(μs) |
| :----------: | :-------: | :-------: | :-------: | :-------: | :---------------: |
| 12 | 4 | 2048 | 4096 | 2 | 10.760 |
| 13 | 8 | 2048 | 2048 | 1 | 6.741 |
| 14 | 16 | 1024 | 1024 | 1 | 4.514 |
| **15** | **32** | **512** | **512** | **1** | **3.866** |
| 16 | 64 | 256 | 256 | 1 | 4.115 |

The performance data is shown in the following figure:

<img src="figures/fig_case3_small_griddim.png" alt="Case 3 Task Duration vs thread block count" width="600" />

**Analysis**:

- The curve shows a valley shape that decreases first then rises, with an **optimal thread block count of 32 (1/2 of physical cores)**, not filling all 64 cores
- As thread block count increases from 4 to 8 to 16 to 32, Task Duration continues to decrease (10.760 → 6.741 → 4.514 → 3.866), with parallelism benefits continuing to improve
- 32→64 (physical cores): Thread block count doubles, Task Duration increases from 3.866μs to 4.115μs, **latency increases by approximately 6.4%**. For small shapes, per-core task volume is small (256/512 elements), and the scheduling overhead from additional thread blocks exceeds the parallelism benefit
- Compared with Case 2: with the same kernel, when the shape is small enough, the optimal thread block count drops from 64 to 32, indicating that the "optimal thread block count" changes as the shape shrinks and must be measured empirically

**Conclusion**:

For small shapes, the experience of "using all physical cores" cannot be directly applied. Multiple thread block count levels must be tested as demonstrated in this case.

---

## Performance Comparison Summary

### Ascend 950PR Performance Data

**Scheduling overhead vs thread block count (Case 0)**:

| SCENARIO_NUM | Thread Block Count | Task Duration(μs) |
| ------------ | --------- | ----------------- |
| 1 | 1 | 1.455 |
| 2 | 32 | 1.589 |
| 3 | 64 | 2.160 |
| 4 | 128 | 3.373 |
| 5 | 1024 | 15.996 |

- Scheduling overhead increases with thread block count

**Impact of VF call count on overhead (Case 1)**:

| Comparison | Thread Block Count | VF Count | Task Duration(μs) | Increase |
| ---- | --------- | ------ | ----------------- | ---- |
| SCENARIO_NUM=2→6 | 32 | 1→2 | 1.589 → 1.780 | +12.0% |
| SCENARIO_NUM=3→7 | 64 | 1→2 | 2.160 → 2.327 | +7.7% |
| SCENARIO_NUM=5→8 | 1024 | 1→2 | 15.996 → 16.598 | +3.8% |

- VF calls also incur scheduling overhead

**Optimal thread block count for large shapes (Case 2)**:

| SCENARIO_NUM | Thread Block Count | Elements per Core | Task Duration(μs) |
| :----------: | :-------: | :-------: | :---------------: |
| 9 | 1024 | 2048 | 78.921 |
| **10** | **64** | **32768** | **59.109** |
| 11 | 32 | 65536 | 110.540 |

- The optimal thread block count is **64** (equal to the physical core count); a thread block count of 64 avoids both the large scheduling overhead from a thread block count of 1024 and the insufficient parallelism from a thread block count of 32

**Optimal thread block count for small shapes (Case 3)**:

| SCENARIO_NUM | Thread Block Count | Thread Count | Elements per Core | Task Duration(μs) |
| :----------: | :-------: | :-------: | :-------: | :---------------: |
| 12 | 4 | 2048 | 4096 | 10.760 |
| 13 | 8 | 2048 | 2048 | 6.741 |
| 14 | 16 | 1024 | 1024 | 4.514 |
| **15** | **32** | **512** | **512** | **3.866** |
| 16 | 64 | 256 | 256 | 4.115 |

- The optimal thread block count is **32** (1/2 of physical cores), not the full 64 cores; latency increases after the thread block count grows from 32 to 64

## Tuning Recommendations

1. **Match thread block count to physical core count for large shapes**: Due to hardware resource limitations, the number of physical cores that can execute in parallel has an upper bound. The thread block count should be as close to the physical core count as possible, avoiding too many thread blocks (queuing and accumulated scheduling overhead) or too few (insufficient parallelism and heavy per-thread-block workload).
2. **Measure thread block count performance for small shapes**: For small shapes, per-core workload is small, and the scheduling overhead from launching more thread blocks may exceed the benefit. The optimal thread block count is not necessarily the maximum physical core count. Refer to Case 3 and test multiple levels to find the optimal thread block count.

## Build and Run

Run the following steps in the root directory of this example to build and run the example.

- Configure environment variables
  Configure environment variables based on the [installation method](../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit on the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the example

  Run the following commands in the example directory.
  ```bash
  SCENARIO_NUM=1                       # Select the execution scenario, options: 1-16
  mkdir -p build && cd build;          # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j;  # Build the project
  ./grid_config                        # Run the example
  ```

- Build option description

  | Option             | Values      | Description              |
  | ---------------- | ----------- | ----------------- |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture: this example only supports dav-3510 (Ascend 950PR/Ascend 950DT) |
  | `SCENARIO_NUM` | `1`-`16` | Example type, default is 1 |

- Execution result

  The following execution result indicates that the accuracy comparison is successful.

  ```text
  [Success] Case accuracy is verification passed.
  ```

## Performance Analysis

### Introduction to the msOpProf Tool

`msOpProf` is a single-operator performance analysis tool. It offers two usage methods: `msopprof` and `msopprof simulator`. The tool helps users identify anomalies in operator memory, operator code, and operator instructions, enabling comprehensive operator tuning. It currently supports performance data collection and automatic parsing for different run modes (on-device or simulation) and different file types (executables or operator binary `.o` files).

- On-device performance collection

    On-device performance collection directly measures the execution time of an operator on an Ascend AI Processor. This method is suitable for quickly locating operator performance issues in an on-device environment.

    Use the `msOpProf` tool to obtain detailed performance data:

    ```bash
    msopprof ./grid_config   # Analyze performance
    ```

    - Performance data description  
      After the command completes, a folder named "OPPROF_{timestamp}_XXX" will be generated in the default directory. The performance data folder structure is as follows:

      ```bash
      ├──dump                       # Raw performance data; users do not need to inspect it
      ├──ArithmeticUtilization.csv  # Cube/Vector instruction cycle proportions
      ├──L2Cache.csv                # L2 Cache hit rate; affects MTE2. Plan data transfer logic properly to increase the hit rate
      ├──Memory.csv                 # Read/write bandwidth rates of UB, L1, and main memory
      ├──MemoryL0.csv               # Read/write bandwidth rates of L0A, L0B, and L0C
      ├──MemoryUB.csv               # Read/write bandwidth rates from Vector and Scalar to UB
      ├──OpBasicInfo.csv            # Basic operator information
      ├──PipeUtilization.csv        # Durations and proportions of computation and data transfer units
      ├──ResourceConflictRatio.csv  # Proportions of UB bank groups, bank conflicts, and resource conflicts among all instructions
      └──visualize_data.bin         # MindStudio Insight presentation file
      ```

View the specific performance analysis results:

```bash
# View Task Duration and various metrics
cat ./OPPROF_*/PipeUtilization.csv
```
