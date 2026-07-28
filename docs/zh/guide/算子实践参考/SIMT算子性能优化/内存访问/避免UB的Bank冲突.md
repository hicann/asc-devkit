# 避免UB的Bank冲突

【优先级】高

【描述】SIMT编程模式下，一个Warp内的多个线程可能在同一条UB访问指令中同时访问UB。对于使用UB中转做转置、重排等操作的算子，即使GM读写已经连续，仍需要分析UB访问模式，避免UB内部访问成为新的性能瓶颈。

<!-- npu="950" id1 -->
下图为UB bank结构示意图。Ascend 950PR/Ascend 950DT的UB划分为16个bank，并组织为8个bank group；SIMT编程模式下，每个bank进一步划分为4个subbank。若同一个Warp内多个线程在同一条UB访问指令中访问同一个bank group的相同编号subbank，硬件需要排队处理，从而形成subbank冲突并增加访问延迟。

**图 1** bank结构示意图
![img](../../../figures/bank结构示意图.png "bank结构示意图")

SIMT编程方式下，bank冲突为更细粒度的subbank冲突，主要有以下两类：

- **写写冲突**：多个写操作同时访问同一个bank group的相同编号subbank。
- **读读冲突**：多个读操作同时访问同一个bank group的相同编号subbank。

详细的UB地址低位交织规则以及冲突场景可参考[bank_conflict样例](../../../../../../examples/03_simt_api/02_features/01_api_features/00_memory_access/bank_conflict)。
<!-- end id1 -->

<!-- npu="950" id2 -->
【样例介绍】以[矩阵转置](../../../../../../examples/03_simt_api/03_best_practices/00_memory_optimizations/matrix_transpose_practice)为例，输入矩阵shape为1024×1024，数据类型为`float`。样例采用32×32的tile。当前基础配置中，Thread Block数量固定为硬件vector core数，每个Thread Block启动1024个线程，循环处理多个32×32 tile。全局访存合并版本先用UB暂存tile，使GM读写尽量保持连续；在此基础上，避免UB Bank冲突版本通过给UB tile增加padding，调整转置读阶段的UB行跨度，减少同一个Warp内线程集中访问相同subbank资源。

【反例】矩阵转置使用UB做中转，转置读阶段产生较严重bank冲突。

```cpp
constexpr int TILE_DIM = 32;

__global__ __launch_bounds__(THREADS_PER_BLOCK) void transpose_ub_kernel(float* output, const float* input,
                                                                         uint32_t width, uint32_t height,
                                                                         uint32_t total_tiles)
{
    __ubuf__ float tile[TILE_DIM][TILE_DIM];
    for (uint32_t tile_id = blockIdx.x; tile_id < total_tiles; tile_id += gridDim.x) {
        ...
        tile[ty][tx] = input[x_index + y_index * width];
        asc_syncthreads();
        output[x_index + y_index * height] = tile[tx][ty];
        asc_syncthreads();
    }
}
```

上述实现通过UB中转保证了GM读写连续，这里的UB中的`tile`数组表示用于暂存32×32矩阵块的二维UB数组。`tile`数组的数据在UB中的排布为32×32。每行32个`float`，共128B，恰好跨越4个bank。转置写回时，同一个Warp的线程会读取UB中的一列元素，即访问`tile[tx][ty]`。由于行跨度固定为32个`float`，32个线程读取同一列时，访问地址会集中落到两个bank group的subbank 0，属于读读冲突场景。

下图展示了UB中的`tile`数组前12行元素按照行优先存储的排布，其中蓝色标记表示每行的第一个元素。按照地址低位交织规则，tile数组的第1行覆盖bank0到bank3，第2行覆盖bank4到bank7，第3行覆盖bank8到bank11，其余行依次类推。在32×32的UB tile中，同一个Warp的32个线程读取同一列时，会集中访问两个bank group的subbank 0。

<img src="../../../figures/避免Bank冲突反例.png">

在1024×1024矩阵转置样例中，该实现对应Case 5，性能数据如下：

| Task Duration(μs) | aiv_time(μs) | aiv_total_cycles | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio |
| :---------------: | :----------: | :--------------: | :--------------: | :-----------: | :-----------------: | :--------------: |
|      23.912      |   22.745    |     37528.9      |     22.297      |     0.980     |        0.435        |      0.019      |

【正例】为UB tile增加padding，打散列方向访问的bank分布。

```cpp
constexpr int TILE_DIM = 32;

constexpr int TILE_PAD = 2;
constexpr int TILE_PAD_STRIDE = TILE_DIM + TILE_PAD;

__global__ __launch_bounds__(THREADS_PER_BLOCK) void transpose_ub_pad_kernel(float* output, const float* input,
                                                                             uint32_t width, uint32_t height,
                                                                             uint32_t total_tiles)
{
    __ubuf__ float tile[TILE_DIM][TILE_PAD_STRIDE];
    for (uint32_t tile_id = blockIdx.x; tile_id < total_tiles; tile_id += gridDim.x) {
        ...
        tile[ty][tx] = input[x_index + y_index * width];
        asc_syncthreads();
        output[x_index + y_index * height] = tile[tx][ty];
        asc_syncthreads();
    }
}
```

上述实现将UB中的`tile`数组从32×32调整为32×34，仅改变UB中每行的物理跨度。增加2列padding后，每行包含34个`float`数据，行跨度变为17个subbank，同一列元素会错开排布到不同的subbank。下图为增加padding后的内存布局；从地址映射看，同一个Warp的32个线程的UB访问会分布到各个bank group的不同subbank中，即同一条访问指令下每个subbank仅有一个线程访问，从而避免上述读读冲突。

<img src="../../../figures/避免Bank冲突正例.png">

在1024×1024矩阵转置样例中，该实现对应Case 6，性能数据如下：

| Task Duration(μs) | aiv_time(μs) | aiv_total_cycles | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio |
| :---------------: | :----------: | :--------------: | :--------------: | :-----------: | :-----------------: | :--------------: |
|      15.152      |   13.641    |     22507.6      |     13.176      |     0.966     |        0.453        |      0.033      |

从Task Duration看，增加padding后的执行耗时为15.152μs，相比未加padding的UB中转版本23.912μs，下降约36.6%。这说明在GM读写已经连续的前提下，UB内部的bank冲突仍会影响算子端到端耗时；通过将UB tile从32×32调整为32×34，列方向访问被分散到各个bank group的不同subbank上，避免了上述读读冲突，因此算子的整体运行时间进一步下降。
<!-- end id2 -->

【总结】当SIMT算子已经通过UB中转解决GM非连续访问后，应继续分析UB访问是否会产生bank冲突。若同一个Warp沿列方向访问二维UB数组，且行跨度容易让访问集中到相同bank资源，可以通过增加padding、调整行跨度或重新排布数据来避免bank冲突。
