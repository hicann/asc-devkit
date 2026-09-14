# Programming Example<a name="ZH-CN_TOPIC_0000002563469531"></a>

<!-- md-trans-meta sourceCommit=4c81e54975ec65486e709c6cf9f279160b0190ce translatedAt=2026-08-26T11:27:58.204Z pushedAt=2026-09-01T02:45:41.856Z -->

This section uses the [gather & adds hybrid programming example](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/05_simd_simt_hybrid/00_introduction/simd_simt_gather_and_adds) to demonstrate the key syntax of SIMD and SIMT hybrid programming.

## Scenarios<a name="zh-cn_topic_0000002563469531_section164411381518"></a>

The sample reads 8192 elements in a scattered manner from the one-dimensional input `input` of length 100000 at the positions specified by the `index` array, and adds 1 to the read results:

```cpp
output[i] = input[index[i]] + 1
```

The discrete read `input[index[i]]` is suitable for parallel execution by SIMT threads, while the add-1 operation on contiguous UB data is suitable for SIMD vector computation.

| Data | Shape | Data Type | Description |
| --- | --- | --- | --- |
| input | [100000] | float | Input data to be read in a scattered manner |
| index | [8192] | uint32_t | Input index corresponding to each output element |
| output | [8192] | float | Output result of adding 1 after gather |

The sample launches 8 Vector Cores, each processing 1024 output elements. Within each core, a SIMT VF function is launched through `asc_vf_call`, and `dim3(THREAD_COUNT)` configures 1024 SIMT threads, with each thread responsible for one `index` element.

## Defining a SIMT VF Function<a name="zh-cn_topic_0000002563469531_section1998615411431"></a>

A SIMT VF function is decorated with `__simt_vf__`. Inside the function, SIMT built-in variables such as `blockIdx`, `blockDim`, and `threadIdx` can be used. In the following code, each thread reads an `index` based on its own `threadIdx.x`, then reads the corresponding data from the GM input `input` and writes it to the UB cache `local_output`.

```cpp
constexpr uint32_t THREAD_COUNT = 1024;

__simt_vf__ __launch_bounds__(THREAD_COUNT) inline void simt_gather(
    __gm__ float* input, __gm__ uint32_t* index, __ubuf__ float* local_output,
    uint32_t input_total_length, uint32_t index_total_length, uint32_t output_total_length)
{
    if (threadIdx.x >= output_total_length) {
        return;
    }

    uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx >= index_total_length) {
        return;
    }

    uint32_t gather_idx = index[idx];
    if (gather_idx >= input_total_length) {
        return;
    }

    local_output[threadIdx.x] = input[gather_idx];
}
```

The key points are as follows:

-   `__simt_vf__` indicates that the function is executed on the SIMT unit.
-   `__launch_bounds__(THREAD_COUNT)` declares the maximum number of threads of the SIMT VF function.
-   `__gm__` is used to identify a GM pointer, and `__ubuf__` is used to identify a UB pointer.
-   `threadIdx.x` indicates the index of the current thread within the thread block, and is used to let different threads process different elements.

## Calling VF Functions<a name="zh-cn_topic_0000002563469531_section199351451451"></a>

In the `__global__ __vector__` kernel function, allocate UB space as shared memory, call the SIMT VF function through `asc_vf_call<simt_gather>` to complete the gather computation, and then call the SIMD VF function through `asc_vf_call<simd_adds>` to perform the subsequent addition computation. When calling the SIMT VF function, the first parameter `dim3(THREAD_COUNT)` of `asc_vf_call` indicates that this SIMT VF call launches 1024 threads.

```cpp
__global__ __vector__ void gather_and_adds_kernel(
    __gm__ float* input, __gm__ uint32_t* index, __gm__ float* output,
    uint32_t input_total_length, uint32_t index_total_length)
{
    uint32_t index_total_length_per_block = index_total_length / AscendC::GetBlockNum();
    extern __ubuf__ float local_output[];

    asc_vf_call<simt_gather>(
        dim3(THREAD_COUNT), input, index, local_output,
        input_total_length, index_total_length, index_total_length_per_block);

    constexpr uint32_t one_repeat_size = AscendC::GetVecLen() / sizeof(float);
    uint16_t repeat_times = (index_total_length_per_block + one_repeat_size - 1) / one_repeat_size;

    // Launch the SIMD VF function to add 1 to the Gather result.
    asc_vf_call<simd_adds>(
        local_output, local_output,
        index_total_length_per_block, one_repeat_size, repeat_times);
}
```

After `asc_vf_call<simt_gather>` is called, a SIMT thread block is launched in the current Vector Core. In this sample, `index_total_length_per_block` is 1024 and `THREAD_COUNT` is also 1024, so a single thread block can process all output elements that the current core is responsible for.

## Configuring Kernel Launch<a name="zh-cn_topic_0000002563469531_section1680714261755"></a>

On the host side, the `__global__ __vector__` kernel is launched using the `<<<>>>` kernel launch operator. In the following code, `num_blocks` indicates that eight Vector Cores are launched, `dyn_ub_size` indicates the dynamic UB size, which is set to `THREAD_COUNT * sizeof(float)` in this sample, and `stream` is used to maintain the asynchronous execution order.

```cpp
constexpr uint32_t input_total_length = 100000;
constexpr uint32_t index_total_length = 8 * 1024;
constexpr uint32_t num_blocks = 8;
constexpr uint32_t dyn_ub_size = THREAD_COUNT * sizeof(float);

gather_and_adds_kernel<<<num_blocks, dyn_ub_size, stream>>>(
    input_device, index_device, output_device, input_total_length, index_total_length);
```

The three configuration parameters in `<<<>>>` are described as follows:

| Parameter | Value in This Sample | Description |
| --- | --- | --- |
| num_blocks | 8 | Launches eight Vector Cores, each processing 1024 output elements |
| dyn_ub_size | THREAD_COUNT * sizeof(float) | Configures the dynamic UB space for the kernel |
| stream | stream | Specifies the ACL runtime stream |

## Data Synchronization<a name="zh-cn_topic_0000002563469531_section_data_sync"></a>

SIMT VF and SIMD VF belong to the Vector pipeline and execute asynchronously with MTE data transfer. Therefore, correct synchronization between data transfer operations and VF function calls must be ensured. In this example, the SIMT VF function directly reads GM input data and does not involve data transfer in. After the SIMD VF finishes execution, data must be written from UB to GM, which requires a synchronization instruction to guarantee the operation order. The overall process is as follows:

```cpp
asc_vf_call<simt_gather>(...);
asc_vf_call<simd_adds>(...);
// Synchronize the Vector computation pipeline and the MTE3 transfer-out pipeline to ensure that the Adds result is written to UB before being transferred to GM.
asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
asc_copy_ub2gm_align(...);
```

## Results<a name="zh-cn_topic_0000002563469531_section1184165711616"></a>

After the sample is executed, `output/output.bin` is generated. The `i`-th element in the output satisfies:

```cpp
output[i] = input[index[i]] + 1
```

The complete project includes an input generation script and a result verification script. When the accuracy verification passes, the following is output:

```text
[Success] Case accuracy is verification passed.
```
