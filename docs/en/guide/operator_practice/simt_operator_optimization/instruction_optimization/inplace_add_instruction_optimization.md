# In-place Addition Instruction Optimization

<!-- md-trans-meta sourceCommit=839e2ff5e0df3428231cc86eab4428f82d327121 translatedAt=2026-11-09T10:41:44.838Z pushedAt=2026-11-19T06:32:19.828Z nl=2 -->

[Priority] Medium

[Description] In-place addition, such as `a[index] += b[index]` and `a[index] += b[index] * c[index]`, is a common form of data update in operators. In the programs using the addition operator described above, the old value of the accumulation target `a[index]` must first be read from L2 Cache into a register before it can participate in the computation. After the addition is completed, the result value is written back from the register to L2 Cache. Each element therefore undergoes a register round trip of reading in the old value and writing out the new value. After switching to an [atomic add API](../../../../api/SIMT-API/atomic_operations/atomic_operations_intro.md) such as `asc_atomic_add(&a[index], b[index])`, the three steps of "reading the old value, computing the new value, and writing back the new value" are completed as an indivisible whole on the L2 Cache side. The old value no longer enters the register, eliminating the data round trip of moving it into and out of the register.

[Sample introduction] This section uses the [inplace_add_atomic sample](../../../../../../examples/03_simt_api/03_best_practices/03_instruction_optimizations/inplace_add_atomic/README.md) to illustrate the optimization direction described above. The sample performs element-wise accumulation on three int32_t arrays, each with a shape of [4194304], and contains two comparison groups: one is the three-operand multiply-add `a[index] += b[index] * c[index]`, and the other is the two-operand addition `a[index] += b[index]`. The sample launches 64 thread blocks with 2048 threads per block. The number of thread blocks is the number of AIV cores queried at runtime. Each comparison group changes only the code of the accumulation statement, that is, plain in-place addition or `asc_atomic_add`, while everything else remains the same.

<!-- npu="950" id1 -->
The performance data below is collected using the msOpProf tool on Ascend 950PR products, with the metric being **Task Duration**, that is, the overall task duration and the end-to-end operator duration.
<!-- end id1 -->

**Optimization principle**

The principle is the same for both comparison groups: in plain in-place addition, the old value of the accumulation target must be read from L2 Cache into a register, and after the addition is completed, the new value is written back from the register to L2 Cache. After switching to atomic add, the reading of the old value, the addition, and the write-back are all completed on the L2 Cache side as an indivisible whole, and the old value no longer undergoes the "L2 Cache → register → L2 Cache" round trip. The data flow of the two accumulation modes is shown in the following figures.

**Figure 1**  Data flow of plain in-place addition

![](../../../figures/inplace_add_atomic_dataflow_plain.png)

**Figure 2**  Data flow of atomic add

![](../../../figures/inplace_add_atomic_dataflow_atomic.png)

Take the three-operand multiply-add as an example: in plain in-place addition, `a[index]`, `b[index]`, and `c[index]` are all read into registers through the L2 Cache. After the multiply-add is completed, the new value of `a[index]` is written back from the register to the L2 Cache, producing 3 reads and 1 write per element. In atomic add, only `b[index]` and `c[index]` are read into registers and their product is computed; the product is then issued as the operand of the atomic add, and `a[index]` does not enter the register, reducing the reads per element from 3 to 2. The reads of `b` and `c` and the multiplication computation are exactly the same, so the performance difference comes solely from the eliminated round trip of the old value of `a[index]`.

**Three-operand multiply-add**

[Negative example] The old value of the accumulation target is read into a register, and the read-modify-write is completed within the core.

```cpp
__global__ __launch_bounds__(THREADS_PER_BLOCK) void vector_muladd_plain(
    int32_t* a, const int32_t* b, const int32_t* c, uint32_t element_count)
{
    uint32_t index = blockIdx.x * blockDim.x + threadIdx.x;
    const uint32_t stride = gridDim.x * blockDim.x;
    for (; index < element_count; index += stride) {
        a[index] += b[index] * c[index];
    }
}
```

In the implementation above, each thread traverses the array in a grid-stride manner, that is, each thread uses the total number of threads as a fixed stride and processes elements at different positions in the array in multiple passes. For each element, the multiply-add requires reading the old value of `a[index]` into a register and writing it back after the addition is completed. The performance data of this implementation is as follows:

| Accumulation Mode | Task Duration (μs) | Latency Relative to Baseline |
| :---: | :---: | :---: |
| a[index] += b[index] * c[index] | 51.62 | 1× |

[Positive example] Switch to atomic add to offload the accumulation to the L2 Cache side.

```cpp
__global__ __launch_bounds__(THREADS_PER_BLOCK) void vector_muladd_atomic(
    int32_t* a, const int32_t* b, const int32_t* c, uint32_t element_count)
{
    uint32_t index = blockIdx.x * blockDim.x + threadIdx.x;
    const uint32_t stride = gridDim.x * blockDim.x;
    for (; index < element_count; index += stride) {
        asc_atomic_add(&a[index], b[index] * c[index]);
    }
}
```

The implementation above has the same functionality as the negative example, with the same number of thread blocks, number of threads, and input read method. The only change is the code of the accumulation statement. In this sample, the target addresses updated by each thread do not overlap, so there is no contention on the same address, and the correctness of the operator result does not depend on atomicity. Atomic add is used here to eliminate the round trip of the old value in and out of the register and obtain performance gains. The performance data of this implementation is as follows:

| Accumulation Mode | Task Duration (μs) | Latency Relative to Baseline |
| :---: | :---: | :---: |
| asc_atomic_add | 47.51 | **0.92×** |

According to the task duration data, after switching to atomic add, the execution latency drops from 51.62 μs to 47.51 μs, a reduction of about 8.0%.

**Two-operand addition**

[Negative example] The old value of the accumulation target is read into a register, and the read-modify-write is completed within the core.

```cpp
__global__ __launch_bounds__(THREADS_PER_BLOCK) void vector_add_plain(
    int32_t* a, const int32_t* b, uint32_t element_count)
{
    uint32_t index = blockIdx.x * blockDim.x + threadIdx.x;
    const uint32_t stride = gridDim.x * blockDim.x;
    for (; index < element_count; index += stride) {
        a[index] += b[index];
    }
}
```

The performance data of this implementation is as follows:

| Accumulation Mode | Task Duration (μs) | Latency Relative to Baseline |
| :---: | :---: | :---: |
| a[index] += b[index] | 39.18 | 1× |

[Positive example] Switch to atomic add, and offload the accumulation to the L2 Cache side for completion.

```cpp
__global__ __launch_bounds__(THREADS_PER_BLOCK) void vector_add_atomic(
    int32_t* a, const int32_t* b, uint32_t element_count)
{
    uint32_t index = blockIdx.x * blockDim.x + threadIdx.x;
    const uint32_t stride = gridDim.x * blockDim.x;
    for (; index < element_count; index += stride) {
        asc_atomic_add(&a[index], b[index]);
    }
}
```

The kernel traversal method and thread scale of this implementation are exactly the same as those of the negative example; only the accumulation statement is rewritten to atomic add. In two-operand addition, the reads per element are reduced from 2 to 1, and the saving ratio of data round trips is comparable to that of three-operand multiply-add. The performance data of this implementation is as follows:

| Accumulation Mode | Task Duration (μs) | Latency Relative to Baseline |
| :---: | :---: | :---: |
| asc_atomic_add | 36.52 | **0.93×** |

According to the task duration data, after switching to atomic add, the execution latency drops from 39.18 μs to 36.52 μs, a reduction of about 6.8%.

The performance data of the two comparison groups is summarized as follows:

| Computation Form | Accumulation Mode | Task Duration (μs) | Latency Relative to Baseline |
| :---: | :---: | :---: | :---: |
| **Three-operand multiply-add** | **asc_atomic_add** | **47.51** | **0.92×** |
| Three-operand multiply-add | a[index] += b[index] * c[index] | 51.62 | 1× |
| **Two-operand addition** | **asc_atomic_add** | **36.52** | **0.93×** |
| Two-operand addition | a[index] += b[index] | 39.18 | 1× |

**Applicable boundaries**

This optimization applies only to element-wise in-place updates. Before using it, ensure that the target addresses of atomic operations across threads do not overlap:

- **Applicable element-wise in-place updates**: Each thread updates only the elements it is responsible for, so target addresses naturally do not overlap across threads, as in the grid-stride traversal of this sample.
- **Inapplicable reduction accumulation**: Multiple threads accumulate to the same address, as in histogram statistics. In this case, atomic operations on the same address can only be executed serially in a queue, and directly using atomic add significantly degrades performance due to address contention. Instead, use [hierarchical reduction](./atomic_instruction_optimization.md): first accumulate locally in the UB of each thread block, and then aggregate to GM.

In addition, the data type supported by atomic add must be a data type that supports instruction optimization. For types such as int64_t, the benefit of this execution path must be confirmed through actual measurement. Whether a better atomic instruction can be generated when the return value is not used depends on the data type. For details, see [asc_atomic_add constraints](../../../../api/SIMT-API/atomic_operations/asc_atomic_add.md#constraints).

[Summary] In in-place accumulation scenarios, if the old value of the accumulation target is used only for accumulation and does not participate in other computations, and the target addresses of threads do not overlap, consider using atomic add to complete the accumulation: the read of the old value, the addition, and the write-back are all completed on the L2 Cache side as an indivisible whole, eliminating the data round trip of the old value into and out of the register. The measured benefit is approximately 7% to 8%.
