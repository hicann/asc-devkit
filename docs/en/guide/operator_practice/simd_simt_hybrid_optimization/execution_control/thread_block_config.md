# Thread Block Count Configuration Optimization

<!-- md-trans-meta sourceCommit=8d22eb82fd0b279bd5d784f696a54970c40f98ac translatedAt=2026-10-23T13:03:14.489Z pushedAt=2026-11-03T07:05:13.363Z nl=2 -->

> [!NOTE] Note
> This performance optimization suggestion applies to the following models:
     <!-- npu="950" id1 -->
> -   Ascend 950PR&950DT products
     <!-- end id1 -->
<!-- @ref: asc-devkit/res/docs/en/guide/operator_practice/simd_simt_hybrid_optimization/execution_control/thread_block_config_res.md#id1 -->

[Priority] High

[Description] When a SIMT kernel is launched, the first parameter of the kernel function call operator <<<>>> configures the number of thread blocks to be launched for this task. These thread blocks need to be executed on the Vector Cores (physical cores) that actually exist in the hardware. Due to hardware limitations, a physical core can host and execute only one thread block at a time, and the launch and scheduling of each thread block introduces fixed overhead. When the thread block count does not exceed the physical core count, all thread blocks can occupy all physical cores at once and execute in parallel. When the thread block count exceeds the physical core count, the excess thread blocks must wait until a physical core becomes idle before they can be scheduled, and the scheduling overhead increases approximately linearly with the number of excess thread blocks. Therefore, the thread block count should be set based on both the physical core count and the data scale.

The physical core count can be queried at runtime through [aclrtGetDeviceInfo](https://gitcode.com/cann/runtime/blob/9.2.0/docs/en/api_ref/04_device_management.md#aclrtGetDeviceInfo). In the following scenarios, the measured physical core count is 64.

> [!NOTE] Note
> There is no absolute boundary between "large" and "small" data scales. The large data volume and small data volume scenarios below are only representatives of two orders of magnitude. The truly reliable approach is to start from the physical core count and test it in practice.
>
> A reference intuition is: all physical cores can process approximately "physical core count x maximum thread count per core" threads in parallel at a time. When the data volume is far larger than this order of magnitude, setting the thread block count to the physical core count can fully utilize the hardware computing power. When the data volume is comparable to or even smaller than this order of magnitude, the proportion of fixed scheduling overhead in the total time becomes sensitive, and the optimal thread block count is no longer obvious, requiring measurement with performance tools.

After determining the physical core count, you also need to set the thread block count based on the data scale.

Under a large data volume, the thread block count should be equal to the physical core count. An excessive thread block count causes the thread blocks exceeding the physical core count to queue waiting, increasing the time consumed. An insufficient thread block count causes idle physical cores, insufficient parallel cores, and increased workload per core, significantly affecting the time consumed.

Under a small data volume, the workload per core is small, and the scheduling overhead introduced by launching more cores may exceed the benefit of effective parallel computation. Therefore, the optimal core count is often smaller than the physical core count.

In addition, when the number of elements actually processed per core is less than 2048, the thread count should be allocated based on the number of elements actually processed per core to avoid wasting performance on idle threads.

[Sample introduction] Take the Gather operator as an example. The operator collects data at the corresponding positions from the input tensor based on the index tensor and writes it to the output. The calculation formula is output\[i\]\[j\] = input\[index\[i\]\[j\]\]. For the complete sample, see [Thread Block Count Configuration and vf Call Optimization Sample](../../../../../../examples/05_simd_simt_hybrid/02_best_practices/simd_simt_grid_dim_config).

[Negative example]

**Negative example 1: Thread block count far exceeds the physical core count under a large data volume**

Take the Gather operator under a large data volume (the index and output shapes are \[1024, 2048\]) as an example. In this scenario, 1024 thread blocks are launched through <<<>>>, far exceeding the physical core count. The excess thread blocks must wait in queue until physical cores become idle before they can be scheduled. The code example is as follows:

```cpp
// Launch 1024 thread blocks, far exceeding the physical core count
gather_kernel<<<1024, 0, stream>>>(input_device, index_device, output_device, ...);

__global__ __vector__ void gather_kernel(...)
{
    asc_vf_call<simt_gather>(dim3(2048), ..., index_total_length);
}
```

**Negative example 2: Thread block count lower than the physical core count under a large data volume**

Under the same large data volume, 32 thread blocks are launched through <<<>>> in this scenario. The thread block count is only half of the physical core count, resulting in insufficient parallelism, and the workload processed by each thread block is doubled. The code example is as follows:

```cpp
// Launch 32 thread blocks, only half of the physical core count, resulting in insufficient parallelism
gather_kernel<<<32, 0, stream>>>(input_device, index_device, output_device, ...);
```

**Negative example 3: Thread block count equal to the physical core count under a small data volume**

Take the Gather operator with a small data volume (the index and output shapes are \[8, 2048\]) as an example. In this scenario, 64 thread blocks (equal to the physical core count) are launched through <<<>>>, but each core processes only 256 elements. The scheduling overhead introduced by increasing the core count outweighs the parallelism gain. The code example is as follows:

```cpp
// Launch 64 thread blocks (equal to the physical core count), with each core processing 256 elements.
gather_kernel<<<64, 0, stream>>>(input_device, index_device, output_device, ...);
```

[Positive example]

**Positive example 1: Thread block count equal to the physical core count under a large data volume**

Under a large data volume, 64 thread blocks (equal to the physical core count) are launched through <<<>>>. All thread blocks occupy the physical cores at once and execute in parallel without queue waiting, providing sufficient parallelism. The code example is as follows:

```cpp
// Large data volume: launch 64 thread blocks (equal to the physical core count), with each thread processing 16 elements.
gather_kernel<<<64, 0, stream>>>(input_device, index_device, output_device, ...);
```

**Positive example 2: Optimal core count measured under a small data volume, with the thread count allocated based on the actual number of elements**

Under a small data volume, the optimal core count obtained from measured performance data is 32. The thread count is allocated based on the actual number of elements processed per core, eliminating idle threads. The code example is as follows:

```cpp
// Small data volume: launch 32 thread blocks (measured optimal), with the thread count set to 512.
gather_kernel<<<32, 0, stream>>>(..., 512);
```

[Performance comparison]

**Thread block count configuration comparison for large data volume (\[1024, 2048\])**

| SCENARIO\_NUM | Thread Block Count | Elements per Core | Task Duration \(μs\) | Description |
|:---:|:---:|:---:|---:|:---|
| 9 | 1024 | 2048 | 78.921 | Thread block count ≫ physical core count, queue waiting |
| 10 | 64 | 32768 | 59.109 | Thread block count = physical core count, optimal |
| 11 | 32 | 65536 | 110.540 | Thread block count < physical core count, insufficient parallelism |

For large data volume, performance is optimal when the thread block count equals the physical core count. When the thread block count is 64, excessive thread block scheduling overhead is avoided while all physical cores are occupied at once for parallel execution. The Task Duration is 59.109 μs, which is about 25.1% lower than that with a thread block count of 1024 and about 46.5% lower than that with a thread block count of 32.

**Thread block count and thread count configuration comparison for small data volume (\[8, 2048\])**

| SCENARIO\_NUM | Thread Block Count | Thread Count | Elements per Core | Task Duration \(μs\) | Description |
|:---:|:---:|:---:|:---:|---:|:---|
| 12 | 4 | 2048 | 4096 | 10.760 | Too few cores, heavy workload per core |
| 13 | 8 | 2048 | 2048 | 6.741 | More cores, significant benefit |
| 14 | 16 | 1024 | 1024 | 4.514 | More cores, still beneficial |
| 15 | 32 | 512 | 512 | 3.866 | Optimal core count (1/2 of the physical core count) |
| 16 | 64 | 256 | 256 | 4.115 | Equal to the physical core count, scheduling overhead outweighs parallelism benefit |

For small data volume, performance is optimal when the thread block count is 32, with a Task Duration of 3.866 μs. As the thread block count increases from 4 to 32, the duration keeps decreasing; as it increases from 32 to 64, the duration rises by about 6.4%. The curve shows a "decrease-then-increase" valley shape, indicating that for small data volume the per-core workload is small, and the scheduling overhead introduced by further increasing the core count outweighs the parallelism benefit.

[Summary]

1. **For large data volume, match the physical core count first**: Limited by hardware resources, there is an upper limit on the number of physical cores that can actually execute in parallel. The thread block count should be as close to the physical core count as possible, avoiding accumulated scheduling overhead caused by too many thread blocks and insufficient parallelism caused by too few thread blocks.
2. **For small data volume, measure the thread block count performance**: For small data volume, the per-core workload is small, and the scheduling overhead introduced by launching more thread blocks outweighs the benefit. The optimal thread block count is often smaller than the physical core count. You can find the optimal thread block count through tiered testing.
