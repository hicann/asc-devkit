# SIMD and SIMT Hybrid Programming Matrix Transpose Performance Tuning Sample

## Overview

Based on the [matrix_transpose_practice sample](../../../03_simt_api/03_best_practices/00_memory_optimizations/matrix_transpose_practice) in SIMT scenarios, this sample further optimizes the custom transpose operator by using SIMD and SIMT hybrid programming. It improves matrix transpose performance by running MTE movement and SIMT computation in parallel in hybrid programming. To show the step-by-step optimization process, this sample starts from direct global memory (GM) transpose. It then introduces Memory Transfer Engine (MTE) movement, Unified Buffer (UB) staging, and 32x32 tiling. It compares two Thread Block mapping methods: launching Thread Blocks by tile groups and fixing the Thread Block count. It reduces bank conflicts by using UB padding. Finally, it uses double buffering (Double Buffer) to run the MTE2 load, SIMT Vector Function (VF) transpose, and MTE3 store pipelines in parallel. This path shows the tuning process for SIMD and SIMT hybrid matrix transpose.

## Supported Products

- Ascend 950PR/Ascend 950DT

## Supported CANN Version

- CANN >= 9.2.0

## Directory Structure

```text
├── simd_simt_matrix_transpose
│   ├── figures                     // Image resources for README
│   ├── CMakeLists.txt              // Build project file
│   ├── matrix_transpose.asc        // SIMD and SIMT hybrid matrix transpose optimization path implementation
│   ├── README.md                   // Sample documentation
│   └── README_en.md                // English sample documentation
```

## Sample Description

- Computation formula:

  ```text
  output(x, y) = input(y, x)
  ```

  - input is the input matrix. Its shape is [H, W], and its data type is float.
  - output is the output matrix. Its shape is [W, H], and its data type is float.

- Sample specifications:

  <table>
  <tr><td rowspan="1" align="center">Sample Type (OpType)</td><td colspan="4" align="center">MatrixTranspose</td></tr>
  <tr><td rowspan="2" align="center">Sample Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">input</td><td align="center">[1024,1024]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Sample Output</td><td align="center">output</td><td align="center">[1024,1024]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">transpose_naive_kernel / transpose_ub_2tile_static_kernel / transpose_ub_2tile_loop_kernel / transpose_ub_pad_loop_kernel / transpose_ub_pad_db_kernel</td></tr>
  </table>

## Sample Implementation

### Case Implementation Description

This sample constructs an optimization path through five cases. The kernel, Thread Block count, threads per Thread Block, and the main change introduced in each case are shown in the following table.

| Case | Kernel | Thread Block Count | Threads per Thread Block | Change Introduced in This Step |
| ---- | ------ | ------------------ | ------------------------ | ------------------------------- |
| 0 | `transpose_naive_kernel` | `core` | 2048 | Directly perform transpose in GM |
| 1 | `transpose_ub_2tile_static_kernel` | `ceil(tiles / 2)` | 2048 | Introduce MTE movement, UB staging, and 32x32 tiling. Set the Thread Block count by tile groups |
| 2 | `transpose_ub_2tile_loop_kernel` | `core` | 2048 | Limit the launched core count to the physical core count. One Thread Block loops over multiple tile groups |
| 3 | `transpose_ub_pad_loop_kernel` | `core` | 2048 | Use UB padding to reduce bank conflicts during the SIMT VF transpose access phase |
| 4 | `transpose_ub_pad_db_kernel` | `core` | 2048 | Use double buffering to run the MTE2 load, SIMT VF transpose, and MTE3 store pipelines in parallel |

Here, `core` is the hardware vector core count. The runtime queries this value by using `aclrtGetDeviceInfo(ACL_DEV_ATTR_VECTOR_CORE_NUM)`. The performance data in this sample was collected in a test environment where `core=64`. `tiles` is the total tile count `(W/32) x (H/32)`. When processing a 1024x1024 matrix, the matrix is divided into 32x32 tiles, so `tiles=1024`.

#### Performance Metrics

| Metric | Description |
| ------ | ----------- |
| Task Duration(μs) | Total task duration, including scheduling time to the accelerator, execution time on the accelerator, and response completion time |
| aiv_time(μs) | Theoretical execution time of the task on the AI Vector Core, in μs |
| aiv_total_cycles | Total execution cycles on each AI Vector Core compute unit after the task is assigned |
| aiv_vec_time(μs) | Duration of vec-type instructions, in μs |
| aiv_vec_ratio | Ratio of vec-type instruction cycles to total cycles |
| aiv_scalar_time(μs) | Duration of scalar-type instructions, in μs |
| aiv_scalar_ratio | Ratio of scalar-type instruction cycles to total cycles |
| aiv_mte2_time(μs) | Duration of MTE2 load instructions, in μs |
| aiv_mte3_time(μs) | Duration of MTE3 store instructions, in μs |

Except for Task Duration, all other metrics are averages across all Thread Blocks.

### Case 0: Direct Global Memory Transpose

**Optimization Goal**: Directly perform coordinate transformation in GM.

**Core Implementation**: The SIMT VF reads `input(row, col)` and directly writes to the transposed position `output(col, row)`. The GM read is contiguous, while the write address strides across rows by column, making the write non-contiguous.

```cpp
uint32_t row = i / width;
uint32_t col = i - row * width;
// Write directly to the transposed GM address. The write direction is strided across rows.
output[col * height + row] = input[i];
```

**Performance Data**:

| Task Duration(μs) | aiv_time(μs) | aiv_total_cycles | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio |
| :---------------: | :----------: | :--------------: | :--------------: | :-----------: | :-----------------: | :--------------: |
|      36.263       |    33.620    |     55473.1      |      33.209      |     0.988     |        0.399        |      0.012       |

**Analysis**:

Case 0 keeps GM reads contiguous and writes data directly to the transposed output position. As a result, write addresses of adjacent threads in the same Warp are distributed across different rows of the output matrix. Matrix transpose involves little computation, so the main bottleneck of this version is non-contiguous GM writes. Later optimizations move the non-contiguous access from GM to UB, so that GM reads and writes become contiguous access patterns.

---

### Case 1: MTE Moves Data to UB Before Transpose Write-Back, and Each Thread Block Processes Two 32x32 Tiles

**Optimization Goal**: Introduce MTE movement, UB staging, and 32x32 tiling. This converts the non-contiguous global memory write in Case 0 into UB access and makes GM reads and writes as contiguous as possible. This case sets the Thread Block count by tile groups. Each Thread Block processes two 32x32 tiles.

**Core Implementation**: The matrix is divided into 32x32 tiles. The total tile count is `tiles=(W/32) x (H/32)`. In this case, each Thread Block launches 2048 threads and can process two tiles at the same time. Therefore, the Thread Block count is set to `ceil(tiles / 2)`. The data path is as follows: MTE2 moves GM data to the UB input buffer, the SIMT VF performs transpose in UB and writes the UB output buffer, and then MTE3 continuously stores the UB output buffer back to GM.

```cpp
__ubuf__ float in_tile[TILES_PER_BLOCK][TILE_DIM][TILE_DIM];
__ubuf__ float out_tile[TILES_PER_BLOCK][TILE_DIM][TILE_DIM];
uint32_t loop_start = block_idx * TILES_PER_BLOCK;
uint32_t loop_step = block_num * TILES_PER_BLOCK;
uint32_t loop_end = loop_start + TILES_PER_BLOCK;

// Case 1 launches Thread Blocks by tile group. Each Thread Block processes only its two assigned tiles.
for (uint32_t tile_base = loop_start; tile_base < loop_end && tile_base < total_tiles; tile_base += loop_step) {
    asc_lock(PIPE_MTE2, SINGLE_BUFFER_MUTEX);
    copy_gm_2tile_to_ub(&in_tile[0][0][0], input, width, tiles_x, tile_base, total_tiles);
    asc_unlock(PIPE_MTE2, SINGLE_BUFFER_MUTEX);

    // After MTE2 load is complete, the SIMT VF reads the UB input buffer.
    asc_lock(PIPE_V, SINGLE_BUFFER_MUTEX);
    asc_vf_call<simt_transpose_2tile>(
        dim3(TILE_DIM, TILE_DIM * TILES_PER_BLOCK, 1), &out_tile[0][0][0], &in_tile[0][0][0], tile_base,
        total_tiles);
    asc_unlock(PIPE_V, SINGLE_BUFFER_MUTEX);

    // After the SIMT VF writes the output buffer, MTE3 stores the result back to GM.
    asc_lock(PIPE_MTE3, SINGLE_BUFFER_MUTEX);
    copy_ub_2tile_to_gm(output, &out_tile[0][0][0], height, tiles_x, tile_base, total_tiles);
    asc_unlock(PIPE_MTE3, SINGLE_BUFFER_MUTEX);
}
```

**Performance Data**:

| Task Duration(μs) | aiv_time(μs) | aiv_total_cycles | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio |
| :---------------: | :----------: | :--------------: | :--------------: | :-----------: | :-----------------: | :--------------: |
|      23.732       |    2.286     |      3772.6      |      1.454       |     0.638     |        0.198        |      0.085       |

**Analysis**:

Case 1 uses UB staging to convert non-contiguous GM writes into transpose access inside UB. Task Duration decreases from 36.263μs in Case 0 to 23.732μs, a latency reduction of about 34.6%. This version launches Thread Blocks by tile groups. A 1024x1024 matrix contains 1024 tiles, which correspond to 512 Thread Blocks. This count is much larger than the hardware vector core count. Extra Thread Block launch and scheduling overhead is reflected in Task Duration. Therefore, the next step optimizes the mapping from Thread Blocks to tiles.

---

### Case 2: Limit the Launched Core Count to the Physical Core Count

**Optimization Goal**: Change the Thread Block count in Case 1 from the tile group count to the hardware vector core count. Each Thread Block loops over multiple tile groups, which reduces Thread Block launch and scheduling overhead.

**Core Implementation**: Each Thread Block launches 2048 threads with the configuration `dim3(32,64,1)`. Each loop iteration processes two contiguous 32x32 tiles at the same time. Threads with `threadIdx.y=0..31` process the first tile, and threads with `threadIdx.y=32..63` process the second tile. The next iteration jumps to the subsequent tile group by `gridDim.x * 2`.

```cpp
uint32_t loop_start = block_idx * TILES_PER_BLOCK;
uint32_t loop_step = block_num * TILES_PER_BLOCK;

// Case 2 fixes the Thread Block count. Each Thread Block loops over multiple tile groups by gridDim.x.
for (uint32_t tile_base = loop_start; tile_base < total_tiles; tile_base += loop_step) {
    asc_lock(PIPE_MTE2, SINGLE_BUFFER_MUTEX);
    copy_gm_2tile_to_ub(&in_tile[0][0][0], input, width, tiles_x, tile_base, total_tiles);
    asc_unlock(PIPE_MTE2, SINGLE_BUFFER_MUTEX);

    // After MTE2 load is complete, the SIMT VF reads the UB input buffer.
    asc_lock(PIPE_V, SINGLE_BUFFER_MUTEX);
    asc_vf_call<simt_transpose_2tile>(
        dim3(TILE_DIM, TILE_DIM * TILES_PER_BLOCK, 1), &out_tile[0][0][0], &in_tile[0][0][0], tile_base,
        total_tiles);
    asc_unlock(PIPE_V, SINGLE_BUFFER_MUTEX);

    // After the SIMT VF writes the output buffer, MTE3 stores the result back to GM.
    asc_lock(PIPE_MTE3, SINGLE_BUFFER_MUTEX);
    copy_ub_2tile_to_gm(output, &out_tile[0][0][0], height, tiles_x, tile_base, total_tiles);
    asc_unlock(PIPE_MTE3, SINGLE_BUFFER_MUTEX);
}
```

**Performance Data**:

| Task Duration(μs) | aiv_time(μs) | aiv_total_cycles | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio |
| :---------------: | :----------: | :--------------: | :--------------: | :-----------: | :-----------------: | :--------------: |
|      18.697       |    17.484    |     28849.4      |      11.559      |     0.661     |        0.846        |      0.048       |

**Analysis**:

Case 2 fixes the Thread Block count to the hardware vector core count. Compared with Case 1, which launches 512 Thread Blocks by tile groups, Case 2 launches and schedules fewer Thread Blocks. Task Duration decreases from 23.732μs to 18.697μs, a latency reduction of about 21.2%.

Compared with the matrix_transpose_practice sample in the SIMT scenario, SIMD and SIMT hybrid programming uses the MTE pipeline to handle data movement between GM and UB, while the SIMT VF only handles transpose access inside UB and writes to the output buffer. This keeps the processing logic inside the SIMT VF more focused. This case uses 2048 threads and two tiles per iteration without observed register spilling. In contrast, the matrix_transpose_practice sample uses the same fixed Thread Block count and 2048 threads in Case 4, but register spilling leads to a Task Duration of 27.113μs. After switching to 1024 threads, Case 5 of that sample is 23.912μs. The Task Duration of this case is 18.697μs, reflecting the advantages of hybrid programming in MTE movement and SIMT thread parallelism. Later versions continue to use 2048 threads and two tiles per iteration. The optimization focus then moves to UB access overhead.

---

### Case 3: Add UB Padding to Reduce UB Bank Conflicts

**Optimization Goal**: Reduce bank conflicts during the SIMT VF transpose access phase by adding UB padding on top of Case 2.

**Core Implementation**:

- The Thread Block layout and tile mapping are the same as those in Case 2.
- Each Thread Block still processes two 32x32 tiles per iteration.
- The physical layout of `in_tile` changes from 32x32 to 32x40.
- `out_tile` still uses a contiguous 32x32 layout. The SIMT VF writes it continuously along the output tile row direction, and MTE3 then stores it continuously to GM.
- The SIMT VF still processes only the valid 32x32 data.

On Ascend 950PR/Ascend 950DT, UB is divided into 16 banks and organized as 8 bank groups. Each bank is further divided into 4 subbanks. If multiple threads in the same Warp access subbanks with the same subbank ID in the same bank group in one UB access instruction inside the SIMT VF, the hardware queues the requests. This causes a subbank conflict and increases access latency.

**Figure 1: UB bank structure**

<img src="./figures/bank结构示意图.png">

When the SIMT VF accesses UB, bank conflicts are finer-grained subbank conflicts. This sample mainly involves the following two types of subbank conflicts:

- **Write-write conflict**: Multiple write operations access subbanks with the same subbank ID in the same bank group at the same time.
- **Read-read conflict**: Multiple read operations access subbanks with the same subbank ID in the same bank group at the same time.

For detailed address low-bit interleaving rules and conflict scenarios, see the [bank_conflict sample](../../../03_simt_api/02_features/01_api_features/00_memory_access/bank_conflict/README_en.md).

In Case 2, the UB `in_tile` array is stored in row-major order. According to the low-bit interleaving rule, the first row of `in_tile` covers bank0 through bank3, the second row covers bank4 through bank7, the third row covers bank8 through bank11, and the remaining rows follow the same pattern. Each row has 32 float elements and exactly spans 4 banks. As shown in Figure 2, the figure shows how the first 12 rows of the tile array are placed in UB. The first element of each row is marked in blue. During SIMT VF transpose, threads in one Warp read one column of `in_tile`. During the UB access, 32 threads are concentrated on subbank0 in two bank groups, which is a read-read conflict. `out_tile` is written continuously along the output tile row direction, so memory access merging can reduce write overhead. Therefore, `out_tile` does not need padding.

**Figure 2: Case 2 tile layout in UB**

<img src="./figures/case2_bank.png">

In SIMT scenarios, a common way to handle bank conflicts during transpose access to UB is to add padding in the column direction of the tile. This padding makes elements in the same column staggered in the physical UB address space. For a 32x32 float tile, adding one subbank width, that is, 2 columns of padding, forms a 32x34 UB layout and avoids subbank conflicts.

In SIMD and SIMT hybrid programming, MTE moves data into UB. When MTE moves a padded two-dimensional array, the address stride between adjacent rows in UB also needs to meet the alignment requirement. The row stride of a 32x34 layout is `34 * sizeof(float) = 136B`, which does not meet the 32B alignment requirement. Therefore, this sample sets the padding column count to 8 and uses a 32x40 layout. The row stride is `40 * sizeof(float) = 160B`, which staggers UB bank accesses and meets the row stride alignment requirement for MTE movement.

As shown in Figure 3, Case 3 adds 8 columns of padding to the UB tile array, changing each row from 32 elements to 40 elements. The row stride of 40 float elements is 160B, which corresponds to 20 subbanks. When the same column is accessed in the transpose direction, elements in adjacent rows are staggered by a stride of 20 subbanks in the physical UB address space. The accesses of 32 threads are no longer concentrated on subbanks with the same subbank ID in the same bank group, which reduces subbank conflicts during the SIMT VF transpose access phase.

Note that the 32x40 layout is a tradeoff for SIMD and SIMT hybrid programming after considering the alignment requirements of MTE movement. Unlike the 32x34 layout formed by adding 2 columns of padding in a SIMT scenario, the 32x40 layout may still have a small number of subbank conflicts. However, its conflict intensity is much lower than that of the 32x32 layout without padding.

**Figure 3: Case 3 tile layout in UB**

<img src="./figures/case3_bank.png">

```cpp
constexpr uint32_t TILE_PAD = 8; // The hybrid MTE movement requires 32B alignment, so a 32x40 UB layout is used.
constexpr uint32_t TILE_PAD_STRIDE = TILE_DIM + TILE_PAD;
__ubuf__ float in_tile[TILES_PER_BLOCK][TILE_DIM][TILE_PAD_STRIDE];
__ubuf__ float out_tile[TILES_PER_BLOCK][TILE_DIM][TILE_DIM];
uint32_t loop_start = block_idx * TILES_PER_BLOCK;
uint32_t loop_step = block_num * TILES_PER_BLOCK;

// Case 3 keeps the fixed Thread Block count and loop mapping. It only changes the input tile to a 32x40 padded layout.
for (uint32_t tile_base = loop_start; tile_base < total_tiles; tile_base += loop_step) {
    asc_lock(PIPE_MTE2, SINGLE_BUFFER_MUTEX);
    copy_gm_2tile_to_padded_ub(&in_tile[0][0][0], input, width, tiles_x, tile_base, total_tiles);
    asc_unlock(PIPE_MTE2, SINGLE_BUFFER_MUTEX);

    // After MTE2 load is complete, the SIMT VF reads the UB input buffer in the 32x40 padded layout.
    asc_lock(PIPE_V, SINGLE_BUFFER_MUTEX);
    asc_vf_call<simt_transpose_2tile_pad>(
        dim3(TILE_DIM, TILE_DIM * TILES_PER_BLOCK, 1), &out_tile[0][0][0], &in_tile[0][0][0], tile_base,
        total_tiles);
    asc_unlock(PIPE_V, SINGLE_BUFFER_MUTEX);

    // The output buffer uses a contiguous 32x32 layout. MTE3 stores it back to GM along the output matrix row direction.
    asc_lock(PIPE_MTE3, SINGLE_BUFFER_MUTEX);
    copy_ub_2tile_to_gm(output, &out_tile[0][0][0], height, tiles_x, tile_base, total_tiles);
    asc_unlock(PIPE_MTE3, SINGLE_BUFFER_MUTEX);
}
```

**Performance Data**:

| Task Duration(μs) | aiv_time(μs) | aiv_total_cycles | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio |
| :---------------: | :----------: | :--------------: | :--------------: | :-----------: | :-----------------: | :--------------: |
|      11.596       |    10.459    |     17258.2      |      4.465       |     0.427     |        0.853        |      0.081       |

**Analysis**:

Case 3 changes only the physical UB layout based on Case 2. It does not change GM read/write coordinates or the mapping from Thread Blocks to tiles. Compared with Case 2, Task Duration decreases from 18.697μs to 11.596μs, a latency reduction of about 38.0%. This result shows that after GM accesses become contiguous and the Thread Block count is fixed, bank conflicts during SIMT VF transpose access to UB become one of the main bottlenecks.

---

### Case 4: Use Double Buffering to Implement Pipeline Parallelism

**Optimization Goal**: Use two UB input/output buffers and multiple mutex IDs on top of Case 3, so that the MTE2 load, SIMT VF transpose, and MTE3 store pipelines run in parallel.

**Core Implementation**:

- Use two input buffers and two output buffers. Each input buffer contains two 32x40 padded tiles, and each output buffer contains two contiguous 32x32 tiles.
- Use different `mutex_id` values to manage the lifecycles of the input buffers and output buffers.
- The SIMT VF waits for the current input buffer to be loaded. Before an output buffer is reused, the SIMT VF waits for the previous MTE3 store on that output buffer to complete.

```cpp
__ubuf__ float in_tile[2][TILES_PER_BLOCK][TILE_DIM][TILE_PAD_STRIDE];
__ubuf__ float out_tile[2][TILES_PER_BLOCK][TILE_DIM][TILE_DIM];
uint32_t loop_count = 0;

for (uint32_t tile_base = curr_tile_base; tile_base < total_tiles; tile_base += loop_step) {
    uint32_t curr_buffer = loop_count & 1;
    uint8_t input_mutex = DB_INPUT_MUTEX_BASE + static_cast<uint8_t>(curr_buffer);
    uint8_t output_mutex = DB_OUTPUT_MUTEX_BASE + static_cast<uint8_t>(curr_buffer);

    asc_lock(PIPE_MTE2, input_mutex);
    copy_gm_2tile_to_padded_ub(&in_tile[curr_buffer][0][0][0], input, width, tiles_x, tile_base, total_tiles);
    asc_unlock(PIPE_MTE2, input_mutex);

    // The SIMT VF waits for the current input buffer load to complete. Before reusing an output buffer,
    // it waits for the previous MTE3 store on that buffer to complete.
    asc_lock(PIPE_V, input_mutex);
    asc_lock(PIPE_V, output_mutex);
    asc_vf_call<simt_transpose_2tile_pad>(
        dim3(TILE_DIM, TILE_DIM * TILES_PER_BLOCK, 1), &out_tile[curr_buffer][0][0][0],
        &in_tile[curr_buffer][0][0][0], tile_base, total_tiles);
    asc_unlock(PIPE_V, input_mutex);
    asc_unlock(PIPE_V, output_mutex);

    asc_lock(PIPE_MTE3, output_mutex);
    copy_ub_2tile_to_gm(output, &out_tile[curr_buffer][0][0][0], height, tiles_x, tile_base, total_tiles);
    asc_unlock(PIPE_MTE3, output_mutex);
    ++loop_count;
}
```

**Performance Data**:

| Task Duration(μs) | aiv_time(μs) | aiv_total_cycles | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio |
| :---------------: | :----------: | :--------------: | :--------------: | :-----------: | :-----------------: | :--------------: |
|       6.831       |    5.679     |      9369.6      |      4.334       |     0.764     |        0.904        |      0.159       |

**Analysis**:

Case 4 uses double buffering to provide physical isolation. This avoids MTE2 writes overwriting the input buffer that the SIMT VF is reading. It also avoids SIMT VF writes overwriting the output buffer that MTE3 is storing. Multiple mutex IDs manage the lifecycles of the two input/output buffer groups separately. The single-buffer serial barrier sequence of MTE2 load, SIMT VF transpose, and MTE3 store is changed into pipeline overlap between adjacent iterations. Compared with Case 3, Task Duration decreases from 11.596μs to 6.831μs, a latency reduction of about 41.1%. Compared with direct GM transpose in Case 0, Task Duration decreases from 36.263μs to 6.831μs, improving overall performance by about 5.31x.

Figure 4 and Figure 5 show the simulated instruction timelines of Case 3 and Case 4. Figure 4 shows that Case 3 uses a single buffer to process each tile group serially. After the MTE2 load is complete, the SIMT VF must wait until the input buffer is readable. After the SIMT VF finishes, MTE3 can store the output buffer. Only after MTE3 completes can the same UB buffer be reused by the next MTE2 load. Therefore, the MTE2, SIMT VF, and MTE3 pipelines have clear serial waits. The gaps between SIMT VF executions mainly come from data movement and synchronization.

Figure 5 shows that Case 4 uses double buffering to overlap adjacent iterations. While the current buffer is processed by the SIMT VF and MTE3, the other buffer can start the next MTE2 load in advance. MTE2, SIMT VF, and MTE3 overlap across adjacent iterations. Therefore, Case 4 hides part of the MTE movement and synchronization overhead, further reducing Task Duration. The remaining waits mainly protect buffer reuse and cross-pipeline data dependencies.

**Figure 4: Simulated instruction timeline of Case 3**

<img src="./figures/case3_trace.png">

**Figure 5: Simulated instruction timeline of Case 4**

<img src="./figures/case4_trace.png">

## Performance Comparison Summary

### Ascend 950PR Performance Data

The following table shows the performance data of each case when processing a 1024x1024 float matrix on Ascend 950PR.

| Case | Task Duration(μs) | aiv_time(μs) | aiv_total_cycles | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio |
| ---- | :---------------: | :----------: | :--------------: | :--------------: | :-----------: | :-----------------: | :--------------: |
| 0    |      36.263       |    33.620    |     55473.1      |      33.209      |     0.988     |        0.399        |      0.012       |
| 1    |      23.732       |    2.286     |      3772.6      |      1.454       |     0.638     |        0.198        |      0.085       |
| 2    |      18.697       |    17.484    |     28849.4      |      11.559      |     0.661     |        0.846        |      0.048       |
| 3    |      11.596       |    10.459    |     17258.2      |      4.465       |     0.427     |        0.853        |      0.081       |
| 4    |       6.831       |    5.679     |      9369.6      |      4.334       |     0.764     |        0.904        |      0.159       |

The main transpose path is Case 0 -> Case 1 -> Case 2 -> Case 3 -> Case 4. Task Duration decreases from 36.263μs to 23.732μs, 18.697μs, 11.596μs, and 6.831μs. Compared with direct GM transpose in Case 0, Case 4 improves performance by about 5.31x.

Matrix transpose needs to read the complete input matrix from GM and write the complete output matrix back to GM. For a 1024x1024 float matrix, the ideal total GM read/write data volume is:

$$
D = 1024 \times 1024 \times 4B \times 2 = 8.39MB
$$

If the estimate uses the 1.6TB/s GM peak bandwidth of Ascend 950PR, the theoretical lower bound is:

$$
T_{theory} = \frac{8.39MB}{1.6TB/s} \approx 5.243μs
$$

Case 0 is far from the theoretical lower bound. The main reason is that transpose write-back makes write addresses of adjacent threads in the same Warp stride across rows, so GM writes cannot form ideal contiguous accesses. Case 1 uses UB staging to move non-contiguous access into UB, improving access continuity on the GM side. However, it launches 512 Thread Blocks by tile groups, which causes high scheduling overhead. Case 2 fixes the Thread Block count and reduces launch and scheduling overhead, but SIMT VF transpose access to UB still has bank conflicts. Case 3 uses 32x40 UB padding to reduce bank conflicts, so latency decreases significantly. Case 4 further uses double buffering to run the MTE2 load, SIMT VF transpose, and MTE3 store pipelines in parallel. Task Duration decreases to 6.831μs, corresponding to an equivalent GM read/write bandwidth of about 1.23TB/s, or about 76.8% of the 1.6TB/s theoretical GM peak bandwidth of Ascend 950PR. This result is close to the theoretical bandwidth limit, but still higher than the theoretical latency lower bound of 5.243μs. The remaining gap mainly comes from non-GM-bandwidth overheads, including UB reads and writes, synchronization between MTE and V, `asc_vf_call` overhead, address calculation, and incomplete pipeline parallelism. These overheads currently have no clear further optimization method in this implementation path.

## Tuning Recommendations

When tuning memory access operators, first use the most direct implementation to expose the main gap. Then use the theoretical bandwidth limit to estimate the available optimization space. In this sample, the gap between direct transpose and the theoretical lower bound mainly comes from non-contiguous GM writes. Therefore, the optimization direction is not to increase compute parallelism, but to reorder memory accesses. In SIMD and SIMT hybrid scenarios, use 32x32 tiles as the granularity. Use MTE to continuously move each tile into the UB input buffer, access it in the transpose direction inside UB, write the UB output buffer, and use MTE3 to continuously store it to GM along the row direction of the output matrix.

After tiling, determine the mapping from Thread Blocks to tiles. Launching Thread Blocks by tile groups is simple. However, when the tile count is much larger than the hardware vector core count, this method introduces extra launch and scheduling overhead. Fixing the Thread Block count to the hardware vector core count and processing multiple tile groups in a loop inside the kernel can reduce this overhead. The thread count and the number of tiles processed by each Thread Block need to be evaluated together with register usage, stack usage, and measured latency.

After GM accesses become contiguous, the bottleneck moves to UB access and pipeline synchronization. When a 32x32 input tile is accessed by column during transpose access to UB, bank conflicts can occur. Padding can change the physical UB layout. In SIMD and SIMT hybrid programming, the layout also needs to meet the alignment requirements of MTE movement. Therefore, the input buffer in this sample uses a 32x40 padded tile. The output buffer is written continuously along the output tile row direction, so a contiguous 32x32 layout is sufficient. On this basis, double buffering rotates two UB input/output buffer groups. When the current buffer group enters the SIMT VF transpose and MTE3 store pipelines, the MTE2 pipeline can load the next batch of data into the other buffer group. This runs the MTE2 load, SIMT VF transpose, and MTE3 store pipelines in parallel and hides part of the movement overhead.

## Build and Run

In the sample root directory, perform the following steps to build and run the sample.

- Configure environment variables.

  Configure environment variables for the CANN development kit package in the current environment by referring to [Environment Variable Configuration](../../../../docs/en/quick_start.md#cann-env-setup).

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Description:** `${install_path}` is the CANN package installation directory. If no installation directory is specified, the default installation directory is `/usr/local/Ascend`.

- Run the sample.

  Run the following commands in the sample directory.

  ```bash
  SCENARIO_NUM=4                                                                     # Select a scenario. Valid values are 0-4.
  mkdir -p build && cd build;                                                        # Create and enter the build directory.
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j;  # Build the project.
  ./matrix_transpose                                                                 # Run the sample.
  ```

- Build options

  | Option | Value | Description |
  | ------ | ----- | ----------- |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture. This sample supports only dav-3510 (Ascend 950PR/Ascend 950DT). |
  | `SCENARIO_NUM` | `0`-`4` | Sample type. The default value is 4. |

  The following output indicates that the accuracy comparison is successful.

  ```text
  [Success] Case accuracy verification passed.
  ```

## Performance Analysis

### Introduction to the msOpProf Tool

`msOpProf` is a single-operator performance analysis tool. It offers two usage methods: `msopprof` and `msopprof simulator`. The tool helps users identify anomalies in operator memory, operator code, and operator instructions, enabling comprehensive operator tuning. It currently supports performance data collection and automatic parsing for different run modes (on-device or simulation) and different file types (executables or operator binary `.o` files).

- On-device performance collection

  On-device performance collection directly measures the execution time of an operator on an Ascend AI Processor. This method is suitable for quickly locating operator performance issues in an on-device environment.

  Run operator tuning on the `matrix_transpose` executable with `msopprof`:

  ```bash
  msopprof ./matrix_transpose
  ```

  - Performance data description
    After the command completes, a folder named "OPPROF_{timestamp}_XXX" is generated in the default directory. The performance data folder structure is as follows:

    ```bash
    ├──dump                       # Raw performance data; users do not need to inspect it
    ├──ArithmeticUtilization.csv  # Cube/Vector instruction cycle proportions
    ├──L2Cache.csv                # L2 Cache hit rate; affects MTE2. Plan data transfer logic properly to increase the hit rate
    ├──Memory.csv                 # Read/write bandwidth rates of UB, L1, and main memory
    ├──MemoryL0.csv               # Read/write bandwidth rates of L0A, L0B, and L0C
    ├──MemoryUB.csv               # Read/write bandwidth rates from Vector and Scalar to UB
    ├──OpBasicInfo.csv            # Basic operator information
    ├──PipeUtilization.csv        # Durations and proportions of computation and data transfer units
    ├──ResourceConflictRatio.csv  # Proportions of UB bank groups, bank conflicts, and resource conflicts among all instructions
    └──visualize_data.bin         # MindStudio Insight presentation file
    ```

  View the specific performance analysis results:

  ```bash
  # View Task Duration and various metrics
  cat ./OPPROF_*/PipeUtilization.csv
  ```

- Simulation performance collection

  Use `msopprof simulator` to perform simulation performance analysis and generate visualized instruction pipeline diagrams. Run the following commands:

  ```bash
  SCENARIO_NUM=4                                                                     # Select an execution scenario. Valid values are 0-4.
  mkdir -p build && cd build;                                                        # Create and enter the build directory.
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j;  # Build the project.
  msopprof simulator --soc-version=<soc_version> ./matrix_transpose
  ```

  > Before using simulation tuning, add the `-g` compilation option to `CMakeLists.txt` to generate debug information. This allows the simulator to collect instruction pipeline diagrams. For how to obtain `soc_version` and for more simulation tuning information, see the [simulator sample](../../../01_simd_cpp_api/01_utilities/08_simulator/README_en.md).
  >

  After the command completes, a folder named `OPPROF_{timestamp}_XXX` is generated in the current directory. The artifact structure is as follows:

  ```text
  OPPROF_{timestamp}_XXX/
  ├── dump                    // Raw performance data; users do not need to inspect it
  └── simulator
      ├── core*.veccore*/     // Simulation instruction pipeline diagram files for each vector core
      └── visualize_data.bin  // MindStudio Insight presentation file
  ```

  After the command completes, open `visualize_data.bin` in **MindStudio Insight** to view the visualized instruction pipeline diagrams.

For more information about how to use `msOpProf`, see [MindStudio Tool Tuning (msOpProf) Quick Start](https://www.hiascend.com/document/detail/zh/canncommercial/900/devaids/optool/docs/zh/quick_start/msopprof_quick_start.md).
