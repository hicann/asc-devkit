# 线程块数量配置优化

【优先级】高

【描述】SIMT编程模式下，通过`<<<...>>>`配置启动核函数的线程块数量，即`gridDim`，这些线程块最终被调度到硬件的Vector Core（物理核）上执行。由于硬件限制，一个物理核同一时刻只能驻留并执行一个线程块，每个线程块的启动和调度都会引入固定开销。当线程块数量远超物理核数时，需要启动调度的线程块数量大幅增加，调度的固定开销累积明显；线程块数量不足时，物理核闲置、并行核数不足、单核工作量增加，会显著影响耗时。此外数据规模越小、每个核工作量越少，调度固定开销占比越高，最优线程块数量会发生变化。因此，线程块数量应结合物理核数与数据规模设置。

线程块数量与物理核数的关系会影响调度开销：当线程块数量 ≤ 物理核数时，所有线程块可以一次性占用全部物理核并行执行，需要启动调度的线程块较少、固定开销有限；当线程块数量 > 物理核数时，超出物理核数的线程块需等待前序线程块执行完成后再调度，需要启动和调度的线程块数量继续增加，多线程块启动和调度的固定开销开始明显累积，调度开销随线程块数量增大而上升。

物理核数可以在运行期通过[aclrtGetDeviceInfo](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/910/API/runtimeapi/aclcppdevg_03_1867.html)查询。
> 注：在下面的两个场景中，实测的物理核数均为64。

确定物理核数后，还需结合数据规模来设置线程块数量。

> [!NOTE]说明
> 数据规模的“大”与“小”没有绝对界限，下文中两个场景只是两个量级上的代表，真正可靠的做法是以物理核数为起点实测尝试。
>
> 一个可参考的直觉是：全部物理核一次大约能并行处理“物理核数 × 单核最大线程数”个线程。数据量远大于这个量级时，将线程块数量设为物理核数就能充分发挥硬件算力；数据量与之相当甚至更小时，调度固定开销在总耗时中的占比变得敏感，最优线程块数量不再显而易见，需要使用msOpProf工具实测。

本优化方法对应的样例请参见[核函数的启动配置性能调优样例](../../../../../../examples/03_simt_api/03_best_practices/01_execution_conf_optimizations/grid_dim_config/README.md)。

## 大数据量场景
下面以Gather算子为例，对比大数据量场景（`1024*2048`，即2097152个元素）下不同线程块数量配置的性能差异。

【反例】一个线程处理一个元素，随数据量增加线程块个数，导致需要启动和调度的线程块大幅增加。

```cpp
// 基础写法：每个线程只处理1个元素，无for循环
// 要求gridDim.x * blockDim.x == 元素总数(本样例1024 * 2048 == 2097152)
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

上述实现让每个线程处理唯一一个元素，因此线程块数量为：`元素总数 / 单核线程数 = 2097152 / 2048 = 1024`，多线程块下启动和调度的固定开销明显累积。该实现的性能数据如下：

| 线程块数量 | Task Duration（us） |
| :-------: | :---------------: |
|   1024    |      78.289       |

【正例】**一个线程处理多个元素**，通过减少线程块数量，来减少调度开销。

```cpp
// 优化写法：线程以总线程数为步长跨步遍历整个数组，处理多个元素
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

// 线程块数量固定为物理核数64，每block仍取硬件上限2048线程
gather_strided_kernel<<<64, MAX_THREAD_NUM, 0, stream>>>(
    input_device, index_device, output_device,
    input_total_length, index_total_length);
```

上述实现的Gather算子功能与反例一致。只是调整线程块数量为64，每个线程块2048个线程，共启动131072个线程；为处理2097152个数据，每个线程需处理16个数据。该实现在大数据量场景下的性能数据如下：

| 线程块数量 | Task Duration（us） |
| :-------: | :---------------: |
|    64     |      58.583       |


上述两种写法的性能数据汇总如下：
| 写法 | 线程块数量 | Task Duration（us） | 
| :--: | :-------: | :---------------: | 
| **单线程多数据** | **64** | **58.583** | 
| 单线程单数据 | 1024 | 78.289 | 

根据Task Duration数据，处理相同的数据量时，采用一个线程处理多个数据写法，并将线程块数量从1024减少到64后，执行耗时从78.289us下降到58.583us，下降约25.2%，整体性能提升约1.34倍。减少线程块数量反而能提升性能，这是因为当线程块数量为1024时，超出物理核数量的线程块需要等待前序线程块执行完成后再调度，额外的启动和调度固定开销会明显增加。

## 小数据量场景

数据量较小时，最优线程块数量没有可以照搬的经验值，只能通过实测寻找。

【反例】小数据量场景下沿用“使用全部物理核”的经验，把线程块数量直接设为物理核数64。

```cpp
// kernel与大数据量场景正例完全相同(gather_strided_kernel)，仅将数据量调整为小数据量场景16384
// 直接沿用"使用全部物理核"的经验，线程块数量=64
gather_strided_kernel<<<64, MAX_THREAD_NUM, 0, stream>>>(
    input_device, index_device, output_device,
    input_total_length, index_total_length);
```

上述实现对小数据量场景也使用64个物理核，每核只处理`16384 / 64 = 256`个元素，此时计算本身耗时很短，启动64个线程块带来的调度固定开销在总耗时中占比升高。该实现在小数据量场景下的性能数据如下：

| 线程块数量 |  每个线程块的线程数 | Task Duration（us） |
| :-------: |  :---------------: | :---------------: |
|    64     |         256        |       4.133       |

【正例】小数据量场景下不直接沿用“使用全部物理核”的经验，而是设置多档线程块数量实测，取Task Duration最低点。

```cpp
// kernel不变，仅设置不同线程块数量，通过msOpProf工具实测性能
// 小数据量场景(16384)实测：线程块数量取4 / 8 / 16 / 32 / 64，最低点在线程块数量=32
gather_strided_kernel<<<32, MAX_THREAD_NUM, 0, stream>>>(
    input_device, index_device, output_device,
    input_total_length, index_total_length);
```

上述实现的kernel不变，仅在4到64之间设置不同的线程块数量并逐档实测。每个线程块的线程数取每个线程块需处理的数据量与硬件线程数上限2048中的较小值：当每个线程块需处理的数据量不小于2048时，线程数设为2048；小于2048时，线程数降为实际需处理的数据量。小数据量场景下Task Duration随线程块数量变化呈“先降后升”的趋势，本样例实测最优线程块数量为32，而非使用全部物理核的线程块数量64。

  | 线程块的数量 | 每个线程块的线程数 | Task Duration（us） |
  | :----------: | :----------------: |:---------------: |
  |      4       |        2048        |      10.831       | 
  |      8       |        2048        |       6.569       |   
  |      16      |        1024        |       4.500       |   
  |      32      |        512         |     **3.696**     |  
  |      64      |        256         |       4.133       |

下图直观展示了上表中Task Duration随线程块数量的变化趋势：

<img src="../../../figures/griddim_config_small_shape.png" alt="小数据量场景（16384） Task Duration随线程块数量的变化" width="480"/>

根据Task Duration数据，在本样例的小数据量场景下，线程块数量从4依次增加到8、16、32时耗时持续下降（10.831 → 6.569 → 4.500 → 3.696us）。这是因为线程块中的线程并不是全部在并行执行，而是以warp为单位调度执行，单核上warp过多时会产生排队等待，线程块数量较小时，每个线程块需要启动更多线程，等待时间更明显；增加线程块数量可以降低单个线程块的线程数并提高并行核数，因此收益明显。当线程块数量从32增加到64时，耗时从3.696us上升到4.133us，说明在该数据规模下继续增加线程块数量带来的调度固定开销超过并行收益。因此，小数据规模场景下需通过实测确定最优线程块数量。

【总结】SIMT算子的线程块数量配置应结合物理核数与数据规模来设置。

一般原则是：

- **大数据量场景优先匹配物理核数**：当数据量远大于“物理核数 × 单核最大线程数”这个量级时，线程块数量直接使用物理核数，既避免线程块数量不足导致单核工作量过重，也避免线程块数量过多带来更多线程块启动和调度开销。
- **小数据量场景需实测线程块数量**：当数据量与“物理核数 × 单核最大线程数”相当甚至更小、每核工作量很小时，启动更多核带来的调度固定开销会超过有效计算收益，最优线程块数量无法预判，必须通过msOpProf工具实测性能，选择Task Duration较低的线程块数量配置。
