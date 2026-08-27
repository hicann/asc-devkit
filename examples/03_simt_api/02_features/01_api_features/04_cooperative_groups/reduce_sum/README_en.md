# Hierarchical Reduction Sample Using Cooperative Groups

## Overview

This sample demonstrates the basic usage of the [cooperative groups API](../../../../../../docs/zh/api/SIMT-API/cooperative_groups/cooperative_groups_intro.md) through reduction sum operations, highlighting the code abstraction capabilities provided by cooperative groups.


## Supported Products

- Ascend 950PR/Ascend 950DT

## Supported CANN Versions

- \>= CANN 9.2.0


## Directory Structure

```text
├── reduce_sum
│   ├── figures           // Image resources for the README
│   ├── CMakeLists.txt    // Build project file
│   ├── reduce_sum.asc    // Sample implementation, three kernels plus host-side invocation
│   ├── README.md
│   └── README_en.md
```


## Sample Description

### Functionality

This sample presents three cooperative-group-based implementations of reduction sum, controlling thread collaboration at grid, thread block, and Warp granularity respectively. The reduction logic is also encapsulated into a common function `reduce_sum`, which adapts to different granularities simply by taking a different cooperative group object, which fully demonstrates the strength of cooperative groups in code abstraction and reuse.

### Specifications

All three cases use the same execution configuration: a grid of 4 thread blocks, 1024 threads per block (the default thread configuration), 4096 threads in total, with dynamic ubuf set to 0.


<table>
<tr><td rowspan="1" align="center">OpType</td><td colspan="4" align="center">Cooperative Groups</td></tr>
<tr><td rowspan="2" align="center">Sample input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">input</td><td align="center">[4096]</td><td align="center">int32</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">Sample output</td><td align="center">output</td><td align="center">[1]</td><td align="center">int32</td><td align="center">ND</td></tr>
</table>


### Implementation

#### The Common Reduction Function

`reduce_sum` reduces the input `val` within the scope of the cooperative group object `g` that is passed in, using the memory pointed to by `workspace` as its scratch area, and the result of the reduction is returned by the thread with rank 0.

**Figure 1**  The halving reduction process

![](./figures/halving_reduction.png)

The figure takes 16 threads as an example. The number of threads participating in the reduction is `N = g.size()`, that is 16. In each round, every active thread adds the value it holds to the value held `i` threads away (`i` starts at `N/2`), after which the number of active threads is halved and the stride `i` halves along with it.

```cpp
template <typename PtrT>
__aicore__ inline int32_t reduce_sum(const thread_group& g, PtrT workspace, int32_t val)
{
    const int32_t lane = static_cast<int32_t>(g.thread_rank());
    for (int32_t i = static_cast<int32_t>(g.size()) / 2; i > 0; i /= 2) {
        workspace[lane] = val;                  // (1) write to the slot of this thread
        g.sync();                               // (2) wait for the whole group to finish writing
        if (lane < i) {
            val += workspace[lane + i];         // (3) read the slot i threads away
        }
        g.sync();                               // (4) wait for the whole group to finish reading
    }
    return lane == 0 ? val : -1;
}
```

#### Case0: Cross-block Reduction

**Core implementation**: create a `grid_group` and reduce all threads at `grid` granularity, where a single call to `reduce_sum` yields the reduction result.

**Figure 2**  Case0 cross-block reduction

![](./figures/reduce_sum_case0.png)

```cpp
__global__ void reduce_grid_group(
    const int32_t* input, int32_t* grid_workspace, int32_t* output, int32_t num_elements)
{
    grid_group grid = this_grid();
    const int32_t global_idx = static_cast<int32_t>(grid.thread_rank());
    volatile int32_t* ws = grid_workspace;
    const int32_t in_val = global_idx < num_elements ? input[global_idx] : 0;
    const int32_t val = reduce_sum(grid, ws, in_val);
    if (grid.thread_rank() == 0) {
        *output = val;
    }
}
```

Reducing at grid granularity requires communication across blocks, so the workspace must be in GM. In addition, to guarantee [cache coherence](../../../../../../docs/zh/guide/programming_guide/advanced_programming/memory_model/cache_coherence.md), the workspace pointer must be qualified with volatile.

#### Case1: Whole-block Reduction

**Core implementation**: reduce within a thread block through the thread_block cooperative group first, then rank 0 of each block accumulates the sum of that block into the same output through asc_atomic_add, which aggregates the total. This case reduces at thread block granularity, so UB can be used as the workspace.

**Figure 3**  Case1 whole-block reduction

![](./figures/reduce_sum_case1.png)

```cpp
__global__ void reduce_thread_block(const int32_t* input, int32_t* output, int32_t num_elements)
{
    __ubuf__ int32_t ub_workspace[THREADS_PER_BLOCK];
    thread_block block = this_thread_block();
    const int32_t global_idx = static_cast<int32_t>(this_grid().thread_rank());
    const int32_t in_val = global_idx < num_elements ? input[global_idx] : 0;
    const int32_t val = reduce_sum(block, ub_workspace, in_val);
    if (block.thread_rank() == 0) {
        asc_atomic_add(output, val);
    }
}
```

#### Case2: Warp-internal Reduction

**Core implementation**: the reduction is done in two levels. The first level creates a thread_block_tile cooperative group of 32 threads, which reduces at Warp granularity. Each thread block has 1024 threads, that is 32 Warps, which produce 32 intermediate results. The second level has Warp 0 reduce these 32 intermediate results, after which its rank 0 accumulates the sum of that block into the output address through `asc_atomic_add`.

**Figure 4**  Case2 warp-internal reduction

![](./figures/reduce_sum_case2.png)

```cpp
__global__ void reduce_block_tile(const int32_t* input, int32_t* output, int32_t num_elements)
{
    __ubuf__ int32_t ub_workspace[THREADS_PER_BLOCK];
    __ubuf__ int32_t ub_partial[WARPS_PER_BLOCK];
    thread_block block = this_thread_block();
    const int32_t global_idx = static_cast<int32_t>(this_grid().thread_rank());
    auto tile = tiled_partition<TILE_SIZE>(block);
    const int32_t offset = static_cast<int32_t>(tile.meta_group_rank()) * TILE_SIZE;
    const int32_t in_val = global_idx < num_elements ? input[global_idx] : 0;
    int32_t val = reduce_sum(tile, ub_workspace + offset, in_val);
    // rank 0 of each warp writes its partial sum into ub_partial
    if (tile.thread_rank() == 0) {
        ub_partial[tile.meta_group_rank()] = val;
    }
    block.sync();

    if (tile.meta_group_rank() == 0) {
        val = reduce_sum(tile, ub_workspace, ub_partial[tile.thread_rank()]);
        // Warp 0 performs the final reduction over the 32 intermediate results in ub_partial
        if (tile.thread_rank() == 0) {
            asc_atomic_add(output, val);
        }
    }
}
```

`TILE_SIZE` is 32, the width of one warp, so that each sub-group produced by `tiled_partition<TILE_SIZE>(block)` falls exactly within one warp.

## Performance Comparison

### Performance Metric

| Metric | Description |
| --- | --- |
| Task Duration (μs) | Total task duration, including the time scheduled onto the accelerator, the execution time on the accelerator, and the response/completion time |

### Performance Comparison Results

The performance data of the three cases is listed below:

| Case | Threads in the reduction | Halving rounds | Synchronization count and scope | Task Duration(us) |
| :--: | :----: | :--: | :--: | :-----------------: |
| Cross-block reduction | 4096 | 12 | 24 grid-level | 16.101 |
| Whole-block reduction | 1024 | 10 | 20 block-level | 3.514 |
| Warp-internal reduction | 32 | 5 + 5 | 20 warp-internal (no overhead) + 1 block-level | 2.397 |

The three cases have the same input size and the same memory traffic, so the performance difference comes mainly from the scope of synchronization: the more threads a single synchronization has to wait for, the longer the threads that arrive early sit idle at the barrier.

- Warp-internal reduction is roughly 31.8% faster than whole-block reduction. The two have the same number of halving rounds and both keep the workspace in UB; they differ only in the scope of synchronization. The 20 `sync()` calls in warp-internal reduction incur no actual overhead due to inherent thread synchronization, with only one block-level barrier between the two levels that generates actual waiting, whereas each of the 20 synchronizations in whole-block reduction waits for all 1024 threads of the block.
- Cross-block reduction takes roughly 4.6 times as long as whole-block reduction, for two reasons. First, every synchronization has to wait for all 4096 threads of the grid, the widest scope of the three. Second, the workspace can only be placed in GM and the pointer additionally needs `volatile`, so every read and write of each round accesses GM directly, and the access latency of GM is far higher than that of UB.

Cooperative groups provide a unified programming interface that lets developers manage threads at different granularities. Building on this abstraction, developers can encapsulate business logic more naturally and build reusable computation modules, which speeds up operator development. At the same time, finer-grained thread partitioning and collaboration help developers fully exploit the potential of the hardware and further improve performance.

## Build and Run

Run the following steps in the root directory of this example to build and execute the example.

- Configure Environment Variables

  Configure environment variables based on the [installation method](../../../../../../docs/zh/quick_start.md#prepare&install) of the CANN development kit in the current environment.

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the Example

  Run the following commands in this example directory. The case number is given as an argument at run time, and a single execution runs only that one case.

  ```bash
  mkdir -p build && cd build;                             # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;    # Build the project
  ./cooperative_groups 0                                  # Run the cross-block reduction case
  ./cooperative_groups 1                                  # Run the whole-block reduction case
  ./cooperative_groups 2                                  # Run the warp-internal reduction case
  ```

- Build Options Description

  | Option | Values | Description |
  |---------------------------|------------|---------------------------------------------------|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture: this example only supports dav-3510 (Ascend 950PR/Ascend 950DT) |


  The output is as follows, which indicates that the accuracy verification passed.

  ```text
  [Success] Case accuracy verification passed.
  ```

## Performance Analysis

Use the `msOpProf` tool to collect detailed performance data:

```bash
msopprof ./cooperative_groups 0   # Analyze the performance of the cross-block reduction case
msopprof ./cooperative_groups 1   # Analyze the performance of the whole-block reduction case
msopprof ./cooperative_groups 2   # Analyze the performance of the warp-internal reduction case
```

After the command completes, a folder named "OPPROF_{timestamp}_XXX" is generated in the default directory. The performance data folder structure example is as follows:

```text
├──dump                       # Raw performance data, no user attention needed
├──ArithmeticUtilization.csv  # cube/vector instruction cycle ratio
├──L2Cache.csv                # L2 Cache hit rate
├──Memory.csv                 # UB, L1, and main memory read/write bandwidth
├──MemoryL0.csv               # L0A, L0B, and L0C read/write bandwidth
├──MemoryUB.csv               # Vector and Scalar to UB read/write bandwidth
├──OpBasicInfo.csv            # Operator basic information
├──PipeUtilization.csv        # Compute unit and transfer unit duration and ratio
├──ResourceConflictRatio.csv  # UB bank group, bank conflict, and resource conflict ratio across all instructions
└──visualize_data.bin         # MindStudio Insight presentation file
```
