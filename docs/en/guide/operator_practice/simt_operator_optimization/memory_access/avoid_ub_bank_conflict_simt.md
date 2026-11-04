# Avoiding UB Bank Conflicts

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-23T13:12:53.840Z pushedAt=2026-11-03T07:05:13.372Z nl=2 -->

[Priority] High

[Description] In SIMT programming mode, multiple threads within a warp may access the Unified Buffer (UB) simultaneously in the same UB access instruction. For operators that use UB as an intermediate buffer for transpose, reorder, and other operations, even if GM reads and writes are already contiguous, the UB access pattern still needs to be analyzed to prevent UB internal access from becoming a new performance bottleneck.

<!-- npu="950" id1 -->
The following figure shows the UB bank structure diagram. On Ascend 950PR&950DT products, the UB is divided into 16 banks, which are organized into 8 bank groups. In SIMT programming mode, each bank is further divided into 4 subbanks. If multiple threads within the same warp access the same-numbered subbank of the same bank group in the same UB access instruction, the hardware processes these accesses in a queue, resulting in subbank conflicts and increased access latency.

**Figure 1** Bank structure diagram
![img](../../../figures/bank_struct.png "Bank structure diagram")

In SIMT programming mode, bank conflicts are finer-grained subbank conflicts, which fall into the following two categories:

- **Write-write conflict**: Multiple write operations access the same-numbered subbank of the same bank group simultaneously.
- **Read-read conflict**: Multiple read operations access the same-numbered subbank of the same bank group simultaneously.

For details about the UB address low-bit interleaving rules and conflict scenarios, see the [bank_conflict sample](../../../../../../examples/03_simt_api/02_features/01_api_features/00_memory_access/bank_conflict).
<!-- end id1 -->

<!-- npu="950" id2 -->
[Sample introduction] Take [cube transpose](../../../../../../examples/03_simt_api/03_best_practices/00_memory_optimizations/matrix_transpose_practice) as an example. The input cube has a shape of 1024×1024 and the data type is `float`. The sample uses 32×32 tiles. In the current basic configuration, the number of Thread Blocks is fixed to the number of hardware vector cores, and each Thread Block launches 1024 threads to process multiple 32×32 tiles in a loop. The global memory coalesced access version first uses the UB to buffer tiles so that GM reads and writes remain as contiguous as possible. On this basis, the version that avoids UB bank conflicts adds padding to the UB tiles and adjusts the UB row stride in the transpose read phase, thereby reducing the concentration of threads within the same warp accessing the same subbank resources.

[Negative example] The cube transpose uses UB as an intermediate buffer, and the transpose read phase incurs severe bank conflicts.

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

The preceding implementation uses UB as an intermediate buffer to keep GM reads and writes contiguous. Here, the `tile` array in UB represents a two-dimensional UB array used to temporarily store a 32x32 cube block. The data of the `tile` array is laid out in UB as 32x32. Each row contains 32 `float` values, totaling 128B, which exactly spans 4 banks. During the transpose write-back, threads in the same warp read one column of elements in UB, that is, they access `tile[tx][ty]`. Because the row stride is fixed at 32 `float` values, when 32 threads read the same column, the access addresses concentrate on subbank 0 of two bank groups, which is a read-read conflict scenario.

The following figure shows the row-major layout of the first 12 rows of the `tile` array in UB, where the blue marks indicate the first element of each row. According to the low-bit interleaving rule of addresses, row 1 of the tile array covers bank0 to bank3, row 2 covers bank4 to bank7, row 3 covers bank8 to bank11, and so on for the remaining rows. In a 32x32 UB tile, when 32 threads in the same warp read the same column, they concentrate their accesses on subbank 0 of two bank groups.

<img src="../../../figures/bank_bad.png">

In the 1024x1024 cube transpose example, this implementation corresponds to Case 5, and the performance data is as follows:

| Task Duration(μs) | aiv_time(μs) | aiv_total_cycles | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio |
| :---------------: | :----------: | :--------------: | :--------------: | :-----------: | :-----------------: | :--------------: |
|      23.912      |   22.745    |     37528.9      |     22.297      |     0.980     |        0.435        |      0.019      |

[Positive example] Add padding to the UB tile to scatter the bank distribution of column-direction accesses.

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

The preceding implementation adjusts the `tile` array in UB from 32x32 to 32x34, changing only the physical stride of each row in UB. After adding 2 columns of padding, each row contains 34 `float` values, and the row stride becomes 17 subbanks. Elements in the same column are then staggered across different subbanks. The following figure shows the memory layout after adding padding. From the address mapping perspective, the UB accesses of 32 threads in the same warp are distributed across different subbanks of each bank group, that is, under the same access instruction, each subbank is accessed by only one thread, thereby avoiding the read-read conflict described above.

<img src="../../../figures/bank_good.png">

In the 1024x1024 cube transpose example, this implementation corresponds to Case 6, and the performance data is as follows:

| Task Duration(μs) | aiv_time(μs) | aiv_total_cycles | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio |
| :---------------: | :----------: | :--------------: | :--------------: | :-----------: | :-----------------: | :--------------: |
|      15.152      |   13.641    |     22507.6      |     13.176      |     0.966     |        0.453        |      0.033      |

In terms of Task Duration, the execution time after adding padding is 15.152 μs, a decrease of about 36.6% compared with the 23.912 μs of the UB transfer version without padding. This indicates that even when GM reads and writes are already contiguous, bank conflicts inside the UB still affect the end-to-end execution time of the operator. By adjusting the UB tile from 32×32 to 32×34, column-wise accesses are distributed across different subbanks of each bank group, eliminating the read-read conflicts described above, and therefore the overall running time of the operator is further reduced.
<!-- end id2 -->

[Summary] After a SIMT operator has resolved non-contiguous GM access through UB transfer, you should continue to analyze whether UB access causes bank conflicts. If the same Warp accesses a two-dimensional UB array along the column direction and the row stride tends to concentrate accesses on the same bank resources, you can avoid bank conflicts by adding padding, adjusting the row stride, or rearranging the data.
