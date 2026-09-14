# Avoiding UB Bank Conflicts

<!-- md-trans-meta sourceCommit=d0ede7621d11bdc88cdafb221560ec6e0bfe996d translatedAt=2026-08-26T14:46:20.311Z -->

[Priority] High

[Description] In SIMT programming mode, multiple threads within a warp may access the UB simultaneously in the same UB access instruction. For operators that use the UB as an intermediate buffer for operations such as transpose and rearrangement, even if GM reads and writes are already contiguous, the UB access pattern still needs to be analyzed to prevent UB internal access from becoming a new performance bottleneck.

Take Ascend 950PR/Ascend 950DT as an example. The following figure shows the UB bank structure diagram. The total UB size is 256 KB, divided into 16 banks, each containing 512 rows of 32 B each, totaling 16 KB. These 16 banks are further organized into 8 bank groups, each containing 2 banks. Bank i and bank i+8 belong to bank group i, that is, `bank_group_id = bank_id % 8`. In SIMT programming mode, each bank is further divided into 4 subbanks, each with a width of 8 B.

In SIMT scenarios, multiple threads within the same warp may access the UB simultaneously in the same UB access instruction. When the accessed data belongs to subbank resources with the same number in the same bank group, the hardware cannot process all requests in one cycle and must queue them, resulting in subbank conflicts and increased access latency. If the accessed data falls within the 8 B address range of the same row of the same subbank, the hardware merges these requests and no subbank conflict occurs.

**Figure 1** Bank structure diagram
![img](../../../figures/bank_structure_diagram.png "Bank structure diagram")

UB addresses use low-bit interleaving, as shown in the following figure. Consecutive addresses are mapped to bank0 through bank15 at a granularity of 32 B: the first 32 B address segment maps to bank0, the second to bank1, and so on; the 16th maps to bank15, and the 17th maps back to the next row of bank0.

**Figure 2** Bank memory layout diagram
![img](../../../figures/bank_memory_layout_diagram.png "Bank memory layout diagram")

In SIMT programming mode, subbank conflicts mainly include the following types:

- **Write-write conflict**: Multiple write operations attempt to access subbanks with the same number in the same bank group simultaneously.
- **Read-read conflict**: Multiple read operations simultaneously attempt to access the subbank with the same number in the same bank group.

[Example Introduction] Take [matrix transpose](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/03_simt_api/03_best_practices/00_memory_optimizations/matrix_transpose_practice) as an example. The input matrix has a shape of 1024×1024 and the data type is `float`. The example uses a 32×32 tile, with each ThreadBlock responsible for processing one tile. The ThreadBlock is configured as `(32, 32, 1)`, and each thread processes one element in the tile. In the coalesced global memory access version, UB is first used to temporarily store the tile so that GM reads and writes remain as continuous as possible. On this basis, the version that avoids UB Bank conflicts adds padding to the UB tile and adjusts the UB row stride in the transpose read phase, thereby reducing the concentration of threads within the same warp accessing the same subbank resource.

[Negative Example] Matrix transpose uses UB as an intermediate buffer, and the transpose read phase produces relatively severe Bank conflicts.

```
constexpr int TILE_DIM = 32;

__global__ void transpose_coalesced_kernel(float *output, const float *input, int width, int height)
{
    __ubuf__ float tile[TILE_DIM][TILE_DIM];

    int x_index = blockIdx.x * TILE_DIM + threadIdx.x;
    int y_index = blockIdx.y * TILE_DIM + threadIdx.y;
    int index_in = x_index + y_index * width;

    tile[threadIdx.y][threadIdx.x] = input[index_in];
    asc_syncthreads();

    x_index = blockIdx.y * TILE_DIM + threadIdx.x;
    y_index = blockIdx.x * TILE_DIM + threadIdx.y;
    int index_out = x_index + y_index * height;

    output[index_out] = tile[threadIdx.x][threadIdx.y];
}
```

The preceding implementation uses UB as an intermediate buffer to ensure continuous GM reads and writes. Here, the `tile` array in UB represents a two-dimensional UB array used to temporarily store a 32×32 matrix block. The data of the `tile` array is laid out in UB as 32×32. Each row contains 32 `float` values, totaling 128B, which exactly spans 4 banks. During the transpose write-back, threads in the same warp read one column of elements from UB, that is, they access `tile[threadIdx.x][threadIdx.y]`. Since the row stride is fixed at 32 `float` values, the 32 elements in the same column concentrate their access on subbank 0 of two bank groups, which is a read-read conflict scenario.

The following figure shows the row-major memory layout of the first 10 rows of the `tile` array in UB, where the blue marks indicate the first element of each row. According to the low-bit address interleaving rule, row 1 of the tile array covers bank0 to bank3, row 2 covers bank4 to bank7, row 3 covers bank8 to bank11, and so on for the remaining rows. In a 32×32 UB tile, when the 32 threads in the same warp read the same column, they concentrate their access on subbank 0 of two bank groups.

<img src="../../../figures/avoid_bank_conflict_negative_example.png">

In the 1024×1024 matrix transpose example, the performance data of this implementation is as follows:

| Task Duration(μs) | aiv_time(μs) | aiv_total_cycles | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio |
| :---------------: | :----------: | :--------------: | :--------------: | :-----------: | :-----------------: | :--------------: |
|      31.926      |    1.606    |     2650.112     |      1.442      |     0.900     |        0.152        |      0.092      |

[Positive Example] Add padding to the UB tile to disperse the bank distribution of column-direction access.

```
constexpr int TILE_DIM = 32;

__global__ void transpose_avoid_bank_conflicts_kernel(float *output, const float *input, int width, int height)
{
    __ubuf__ float tile[TILE_DIM][TILE_DIM + 2];

    int x_index = blockIdx.x * TILE_DIM + threadIdx.x;
    int y_index = blockIdx.y * TILE_DIM + threadIdx.y;
    int index_in = x_index + y_index * width;

    tile[threadIdx.y][threadIdx.x] = input[index_in];
    asc_syncthreads();

    x_index = blockIdx.y * TILE_DIM + threadIdx.x;
    y_index = blockIdx.x * TILE_DIM + threadIdx.y;
    int index_out = x_index + y_index * height;

    output[index_out] = tile[threadIdx.x][threadIdx.y];
}
```

The preceding implementation adjusts the `tile` array in UB from 32×32 to 32×34. The algorithm, ThreadBlock partitioning, thread mapping, and GM access method remain unchanged; only the physical stride of each row in UB is changed. After adding 2 columns of padding, each row contains 34 `float` values, and the row stride becomes 17 subbanks. Elements in the same column are then staggered across different subbanks. The following figure shows the memory layout after padding is added. From the perspective of address mapping, the UB accesses of the 32 threads in the same warp are distributed across different subbanks of each bank group. That is, under the same access instruction, each subbank is accessed by only one thread, thereby avoiding the read-read conflict described above.

<img src="../../../figures/avoid_bank_conflict_positive_example.png">

In the 1024×1024 matrix transpose sample, the performance data of this implementation is as follows:

| Task Duration(μs) | aiv_time(μs) | aiv_total_cycles | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio |
| :---------------: | :----------: | :--------------: | :--------------: | :-----------: | :-----------------: | :--------------: |
|      23.483      |    1.055    |     1740.038     |      0.883      |     0.847     |        0.151        |      0.138      |

In terms of task duration, the execution time after adding padding is 23.483 μs, a decrease of about 26.4% compared with the 31.926 μs of the UB transfer version without padding. This indicates that, even when GM reads and writes are already continuous, bank conflicts inside the UB still affect the end-to-end duration of the operator. By adjusting the UB tile from 32×32 to 32×34, column-wise accesses are distributed to different subbanks of each bank group, avoiding the read-read conflicts described above, so the overall running time of the operator is further reduced.

[Summary] After a SIMT operator has resolved non-contiguous GM access through UB transfer, you should continue to analyze whether UB access causes bank conflicts. If the same warp accesses a two-dimensional UB array along the column direction and the row stride tends to concentrate accesses on the same bank resources, you can avoid bank conflicts by adding padding, adjusting the row stride, or rearranging the data.
