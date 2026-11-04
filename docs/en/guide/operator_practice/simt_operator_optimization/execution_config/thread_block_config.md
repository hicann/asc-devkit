# Thread Block Count Configuration Optimization

<!-- md-trans-meta sourceCommit=8d22eb82fd0b279bd5d784f696a54970c40f98ac translatedAt=2026-10-23T13:09:33.766Z pushedAt=2026-11-03T07:05:13.371Z nl=2 -->

[Priority] High

[Description] In SIMT programming mode, the number of thread blocks for launching a kernel is configured through `<<<...>>>`, that is, `gridDim`. These thread blocks are ultimately scheduled to the hardware Vector Cores (physical cores) for execution. Due to hardware limitations, a physical core can only host and execute one thread block at a time, and the launch and scheduling of each thread block introduces fixed overhead. When the number of thread blocks far exceeds the number of physical cores, the number of thread blocks that need to be launched and scheduled increases significantly, and the fixed scheduling overhead accumulates noticeably. When the number of thread blocks is insufficient, physical cores become idle, the number of parallel cores is insufficient, and the workload per core increases, which significantly affects the execution time. In addition, the smaller the data size and the less workload per core, the higher the proportion of fixed scheduling overhead, and the optimal thread block count changes accordingly. Therefore, the number of thread blocks should be set based on both the number of physical cores and the data size.

The relationship between the number of thread blocks and the number of physical cores affects scheduling overhead: when the number of thread blocks ≤ the number of physical cores, all thread blocks can occupy all physical cores at once and execute in parallel, so fewer thread blocks need to be launched and scheduled, and the fixed overhead is limited. When the number of thread blocks > the number of physical cores, the thread blocks exceeding the number of physical cores must wait for the preceding thread blocks to finish execution before being scheduled. The number of thread blocks that need to be launched and scheduled continues to increase, the fixed overhead of launching and scheduling multiple thread blocks begins to accumulate noticeably, and the scheduling overhead rises as the number of thread blocks increases.

The number of physical cores can be queried at runtime through [aclrtGetDeviceInfo](https://gitcode.com/cann/runtime/blob/9.2.0/docs/en/api_ref/04_device_management.md#aclrtGetDeviceInfo).
> Note: In the following two scenarios, the measured number of physical cores is 64.

After determining the number of physical cores, you also need to set the number of thread blocks based on the data size.

> [!NOTE] Note
> There is no absolute boundary between "large" and "small" data sizes. The two scenarios below are only representatives of two orders of magnitude. The truly reliable approach is to start from the number of physical cores and test in practice.
>
> A useful intuition is: all physical cores can process approximately "number of physical cores × maximum thread count per core" threads in parallel at once. When the data volume is far larger than this order of magnitude, setting the number of thread blocks to the number of physical cores can fully utilize the hardware computing power. When the data volume is comparable to or even smaller than this order of magnitude, the proportion of fixed scheduling overhead in the total execution time becomes sensitive, the optimal thread block count is no longer obvious, and you need to use the msOpProf tool for actual measurement.

For the sample corresponding to this optimization method, see [Kernel Launch Configuration Performance Tuning Sample](../../../../../../examples/03_simt_api/03_best_practices/01_execution_conf_optimizations/grid_dim_config/README.md).

## Large Data Volume Scenario
The following uses the Gather operator as an example to compare the performance differences of different thread block count configurations in a large data volume scenario (`1024*2048`, that is, 2097152 elements).

[Negative example] A thread processes one element, and the number of thread blocks increases with the data volume, causing a significant increase in the thread blocks that need to be launched and scheduled.

```cpp
// Basic implementation: each thread processes only one element, without a for loop
// Requires gridDim.x * blockDim.x == total number of elements (1024 * 2048 == 2097152 in this sample)
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

The above implementation lets each thread process a unique element, so the number of thread blocks is: `total number of elements / threads per core = 2097152 / 2048 = 1024`. With multiple thread blocks, the fixed overhead of launching and scheduling accumulates significantly. The performance data of this implementation is as follows:

| Number of Thread Blocks | Task Duration (us) |
| :-------: | :---------------: |
|   1024    |      78.289       |

[Positive example] **A thread processes multiple elements**, reducing the scheduling overhead by decreasing the number of thread blocks.

```cpp
// Optimized implementation: threads traverse the entire array with a stride equal to the total thread count, processing multiple elements
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

// The number of thread blocks is fixed to the number of physical cores (64), and each block still uses the hardware upper limit of 2048 threads
gather_strided_kernel<<<64, MAX_THREAD_NUM, 0, stream>>>(
    input_device, index_device, output_device,
    input_total_length, index_total_length);
```

The Gather operator implemented above has the same functionality as the counter-example. The only difference is that the number of thread blocks is adjusted to 64, with 2048 threads per thread block, launching a total of 131072 threads. To process 2097152 data elements, each thread needs to process 16 data elements. The performance data of this implementation in the large data volume scenario is as follows:

| Number of Thread Blocks | Task Duration (us) |
| :-------: | :---------------: |
|    64     |      58.583       |


The performance data of the two implementations above is summarized as follows:

| Implementation | Number of Thread Blocks | Task Duration (us) | 
| :--: | :-------: | :---------------: | 
| **Single thread, multiple data** | **64** | **58.583** | 
| Single thread, single data | 1024 | 78.289 | 

According to the Task Duration data, when processing the same data volume, using the single-thread-multiple-data approach and reducing the number of thread blocks from 1024 to 64 lowers the execution time from 78.289 us to 58.583 us, a decrease of about 25.2%, improving overall performance by about 1.34 times. Reducing the number of thread blocks actually improves performance. This is because when the number of thread blocks is 1024, thread blocks that exceed the number of physical cores must wait for the preceding thread blocks to finish before being scheduled, which significantly increases the additional launch and scheduling overhead.

## Small Data Volume Scenario

When the data volume is small, there is no rule of thumb for the optimal number of thread blocks that can be directly applied; it can only be found through actual measurement.

[Negative example] In the small data volume scenario, the rule of "using all physical cores" is followed, and the number of thread blocks is directly set to the number of physical cores, 64.

```cpp
// The kernel is exactly the same as the example in the large data volume scenario (gather_strided_kernel), except that the data volume is adjusted to 16384 for the small data volume scenario.
// Directly follow the rule of "using all physical cores", with the number of thread blocks = 64.
gather_strided_kernel<<<64, MAX_THREAD_NUM, 0, stream>>>(
    input_device, index_device, output_device,
    input_total_length, index_total_length);
```

The above implementation also uses 64 physical cores for the small data volume scenario, with each core processing only `16384 / 64 = 256` elements. In this case, the computation itself takes very little time, and the fixed scheduling overhead introduced by launching 64 thread blocks accounts for a higher proportion of the total duration. The performance data of this implementation in the small data volume scenario is as follows:

| Number of Thread Blocks |  Threads per Thread Block | Task Duration (us) |
| :-------: |  :---------------: | :---------------: |
|    64     |         256        |       4.133       |

[Positive example] In the small data volume scenario, instead of directly following the rule of "using all physical cores", multiple levels of thread block counts are set for actual measurement, and the lowest point of Task Duration is taken.

```cpp
// The kernel remains unchanged. Only different numbers of thread blocks are set, and the performance is measured using the msOpProf tool.
// Actual measurement in the small data volume scenario (16384): the number of thread blocks is set to 4 / 8 / 16 / 32 / 64, and the lowest point is at 32 thread blocks.
gather_strided_kernel<<<32, MAX_THREAD_NUM, 0, stream>>>(
    input_device, index_device, output_device,
    input_total_length, index_total_length);
```

The kernel in the above implementation remains unchanged. Only the number of thread blocks is set to different values between 4 and 64 and measured level by level. The thread count of each thread block is set to the smaller of the data volume that each thread block needs to process and the hardware thread count upper limit of 2048: when the data volume that each thread block needs to process is not less than 2048, the thread count is set to 2048; when it is less than 2048, the thread count is reduced to the actual data volume to be processed. In the small data volume scenario, Task Duration shows a "decrease first and then increase" trend as the number of thread blocks changes. In this sample, the measured optimal number of thread blocks is 32, rather than 64, which uses all physical cores.

  | Number of Thread Blocks | Thread Count per Thread Block | Task Duration (us) |
  | :----------: | :----------------: |:---------------: |
  |      4       |        2048        |      10.831       | 
  |      8       |        2048        |       6.569       |   
  |      16      |        1024        |       4.500       |   
  |      32      |        512         |     **3.696**     |  
  |      64      |        256         |       4.133       |

The following figure intuitively shows the trend of Task Duration changing with the number of thread blocks in the table above:

<img src="../../../figures/griddim_config_small_shape.png" alt="Small data volume scenario (16384): Task Duration changes with the number of thread blocks" width="480"/>

According to the Task Duration data, in the small data volume scenario of this sample, the duration keeps decreasing as the number of thread blocks increases from 4 to 8, 16, and 32 in sequence (10.831 → 6.569 → 4.500 → 3.696 us). This is because the threads in a thread block are not all executed in parallel; instead, they are scheduled and executed in units of warps. When there are too many warps on a single core, queue waiting occurs. When the number of thread blocks is small, each thread block needs to launch more threads, making the waiting time more obvious. Increasing the number of thread blocks can reduce the thread count of a single thread block and increase the number of parallel cores, so the benefit is significant. When the number of thread blocks increases from 32 to 64, the duration rises from 3.696 us to 4.133 us, indicating that at this data size, continuing to increase the number of thread blocks brings scheduling fixed overhead that exceeds the parallel benefit. Therefore, in the small data size scenario, the optimal number of thread blocks must be determined through actual measurement.

[Summary] The number of thread blocks of a SIMT operator should be set by combining the number of physical cores and the data size.

The general principles are as follows:

- **In the large data volume scenario, match the number of physical cores first**: When the data volume is far greater than the magnitude of "number of physical cores × maximum thread count per core", directly use the number of physical cores as the number of thread blocks. This avoids both an insufficient number of thread blocks causing an excessive workload on a single core and an excessive number of thread blocks bringing more thread block launch and scheduling overhead.
- **In the small data volume scenario, measure the number of thread blocks**: When the data volume is comparable to or even smaller than "number of physical cores × maximum thread count per core" and the workload per core is very small, the scheduling fixed overhead brought by launching more cores will exceed the effective computation benefit. The optimal number of thread blocks cannot be predicted and must be measured using the msOpProf tool to select the number of thread blocks configuration with a lower Task Duration.
