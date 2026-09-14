# Thread Block Count Configuration Optimization

<!-- md-trans-meta sourceCommit=e4a5905334db2bef02e668063a0b99d64a2e7709 translatedAt=2026-08-26T14:50:05.110Z -->

[Priority] High

[Description] In SIMT programming mode, the thread block count for launching a kernel function, that is, `gridDim`, is configured through `<<<...>>>`. These thread blocks are eventually scheduled to the Vector Cores (physical cores) of the hardware for execution. Due to hardware limitations, a physical core can host and execute only one thread block at a time, and the launch and scheduling of each thread block introduces fixed overhead. When the thread block count far exceeds the physical core count, the number of thread blocks that need to be launched and scheduled increases significantly, and the fixed scheduling overhead accumulates noticeably. When the thread block count is insufficient, physical cores become idle, the number of parallel cores is insufficient, and the workload per core increases, which significantly affects the duration. In addition, the smaller the data size and the less workload per core, the higher the proportion of fixed scheduling overhead, and the optimal thread block count changes accordingly. Therefore, the thread block count should be set based on both the physical core count and the data size.

The relationship between the thread block count and the physical core count affects the scheduling overhead: when the thread block count ≤ the physical core count, all thread blocks can occupy all physical cores at once and execute in parallel, so fewer thread blocks need to be launched and scheduled and the fixed overhead is limited. When the thread block count > the physical core count, the thread blocks exceeding the physical core count must wait for the preceding thread blocks to finish execution before being scheduled, so the number of thread blocks that need to be launched and scheduled continues to increase, the fixed overhead of launching and scheduling multiple thread blocks begins to accumulate noticeably, and the scheduling overhead rises as the thread block count increases.

The physical core count can be queried at runtime through [aclrtGetDeviceInfo](https://www.hiascend.com/document/detail/en/CANNCommunityEdition/910/API/runtimeapi/aclcppdevg_03_1867.html).
> Note: In the following two scenarios, the measured physical core count is 64.

After determining the physical core count, you also need to set the thread block count based on the data size.

> [!NOTE] Note
> There is no absolute boundary between "large" and "small" data sizes. The two scenarios below are only representatives of two orders of magnitude. The truly reliable approach is to start from the physical core count and test it in practice.
>
> A useful intuition is that all physical cores can process approximately "physical core count × maximum thread count per core" threads in parallel at one time. When the data volume is far larger than this order of magnitude, setting the thread block count to the physical core count can fully utilize the hardware computing power. When the data volume is comparable to or even smaller than this order of magnitude, the proportion of fixed scheduling overhead in the total duration becomes sensitive, and the optimal thread block count is no longer obvious, so you need to use the msOpProf tool for actual measurement.

For the sample corresponding to this optimization method, see the [kernel function launch configuration performance tuning sample](../../../../../examples/03_simt_api/03_best_practices/01_execution_conf_optimizations/grid_dim_config/README.md).

## Large Data Volume Scenario
The following uses the Gather operator as an example to compare the performance differences of different thread block count configurations in a large data volume scenario (`1024*2048`, that is, 2097152 elements).

[Negative Example] One thread processes one element, and the number of thread blocks increases with the data size, causing a significant increase in the thread blocks that need to be launched and scheduled.

```cpp
// Basic implementation: each thread processes only one element, without a for loop
// Requires gridDim.x * blockDim.x == total number of elements (in this sample, 1024 * 2048 == 2097152)
__global__ __launch_bounds__(MAX_THREAD_NUM) void gather_naive_kernel(
    float* input, uint32_t* index, float* output,
    uint32_t input_total_length, uint32_t index_total_length)
{
    uint32_t global_idx = blockIdx.x * blockDim.x + threadIdx.x;

    uint32_t gather_idx = index[global_idx];
    if (gather_idx >= input_total_length) {
        gather_idx = 0;
    }
    output[global_idx] = input[gather_idx];
}

gather_naive_kernel<<<1024, INDEX_WIDTH, 0, stream>>>(
    input_device, index_device, output_device,
    input_total_length, index_total_length);
```

The above implementation lets each thread process a unique element, so the thread block count is: `total number of elements / threads per core = 2097152 / 2048 = 1024`. With multiple thread blocks, the fixed overhead of launching and scheduling accumulates significantly. The performance data of this implementation is as follows:

| Thread Block Count | Task Duration (us) |
| :-------: | :---------------: |
|   1024    |      78.289       |

[Positive Example] **One thread processes multiple elements**, reducing the scheduling overhead by decreasing the thread block count.

```cpp
// Optimized implementation: threads stride through the entire array with the total thread count as the step size, processing multiple elements
__global__ __launch_bounds__(MAX_THREAD_NUM) void gather_strided_kernel(
    float* input, uint32_t* index, float* output,
    uint32_t input_total_length, uint32_t index_total_length)
{
    uint32_t global_idx = blockIdx.x * blockDim.x + threadIdx.x;
    uint32_t stride = gridDim.x * blockDim.x;

    for (uint32_t i = global_idx; i < index_total_length; i += stride) {
        uint32_t gather_idx = index[i];
        if (gather_idx >= input_total_length) {
            gather_idx = 0;
        }
        output[i] = input[gather_idx];
    }
}

// The thread block count is fixed at the physical core count of 64, and each block still uses the hardware upper limit of 2048 threads
gather_strided_kernel<<<64, MAX_THREAD_NUM, 0, stream>>>(
    input_device, index_device, output_device,
    input_total_length, index_total_length);
```

The Gather operator implemented above provides the same functionality as the negative example. The only difference is that the thread block count is adjusted to 64, with 2048 threads per thread block, launching a total of 131072 threads. To process 2097152 data elements, each thread needs to process 16 data elements. The performance data of this implementation in the large data volume scenario is as follows:

| Thread Block Count | Task Duration (us) |
| :-------: | :---------------: |
|    64     |      58.583       |


The performance data of the two implementations above is summarized as follows:
| Implementation | Thread Block Count | Task Duration (us) | 
| :--: | :-------: | :---------------: | 
| **Single thread, multiple data** | **64** | **58.583** | 
| Single thread, single data | 1024 | 78.289 | 

According to the task duration data, when processing the same data size, using the approach where a single thread processes multiple data elements and reducing the thread block count from 1024 to 64 lowers the execution time from 78.289 us to 58.583 us, a decrease of about 25.2%, improving overall performance by about 1.34x. Reducing the thread block count actually improves performance because when the thread block count is 1024, thread blocks that exceed the physical core count must wait for the preceding thread blocks to finish execution before being scheduled, which significantly increases the additional launch and scheduling fixed overhead.

## Small Data Volume Scenario

When the data volume is small, there is no rule of thumb for the optimal thread block count that can be directly applied; it can only be determined through actual measurement.

[Negative Example] In the small data volume scenario, the experience of "using all physical cores" is followed, and the thread block count is directly set to the physical core count of 64.

```cpp
// The kernel is exactly the same as the positive example in the large data volume scenario (gather_strided_kernel), with only the data volume adjusted to 16384 for the small data volume scenario.
// Directly follow the experience of "using all physical cores", with thread block count = 64.
gather_strided_kernel<<<64, MAX_THREAD_NUM, 0, stream>>>(
    input_device, index_device, output_device,
    input_total_length, index_total_length);
```

The above implementation also uses 64 physical cores for the small data volume scenario, with each core processing only `16384 / 64 = 256` elements. In this case, the computation itself takes very little time, and the fixed scheduling overhead introduced by launching 64 thread blocks accounts for a higher proportion of the total duration. The performance data of this implementation in the small data volume scenario is as follows:

| Thread Block Count |  Threads per Thread Block | Task Duration (us) |
| :-------: |  :---------------: | :---------------: |
|    64     |         256        |       4.133       |

[Positive Example] In the small data volume scenario, instead of directly following the experience of "using all physical cores", multiple thread block counts are set and measured, and the lowest point of task duration is selected.

```cpp
// The kernel remains unchanged; only different thread block counts are set, and the performance is measured using the msOpProf tool.
// Small data volume scenario (16384) measurement: thread block counts of 4 / 8 / 16 / 32 / 64 are tested, and the lowest point is at thread block count = 32.
gather_strided_kernel<<<32, MAX_THREAD_NUM, 0, stream>>>(
    input_device, index_device, output_device,
    input_total_length, index_total_length);
```

In the above implementation, the kernel remains unchanged. Only the thread block count is set to different values between 4 and 64, and each value is measured one by one. The thread count of each thread block is set to the smaller of the data volume that each thread block needs to process and the hardware thread count upper limit of 2048: when the data volume that each thread block needs to process is not less than 2048, the thread count is set to 2048; when it is less than 2048, the thread count is reduced to the actual data volume to be processed. In the small data volume scenario, task duration shows a "decrease first and then increase" trend as the thread block count changes. In this sample, the measured optimal thread block count is 32, rather than 64, which is the thread block count that uses all physical cores.

  | Number of Thread Blocks | Thread Count per Thread Block | Task Duration (us) |
  | :----------: | :----------------: |:---------------: |
  |      4       |        2048        |      10.831       | 
  |      8       |        2048        |       6.569       |   
  |      16      |        1024        |       4.500       |   
  |      32      |        512         |     **3.696**     |  
  |      64      |        256         |       4.133       |

The following figure intuitively shows the trend of task duration as the thread block count changes in the table above:

<img src="../../../figures/griddim_config_small_shape.png" alt="Small data volume scenario (16384): task duration changes with thread block count" width="480"/>

According to the task duration data, in the small data volume scenario of this sample, the duration keeps decreasing (10.831 → 6.569 → 4.500 → 3.696 us) as the thread block count increases from 4 to 8, 16, and 32 in sequence. This is because the threads in a thread block are not all executed in parallel; instead, they are scheduled and executed in units of warps. When there are too many warps on a single core, queueing occurs. When the thread block count is small, each thread block needs to launch more threads, making the waiting time more obvious. Increasing the thread block count can reduce the thread count of a single thread block and increase the number of parallel cores, so the benefit is significant. When the thread block count increases from 32 to 64, the duration rises from 3.696 us to 4.133 us, indicating that at this data size, the scheduling fixed overhead brought by further increasing the thread block count exceeds the parallel benefit. Therefore, in the small data size scenario, the optimal thread block count must be determined through actual measurement.

[Summary] The thread block count configuration of a SIMT operator should be set by considering both the physical core count and the data size.

The general principles are as follows:

- **For large data volume scenarios, prioritize matching the physical core count**: When the data volume is far greater than the magnitude of "physical core count × maximum thread count per core", use the physical core count directly as the thread block count. This avoids both an excessive workload on a single core caused by an insufficient thread block count and the additional thread block launch and scheduling overhead caused by an excessive thread block count.
- **For small data volume scenarios, measure the thread block count**: When the data volume is comparable to or even smaller than "physical core count × maximum thread count per core" and the workload per core is very small, the fixed scheduling overhead brought by launching more cores will exceed the effective computation benefit. The optimal thread block count cannot be predicted and must be measured using the msOpProf tool, and the thread block count configuration with a lower task duration should be selected.
