# Matrix Transpose Performance Tuning Example

## Overview

This example uses matrix transpose to demonstrate memory access optimization strategies in the Ascend C SIMT programming model. First, it establishes a GM contiguous read/write baseline with 1D contiguous copy and uses direct global memory transpose to expose the main cost of non-contiguous writes. Then, it introduces UB staging and 32x32 tiling to move the non-contiguous GM write into a transpose-direction UB access. Next, it compares launching Thread Blocks per tile group with fixing the Thread Block count to the hardware vector core count, and analyzes thread count selection through register spill under the 2048-thread configuration. After that, it eliminates bank conflicts during the transpose read phase through UB padding. Finally, it uses double buffering (Double Buffer) to remove the trailing synchronization in the loop, presenting the complete tuning path for SIMT matrix transpose.

## Supported Products

- Ascend 950PR/Ascend 950DT

## Supported CANN Software Version

- \>= CANN 9.1.0

## Directory Structure

```text
├── matrix_transpose_practice
│   ├── figures                     // Image resources for README
│   ├── CMakeLists.txt              // Build project file
│   ├── matrix_transpose.asc        // SIMT matrix transpose optimization path implementation
│   ├── README.md
│   └── README_en.md
```

## Example Description

- Computation Formula:

  ```text
  output(x, y) = input(y, x)
  ```

  - input is the input matrix with shape [H, W] and data type float
  - output is the output matrix with shape [W, H] and data type float
  - The contiguous copy baseline (Case 0/1) does not perform transpose; the output is verified to be equal to the input

- Example Specifications:

  <table>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center">MatrixTranspose</td></tr>
  <tr><td rowspan="2" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">input</td><td align="center">[1024,1024]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">output</td><td align="center">[1024,1024]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">copy_kernel / transpose_naive_kernel / transpose_ub_2tile_naive_kernel / transpose_ub_2tile_kernel / transpose_ub_kernel / transpose_ub_pad_kernel / transpose_ub_pad_db_kernel</td></tr>
  </table>

## Example Implementation

### Case Implementation Description

This example constructs an optimization path through 8 cases. The kernel, thread block count, threads per thread block, and the main change introduced in each case are shown in the table below.

| Case | Kernel | Thread Block Count | Threads per Thread Block | Change Introduced in This Step |
| --- | --- | --- | --- | --- |
| 0 | `copy_kernel` | `core` | 2048 | Establish a latency baseline for contiguous read/write |
| 1 | `copy_kernel` | `core` | 1024 | Only reduce the thread count, matching the launch configuration of Case 5, Case 6, and Case 7 |
| 2 | `transpose_naive_kernel` | `core` | 2048 | Directly perform transpose in GM |
| 3 | `transpose_ub_2tile_naive_kernel` | `ceil(tiles / 2)` | 2048 | Introduce UB and 32x32 tile-based tiling, setting the thread block count according to the number of partitioned tiles |
| 4 | `transpose_ub_2tile_kernel` | `core` | 2048 | Limit the launched core count to the physical core count, with each thread block looping over multiple tile groups |
| 5 | `transpose_ub_kernel` | `core` | 1024 | Reduce the thread count to avoid register spilling, with each thread block looping over multiple 32x32 tiles |
| 6 | `transpose_ub_pad_kernel` | `core` | 1024 | Use UB padding to reduce bank conflicts during the transpose read phase |
| 7 | `transpose_ub_pad_db_kernel` | `core` | 1024 | Use double buffering to reduce trailing synchronization in the loop |

Here `core` is the hardware vector core count, queried at runtime via `aclrtGetDeviceInfo(ACL_DEV_ATTR_VECTOR_CORE_NUM)`. `tiles` is the total tile count `(W/32) x (H/32)`, which is 1024 for a 1024x1024 matrix.

#### Thread Block to Tile Mapping

Starting from Case 3, the matrix is processed in 32x32 tiles. The left side of the figure below shows a 1024x1024 matrix divided into 32x32 tiles. The right side shows the thread mapping within one 32x32 tile. The in-tile column index is denoted `tx`, and the in-tile row index is denoted `ty`.

**Figure 1: Thread Block to tile mapping**

<img src="./figures/block_mapping.png" width="60%">

Taking the green element in the figure as an example, its input coordinates in GM can be derived from the tile coordinates and the local coordinates within the tile.

- The tile coordinates of the green element are `(2, 1)`. For a 1024x1024 matrix, `tiles_x = 1024 / 32 = 32`, so `tile_id = 2 * 32 + 1 = 65`.
- The local coordinates of the green element within the tile are `(2, 29)`, corresponding to `ty = 2` and `tx = 29`.

Substituting into the coordinate computation:

```cpp
uint32_t tile_y = tile_id / tiles_x;                     // 65 / 32 = 2
uint32_t tile_x = tile_id - tile_y * tiles_x;             // 65 - 2 * 32 = 1
uint32_t x_index = tile_x * TILE_DIM + tx;                // 1 * 32 + 29 = 61
uint32_t y_index = tile_y * TILE_DIM + ty;                // 2 * 32 + 2  = 66
uint32_t index = x_index + width * y_index;              // 61 + 1024 * 66
```

Therefore, the input element coordinates processed by this thread in GM are `input[66,61]`.

### Performance Metrics Description

| Metric | Description |
| --- | --- |
| Task Duration(μs) | Total task duration, including scheduling time to the accelerator, execution time on the accelerator, and response completion time |
| aiv_time(μs) | Theoretical execution time of the task on the AI Vector Core, in microseconds |
| aiv_total_cycles | Total execution cycles on each AI Vector Core compute unit |
| aiv_vec_time(μs) | Duration of vec-type instructions (vector computation instructions), in microseconds |
| aiv_vec_ratio | Ratio of vec-type instruction cycles to total cycles |
| aiv_scalar_time(μs) | Duration of scalar-type instructions (scalar computation instructions), in microseconds |
| aiv_scalar_ratio | Ratio of scalar-type instruction cycles to total cycles |

Except for Task Duration, all other metrics are averages across all thread blocks.

### Case 0 / Case 1: 1D Contiguous Copy Baseline

**Optimization Goal**: Measure the pure contiguous GM read/write data transfer capacity on the current hardware, as the performance upper bound that subsequent transpose implementations can approach.

**Core Implementation**: Treat the matrix as a 1D array and access input and output by linear addresses, keeping GM accesses contiguous. Case 0 launches 2048 threads per thread block, and Case 1 launches 1024 threads per thread block.

```cpp
__global__ __launch_bounds__(THREADS_PER_BLOCK) void copy_kernel(float* output, const float* input, uint32_t elements)
{
    uint32_t tid = blockIdx.x * blockDim.x + threadIdx.x;
    uint32_t stride = gridDim.x * blockDim.x;
    for (uint32_t i = tid; i < elements; i += stride) {
        output[i] = input[i];
    }
}
```

**Performance Data**:

| Case | Task Duration(μs) | aiv_time(μs) | aiv_total_cycles | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio |
| --- | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| 0 | 6.262 | 4.971 | 8201.7 | 4.516 | 0.909 | 0.443 | 0.089 |
| 1 | 8.649 | 7.321 | 12079.8 | 6.872 | 0.939 | 0.437 | 0.059 |

**Analysis**:

Case 0/1 only performs contiguous read/write, without coordinate swapping, UB staging, or thread block synchronization. It is mainly used to measure the contiguous GM transfer capacity on the current hardware at this data scale. Copying a 1024x1024 float matrix requires reading 4MB from GM and writing back 4MB, giving a total read/write data volume of:

$$D = 1024 \times 1024 \times 4B \times 2 = 8.39MB$$

With the GM peak bandwidth of Ascend 950PR estimated at 1.6TB/s, the theoretical latency for contiguous copy is:

$$T_{theory} = \frac{8.39MB}{1.6TB/s} \approx 5.243μs$$

The Task Duration of Case 0 is 6.262μs, corresponding to a read/write mixed bandwidth of approximately 1.34TB/s, reaching approximately 83.7% of the theoretical peak bandwidth. The Task Duration of Case 1 is 8.649μs, corresponding to a read/write mixed bandwidth of approximately 0.97TB/s, reaching approximately 60.6% of the theoretical peak bandwidth. In this contiguous read/write transfer scenario, the 2048-thread configuration is closer to the hardware contiguous transfer upper bound. The main reason why the theoretical peak bandwidth is not reached is that the actual duration includes Task scheduling and other compute instruction overhead in addition to data read/write itself. These non-transfer overheads reduce the effective bandwidth. When the thread count is smaller, each thread performs more iterations, so the extra overhead accounts for a larger proportion and bandwidth utilization is lower. Subsequent transpose cases, in addition to completing coordinate swapping, also introduce UB access and synchronization overhead, so the contiguous copy latency can serve as the performance upper bound reference for matrix transpose optimization.

---

### Case 2: Direct Global Memory Transpose

**Optimization Goal**: Directly perform coordinate transformation in GM.

**Core Implementation**: No tiling and no UB. Each thread reads `input(row, col)` and writes directly to the transposed position `output(col, row)`. The global memory read is contiguous, while the write address strides across rows by column, making it a non-contiguous write.

```cpp
uint32_t row = i / width;
uint32_t col = i - row * width;
// input(row,col) -> output(col,row)
output[col * height + row] = input[i];
```

**Performance Data**:

| Task Duration(μs) | aiv_time(μs) | aiv_total_cycles | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| 36.337 | 34.022 | 56136.2 | 33.568 | 0.987 | 0.442 | 0.013 |

**Analysis**:

Case 2 keeps the GM read contiguous while writing data directly to the transposed output position, causing write addresses of adjacent threads within the same Warp to spread across different rows of the output matrix. Compared to the Case 0 contiguous copy baseline, Task Duration increases from 6.262μs to 36.337μs, approximately 5.8x the copy baseline. Since matrix transpose itself involves very little computation, this gap mainly comes from non-contiguous GM writes. Subsequent optimizations need to transfer the non-contiguous access from GM to UB, so that GM read and write both become contiguous access patterns.

---

### Case 3: UB Staging Transpose, Each Thread Block Processes Two 32x32 Tiles

**Optimization Goal**: Introduce UB and tiling, converting the non-contiguous global memory write of Case 2 into UB-internal access, so that both GM read and write become contiguous again. This case sets the thread block count according to the number of partitioned tiles, with each thread block processing two 32x32 tiles.

**Core Implementation**: The matrix is partitioned into 32x32 tiles, with the total tile count `tiles=(W/32)x(H/32)`. In this case, each thread block launches 2048 threads and can process two tiles at the same time, so the thread block count is set to `blocks_per_grid=ceil(tiles / 2)`. The thread configuration is `dim3(32,64,1)`, where `threadIdx.y=0..31` processes the first tile and `threadIdx.y=32..63` processes the second tile; the code obtains the tile index handled by the current thread within the thread block via `local_tile=threadIdx.y>>5`, and the in-tile row index via `ty=threadIdx.y&31`. Threads first write the tile into UB in the original layout, and after [`asc_syncthreads()`](../../../../../docs/zh/api/SIMT-API/sync_and_memory_fence/sync_interface/asc_syncthreads.md) synchronization, read it from UB in the transpose direction and write it back to GM contiguously.

The core idea of UB staging is shown in the figure below: in the GM read phase, the tile is contiguously loaded into UB row by row according to the input matrix layout; in the GM writing phase, the output tile coordinates are swapped so that the same Warp writes one row of the output matrix. The originally non-contiguous GM write is transferred to a transpose-direction read within UB.

**Figure 2: Case 3 UB staging transpose data flow**

<img src="./figures/case3.png" width="60%">

The direct element-wise GM write in Case 2 needs to swap row and column coordinates, so output addresses stride across rows by column. This is the main issue with direct global memory transpose. After tiling, the processing granularity changes from a single element to a local tile: threads first contiguously load the tile into UB row by row according to the input matrix layout, then read from UB in the transpose direction, and finally write to GM contiguously row by row according to the output matrix layout. This confines the non-contiguous access introduced by transpose to within UB, while GM read and write both maintain contiguous access patterns.

The tile size is chosen as 32x32 because the 32 threads in the `threadIdx.x` direction can correspond to one row of elements within the tile, so that GM read/write access of one Warp covers 32 contiguous floats. At the same time, a 32x32 tile contains 1024 elements, making it convenient for one thread block to process one tile under the 1024-thread configuration, or two tiles under the 2048-thread configuration.

```cpp
__ubuf__ float tile[TILES_PER_BLOCK][TILE_DIM][TILE_DIM];
uint32_t local_tile = threadIdx.y >> 5;                // 0 or 1, distinguishing the two tiles in a thread block
uint32_t ty = threadIdx.y & (TILE_DIM - 1);
uint32_t tx = threadIdx.x;
uint32_t tiles_x = width / TILE_DIM;

uint32_t tile_id = blockIdx.x * TILES_PER_BLOCK + local_tile;
uint32_t tile_y = tile_id / tiles_x;
uint32_t tile_x = tile_id - tile_y * tiles_x;
uint32_t x_index = tile_x * TILE_DIM + tx;
uint32_t y_index = tile_y * TILE_DIM + ty;
tile[local_tile][ty][tx] = input[x_index + y_index * width];
asc_syncthreads();

x_index = tile_y * TILE_DIM + tx;
y_index = tile_x * TILE_DIM + ty;
output[x_index + y_index * height] = tile[local_tile][tx][ty];
```

**Performance Data**:

| Task Duration(μs) | aiv_time(μs) | aiv_total_cycles | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| 28.190 | 2.896 | 4778.1 | 2.712 | 0.938 | 0.172 | 0.058 |

**Analysis**:

Case 3 converts the non-contiguous GM write into a UB-internal transpose read through UB staging, reducing Task Duration from 36.337μs in Case 2 to 28.190μs, indicating that making GM read and write contiguous already brings gains. At this point, the new bottleneck mainly comes from the thread block mapping strategy: this implementation sets the thread block count according to the tile groups after partitioning, where 1024 tiles correspond to 512 thread blocks, significantly exceeding the hardware vector core count. The extra thread block launch and scheduling overhead is reflected in Task Duration. Therefore, the next priority is to optimize the mapping from thread blocks to tiles and reduce the number of thread blocks that need to be launched and scheduled.

---

### Case 4: Limit the Launched Core Count to the Physical Core Count

**Optimization Goal**: Change the thread block count of Case 3 from the tile group count after partitioning to the hardware vector core count, with each thread block processing multiple tile groups in a loop, and observe register usage under the 2048-thread configuration.

**Core Implementation**: Each thread block launches 2048 threads with configuration `dim3(32,64,1)`. Each loop iteration processes two contiguous 32x32 tiles simultaneously: `threadIdx.y=0..31` processes the first tile, and `threadIdx.y=32..63` processes the second tile; the next iteration processes the subsequent tile group by `gridDim.x*2`.

```cpp
__ubuf__ float tile[TILES_PER_BLOCK][TILE_DIM][TILE_DIM];
...
tile[local_tile][ty][tx] = input[x_index + y_index * width];
asc_syncthreads();
output[x_index + y_index * height] = tile[local_tile][tx][ty];
asc_syncthreads();
```

**Performance Data**:

| Task Duration(μs) | aiv_time(μs) | aiv_total_cycles | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| 27.113 | 25.043 | 41321.5 | 24.581 | 0.982 | 0.450 | 0.018 |

**Analysis**:

Case 4 fixes the thread block count to the hardware vector core count. Compared to Case 3 launching 512 thread blocks, Case 4 launches and schedules fewer thread blocks, reducing Task Duration from 28.190μs to 27.113μs. After adding the `--cce-res-usage` compilation option, register and stack usage can be seen in the compilation log. The output for Case 4 is as follows:

```text
[BISHENG] Function properties for _Z25transpose_ub_2tile_kernelPfPKfjjj_simt_entry: Stack size: 24 bytes, Used register number: 16
```

This output indicates that Case 4 already has register spill. Both Case 4 and Case 3 use the 2048-thread configuration, but after Case 4 fixes the thread block count to the hardware vector core count, it needs to loop over multiple tile groups inside the kernel, and the index update logic is more complex than Case 3, requiring more intermediate variables. At the same time, `__launch_bounds__(2048)` limits each thread to at most 16 registers; once registers are insufficient, some variables spill to the stack, introducing extra memory access. Register spill offsets the gains from fixing the thread block count. Case 5 switches to the 1024-thread configuration, with one thread block processing only one tile per iteration, to first eliminate register spill.

---

### Case 5: Reduce the Thread Count to Avoid Register Spilling

**Optimization Goal**: Switch to 1024 threads, adjusting the per-iteration granularity to one 32x32 tile, establishing the baseline configuration for subsequent optimizations.

**Core Implementation**: Based on Case 4, each thread block launches 1024 threads with configuration `dim3(32,32,1)`. One 32x32 tile contains 1024 elements, so one thread block processes exactly one tile per loop iteration; the next iteration processes the subsequent tile by `gridDim.x`.

```cpp
__ubuf__ float tile[TILE_DIM][TILE_DIM];
...
tile[ty][tx] = input[x_index + y_index * width];
asc_syncthreads();
output[x_index + y_index * height] = tile[tx][ty];
asc_syncthreads();
```

**Performance Data**:

| Task Duration(μs) | aiv_time(μs) | aiv_total_cycles | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| 23.912 | 22.745 | 37528.9 | 22.297 | 0.980 | 0.435 | 0.019 |

**Analysis**:

After switching to 1024 threads, the Stack size drops to zero and there is no longer spill. Task Duration is 23.912μs, better than the 2048-thread version of Case 4, indicating that 1024 threads is more suitable for this transpose implementation. At this point aiv_time is already close to Task Duration, and thread block launch and scheduling overhead accounts for a small share. Compared to the direct transpose of Case 2 (36.337μs), UB staging converts the non-contiguous global memory write into UB-internal access, and combined with the loop processing of a fixed thread block count, latency decreases by approximately 34%.

In the current configuration, register spill has been eliminated and thread block launch and scheduling overhead already accounts for a small share, so the next issue to focus on is the UB-internal access overhead. During the GM writing phase, the kernel needs to read `tile[tx][ty]` in the transpose direction, that is, access the 32x32 UB tile by column. This is prone to bank conflicts, so subsequent optimization should first address UB bank conflicts during the transpose read phase.

---

### Case 6: Add UB Padding to Reduce UB Bank Conflicts

**Optimization Goal**: Based on Case 5, eliminate bank conflicts during the transpose read phase through UB padding.

**Core Implementation**:

- The GM-to-UB phase is identical to Case 5.
- The only difference is the UB layout, changed from 32x32 to 32x34.
- After synchronization, the UB-to-GM writing path is identical to Case 5.
- This version does not change the algorithm path, nor the thread block and thread partitioning; it only adjusts the physical layout in UB.

The following uses the UB partitioning rules of Ascend 950PR/Ascend 950DT as an example to explain how bank conflicts arise in this example, and the theoretical conflict intensity differences between Case 5 and Case 6.

The bank partitioning in UB is shown in the figure below. The UB of Ascend 950PR/Ascend 950DT is divided into 16 banks and organized as 8 bank groups; in SIMT programming mode, each bank is further divided into 4 subbanks. If multiple threads in the same Warp access subbanks with the same index in the same bank group within one UB access instruction, the hardware needs to queue these requests, forming subbank conflicts and increasing access latency.

In SIMT programming, bank conflicts are more fine-grained subbank conflicts, mainly including the following two types:

- **Write-write conflict**: Multiple write operations access subbanks with the same index in the same bank group at the same time.
- **Read-read conflict**: Multiple read operations access subbanks with the same index in the same bank group at the same time.

For detailed low-bit UB address interleaving rules and conflict scenarios, see the [bank_conflict example](../../../02_features/01_api_features/00_memory_access/bank_conflict).

**Figure 3: UB bank structure**

<img src="./figures/bank结构示意图.png">

In Case 5, the tile array in UB is stored in row-major order. According to the low-bit address interleaving rule, the first row of the tile array covers bank0 through bank3, the second row covers bank4 through bank7, the third row covers bank8 through bank11, and the remaining rows follow the same pattern. Each row of 32 float elements spans exactly 4 banks. In Case 5, one Warp of threads reads one column of the tile and writes it to the GM output matrix; when accessing UB, the 32 threads concentrate on subbank 0 in two bank groups, which is a read-read conflict scenario.

**Figure 4: Case 5 tile layout in UB**

<img src="./figures/case5_bank.png">

In Case 6, two columns of padding are added to the tile array in UB, changing from 32 elements per row to 34 elements per row. The layout in UB is shown in the figure below. Since each row has 34 elements, the row stride becomes 17 subbanks, so elements in the same column are staggered across different subbanks. When Case 6 accesses UB, the accesses from 32 threads are distributed across each subbank of each bank group. In other words, each subbank is accessed by only one thread under the same access instruction, avoiding the read-read conflict described above.

**Figure 5: Case 6 tile layout in UB**

<img src="./figures/case6_bank.png">

```cpp
constexpr int TILE_PAD = 2;                          // Number of UB padding columns in Case 6/7 (32x34)
constexpr int TILE_PAD_STRIDE = TILE_DIM + TILE_PAD;
__ubuf__ float tile[TILE_DIM][TILE_PAD_STRIDE];
...
tile[ty][tx] = input[x_index + y_index * width];
asc_syncthreads();
output[x_index + y_index * height] = tile[tx][ty];
asc_syncthreads();
```

**Performance Data**:

| Task Duration(μs) | aiv_time(μs) | aiv_total_cycles | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| 15.152 | 13.641 | 22507.6 | 13.176 | 0.966 | 0.453 | 0.033 |

**Analysis**:

Compared to 23.912μs in Case 5, padding reduces Task Duration to 15.152μs, a decrease of approximately 37%. This optimization only adjusts the UB physical layout, without changing the GM read/write coordinates or the algorithm path, indicating that bank conflicts during the transpose read phase are the main bottleneck of Case 5. After eliminating bank conflicts, each loop iteration still retains two synchronizations, one after data loading and one after GM writing. The synchronization overhead can be further reduced.

---

### Case 7: Use Double Buffering to Reduce Synchronization Overhead

**Optimization Goal**: Remove the `asc_syncthreads()` at the end of each loop iteration in Case 6, reducing synchronization overhead.

**Core Implementation**: Each loop iteration in Case 6 has two synchronizations: one after data loading into UB, to ensure that the current tile is fully written to UB before the transpose-direction read starts; and one after GM writing, to ensure that the transpose read of the current tile is complete before the next data loading phase starts. This prevents the next data loading phase from overwriting the UB tile that is still being read. This version uses two padded UB tiles and selects the buffer for the current iteration by `cnt`. After the trailing synchronization is removed, the GM writing phase of the current iteration can overlap with the address computation before data loading and the GM read phase of the next iteration. Therefore, only the synchronization after data loading is retained in each iteration.

```cpp
__ubuf__ float tile[2][TILE_DIM][TILE_PAD_STRIDE]; // two-buffer rotation: rotate buffers and remove trailing synchronization
uint32_t cnt = 0;
for (uint32_t tile_id = blockIdx.x; tile_id < total_tiles; tile_id += gridDim.x) {
    ...
    tile[cnt][ty][tx] = input[x_index + y_index * width];
    asc_syncthreads(); // Keep the synchronization after data loading
    output[x_index + y_index * height] = tile[cnt][tx][ty];
    cnt ^= 1; // Switch to the next buffer
}
```

**Performance Data**:

| Task Duration(μs) | aiv_time(μs) | aiv_total_cycles | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| 12.074 | 10.725 | 17696.8 | 10.285 | 0.959 | 0.428 | 0.040 |

**Analysis**:

Compared to 15.152μs in Case 6, after removing the synchronization at the end of each loop iteration, Task Duration drops to 12.074μs, a decrease of approximately 20%. Under the 1024x1024 matrix and 64 thread block configuration, each thread block needs to loop over 16 tiles, so double buffering is equivalent to each thread block reducing 16 thread block synchronizations. The cost of this method is an extra UB tile (two 32x34 tiles in total), but it does not introduce register spill. Case 7 is the lowest-latency SIMT transpose version in this example, approximately 1.9x the Case 0 copy baseline (6.262μs).

The simulation instruction pipeline diagrams of Case 6 and Case 7 are shown in Figure 6 and Figure 7, respectively. The most time-consuming instructions, SIMT_LDG and SIMT_STG, are the instructions for reading data from GM and writing data to GM in SIMT programming mode. In Case 6, each loop iteration is constrained by the trailing `asc_syncthreads()`. The next iteration can start only after the GM writing phase of the current iteration is complete, so the address computation before data loading and the GM read phase of the next iteration must execute serially. In Case 7, double buffering rotates two UB tiles and removes the trailing synchronization. As a result, the GM writing phase of the current iteration can overlap with the address computation before data loading and the GM read phase of the next iteration, reducing the synchronization wait between the data loading phase and the address computation phase. Therefore, the Task Duration of Case 7 further decreases compared with Case 6.

**Figure 6: Case 6 simulation instruction pipeline diagram**

<img src="./figures/case6_trace.png">

**Figure 7: Case 7 simulation instruction pipeline diagram**

<img src="./figures/case7_trace.png">

## Performance Comparison Summary

### Ascend 950PR Performance Data

| Case | Task Duration(μs) | aiv_time(μs) | aiv_total_cycles | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio |
| --- | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| 0 | 6.262 | 4.971 | 8201.7 | 4.516 | 0.909 | 0.443 | 0.089 |
| 1 | 8.649 | 7.321 | 12079.8 | 6.872 | 0.939 | 0.437 | 0.059 |
| 2 | 36.337 | 34.022 | 56136.2 | 33.568 | 0.987 | 0.442 | 0.013 |
| 3 | 28.190 | 2.896 | 4778.1 | 2.712 | 0.938 | 0.172 | 0.058 |
| 4 | 27.113 | 25.043 | 41321.5 | 24.581 | 0.982 | 0.450 | 0.018 |
| 5 | 23.912 | 22.745 | 37528.9 | 22.297 | 0.980 | 0.435 | 0.019 |
| 6 | 15.152 | 13.641 | 22507.6 | 13.176 | 0.966 | 0.453 | 0.033 |
| 7 | 12.074 | 10.725 | 17696.8 | 10.285 | 0.959 | 0.428 | 0.040 |

**Overall Optimization Effect**:

- For the main transpose path Case 2 -> Case 5 -> Case 6 -> Case 7, Task Duration decreases from 36.337μs to 23.912μs, 15.152μs, and 12.074μs in turn, an improvement of approximately 3.01x over Case 2.
- The lowest-latency Case 7 is approximately 1.93x the Case 0 copy baseline. The gap mainly comes from UB staging, thread block synchronization, and UB access overhead.

## Tuning Recommendations

When tuning memory-access operators, use contiguous copy first to confirm the GM transfer upper bound at the current data scale, and then use the most direct implementation to expose the main performance gap. In this example, the gap between direct transpose and the copy baseline mainly comes from non-contiguous GM writes. Therefore, the optimization direction is not to increase compute parallelism, but to reorganize memory access: contiguously load data into UB by 32x32 tiles, read data in the transpose direction within UB, and then write data to GM contiguously by rows of the output matrix.

After tiling, first determine the mapping from Thread Blocks to tiles. Launching Thread Blocks by partitioned tile groups is simple, but when the tile count is much larger than the hardware vector core count, it introduces extra launch and scheduling overhead. Fixing the Thread Block count to the hardware vector core count and processing multiple tiles in a loop inside the kernel can reduce this overhead. The thread count cannot be selected only according to the contiguous transfer scenario. A high thread count limits the number of registers available to each thread. When the index logic becomes more complex, register spill may occur, so select the thread count based on both `--cce-res-usage` and measured latency.

After GM access becomes contiguous, the bottleneck shifts to UB access and synchronization. When UB is read in the transpose direction, column-wise access to a 32x32 tile can easily cause bank conflicts. Padding can change the physical UB layout. Synchronization in the loop also needs to be judged based on data overwrite relationships. Double buffering can reduce one synchronization without overwriting data that is still being read.

## Build and Run

Run the following steps in the root directory of this example to build and execute the example.

- Configure Environment Variables  
  Configure environment variables based on the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the Example

  Run the following commands in this example directory.

  ```bash
  SCENARIO_NUM=7                                                                     # Select the execution scenario, options 0-7
  mkdir -p build && cd build;                                                        # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j;  # Build the project
  ./matrix_transpose                                                        # Run the example
  ```

- Build Options Description

  | Option                      | Values      | Description                                                        |
  | --------------------------- | ----------- | ------------------------------------------------------------------ |
  | `CMAKE_ASC_ARCHITECTURES`   | `dav-3510`  | NPU architecture: this example only supports dav-3510 (Ascend 950PR/Ascend 950DT) |
  | `SCENARIO_NUM`              | `0`-`7`     | Example type, default is 7                                         |

  - `--cce-res-usage` outputs compilation resource information such as register and stack usage, making resource usage easier to analyze.

  The following output indicates that the accuracy verification is successful.

  ```text
  [Success] Case accuracy verification passed.
  ```

## Performance Analysis

Use the `msOpProf` tool to collect detailed performance data:

```bash
msopprof ./matrix_transpose   # Analyze case performance
```

After the command completes, a folder named `OPPROF_{timestamp}_XXX` is generated in the output directory. The performance data folder structure example is as follows:

```text
├── dump                       // Raw performance data
├── ArithmeticUtilization.csv  # cube/vector instruction cycle ratio
├── L2Cache.csv                # L2 Cache hit rate
├── Memory.csv                 # UB, L1, and main memory read/write bandwidth
├── MemoryL0.csv               # L0A, L0B, and L0C read/write bandwidth
├── MemoryUB.csv               # Vector and Scalar to UB read/write bandwidth
├── OpBasicInfo.csv            # Operator basic information, including Task Duration(μs)
├── PipeUtilization.csv        # Compute unit and transfer unit duration and ratio
├── ResourceConflictRatio.csv  # UB bank group, bank conflict, and resource conflict ratio
└── visualize_data.bin         # MindStudio Insight presentation file
```

## Simulation Tuning

Use `msopprof simulator` to perform simulation performance analysis and generate visualized instruction pipeline diagrams. The commands are as follows:

```bash
SCENARIO_NUM=7                                                                     # Select the execution scenario, options 0-7
mkdir -p build && cd build;                                                        # Create and enter the build directory
cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j;  # Build the project
msopprof simulator --soc-version=<soc_version> ./matrix_transpose
```

> Before using simulation tuning, add the `-g` compilation option to CMakeLists.txt to generate debug information. This allows the simulator to collect instruction pipeline diagrams. For how to obtain `soc_version` and for more simulation tuning information, see the [simulator sample](../../../01_utilities/07_simulator).

After the command completes, a folder named `OPPROF_{timestamp}_XXX` is generated in the current directory. The artifact structure is as follows:

```text
OPPROF_{timestamp}_XXX/
├── dump                    // Raw performance data, which does not require user attention
└── simulator
    ├── core*.veccore*/     // Simulation instruction pipeline diagram files of each vector core
    └── visualize_data.bin  // MindStudio Insight presentation file
```

After the command completes, open `visualize_data.bin` in **MindStudio Insight** to view the visualized instruction pipeline diagrams.

For more information about how to use `msOpProf`, see [MindStudio Tool Tuning (msOpProf) Quick Start](https://www.hiascend.com/document/detail/zh/canncommercial/900/devaids/optool/docs/zh/quick_start/msopprof_quick_start.md).
