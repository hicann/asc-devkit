# 同一Warp内避免分支跳转

【优先级】高

【描述】SIMT编程模式下，Warp是基本的调度和执行单位，同一Warp内的32个线程执行同一条指令。流控制指令（if、switch、do、for、while）会产生分支跳转。当同一Warp内不同线程的跳转目标不一致时线程执行路径产生分歧，引起分支发散（Warp Divergence）。当发生分支发散时，只有进入当前分支的线程（活跃线程）正常执行，其余线程被掩码屏蔽等待，直至所有分支路径执行完毕后Warp内线程才能重新汇合。分支发散会增加Warp实际执行的指令总数，降低执行效率，严重影响算子性能。因此，同一Warp内的线程应尽量避免分支跳转。

**图1** 分支发散示意图

![](../../../figures/分支发散.png)

在一个线程块内，所有线程按线性顺序被硬件自动划分为每32个线程一组的Warp，当控制流条件与线程ID有关时，应尽量保证同一Warp内的线程进入相同分支，减少分支发散。例如，对于一维线程块，当控制条件仅依赖于`threadIdx.x / warpSize`时，同一Warp内的线程走相同分支路径，无分支发散。

对于for循环等循环类控制流，应尽量使同一Warp内线程的循环次数保持一致，避免因循环次数不同产生较为严重的分支发散。当同一Warp内不同线程的循环次数不一致时，先退出循环的线程处于掩码屏蔽状态等待其余线程，直至Warp中最后一个线程完成循环，造成Warp内计算资源的严重浪费。

对于一些较为简单的分支，编译器可能通过谓词（Predication）优化避免产生分支跳转。此时，所有指令都会被执行，但其最终是否生效由每个线程的谓词决定。通过这种机制，可以减少分支跳转带来的性能损失。

【样例介绍】以[SpMV Warp Divergence样例](../../../../../../examples/03_simt_api/03_best_practices/02_control_flow/warp_divergence)为例，样例中通过对比"一个线程处理一行数据"和"一个Warp协作处理一行数据"两种方式，展示Warp Divergence对性能的影响。该样例的功能为计算CSR格式存储的稀疏矩阵与稠密向量相乘。

稀疏矩阵是指大部分元素为零的矩阵。为了节省存储空间，稀疏矩阵通常采用CSR（Compressed Sparse Row）格式存储，仅保存非零元素及其位置信息。CSR格式使用3个数组：

- **values**：所有非零元素的值，按行优先顺序排列。
- **col_idx**：每个非零元素的列索引，与values一一对应。
- **row_ptr**：行偏移数组，记录每一行的非零元素在values数组中的起始和结束索引。例如，第i行的非零元素在values中的索引为`[row_ptr[i], row_ptr[i+1])`。

SpMV计算`y = A * x`，其中A为CSR格式稀疏矩阵，x为稠密向量，y为输出向量。

【反例】一个线程处理一行数据，同一Warp内线程的循环次数不一致，产生严重的Warp Divergence。

```cpp
__global__ void kernel_spmv_thread(float* values, uint32_t* col_idx,
                                   uint32_t* row_ptr, float* x,
                                   float* y, uint32_t num_rows)
{
    uint32_t row = blockIdx.x * blockDim.x + threadIdx.x;
    if (row >= num_rows) return;
    float sum = 0.0f;
    uint32_t row_start = row_ptr[row];
    uint32_t row_end = row_ptr[row + 1];
    for (uint32_t j = row_start; j < row_end; j++) {
        sum += values[j] * x[col_idx[j]];
    }
    y[row] = sum;
}
```

上述实现中，每个线程根据全局ID确定处理的行号，独立遍历该行的所有非零元素并累加乘积。同一Warp内32个线程处理32行，各行非零元素个数不同，循环次数从1到64不等。由于不同行的非零元素数量不同，同一Warp内的线程退出循环的时间不一致，产生严重的Warp Divergence。假设一个Warp中线程0处理的行非零元素个数为3，线程1处理的行非零元素个数为64，线程0在3次循环后进入空闲等待，而线程1需要执行64次循环。这种等待在每个Warp中都会发生，造成大量计算资源浪费。

反例算子的性能数据如下：

| Task Duration(μs) | aiv_total_cycles | Read Main Memory |
| :---------------: | :--------------: | :--------------: |
|      81.811       |     127124       |      39808       |

【正例】一个Warp协作处理一行数据，同一Warp内线程的循环次数基本相同，减小Warp Divergence。

```cpp
__global__ void kernel_spmv_warp(float* values, uint32_t* col_idx,
                                 uint32_t* row_ptr, float* x,
                                 float* y, uint32_t num_rows)
{
    uint32_t lane_id = laneid();
    uint32_t global_warp_id = (blockIdx.x * blockDim.x + threadIdx.x) / warpSize;
    uint32_t num_warps = (blockDim.x / warpSize) * gridDim.x;

    for (uint32_t row = global_warp_id; row < num_rows; row += num_warps) {
        float sum = 0.0f;
        uint32_t row_start = row_ptr[row];
        uint32_t row_end = row_ptr[row + 1];
        for (uint32_t j = row_start + lane_id; j < row_end; j += warpSize) {
            sum += values[j] * x[col_idx[j]];
        }
        sum = asc_reduce_add(sum);
        if (lane_id == 0) {
            y[row] = sum;
        }
    }
}
```

上述实现中，一个Warp的32个线程协作处理同一行数据，各线程以`warpSize`为步长访问该行的非零元素，循环次数基本相同。仅在最后一轮循环中部分线程可能不参与计算，Warp Divergence较小。使用[asc_reduce_add()](../../../../api/SIMT-API/Warp_functions/Warp_reduce_functions/asc_reduce_add.md)对Warp内各线程的部分和进行归约计算，由线程0写出最终结果。

正例算子的性能数据如下：

| Task Duration(μs) | aiv_total_cycles | Read Main Memory |
| :---------------: | :--------------: | :--------------: |
|      31.772       |      49727       |      2278        |

从Task Duration看，正例的执行耗时为31.772μs，相比反例的81.811μs，下降约61.1%。从Read Main Memory看，正例为2278次，相比反例的39808次，减少约94.3%。性能提升来自两方面：一是同一Warp内线程协作处理同一行数据，循环次数基本相同，Warp Divergence显著减小；二是Warp内各线程访问同一行的连续元素，实现了访存合并，读主存储器次数大幅减少。

【总结】在SIMT算子中，对于for循环等循环类控制流，应尽量使同一Warp内线程的循环次数保持一致。通过调整线程与数据的映射关系，将"一个线程处理一行数据"改为"一个Warp协作处理一行数据"，可显著减小Warp Divergence，同时实现访存合并，大幅提升算子性能。
