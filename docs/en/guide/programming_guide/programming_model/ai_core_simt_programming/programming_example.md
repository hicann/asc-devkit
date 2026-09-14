# Programming Example<a name="ZH-CN_TOPIC_0000002523571814"></a>

<!-- md-trans-meta sourceCommit=d1fe9e831a69eaae67fc1f463ea1c4ea17a8f417 translatedAt=2026-08-20T11:30:55.441Z pushedAt=2026-08-26T03:06:59.459Z -->

For the built-in keywords and APIs required for SIMT-based operator development, see [SIMT Built-In Keywords](../../language_extension/SIMT-BuiltIn_keyword.md) and [SIMT Language Extension Layer C APIs](../../language_extension/simt_language_extension_layer_C_API.md). Currently, SIMT programming does not support some syntax structures. For related restrictions, see [Syntax Restrictions](../../../technical_appendix/cpp_standard_support/syntax_restrictions/syntax_restrictions.md).

Consider the following computation scenario: obtain 12288 rows of data corresponding to specified indices from a two-dimensional vector with a shape of 100000 \* 128. The formula for calculating the data in row i of the operator output is as follows:

```
output[i] = input[index[i]]
```

In the kernel function, the computation logic for one row of data is completed, and data computation for different rows is performed by configuring multiple threads. The implementation logic of the kernel function is as follows:

-   Find the data offset to be computed by the current thread based on the thread index unique to each thread.

    ```
    int32_t out_row = blockIdx.x * blockDim.x + threadIdx.x;
    ```

    A thread completes one computation operation of the kernel function. Inside the kernel function, the index offset is obtained by calculating blockIdx.x \* blockDim.x + threadIdx.x, where blockIdx is the index of the current thread block, blockDim is the number of threads enabled in each thread block, and threadIdx is the index of the current thread within the thread block. For details, see [SIMT Built-In Keywords](../../language_extension/SIMT-BuiltIn_keyword.md).

-   Copy the input data at the offset position to the output through the subscript offset, thereby implementing the function of obtaining the specified data.

    ```
    uint32_t in_row = index[out_row];
    for (int32_t col = 0; col < in_width; col++) { 
        //Each thread processes one row of data.
        int input_idx = in_row * in_width + col;
        int output_idx = out_row * in_width + col;
        gather_output[output_idx] = input[input_idx];
    }
    ```

The implementation of the kernel function is shown in the following code. For the complete sample, see [Simple Gather Operator Sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/03_simt_api/00_introduction/01_gather/basic_gather/gather_2d).

```
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

The operator needs to process a total of 12288 rows of data, with each row processed by the kernel function. Therefore, 12288 threads are required to process all the data. On the host side, the kernel function is called through <<<...\>\>\>, with 48 thread blocks and 256 threads per thread block configured. The sample code is as follows.

```
int32_t main(int32_t argc, char* argv[])
{
    ...
    uint32_t blocks_per_grid = 48;
    uint32_t threads_per_block = 256;
    uint32_t dyn_ubuf_size = 0;  // No dynamic memory required in this sample
    // Call kernel function with <<<...>>>.
    gather_2d_custom<<<blocks_per_grid, threads_per_block, dyn_ubuf_size, stream>>>(
              input_device, index_device, output_device, in_shape[1], index_total_length);
    ...
}
```