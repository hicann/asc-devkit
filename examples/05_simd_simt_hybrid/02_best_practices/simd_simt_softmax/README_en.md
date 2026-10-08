# SIMD and SIMT Hybrid Programming Softmax Performance Tuning Sample

## Overview

This sample uses softmax as an example to introduce tuning methods for in-row reduction operators in SIMD and SIMT hybrid programming. It applies three optimization techniques in sequence: a baseline where the SIMT VF reads GM directly, MTE-based UB staging with double buffering to overlap data movement and computation, and keeping the intermediate values of a whole row in registers to eliminate the repeated reads and repeated computation in the in-row computation.

## Supported Products and CANN Versions

| Product | CANN Version |
|------|-------------|
| Ascend 950PR&950DT products | >= CANN 9.2.0 |

## Directory Structure

```text
├── simd_simt_softmax
│   ├── CMakeLists.txt                  // Build project file
│   ├── data_utils.h                    // Host-side bin file read/write utility
│   ├── figures                        // Image resources for README
│   ├── scripts
│   │   ├── gen_data.py                 // Input and golden data generation script
│   │   └── verify_result.py            // Golden data comparison script
│   ├── softmax_gm.h                    // Case 0: baseline implementation where SIMT reads GM directly
│   ├── softmax_ub.h                    // Case 1: UB staging and MTE double buffering implementation
│   ├── softmax_bucket.h                // Case 2: implementation with intermediate values in registers
│   ├── softmax_host.asc                // Host-side running entry
│   ├── README.md                       // Chinese sample documentation.
│   └── README_en.md                    // English sample documentation.
```

## Sample Description

- Computation formula:

  $$
  y_{i,j} = \frac{\exp(x_{i,j} - \max_{k}{x_{i,k}})}{\sum_{k}\exp(x_{i,k} - \max_{k}{x_{i,k}})}
  $$

  where $x_{i,j}$ is the element at row $i$ and column $j$ of the input, $y_{i,j}$ is the corresponding output, $\max_{k}{x_{i,k}}$ is the maximum over all columns of row $i$ ($k$ iterates over all columns of the row), and $\sum_{k}$ denotes the sum over all columns of row $i$.

- Computation rule:

  Softmax is computed row by row, and rows are independent of each other. For each row: first compute the in-row maximum, then subtract the maximum from each element and apply the exponential function, sum the exponential results across the row, and finally divide each exponential value by the row sum to obtain the normalized output.

- Sample specifications:

  <table>
  <tr><td rowspan="1" align="center">Sample Type (OpType)</td><td colspan="4" align="center">Softmax</td></tr>
  <tr><td rowspan="2" align="center">Sample Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">input</td><td align="center">[8192, 197]</td><td align="center">float16</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Sample Output</td><td align="center">output</td><td align="center">[8192, 197]</td><td align="center">float16</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">softmax_gm_kernel / softmax_ub_kernel / softmax_bucket_kernel</td></tr>
  </table>

  > **Input Constraint:** The default shape of this sample is [8192, 197]. A custom shape can be specified through running arguments, each row contains 1 to 512 elements (cols), and the total number of elements rows x cols must not exceed UINT32_MAX (4294967295).

## Sample Implementation

### Case Implementation Description

This sample constructs an optimization path through three cases. The kernel, Thread Block count, threads per Thread Block, and the main change introduced in each case are shown in the following table.

| Case | Kernel | Thread Block Count | Threads per Thread Block | Change Introduced in This Step |
| ---- | ------ | ------------------ | ------------------------ | ------------------------------- |
| 0 | softmax_gm_kernel | Dynamically calculated based on the data size | 32 x warps_per_block, adaptive to rows, up to 2048 | The SIMT VF reads GM directly. One Warp processes one row, and Warp reduction completes the in-row max and sum |
| 1 | softmax_ub_kernel | Dynamically calculated based on the data size | 32 x rows_per_tile, adaptive to rows and UB capacity, up to 2048 | Input is moved to UB through MTE for staging. Double tile buffers are used in rotation so that movement and computation overlap |
| 2 | softmax_bucket_kernel | Dynamically calculated based on the data size | 512/1024/2048 (dynamically selected based on the shape) | The compile-time bucket keeps intermediate values in registers. UB is padded to the bucket width |

The Thread Block count and the thread count of each Thread Block are dynamically calculated on the Host side based on the data size; see the core implementation of each case for the details. All three cases use the thread organization of "one Warp processes one row". The three cases are selected through the build option `SCENARIO_NUM`, and the default value is 2.

### Performance Metrics

| Metric | Description |
| ------ | ----------- |
| Task Duration(μs) | Total task duration, including scheduling time to the accelerator, execution time on the accelerator, and response completion time |
| aiv_time(μs) | Theoretical execution time of the task on the AI Vector Core, in μs |
| aiv_total_cycles | Total execution cycles on each AI Vector Core compute unit after the task is assigned |
| aiv_vec_time(μs) | Duration of vec-type instructions (vector computation instructions). Thread computation in SIMT VF calls is also included in this type of statistics |
| aiv_vec_ratio | Ratio of vec-type instruction cycles to total cycles |
| aiv_scalar_time(μs) | Duration of scalar-type instructions, in μs |
| aiv_scalar_ratio | Ratio of scalar-type instruction cycles to total cycles |
| aiv_mte2_time(μs) | Duration of MTE2 load instructions, mainly corresponding to GM-to-UB movement |
| aiv_mte3_time(μs) | Duration of MTE3 store instructions, mainly corresponding to UB-to-GM movement |

Except for Task Duration, all other metrics are averages across all Thread Blocks.

### Case 0: SIMT Direct GM Read Baseline

**Optimization Goal**: Establish the functional correctness and performance baseline with the most direct implementation: the Warp reads input directly from GM, completes the in-row max, exp/sum, and normalization, and introduces no intermediate buffer.

**Core Implementation**: The in-row softmax is completed in three passes: the row maximum, the exponential sum, and the normalized output, all reading GM directly. The thread organization is "one Warp processes one row": the 32 threads in a Warp are respectively responsible for different columns of the row, and the in-row max and sum are completed with Warp reduction primitives. Multiple Warps in a Thread Block each process different rows, and the row direction covers all rows through a grid-stride loop; the number of Warps launched per Thread Block adapts to rows, and when rows are fewer than the AIV core count, each Thread Block processes only one row. Take the default shape [8192, 197] as an example. The 197 columns of a row are processed by 32 threads in multiple rounds: each thread starts from the column equal to its thread ID and processes the following columns in steps of 32. Since 197 = 6 x 32 + 5, all 32 threads participate in the first six rounds, and in the seventh round only the first 5 threads still have data to process. One Warp processes one row, and rows are processed in parallel, as shown in the following figure.

<img src="./figures/warp_per_row.png">

Taking the in-row max as an example, the 32 threads in a Warp first compute their partial maximums and then obtain the row maximum through the Warp reduction primitive, as shown in the following figure.

<img src="./figures/case0.png">

```cpp
for (uint32_t row = blockIdx.x * warps_per_block + warp_in_block;  // One Warp processes one row.
     row < rows;
     row += row_stride) {  // Grid-stride loop covering all rows.
    const uint32_t row_offset = row * cols;

    // Pass 1: read the whole row from GM to compute the maximum.
    // asc_reduce_max reduces the maximum across the 32 threads in the Warp.
    float max_value = -ASCRT_INF_F;
    for (uint32_t col = thread_in_warp; col < cols; col += warpSize) {
        const float value = static_cast<float>(input[row_offset + col]);
        max_value = max_value > value ? max_value : value;
    }
    max_value = asc_reduce_max(max_value);

    // Pass 2: re-read GM, compute expf(x - max), and reduce the sum with asc_reduce_add across the Warp.
    float sum_value = 0.0F;
    for (uint32_t col = thread_in_warp; col < cols; col += warpSize) {
        const float value = static_cast<float>(input[row_offset + col]);
        sum_value += expf(value - max_value);
    }
    sum_value = asc_reduce_add(sum_value);

    // Pass 3: re-read GM and complete the normalized write-back.
    // The three passes read the whole row three times and compute exp twice.
    for (uint32_t col = thread_in_warp; col < cols; col += warpSize) {
        const float value = static_cast<float>(input[row_offset + col]);
        output[row_offset + col] = static_cast<half>(expf(value - max_value) / sum_value);
    }
}
```

**Performance Data**:

| Task Duration(μs) | aiv_time(μs) | aiv_total_cycles | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| 12.975 | 11.553 | 19063.1 | 10.917 | 0.946 | 0.633 | 0.054 | 0.005 | 0.000 | 0.002 | 0.000 |

**Analysis**:

The `aiv_vec_ratio` of Case 0 reaches 0.946, and the time is concentrated on GM reads/writes and in-row computation of the SIMT threads. In terms of implementation, the same input row is read three times in the three passes, `exp` is computed twice, and GM access and computation are executed serially in the same SIMT thread, so they cannot overlap with the movement pipeline. The next step separates data movement from computation and moves the input to UB through MTE for staging.

---

### Case 1: UB Staging with MTE Double Buffering

**Optimization Goal**: Use MTE to move data between GM and UB, let the SIMT VF compute on the data in UB, and use double buffering to run computation and data movement as parallel pipelines.

**Core Implementation**:

- Each Thread Block processes row data in tiles. The number of rows in a tile determines the number of Warps launched by one VF call (one Warp processes one row). The row count takes the minimum of the following three values: the rows assigned to this Thread Block, the maximum number of rows that the four tile buffers can hold within the UB capacity, and the 64 rows corresponding to the limit that one Thread Block launches at most 2048 threads (64 Warps).
- The kernel divides the dynamic UB into two input tile buffers and two output tile buffers. The two buffer sets are used alternately by tile, and their lifecycles are managed by separate events.
- MTE2 moves each GM row into UB by the real cols, and MTE3 moves the result back to GM in the same way.
- The SIMT VF still completes the max, exp/sum, and normalization in three passes with one Warp processing one row. The in-row processing logic is the same as Case 0, and the input comes from UB.

```cpp
uint32_t tile_idx = 0;
for (uint32_t row_base = first_row_base; row_base < rows; row_base += tile_stride) {
    const uint32_t slot = tile_idx & 1;
    const event_t event_id = slot == 0 ? EVENT_ID0 : EVENT_ID1;
    const uint32_t rows_remaining = rows - row_base;
    const uint32_t rows_in_tile = rows_remaining < rows_per_tile ? rows_remaining : rows_per_tile;
    __ubuf__ half* input_tile_ub = slot == 0 ? input_tile_ub0 : input_tile_ub1;
    __ubuf__ half* output_tile_ub = slot == 0 ? output_tile_ub0 : output_tile_ub1;

    asc_sync_wait(PIPE_MTE2, PIPE_V, event_id);
    const uint32_t next_row_base = row_base + tile_stride;
    if (next_row_base < rows) {
        // Before computing the current tile, move the next tile into the other input buffer set.
        const uint32_t next_slot = slot ^ 1;
        const event_t next_event_id = next_slot == 0 ? EVENT_ID0 : EVENT_ID1;
        const uint32_t next_rows_remaining = rows - next_row_base;
        const uint32_t next_rows_in_tile =
            next_rows_remaining < rows_per_tile ? next_rows_remaining : rows_per_tile;
        __ubuf__ half* next_input_tile_ub = next_slot == 0 ? input_tile_ub0 : input_tile_ub1;

        asc_sync_wait(PIPE_V, PIPE_MTE2, next_event_id);
        asc_copy_gm2ub_align(
            next_input_tile_ub, mutable_gm_ptr(input + next_row_base * cols),
            static_cast<uint16_t>(next_rows_in_tile), row_bytes, 0, 0, false,
            asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, row_bytes, padded_row_bytes);
        asc_sync_notify(PIPE_MTE2, PIPE_V, next_event_id);
    }

    asc_sync_wait(PIPE_MTE3, PIPE_V, event_id);
    asc_vf_call<softmax_ub_vf>(
        dim3(warpSize, rows_in_tile), output_tile_ub, input_tile_ub, rows_in_tile, cols, ub_stride);
    asc_sync_notify(PIPE_V, PIPE_MTE2, event_id);

    asc_sync_notify(PIPE_V, PIPE_MTE3, event_id);
    asc_sync_wait(PIPE_V, PIPE_MTE3, event_id);
    asc_copy_ub2gm_align(
        output + row_base * cols, output_tile_ub, static_cast<uint16_t>(rows_in_tile), row_bytes,
        asc_store_l2_cache_mode::NORMAL_FIRST_VICTIM, row_bytes, padded_row_bytes);
    asc_sync_notify(PIPE_MTE3, PIPE_V, event_id);
    ++tile_idx;
}
```

**Performance Data**:

| Task Duration(μs) | aiv_time(μs) | aiv_total_cycles | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| 11.923 | 10.362 | 17096.9 | 7.833 | 0.758 | 0.721 | 0.069 | 2.495 | 0.241 | 0.568 | 0.055 |

**Analysis**:

Case 1 moves GM access to the MTE pipeline and introduces double buffering, so the SIMT VF no longer reads GM directly. `aiv_mte2_time` and `aiv_mte3_time` increase from nearly 0 to 2.495μs and 0.568μs respectively, indicating that GM access has moved to the MTE movement path. `aiv_vec_time` decreases from 10.917μs to 7.833μs, a reduction of about 28.2%. `Task Duration` decreases from 12.975μs to 11.923μs, a reduction of about 8.1%. At this point, `aiv_vec_ratio` reaches 0.758, the highest proportion among all components. The bottleneck has shifted from GM access to the in-row computation, and the next step is to reduce the computation time.

The computation time of Case 1 is mainly spent on how the in-row computation is organized: the in-row computation is organized in three passes: max, exp/sum, and normalization. Each pass re-reads the UB input row, and `exp` is computed twice. The next optimization direction is to cache the elements assigned to each thread in a local array to reuse the `exp` results and keep the array in registers.

---

### Case 2: Intermediate Values in Registers

**Optimization Goal**: Keep the intermediate results of the computation, that is, the exp value of each element, in registers, avoiding repeated data movement and computation. The first step introduces a static array, reads the whole row from UB, and completes the max, exp/sum, and normalization in one read; the second step keeps the array in registers. To this end, the implementation converges to compile-time buckets: cols upper bounds 64/128/256/384/512 are divided into five levels, and a specialized kernel is generated for each level; the Host side dynamically selects the thread count based on the shape.

**Core Implementation**:

- Compile-time bucket: The cols covered by this sample does not exceed 512, and each thread is assigned at most 512/32 = 16 elements, which fits entirely in registers; this requires that all array access addresses, that is, the computation of all array indices, can be determined at compile time. However, the for loop that traverses the array takes cols as its condition, and cols is a runtime parameter, so neither the number of iterations nor the array indices accessed in each iteration can be determined at compile time. The compiler cannot unroll the loop, and the array can only be allocated in stack space, whose access efficiency is lower than that of registers. Therefore, cols is divided into five buckets by the upper bounds 64/128/256/384/512, and a specialized kernel is instantiated per bucket: the bucket capacity is a compile-time constant, the for loop condition is a constant, the loop can be fully expanded, and `elements[kWarpIterations]` can stay in registers. Invalid tail columns are masked with `-ASCRT_INF_F` and excluded from the reduction and normalization.
- UB layout: tile buffers are allocated by the bucket capacity `kBucketCols`. MTE moves each row based on the real cols at runtime, and the space after the real elements is left unused. The computation results are written to UB, and MTE3 moves the result back to GM by the real cols. Taking the in-row max as an example, the reduction process of Case 2 after the tail columns are padded is shown in the following figure.

  <img src="./figures/case2.png">

- Dynamic thread selection: the UB buffers are static arrays whose dimensions are determined by the thread count and the bucket capacity, so they must be determined at compile time. Therefore, the thread count can only take the fixed levels 512/1024/2048, and the actually launched thread count equals the selected level. The Host selects the level based on the rows per core. 512/1024/2048 threads correspond to at most 16/32/64 rows per tile. When `cols > 256`, considering register pressure, at most 1024 threads are launched. Each Thread Block processes a fixed number of rows: when rows are small, some Thread Blocks have no rows to process and exit directly.

The SIMT VF reads the elements assigned to each thread into the array once, and the subsequent computation reuses the loaded data, reducing data copies:

```cpp
template <uint32_t kThreadCount, uint32_t kBucketCols>
__simt_vf__ __launch_bounds__(kThreadCount) inline void softmax_bucket_vf(
    __ubuf__ half* output_ub, __ubuf__ const half* input_ub, uint32_t row_count, uint32_t cols)
{
    // Number of elements assigned to each thread, a compile-time constant.
    constexpr uint32_t kWarpIterations = (kBucketCols + warpSize - 1) / warpSize;
    const uint32_t row_in_tile = threadIdx.x / warpSize;
    const uint32_t thread_in_warp = threadIdx.x % warpSize;
    if (row_in_tile >= row_count) {
        return;
    }

    // Each row in the tile is stored with the stride of kBucketCols.
    // The space beyond the real data is left unused.
    const uint32_t row_offset = row_in_tile * kBucketCols;
    // The loop bounds and access indices for the array are compile-time
    // constants, so the array can stay in registers.
    float elements[kWarpIterations];

    // Read the elements assigned to this thread in one pass. Invalid tail
    // columns are masked with -inf and excluded from the following computation.
    for (uint32_t iter = 0; iter < kWarpIterations; ++iter) {
        const uint32_t col = thread_in_warp + iter * warpSize;
        elements[iter] = col < cols ? static_cast<float>(input_ub[row_offset + col]) : -ASCRT_INF_F;
    }

    // Compute the max over the loaded array elements, then obtain the row
    // maximum through the Warp reduction.
    float max_value = elements[0];
    for (uint32_t iter = 1; iter < kWarpIterations; ++iter) {
        max_value = max(max_value, elements[iter]);
    }
    max_value = asc_reduce_max(max_value);

    // Compute exp only once, write the results back to the array for reuse,
    // and obtain the row exponential sum through the Warp reduction.
    float sum_value = 0.0F;
    for (uint32_t iter = 0; iter < kWarpIterations; ++iter) {
        elements[iter] = expf(elements[iter] - max_value);
        sum_value += elements[iter];
    }
    sum_value = asc_reduce_add(sum_value);

    // Normalized write-back to the padded UB (invalid tail columns are written as 0);
    // MTE3 moves back only the real cols.
    for (uint32_t iter = 0; iter < kWarpIterations; ++iter) {
        const uint32_t col = thread_in_warp + iter * warpSize;
        output_ub[row_offset + col] = static_cast<half>(elements[iter] / sum_value);
    }
}
```

**Performance Data**:

| Task Duration(μs) | aiv_time(μs) | aiv_total_cycles | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| 8.588 | 7.067 | 11661.3 | 4.593 | 0.653 | 0.583 | 0.081 | 2.557 | 0.362 | 0.570 | 0.081 |

**Analysis**:

The main change of Case 2 over Case 1 is that all in-row intermediate values stay in registers: after "runtime cols" becomes "compile-time bucket upper bound", the loop bounds and indices for traversing the array are determined at compile time, and both the input row and `exp` are processed only once. `aiv_vec_time` further decreases from 7.833μs to 4.593μs, a reduction of about 41.4%. `Task Duration` decreases from 11.923μs to 8.588μs, a reduction of about 28.0%. The bucket only affects the stride of the UB layout. MTE still moves data by the real cols, and `aiv_mte2_time` stays nearly the same as Case 1. The reduction of the total time makes `aiv_vec_ratio` drop from 0.758 to 0.653 and `aiv_mte2_ratio` rise from 0.241 to 0.362. The time proportions of in-row computation and MTE2 movement are already close. Compared with Case 0, `Task Duration` decreases from 12.975μs to 8.588μs, an overall reduction of about 33.8%.

## Performance Comparison Summary

The following table shows the performance data of each case on Ascend 950PR with the default shape [8192, 197]. The performance data in this sample was collected in a test environment with 64 AIV cores, and the Thread Block count and thread count are calculated based on this core count.

| Case | Optimization | Task Duration(μs) | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_mte2_time(μs) | aiv_mte3_time(μs) |
| ---- | ------------ | :---: | :---: | :---: | :---: | :---: | :---: |
| 0 | SIMT direct GM read baseline | 12.975 | 11.553 | 10.917 | 0.946 | 0.005 | 0.002 |
| 1 | UB staging with MTE double buffering | 11.923 | 10.362 | 7.833  | 0.758 | 2.495 | 0.568 |
| 2 | Intermediate values in registers | 8.588  | 7.067  | 4.593  | 0.653 | 2.557 | 0.570 |

In the main tuning path Case 0 -> Case 1 -> Case 2, `Task Duration` decreases from 12.975μs to 11.923μs and then to 8.588μs. Case 2 achieves an overall reduction of about 33.8% over Case 0.

The metric changes show how the bottleneck shifts at each stage. In Case 0, the time is concentrated on SIMT threads directly reading and writing GM (`aiv_vec_ratio` is 0.946, and MTE is close to 0). After Case 1 moves GM access to MTE2, `aiv_vec_time` decreases by 28.2%, but the three passes still re-read the UB input rows and re-compute `exp`. After Case 2 keeps all in-row intermediate values in registers through the compile-time bucket, `aiv_vec_time` decreases by another 41.4%, `aiv_vec_ratio` drops to 0.653, and `aiv_mte2_ratio` rises to 0.362. The time proportions of in-row computation and MTE2 movement are already close.

With rows fixed at 8192 and the per-row data size gradually increasing (cols from 64 to 512), the `Task Duration` of Case 1 and Case 2 changes as shown in the following figure, where the vertical dotted lines indicate the bucket boundaries.

<img src="./figures/perf_trend.png">

The `Task Duration` of Case 1 keeps rising as cols increases: the in-row computation traverses the row three times in series, and the number of elements processed by each thread is proportional to cols. The growth of Case 2 is staircase-like: within the same bucket, the `Task Duration` stays nearly flat, because the per-thread computation is determined by `kWarpIterations` and remains unchanged, and only the data volume moved by MTE with the real cols grows slightly with cols. When crossing a bucket boundary, the computation jumps by one level, and the `Task Duration` jumps accordingly. Case 2 outperforms Case 1 across the whole cols range, and the advantage grows with cols. At cols = 512, the `Task Duration` of Case 2 is about 48% of Case 1. In the middle of a bucket where the bucket upper bound exceeds the real cols by a large margin (for example, cols = 96 and 197), Case 2 has to process more invalid tail columns, and its relative advantage is smaller than at bucket boundaries (for example, cols = 128, 384, and 512).

## Tuning Recommendations

1. Organize threads along the reduction dimension for reduction computation: assign the data within the reduction dimension to one Warp and complete the reduction with Warp reduction primitives. Dimensions are naturally parallel, and no cross-Warp synchronization is needed.
2. To keep an array in registers, its access indices must be determined at compile time. When the loop bounds or indices depend on runtime parameters and the number of iterations is small, classify the parameters into a small number of upper bounds and fix them as compile-time constants, so that the loops can be fully expanded, avoiding repeated input reads and repeated computation.

## Build and Run

In the sample root directory, perform the following steps to build and run the sample.

- Configure environment variables.

  Configure environment variables based on the [installation method](../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit on the current environment.

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Description:** `${install_path}` is the CANN package installation directory. If no installation directory is specified, the default installation directory is `/usr/local/Ascend`.

- Run the sample.

  Run the following commands in the sample directory. By default, the sample runs with the shape [8192, 197].

  ```bash
  SCENARIO_NUM=2                                                                # Select a scenario. Valid values are 0-2.
  mkdir -p build && cd build;                                                   # Create and enter the build directory.
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j;  # Build the project.
  python3 ../scripts/gen_data.py                                                # Generate input data and golden data.
  ./softmax                                                                     # Run the sample.
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin       # Verify the golden data.
  ```

  A custom shape is also supported. Run `python3 ../scripts/gen_data.py <rows> <cols>` first, and then run `./softmax <rows> <cols>`, where each row contains 1 to 512 elements.

  When using NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Example:
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM ..; make -j;   # NPU simulation mode
  ```

  > **Note:** Clear the cmake cache before switching build modes. Execute `rm CMakeCache.txt` in the build directory, then run cmake again.

- Build option description

  | Option | Values | Description |
  |:---|:---|:---|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `sim` | Run mode: NPU execution, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture. This sample supports only dav-3510 (Ascend 950PR&950DT products). |
  | `SCENARIO_NUM` | `0`, `1`, `2` | Sample type. The default value is 2. 0 = SIMT direct GM read, 1 = UB staging with MTE double buffering, 2 = intermediate values in registers. |

- Execution result

  The following execution result indicates that the accuracy comparison is successful.

  ```text
  [Success] Case accuracy verification passed.
  ```

## Performance Debugging

### Introduction to the msOpProf Tool

`msOpProf` is a single-operator performance analysis tool. It offers two usage methods: `msopprof` and `msopprof simulator`. The tool helps users identify anomalies in operator memory, operator code, and operator instructions, enabling comprehensive operator tuning. It currently supports performance data collection and automatic parsing for different run modes (on-device or simulation) and different file types (executables or operator binary `.o` files).

Through on-device performance collection, the running time of an operator on the Ascend AI processor can be directly measured. This method is suitable for quickly locating operator performance issues in a board environment.

Run operator tuning on the executable `softmax` with `msopprof`:

```bash
msopprof ./softmax
```

After the command is complete, a performance data folder named `OPPROF_{timestamp}_XXX` is generated in the default directory. The folder structure is as follows:

```text
├──dump                       # Raw performance data. Users do not need to pay attention to it.
├──ArithmeticUtilization.csv  # cube/vector instruction cycle ratio
├──L2Cache.csv                # L2 Cache hit rate, which affects MTE2. Properly plan data movement logic to increase the hit rate.
├──Memory.csv                 # UB, L1, and main memory read/write bandwidth rates
├──MemoryL0.csv               # L0A, L0B, and L0C read/write bandwidth rates
├──MemoryUB.csv               # Vector and Scalar to UB read/write bandwidth rates
├──OpBasicInfo.csv            # Basic operator information
├──PipeUtilization.csv        # Duration and ratio of compute units and movement units
├──ResourceConflictRatio.csv  # Ratio of UB bank groups, bank conflicts, and resource conflicts among all instructions
└──visualize_data.bin         # MindStudio Insight presentation file
```

View the specific performance analysis results:

```bash
# View Task Duration and other metrics
cat ./OPPROF_*/PipeUtilization.csv
```
