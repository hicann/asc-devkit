# SpMV Warp Divergence Sample

## Overview

This sample uses sparse matrix-vector multiplication (SpMV) to demonstrate the impact of Warp Divergence on performance in Ascend C SIMT programming and the optimization approach. The sample includes two cases:

- **Case 1**: One thread processes one row of data. Because different rows have different numbers of non-zero elements, threads within the same Warp have inconsistent loop iterations, resulting in severe Warp Divergence.
- **Case 2**: One Warp cooperatively processes one row of data, reducing Warp Divergence.

## Supported Products

- Ascend 950PR/Ascend 950DT

## Supported CANN Software Version

- \>= CANN 9.1.0

## Directory Structure

```text
├── warp_divergence
│   ├── CMakeLists.txt              // cmake compilation file
│   ├── spmv.asc                    // SIMT SpMV sample implementation
│   ├── README.md
│   └── README_en.md
```

## Sample Description

- Sample Function

  This sample performs sparse matrix-vector multiplication (SpMV) with a sparse matrix stored in CSR format and a dense vector. By comparing the "one thread processes one row of data" approach with the "one Warp cooperatively processes one row of data" approach, the sample demonstrates the impact of Warp Divergence on performance.

  A sparse matrix is a matrix where most elements are zero. To save storage space, sparse matrices are typically stored in CSR (Compressed Sparse Row) format, which stores only the non-zero elements and their position information. The CSR format uses three arrays:
  - **values**: All non-zero element values, arranged in row-major order.
  - **col_idx**: The column index of each non-zero element, corresponding one-to-one with values.
  - **row_ptr**: The row offset array, recording the starting and ending indices of non-zero elements for each row in the values array. For example, the non-zero elements of row i are located at indices `[row_ptr[i], row_ptr[i+1])` in values.

  The following example shows a 4x5 sparse matrix:

  Original matrix (4x5):

  ```
       c0   c1   c2   c3   c4
  r0 [ 1.0    0  2.0    0    0 ]
  r1 [   0  3.0    0  4.0    0 ]
  r2 [   0    0    0  5.0  6.0 ]
  r3 [ 7.0    0    0    0    0 ]
  ```

  CSR format:

  ```
  values  = [1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0]
  col_idx = [  0,   2,   1,   3,   3,   4,   0]
  row_ptr = [  0,   2,   4,   6,   7]
  ```

  Use row_ptr to locate the non-zero elements of each row:

  - Row 0: `row_ptr[0]=0, row_ptr[1]=2` → `values[0:2] = [1.0, 2.0]`, `col_idx[0:2] = [0, 2]`
  - Row 1: `row_ptr[1]=2, row_ptr[2]=4` → `values[2:4] = [3.0, 4.0]`, `col_idx[2:4] = [1, 3]`
  - Row 2: `row_ptr[2]=4, row_ptr[3]=6` → `values[4:6] = [5.0, 6.0]`, `col_idx[4:6] = [3, 4]`
  - Row 3: `row_ptr[3]=6, row_ptr[4]=7` → `values[6:7] = [7.0]`, `col_idx[6:7] = [0]`

  SpMV computes `y = A * x`, where A is a sparse matrix in CSR format, x is a dense vector, and y is the output vector.

- Sample Specifications:

  <table>
  <tr><td rowspan="1" align="center">OpType</td><td colspan="4" align="center">SpMV</td></tr>
  <tr><td rowspan="5" align="center">Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">values</td><td align="center">[nnz]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">col_idx</td><td align="center">[nnz]</td><td align="center">uint32</td><td align="center">ND</td></tr>
  <tr><td align="center">row_ptr</td><td align="center">[8193]</td><td align="center">uint32</td><td align="center">ND</td></tr>
  <tr><td align="center">x</td><td align="center">[1024]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Output</td><td align="center">y</td><td align="center">[8192]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Name</td><td colspan="4" align="center">kernel_spmv_thread / kernel_spmv_warp</td></tr>
  </table>

> nnz indicates the number of non-zero elements in the sparse matrix. The sparse matrix used in this sample has a size of 8192x1024, with 1 to 64 non-zero elements per row.

## Sample Implementation

This sample includes two independent kernels, each corresponding to a specific case version.

| Case   | Implementation Feature                                              | Kernel             | Optimization Feature                |
|--------|---------------------------------------------------|--------------------|---------------------|
| Case 1 | Each thread independently processes one row of data                   | kernel_spmv_thread | Baseline version: one thread processes one row of data          |
| Case 2 | One Warp cooperatively processes one row of data | kernel_spmv_warp   | Warp cooperative processing of one row of data, reducing Warp Divergence |

Both cases use the same launch configuration: `gridDim=(8, 1, 1), blockDim=(1024, 1, 1)`, with a total of 8192 threads. Except for the kernel implementation, all other configurations are identical.

### Performance Metrics Description

|             Metric             | Description                                             |
|:---------------------------:|:-------------------------------------------------|
|      Task Duration(μs)      | Overall task duration, including the time from scheduling to the accelerator, execution time on the accelerator, and the response completion time.          |
|      aiv_total_cycles       | Total CPU cycles consumed by the task executing on the Vector Core.          |
|   Read Main Memory          | Number of reads from the main memory.                                       |

### Case 1: One Thread Processes One Row of Data (Baseline)

**Implementation**: Each thread determines the row number to process based on the global ID, independently iterates through all non-zero elements of that row, and accumulates the products. Within the same Warp, 32 threads process 32 rows. Because different rows have different numbers of non-zero elements, the loop iterations range from 1 to 64.

**Key Code**:

```cpp
float sum = 0.0f;
uint32_t row_start = row_ptr[row];
uint32_t row_end = row_ptr[row + 1];
for (uint32_t j = row_start; j < row_end; j++) {
    sum += values[j] * x[col_idx[j]];
}
y[row] = sum;
```

**Performance Data**:

| Task Duration(μs) | aiv_total_cycles | Read Main Memory |
|:-----------------:|:----------------:|:----------------:|
|      81.811       |     127124       |      39808       |

**Performance Data Analysis**:

- Warp Divergence occurs when all threads within a Warp execute the same instruction, but due to differences in control flow, only some threads enter a particular branch. The threads that enter the branch continue executing, while the threads that do not enter the branch are masked and wait. Warp Divergence causes execution efficiency to decrease. For more details, refer to [Warp Execution Mechanism](../../../../../docs/zh/guide/编程指南/编程模型/AI-Core-SIMT编程/线程架构.md#warp执行机制). In this implementation, different rows have different numbers of non-zero elements (1 to 64). The 32 threads within the same Warp each process different rows, resulting in inconsistent loop iterations and inconsistent loop exit times, causing severe Warp Divergence. Suppose thread 0 in a Warp processes a row with 3 non-zero elements, and thread 1 processes a row with 64 non-zero elements. Thread 0 becomes idle after 3 loop iterations, while thread 1 needs to execute 64 loop iterations. This waiting occurs in every Warp, resulting in significant waste of computing resources.

**Optimization Suggestion**:

> 💡 **Use Warp cooperative processing for one row of data**
>
> Change the "one thread processes one row of data" approach to "one Warp cooperatively processes one row of data", making the loop iterations of threads within the same Warp basically the same, reducing Warp Divergence.

### Case 2: One Warp Cooperatively Processes One Row of Data

**Implementation**: The 32 threads of one Warp cooperatively process the same row of data. Each thread accesses the non-zero elements of that row with a step size of `warpSize`, resulting in basically the same loop iterations. Use [asc_reduce_add()](../../../../../docs/zh/api/SIMT-API/Warp_functions/Warp_reduce_functions/asc_reduce_add.md) to reduce the partial sums of all threads within the Warp, and thread 0 writes the final result.

**Key Code**:

```cpp
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
```

**Performance Data**:

| Task Duration(μs) | aiv_total_cycles | Read Main Memory |
|:-----------------:|:----------------:|:----------------:|
|      31.772       |      49727       |      2278        |

**Optimization Effect Analysis**:

- All threads within the same Warp cooperatively process the same row of data. Only in the last loop iteration, some threads may not participate in the computation, resulting in minimal Warp Divergence. Compared with 81.811μs in Case 1, the Task Duration is reduced to 31.772μs.
- Unlike Case 1 where threads within the same Warp access scattered addresses of different rows, Case 2 has threads within the Warp accessing consecutive elements of the same row, achieving memory access coalescing. The number of reads from the main memory is reduced from 39808 in Case 1 to 2278.

## Performance Comparison Summary

### Ascend 950PR Performance Data

**Overall Optimization Effect**:
- From the Case 1 baseline version to the Case 2 optimized version, the Task Duration decreases from 81.811μs to 31.772μs, reducing the duration by approximately 61.1%.

| Case   | Task Duration(μs) | Task Duration Relative to Case 1 | Optimization Point                        |
|--------|-------------------|-----------------------|----------------------------|
| Case 1 | 81.811            | **1x**                | Baseline version: one thread processes one row of data          |
| Case 2 | 31.772            | **0.39x duration**          | Warp cooperative processing of one row of data, reducing Warp Divergence, memory access coalescing |

## Tuning Suggestions

1. **Reduce Warp Divergence**: When threads within the same Warp enter different code execution paths due to branching, some threads need to wait for the remaining threads to complete the current branch, resulting in waste of computing resources. Common scenarios include inconsistent loop iterations and conditional branch differences. This sample addresses the scenario of inconsistent loop iterations by using Warp cooperative processing to make the loop iterations of threads within the same Warp basically the same, reducing Warp Divergence.

2. **Leverage memory access coalescing**: Ensure that threads within a Warp access consecutive memory addresses to improve memory access efficiency.

## Compilation and Running

Perform the following steps in the root directory of this sample to compile and run the sample.
- Configure environment variables  
  Configure the environment variables based on the [installation method](../../../../../docs/zh/quick_start.md#prepare&install) of the CANN development toolkit on the current environment.

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Description:** `${install_path}` is the CANN package installation directory. If the installation directory is not specified, the default installation path is `/usr/local/Ascend`.

- Sample Execution

  Run the following commands in the directory of this sample.

  ```bash
  SCENARIO_NUM=1                       # Select the execution scenario. Options: 1-2
  mkdir -p build && cd build;          # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j;  # Compile the project
  ./demo                               # Run the sample
  ```

- Compilation Option Description

  | Option                        | Available Values        | Description                                                |
  |---------------------------|------------|---------------------------------------------------|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture: this sample only supports dav-3510 (Ascend 950PR/Ascend 950DT) |
  | `SCENARIO_NUM`            | `1`-`2`    | Sample type, default is 1                                         |

  The following output indicates that the accuracy verification is successful:

  ```text
  [Success] Case accuracy verification passed.
  ```

## Performance Data Collection

  Use the `msopprof` tool to collect performance data on a single component:

  ```bash
  msopprof ./demo   # Analyze example performance
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

  View specific performance analysis results:

  ```
  # For example, view Task Duration related data
  cat ./OPPROF_*/OpBasicInfo.csv
  ```
