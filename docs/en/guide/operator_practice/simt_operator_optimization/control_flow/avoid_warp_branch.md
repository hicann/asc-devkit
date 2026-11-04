# Avoiding Branch Jumps Within the Same Warp

<!-- md-trans-meta sourceCommit=dd293a1c6b873d149bf289bb2b8098dadf9f5cbd translatedAt=2026-10-23T13:09:16.390Z pushedAt=2026-11-03T07:05:13.370Z nl=2 -->

[Priority] High

[Description] In the SIMT programming model, a warp is the basic scheduling and execution unit, and the 32 threads within the same warp execute the same instruction. Control flow instructions (if, switch, do, for, while) generate branch jumps. When the jump targets of different threads within the same warp are inconsistent, the thread execution paths diverge, causing branch divergence (Warp Divergence). When branch divergence occurs, only the threads that enter the current branch (active threads) execute normally, while the remaining threads are masked and wait until all branch paths are executed before the threads within the warp can reconverge. Branch divergence increases the total number of instructions actually executed by the warp, reduces execution efficiency, and severely affects operator performance. Therefore, threads within the same warp should avoid branch jumps as much as possible.

**Figure 1** Branch divergence diagram

![](../../../figures/branch_div.png)

Within a thread block, all threads are automatically divided by hardware into warps of 32 threads each in linear order. When the control flow condition is related to the thread ID, you should try to ensure that threads within the same warp enter the same branch to reduce branch divergence. For example, for a one-dimensional thread block, when the control condition depends only on `threadIdx.x / warpSize`, threads within the same warp take the same branch path, and no branch divergence occurs.

For loop-type control flow such as for loops, you should try to keep the loop count of threads within the same warp consistent to avoid severe branch divergence caused by inconsistent loop counts. When the loop counts of different threads within the same warp are inconsistent, the threads that exit the loop first remain masked and wait for the remaining threads until the last thread in the warp completes the loop, causing serious waste of computing resources within the warp.

For some relatively simple branches, the compiler may use predication to avoid generating branch jumps. In this case, all instructions are executed, but whether they ultimately take effect is determined by the predication of each thread. Through this mechanism, the performance loss caused by branch jumps can be reduced.

[Sample introduction] Take the [SpMV Warp Divergence sample](../../../../../../examples/03_simt_api/03_best_practices/02_control_flow/warp_divergence) as an example. The sample compares two approaches, "one thread processing one row of data" and "one warp collaboratively processing one row of data", to demonstrate the impact of Warp Divergence on performance. The function of this sample is to compute the multiplication of a sparse cube stored in CSR format and a dense vector.

A sparse cube is a cube in which most elements are zero. To save storage space, sparse cubes are usually stored in CSR (Compressed Sparse Row) format, which stores only the non-zero elements and their position information. The CSR format uses three arrays:

- **values**: values of all non-zero elements, arranged in row-major order.
- **col_idx**: column index of each non-zero element, corresponding to values one by one.
- **row_ptr**: row offset array that records the start and end indices of the non-zero elements of each row in the values array. For example, the indices of the non-zero elements of row i in values are `[row_ptr[i], row_ptr[i+1])`.

SpMV computes `y = A * x`, where A is a sparse cube in CSR format, x is a dense vector, and y is the output vector.

[Negative example] One thread processes one row of data, and the loop counts of threads within the same warp are inconsistent, causing severe warp divergence.

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

In the preceding implementation, each thread determines the row to process based on its global ID, independently traverses all non-zero elements of that row, and accumulates the products. The 32 threads within the same warp process 32 rows, and the number of non-zero elements in each row differs, with loop counts ranging from 1 to 64. Because different rows have different numbers of non-zero elements, the threads within the same warp exit the loop at different times, causing severe warp divergence. Assume that in a warp, the row processed by thread 0 has 3 non-zero elements and the row processed by thread 1 has 64 non-zero elements. Thread 0 enters idle waiting after 3 loops, while thread 1 needs to execute 64 loops. This waiting occurs in every warp, wasting a large amount of computing resources.

The performance data of the negative example operator is as follows:

| Task Duration (μs) | aiv_total_cycles | Read Main Memory |
| :---------------: | :--------------: | :--------------: |
|      81.811       |     127124       |      39808       |

[Positive example] One warp collaboratively processes one row of data, and the loop counts of threads within the same warp are basically the same, reducing warp divergence.

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

In the preceding implementation, the 32 threads of one warp collaboratively process the same row of data. Each thread accesses the non-zero elements of that row with a stride of `warpSize`, so the loop counts are basically the same. Only in the last round of the loop may some threads not participate in the computation, resulting in minor warp divergence. Use [asc_reduce_add()](../../../../api/SIMT-API/Warp_functions/Warp_reduce_functions/asc_reduce_add.md) to perform reduction on the partial sums of the threads within the warp, and thread 0 writes the final result.

The performance data of the positive example operator is as follows:

| Task Duration (μs) | aiv_total_cycles | Read Main Memory |
| :---------------: | :--------------: | :--------------: |
|      31.772       |      49727       |      2278        |

In terms of **Task Duration**, the positive example takes 31.772 μs, a decrease of about 61.1% compared with 81.811 μs of the negative example. In terms of Read Main Memory, the positive example performs 2278 reads, a reduction of about 94.3% compared with 39808 reads of the negative example. The performance improvement comes from two aspects: first, threads within the same warp collaboratively process the same row of data, so the loop counts are basically the same and warp divergence is significantly reduced; second, threads within the warp access consecutive elements of the same row, achieving memory access coalescing and greatly reducing the number of main memory reads.

[Summary] In SIMT operators, for loop-based control flow such as for loops, keep the loop counts of threads within the same warp as consistent as possible. By adjusting the mapping between threads and data, changing "one thread processes one row of data" to "one warp collaboratively processes one row of data", you can significantly reduce warp divergence while achieving memory access coalescing, greatly improving operator performance.
