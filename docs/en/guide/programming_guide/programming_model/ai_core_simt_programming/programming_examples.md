# Programming Example<a name="ZH-CN_TOPIC_0000002523571814"></a>

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-10-28T09:22:42.012Z pushedAt=2026-11-03T07:08:03.568Z nl=2 -->

For the built-in keywords and APIs required for operator development based on SIMT, see [SIMT BuiltIn Keywords](../../language_extension/simt_builtin_keywords.md) and [SIMT Language Extension C APIs](../../language_extension/simt_language_extension_c_api.md). Currently, SIMT programming does not support some syntax structures. For related restrictions, see [Syntax Restrictions](../../../technical_appendix/cpp_standard_support/syntax_restrictions/syntax_restrictions.md).

Consider the following computation scenario: obtain 12288 rows of data at specified indices from a two-dimensional vector of shape 100000 \* 128. The formula for computing the i-th row of the operator output is as follows:

```
output[i] = input[index[i]]
```

The computation logic for one row of data is implemented in the kernel function, and multiple threads are configured to compute data of different rows. The implementation logic of the kernel function is as follows:

-   Use the thread index unique to each thread to find the data offset that the current thread needs to compute.

    ```cpp
    int32_t out_row = blockIdx.x * blockDim.x + threadIdx.x;
    ```

    A thread completes one computation of the kernel function. Inside the kernel function, the index offset is obtained by computing blockIdx.x \* blockDim.x + threadIdx.x, where blockIdx is the index of the current thread block, blockDim is the number of threads enabled in each thread block, and threadIdx is the index of the current thread within the thread block. For more details, see [SIMT BuiltIn Keywords](../../language_extension/simt_builtin_keywords.md).

-   Copy the input data at the offset position to the output based on the subscript offset, thereby obtaining the specified data.

    ```cpp
    uint32_t in_row = index[out_row];
    for (int32_t col = 0; col < in_width; col++) { 
        // Each thread processes one row of data
        int input_idx = in_row * in_width + col;
        int output_idx = out_row * in_width + col;
        gather_output[output_idx] = input[input_idx];
    }
    ```

The implementation of the kernel function is shown in the following code. For the complete sample, see [Simple Gather Operator Sample](../../../../../../examples/03_simt_api/00_introduction/01_gather/basic_gather/gather_2d).

```cpp
template <typename type_data, typename type_idx>
__global__ void gather_2d_custom(
    type_data* input,
    type_idx* index,
    type_data* gather_output,
    uint32_t in_width,
    uint32_t index_total_length)
{
    // Calculate global thread ID
    int32_t out_row = blockIdx.x * blockDim.x + threadIdx.x;

    // Maps to the row index of output tensor
    if (out_row >= index_total_length) {
        return;
    }

    // Single thread processes an entire row (all columns).
    uint32_t in_row = index[out_row];
    for (int32_t col = 0; col < in_width; col++) {
        int input_idx = in_row * in_width + col;
        int output_idx = out_row * in_width + col;
        gather_output[output_idx] = input[input_idx];
    }
}
```

The operator needs to process a total of 12288 rows of data, and each row is processed by the kernel function. Therefore, 12288 threads are required to process all the data. On the host side, the kernel function is called through <<<...\>\>\>, with 48 thread blocks enabled and 256 threads in each thread block. The sample code is as follows.

```cpp
int32_t main(int32_t argc, char* argv[])
{
    ...
    uint32_t blocks_per_grid = 48;
    uint32_t threads_per_block = 256;
    uint32_t dyn_ubuf_size = 0;  // No dynamic memory required in this sample
    // Call kernel function with <<<...>>>
    gather_2d_custom<<<blocks_per_grid, threads_per_block, dyn_ubuf_size, stream>>>(
              input_device, index_device, output_device, in_shape[1], index_total_length);
    ...
}
```
