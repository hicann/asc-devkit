# 编程示例<a name="ZH-CN_TOPIC_0000002523571814"></a>

基于SIMT进行算子开发需要使用的内置关键字和API请参见[SIMT BuiltIn关键字](../../语言扩展层/SIMT-BuiltIn关键字.md)、[SIMT语言扩展层C API](../../语言扩展层/SIMT语言扩展层C-API.md)。当前SIMT编程暂不支持部分语法结构，相关限制请参考[语法限制](../../../technical_appendix/cpp_standard_support/syntax_restrictions/syntax_restrictions.md)。

考虑如下计算场景：从形状为100000 \* 128的二维向量中获取指定索引所对应的12288行数据。算子输出output第i行数据的计算公式为：

```
output[i] = input[index[i]]
```

在核函数中完成一行数据量的计算逻辑，通过配置多个线程完成不同行的数据计算操作。核函数的实现逻辑具体为：

-   通过每个线程独有的线程索引找到当前线程需要计算的数据偏移量。

    ```cpp
    int32_t out_row = blockIdx.x * blockDim.x + threadIdx.x;
    ```

    一个线程完成一次核函数的计算操作，核函数内通过计算blockIdx.x \* blockDim.x + threadIdx.x得到索引偏移，其中blockIdx是当前线程块的索引，blockDim是每个线程块启用的线程数，threadIdx是当前线程在线程块内的索引，更多详细介绍请参考[SIMT BuiltIn关键字](../../语言扩展层/SIMT-BuiltIn关键字.md)。

-   通过下标偏移将偏移位置的输入数据拷贝到输出中，从而完成获取指定数据的功能。

    ```cpp
    uint32_t in_row = index[out_row];
    for (int32_t col = 0; col < in_width; col++) { 
        //每个线程处理一行数据
        int input_idx = in_row * in_width + col;
        int output_idx = out_row * in_width + col;
        gather_output[output_idx] = input[input_idx];
    }
    ```

核函数的实现参考如下代码。完整的样例请参考[简单gather算子样例](../../../../../../examples/03_simt_api/00_introduction/01_gather/basic_gather/gather_2d)。

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

算子需要处理总共12288行数据，每行数据由核函数完成处理，因此需要12288个线程来完成对所有数据的处理。在Host侧通过<<<...\>\>\>调用核函数，同时设置启动48个线程块、每个线程块包含256个线程，示例代码如下。

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
