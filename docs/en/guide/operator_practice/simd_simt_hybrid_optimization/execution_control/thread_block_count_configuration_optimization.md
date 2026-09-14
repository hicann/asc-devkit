# Thread Block Count Configuration Optimization

<!-- md-trans-meta sourceCommit=495e5c0fea55c9415e21a5b99af6b0b13483d77a translatedAt=2026-08-26T14:39:34.954Z -->

> [!NOTE] Note
> This performance optimization suggestion applies to the following models:
> -   Ascend 950PR/Ascend 950DT

[Priority] High

[Description] When a SIMT kernel is launched, the first parameter of the kernel call operator <<<>>> configures the number of thread blocks to be launched for this task. These thread blocks need to be executed on the Vector Cores (physical cores) that actually exist in the hardware. Due to hardware limitations, a physical core can only host and execute one thread block at a time, and the launch and scheduling of each thread block introduces fixed overhead. When the number of thread blocks does not exceed the number of physical cores, all thread blocks can occupy the physical cores at once and execute in parallel. When the number of thread blocks exceeds the number of physical cores, the excess thread blocks must wait until a physical core becomes idle before they can be scheduled, and the scheduling overhead increases approximately linearly with the number of excess thread blocks. Therefore, the number of thread blocks should be set based on both the number of physical cores and the data scale.

The number of physical cores can be queried at runtime through [aclrtGetDeviceInfo](https://www.hiascend.com/document/detail/en/CANNCommunityEdition/910/API/runtimeapi/aclcppdevg_03_1867.html).
> Note: In the following scenarios, the measured number of physical cores is 64.

> [!NOTE] Note
> There is no absolute boundary between "large" and "small" data scales. The large data volume and small data volume scenarios below are only representatives of two orders of magnitude. The truly reliable approach is to start from the number of physical cores and test empirically.
>
> A useful intuition is that all physical cores can process approximately "number of physical cores x maximum threads per core" threads in parallel at once. When the data volume is far larger than this order of magnitude, setting the number of thread blocks to the number of physical cores fully utilizes the hardware computing power. When the data volume is comparable to or even smaller than this, the proportion of fixed scheduling overhead in the total time becomes sensitive, and the optimal number of thread blocks is no longer obvious, requiring measurement with performance tools.

After determining the number of physical cores, you also need to set the number of thread blocks based on the data scale.

For a large data volume, the number of thread blocks should equal the number of physical cores. Too many thread blocks cause the thread blocks exceeding the number of physical cores to queue and wait, increasing the time consumed. Too few thread blocks cause physical cores to remain idle, insufficient parallel cores, and increased workload per core, significantly affecting the time consumed.

For a small data volume, the workload per core is small, and the scheduling overhead introduced by launching more cores may exceed the benefit of effective parallel computation. The optimal number of cores is often smaller than the number of physical cores.

In addition, when the number of elements actually processed per core is less than 2048, the thread count should be allocated according to the number of elements actually processed per core to avoid wasting performance on idle threads.

[Example Introduction] Take the Gather operator as an example. The operator collects data at corresponding positions from the input tensor based on the index tensor and writes it to the output. The calculation formula is output\[i\]\[j\] = input\[index\[i\]\[j\]\]. For the complete example, see [Thread Block Count Configuration and vf Call Optimization Example](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/05_simd_simt_hybrid/02_best_practices/simd_simt_grid_dim_config).

[Negative Example]

**Negative example 1: The number of thread blocks far exceeds the number of physical cores under a large data volume**

Take the Gather operator under a large data volume (the shapes of index and output are \[1024, 2048\]) as an example. In this scenario, 1024 thread blocks are launched through <<<>>>, far exceeding the number of physical cores. The excess thread blocks must queue waiting for physical cores to become idle before they can be scheduled. The code example is as follows:

```cpp
// Launch 1024 thread blocks, far exceeding the number of physical cores
gather_kernel<<<1024, 0, stream>>>(input_device, index_device, output_device, ...);

__global__ __vector__ void gather_kernel(...)
{
    asc_vf_call<simt_gather>(dim3(2048), ..., index_total_length);
}
```

**Negative example 2: The number of thread blocks is lower than the number of physical cores under a large data volume**

Under the same large data volume, 32 thread blocks are launched through <<<>>> in this scenario. The number of thread blocks is only half of the number of physical cores, resulting in an insufficient degree of parallelism, and the workload processed by each thread block doubles. The code example is as follows:

```cpp
// Launch 32 thread blocks, only half of the number of physical cores, resulting in an insufficient degree of parallelism
gather_kernel<<<32, 0, stream>>>(input_device, index_device, output_device, ...);
```

**Negative example 3: The number of thread blocks equals the number of physical cores under a small data volume**

Take the Gather operator under a small data volume (the shapes of index and output are \[8, 2048\]) as an example. In this scenario, 64 thread blocks (equal to the number of physical cores) are launched through <<<>>>, but each core processes only 256 elements. The scheduling overhead introduced by increasing the number of cores outweighs the parallelism benefit. The code example is as follows:

```cpp
// Launch 64 thread blocks (equal to the number of physical cores), with each core processing 256 elements.
gather_kernel<<<64, 0, stream>>>(input_device, index_device, output_device, ...);
```

[Positive Example]

**Positive example 1: Number of thread blocks equals the number of physical cores for large data volume**

For large data volume, launch 64 thread blocks (equal to the number of physical cores) through <<<>>>. All thread blocks occupy the physical cores at once and execute in parallel, with no queue waiting and sufficient degree of parallelism. The code example is as follows:

```cpp
// Large data volume: launch 64 thread blocks (equal to the number of physical cores), with each thread processing 16 elements.
gather_kernel<<<64, 0, stream>>>(input_device, index_device, output_device, ...);
```

**Positive example 2: Use the measured optimal number of cores for small data volume and allocate the thread count based on the actual number of elements**

For small data volume, the optimal number of cores obtained from measured performance data is 16. The thread count is allocated based on the actual number of elements processed per core, eliminating idle threads. The code example is as follows:

```cpp
// Small data volume: launch 16 thread blocks (measured optimal), with the thread count set to 1024.
gather_kernel<<<16, 0, stream>>>(..., 1024);
```

[Performance Comparison]

**Comparison of thread block count configurations for large data volume ([1024, 2048])**

| SCENARIO\_NUM | Number of Thread Blocks | Elements Processed per Core | Task Duration\(μs\) | Description |
|:---:|:---:|:---:|---:|:---|
| 9 | 1024 | 2048 | 78.533 | Number of thread blocks ≫ number of physical cores, queue waiting |
| 10 | 64 | 32768 | 62.649 | Number of thread blocks = number of physical cores, optimal |
| 11 | 32 | 65536 | 113.609 | Number of thread blocks < number of physical cores, insufficient degree of parallelism |

For large data volumes, performance is optimal when the number of thread blocks equals the number of physical cores. When the number of thread blocks is 64, excessive thread block scheduling overhead is avoided while all physical cores are occupied for parallel execution at once. The Task Duration is 62.649μs, which is about 20.2% lower than when the number of thread blocks is 1024, and about 44.8% lower than when the number of thread blocks is 32.

**Comparison of number of thread blocks and thread count for small data volume (\[8, 2048\])**

| SCENARIO\_NUM | Number of Thread Blocks | Thread Count | Elements Processed per Core | Task Duration\(μs\) | Description |
|:---:|:---:|:---:|:---:|---:|:---|
| 12 | 4 | 2048 | 4096 | 9.989 | Too few cores, heavy workload per core |
| 13 | 8 | 2048 | 2048 | 8.755 | More cores, significant benefit |
| 14 | 16 | 1024 | 1024 | 7.832 | Optimal number of cores (1/4 of the number of physical cores) |
| 15 | 32 | 512 | 512 | 7.959 | More cores, duration rises again |
| 16 | 64 | 256 | 256 | 8.820 | Equal to the number of physical cores, scheduling overhead outweighs parallelism benefit |

For small data volumes, performance is optimal when the number of thread blocks is 16, with a Task Duration of 7.832μs. As the number of thread blocks increases from 4 to 16, the duration keeps decreasing; as it increases from 16 to 64, the duration keeps increasing, forming a valley-shaped curve that first drops and then rises. When the number of thread blocks is 32, the duration increases by about 1.6%; when it is 64, the duration increases by about 10.8%. This indicates that for small data volumes, the workload per core is small, and the scheduling overhead introduced by adding more cores outweighs the parallelism benefit.

[Summary]

1. **For large data volumes, match the number of physical cores first**: Limited by hardware resources, there is an upper limit on the number of physical cores that can actually execute in parallel. The number of thread blocks should be as close as possible to the number of physical cores, avoiding excessive thread blocks that accumulate scheduling overhead, and also avoiding too few thread blocks that lead to insufficient degree of parallelism.
2. **For small data volumes, measure the performance of the number of thread blocks**: For small data volumes, the workload per core is small, and the scheduling overhead introduced by launching more thread blocks outweighs the benefit. The optimal number of thread blocks is often smaller than the number of physical cores. You can find the optimal number of thread blocks through tiered testing.
