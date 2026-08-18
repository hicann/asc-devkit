# 核函数（Kernel）<a name="ZH-CN_TOPIC_0000002554438935"></a>

正如在[概述](./overview.md)中所介绍的，能够在AI处理器（NPU）上并行执行、并由主机端（Host）发起调用的函数被称为核函数（Kernel）。核函数（Kernel）被设计为由大量线程（Threads）同时并行运行。每个线程都拥有独立的寄存器和栈，它们协同工作以完成海量的数据计算任务。

## 核函数（Kernel）的定义<a name="section962384613331"></a>

Ascend C支持开发者自定义核函数（Kernel）来扩展C++。定义核函数（Kernel）时，通过引入特定的函数限定符，编译器能够识别该函数需要在设备端（Device）编译，并允许其从主机端被发起调用。

核函数（Kernel）的定义示例如下：

```cpp
// 核函数（Kernel）定义示例
__global__ void vec_add(float* x, float* y, float* z) 
{
    // 具体的计算逻辑
}
```

定义核函数（Kernel）时需要遵循以下规则：

-   使用函数类型限定符\_\_global\_\_，标识它是一个核函数（Kernel）。
-   核函数（Kernel）必须具有void返回类型。
-   核函数（Kernel）参数列表需遵循[函数参数列表限制](../../../technical_appendix/cpp_standard_support/syntax_restrictions/functions.md#section_kernel_function_argument_list_constraint)。

## 核函数（Kernel）的调用<a name="section33493239410"></a>

算子程序中的函数可以分为三类：host侧执行函数、核函数（Kernel）（device侧执行）、device侧执行函数（除核函数（Kernel）之外）。下图以核函数（Kernel）直调算子开发方式为例，描述三者的调用关系：

-   host侧执行函数可以调用其它host执行函数，也就是通用C/C++编程中的函数调用；也可以通过<<<...\>\>\>调用核函数（Kernel）。
-   核函数（Kernel）可以调用除核函数（Kernel）之外的其它device侧执行函数。
-   device侧执行函数（除核函数（Kernel）之外）使用类型限定符\_\_aicore\_\_标识，可以调用同类的其它device侧执行函数。

**图1**  核函数（Kernel）、host侧执行函数、device侧执行函数调用关系<a name="fig14484835135913"></a><br>
![](../../../figures/op_func_call.png "算子程序中三种函数间的调用关系")

Host侧通过核函数（Kernel）调用符<<<...>>>的语法形式调用核函数（Kernel），如下所示：

```cpp
kernel_name<<<blocks_per_grid, threads_per_block, dyn_ubuf_size, stream>>>(argument list)
```

`<<<...>>>`内的参数为核函数（Kernel）的执行配置，由4个参数决定，详细用法请参考[核函数（Kernel）配置](../../language_extension/simt_builtin_keywords.md#section97005415463)：

-   blocks\_per\_grid：[dim3](../../language_extension/simt_builtin_keywords.md#内置结构体)类型，用于指定网格（grid）的维度与规模，blocks\_per\_grid.x \* blocks\_per\_grid.y \* blocks\_per\_grid.z等于启动的线程块总数，不得大于65535。
-   threads\_per\_block：[dim3](../../language_extension/simt_builtin_keywords.md#内置结构体)类型，用于指定每个线程块（block）的维度与规模，threads\_per\_block.x \* threads\_per\_block.y \* threads\_per\_block.z等于每个线程块包含的线程数，需要小于等于\_\_launch\_bounds\_\_配置。
-   <a name="li12421238101815"></a>dyn\_ubuf\_size：size_t类型，该参数指定除静态分配的内存外，本次调用为每个线程块动态分配的共享内存字节数，单位为bytes，默认设置为0。具体用法请参考[共享内存](./memory_hierarchy.md#section66329146410)中的“动态申请”方式；
-   stream：aclrtStream类型指针，指定关联的流，用于维护异步操作的执行顺序。

## Grid与Thread索引内置变量

在核函数（Kernel）内，Ascend C提供了[内置变量](../../language_extension/simt_builtin_keywords.md#内置变量)来获取执行配置的参数以及线程或线程块的索引。

常用的内置索引变量说明如下：
-   threadIdx：获取当前线程在其所属线程块内的索引。threadIdx.x，threadIdx.y，threadIdx.z分别表示当前线程在3个维度的索引，threadIdx.x的范围为[0, blockDim.x)，threadIdx.y的范围为[0, blockDim.y)，threadIdx.z的范围为[0, blockDim.z)。
-   blockDim：获取线程块中配置的三维层次结构，即核函数（Kernel）启动时配置的dim3结构体实例值。blockDim.x，blockDim.y，blockDim.z分别表示线程块中三个维度的线程数。在核函数（Kernel）启动的执行配置中指定，对应核函数（Kernel）配置中的threads_per_block参数。
-   blockIdx: 获取当前线程块在其所属网格中的索引，表示当前线程所在的线程块在整个网格中的位置坐标。
-   gridDim：表示整个计算任务在各个维度上分别由多少个线程块构成。在核函数（Kernel）启动的执行配置中指定，对应核函数（Kernel）配置中的blocks_per_grid参数。各个维度上线程块关系需满足gridDim.x * gridDim.y * gridDim.z <= 65535。

开发者可通过组合上述变量，计算出当前线程在整个庞大计算网格空间中的全局唯一数据索引（Global Index）。这使得成千上万个执行相同代码的线程能够精准地定位并处理属于自己的特定数据分片。

### 边界检查

在实际一维并行处理开发中，待处理的数据总长度往往无法被单一线程块的尺寸完美整除。因此，通常需要分配多余的物理线程，并在核函数（Kernel）内部引入边界检查机制，以防止内存越界。

一维数据索引的标准计算公式为：

```cpp
int global_index = blockIdx.x * blockDim.x + threadIdx.x;
```

以下代码片段展示了如何结合索引机制与边界检查实现动态长度的矢量相加：

```cpp
// 1. Kernel implementation
__global__ void vec_add(float* x, float* y, float* z, int vector_length) 
{
    // Calculate the global element index corresponding to the current thread
    int global_index = blockIdx.x * blockDim.x + threadIdx.x;

    // Boundary check: Constrain the computation range to prevent excess threads from accessing out-of-bounds memory
    if (global_index < vector_length) 
    {
        // Only threads with indices within the valid range will execute the specific addition operation
        z[global_index] = x[global_index] + y[global_index];
    }
}

// 2. Host-side invocation example
int main() 
{
    // Assume memory allocation and initialization on both the Host and Device sides are complete
    int vector_length = 1000;
    
    // Empirical configuration: Specify that each block contains 256 threads
    int threads = 256; 
    
    // Calculate parameters: Calculate the required number of blocks based on the ceiling principle
    // Algorithm: (length + threads - 1) / threads
    int blocks = (vector_length + threads - 1) / threads; // The result is 4

    // Execute invocation: Launch 4 blocks, with 256 threads per block (a total of 1024 physical threads dispatched)
    vec_add<<<blocks, threads, 0, stream>>>(dev_x, dev_y, dev_z, vector_length);
    
    // ... Subsequent stream synchronization and resource release logic ...
    return 0;
}
```
在本示例中，目标处理元素总数为1000个。按单块256线程的策略划分，Host侧实际向硬件下发了4个线程块，总计启动了$4 \times 256 = 1024$个物理线程。当程序运行至最后一个线程块即blockIdx.x为3时，该块内末尾的24个线程（从索引1000到1023）推导出的global_index将落在[1000, 1023]区间。此时，通过if (global_index < vector_length)这道边界拦截，这部分超出有效数据范围的“多余”线程将直接结束任务并安全退出。该机制在保证计算结果完备性的同时，有效规避了内存越界访问的风险。
