# SIMT Programming Reduction Sum Example

## Overview

This example implements reduction sum for 1D `float` input based on the Ascend C SIMT programming model. Through 2 progressive scenarios, it demonstrates typical usage of intra-block synchronization and inter-block memory ordering control.

The 2 scenarios correspond to reduction sum for small shape input and large shape input respectively, focusing on the usage of [`asc_syncthreads()`](../../../../../../docs/zh/api/SIMT-API/sync_and_memory_fence/sync_interface/asc_syncthreads.md) and [`asc_threadfence()`](../../../../../../docs/zh/api/SIMT-API/sync_and_memory_fence/memory_fence/asc_threadfence.md) under different reduction scales.

## Supported Products

- Ascend 950PR/Ascend 950DT

## Supported CANN Software Version
- \>= CANN 9.1.0

## Directory Structure

```text
├── memory_fence
│   ├── figures               // Image resources for README
│   ├── sync_barrier.asc      // Scenario 1: Small shape reduction example, demonstrating asc_syncthreads() intra-block synchronization usage
│   ├── memory_fence.asc      // Scenario 2: Large shape reduction example, demonstrating asc_threadfence() and atomic counter inter-block coordination usage
│   ├── CMakeLists.txt        // Build project file
│   └── README.md
```

## Example Description

### Example Function

<table border="1" align="center">
  <tr>
    <td align="center">SCENARIO_NUM Value</td>
    <td align="center">Function Scenario</td>
    <td align="center">Scenario Description</td>
    <td align="center">Corresponding File</td>
  </tr>
  <tr>
    <td align="center">1</td>
    <td align="center">Small shape reduction scenario</td>
    <td align="center">Use asc_syncthreads() to complete intra-block reduction synchronization</td>
    <td align="center">sync_barrier.asc</td>
  </tr>
  <tr>
    <td align="center">2</td>
    <td align="center">Large shape reduction scenario</td>
    <td align="center">Use asc_threadfence() to complete multi-block partial sum merge and summation</td>
    <td align="center">memory_fence.asc</td>
  </tr>
</table>

This example controls build branches through `SCENARIO_NUM`. The 2 scenarios are classified by input scale: Scenario 1 demonstrates reduction sum and `asc_syncthreads()` synchronization usage for small shape input. When the number of input elements is small, only 1 block is needed to cover all data, and no inter-block synchronization is required. Scenario 2 extends to multi-block segmented reduction for large shape input, introducing `asc_threadfence()` for inter-block merging.

### Example Specifications

#### SCENARIO_NUM=1 (Small Shape Reduction Scenario)

- Example Function:

  Perform **reduction sum** on 1024 `float` input elements within a single thread block.

- Example Specifications:

  <table border="1" align="center">
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center">SyncBarrierSingleBlock</td></tr>
  <tr><td rowspan="2" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">input</td><td align="center">[1024]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">output</td><td align="center">[1]</td><td align="center">float</td><td align="center">ND</td></tr>
  </table>

- Data Partitioning:

  - gridDim: (1, 1, 1)
  - blockDim: (1024, 1, 1)
  - Per-thread processing: 1 input element

#### SCENARIO_NUM=2 (Large Shape Reduction Scenario)

- Example Function:

  Perform **segmented reduction sum** on `1024 * 1024` `float` input elements. Each thread block first completes the **partial sum** for its assigned segment, then a single thread block performs a second reduction sum on all thread block partial sums to obtain the final total.

- Example Specifications:

  <table border="1" align="center">
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center">ThreadFenceMultiBlock</td></tr>
  <tr><td rowspan="2" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">input</td><td align="center">[1024 * 1024]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">output</td><td align="center">[1]</td><td align="center">float</td><td align="center">ND</td></tr>
  </table>

- Data Partitioning:

  - gridDim: (1024, 1, 1)
  - blockDim: (1024, 1, 1)
  - Per-thread processing: 1 input element

### Example Implementation

#### 1: Small Shape Reduction Scenario

Each thread in a single thread block first reads one input element, then obtains the final result through **two-phase reduction sum**:

- **Phase 1 (intra-warp reduction)**: Call the [`asc_reduce_add()`](../../../../../../docs/zh/api/SIMT-API/Warp_functions/Warp_reduce_functions/asc_reduce_add.md) interface to perform reduction on all thread values within the current warp, obtaining the sum of data within the warp, and write to shared memory.
- **Phase 2 (block-level sequential accumulation)**: After cross-warp synchronization, thread 0 sequentially accumulates the partial sums from each warp to obtain the final result.

Taking 128 elements (4 warps) as an example, the two-phase reduction process is shown in Figure 1:

<p align="center">
  <img src="./figures/两阶段归约求和.png" width="50%">
   </p>
<p align="center">
Figure 1: Two-phase reduction process illustration
</p>

In Phase 1, each warp completes intra-warp summation through `asc_reduce_add()` and writes the result to shared memory. In Phase 2, thread 0 sequentially reads the partial sums from each warp and accumulates them to obtain the final result. Between the two phases, `asc_syncthreads()` must be called for synchronization.

`asc_syncthreads()` is used to block all threads in the current thread block until all threads reach the synchronization point. In this scenario, `asc_syncthreads()` ensures that the first thread of each warp has already written the intra-warp reduction result to shared memory, so that the subsequent block-level sequential accumulation can read complete shared memory data.

#### 2: Large Shape Reduction Scenario

When the total number of input elements is large, **multiple thread blocks** are needed for segmented processing.

Each thread block first completes the two-phase partial reduction described above within the block, then the thread with `tid = 0` writes the block's partial sum to `block_sums[blockIdx.x]`. After writing, `asc_threadfence()` is executed, and then the global counter is incremented by 1. The last thread block to increment the counter (that is, the thread block with `ticket = gridDim.x - 1`) reads `block_sums` and performs a second two-phase reduction to output the final result.

Taking 8 thread blocks as an example, the inter-block coordination process is shown in Figure 2:

<p align="center">
  <img src="./figures/跨线程块协作.png" width="75%">
   </p>
<p align="center">
Figure 2: Inter-block coordination process illustration
</p>

In the above flow, multiple thread blocks need to read and write the same global memory `block_sums`, which may cause data races.

`asc_threadfence()` serves as an inter-core memory barrier, ensuring that all global memory and shared memory write operations before `asc_threadfence()` by the calling thread are visible to other threads, and that these write operations are not reordered after the memory barrier. Therefore, each thread block must execute in the following order: write partial sum to `block_sums` -> increment atomic counter. This order ensures that when the last thread block reads `block_sums` to perform global reduction, it will definitely see all partial sums already written by other thread blocks. If `asc_threadfence()` is omitted, stale data may be read, causing incorrect results.

## Build and Run

Run the following steps in the root directory of this example to build and execute the operator.
- Configure Environment Variables  
  Configure environment variables based on the [installation method](../../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the Example

  Run the following commands in this example directory.

  ```bash
  SCENARIO_NUM=1                # Value is 1 or 2
  mkdir -p build && cd build;   # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM ..; make -j;   # Build the project
  ./demo                        # Run the example
  ```

  Build Options Description

  | Option | Values | Description |
  |------|--------|------|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture: this example only supports dav-3510 (Ascend 950PR/Ascend 950DT) |
  | `SCENARIO_NUM` | `1`, `2` | Scenario number: 1 indicates small shape reduction scenario, 2 indicates large shape reduction scenario |

  The following output indicates that the accuracy verification is successful.
  ```
  [Success] Case accuracy is verification passed.
  ```
