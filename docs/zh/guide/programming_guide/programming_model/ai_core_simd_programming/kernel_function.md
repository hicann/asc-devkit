# 核函数（Kernel）<a name="ZH-CN_TOPIC_0000001564710756"></a>

核函数（Kernel）是在设备侧并行执行的C/C++函数。它由主机端代码调用，调用时被实例化为多份并行副本，分别调度到多个AI Core上运行（每个AI Core被抽象为一个Block）。这些Block利用AI Core的计算能力，协同完成某一算子的计算。


## 核函数（Kernel）的定义<a name="zh-cn_topic_0000001447989210_section1915102519220"></a>

定义核函数（Kernel）时需要遵循以下规则。

-   **使用函数类型限定符和函数执行空间限定符**

    除了需要按照C/C++函数声明的方式定义核函数（Kernel）之外，还要为核函数（Kernel）加上额外的函数类型限定符。

    必须使用\_\_global\_\_函数类型限定符来标识它是一个核函数（Kernel），可以被<<<...\>\>\>调用。同时，需要根据算子实际执行的硬件单元，选择以下一种函数执行空间限定符，详情参考[函数执行空间限定符](../../language_extension/simd_builtin_keywords.md#section1074418132518)：
    -   \_\_aicore\_\_：标识该核函数（Kernel）在设备端AI核上执行，不区分具体核类型（通常用于[耦合模式](../../advanced_programming/hardware_implementation/basic_architecture.md#section1574769433)）。
    -   \_\_vector\_\_：标识该核函数（Kernel）在设备端Vector核上执行，适用于仅包含Vector计算的算子。
    -   \_\_cube\_\_：标识该核函数（Kernel）在设备端Cube核上执行，适用于仅包含Cube计算的算子。
    -   \_\_mix\_\_：标识该核函数（Kernel）同时在Cube核和Vector核上执行，适用于包含Vector和Cube融合计算的算子。

-   **使用变量类型限定符**

    指针入参变量需要增加变量类型限定符\_\_gm\_\_，表明该指针变量指向Global Memory上某处内存地址。

-   **其他规则**<a name="zh-cn_topic_0000001447989210_li94421027152419"></a>
    1.  规则：核函数（Kernel）必须具有void返回类型。
    2.  规则：核函数（Kernel）参数列表需遵循[函数参数列表限制](../../../technical_appendix/cpp_standard_support/syntax_restrictions/functions.md#section_kernel_function_argument_list_constraint)。

-   代码示例如下：
    ```
    // Use the __global__ and __vector__ qualifiers to define kernel functions.
    __global__ __vector__ void add_kernel(__gm__ float* x, __gm__ float* y, __gm__ float* z)
    {
        ...
    }

    // Template-based definition of kernel functions is supported. An example of such a definition is provided below; it includes one type template parameter.
    template<typename T>
    __global__ __vector__ void add_kernel(__gm__ T* x, __gm__ T* y, __gm__ T* z)
    {
        ...
    }

    ```

## 核函数（Kernel）的调用

在Ascend C算子编程中，根据代码执行空间的不同，函数被严格划分为三类。它们之间的调用关系遵循如下规则：

-   Host侧执行函数：运行于主机端（CPU）。除了支持标准C/C++语义下的同类函数互调外，其核心作用是作为计算任务的发起者，通过内核调用符<<<...\>\>\>下发执行配置，跨界调用设备侧的核函数（Kernel）。
-   核函数（Kernel）：运行于设备端（NPU），是Host侧任务下发的执行入口。核函数（Kernel）在组织并行计算逻辑时，可以向下调用其他的Device侧执行函数。
-   Device侧执行函数：运行于设备端（NPU）的普通函数（除核函数（Kernel）外）。此类函数主要用于封装和复用底层计算逻辑，其调用权限被严格限制在设备侧内部，仅支持被核函数（Kernel）或同类的Device侧执行函数调用。

**图1**  核函数（Kernel）、host侧执行函数、device侧执行函数调用关系<a name="fig321723224511"></a><br>
![](../../../figures/kernel_call.png "核函数（Kernel）-host侧执行函数-device侧执行函数调用关系")

Host侧通过内核调用符<<<...>>>的语法形式调用核函数（Kernel），如下所示：

```
kernel_name<<<numBlocks, dynUBufSize, stream>>>(argument list);
```

`<<<...>>>`内的参数为核函数（Kernel）的执行配置，由3个参数决定，详细用法请参考[核函数（Kernel）配置](../../language_extension/simd_builtin_keywords.md#核函数配置)：

-   numBlocks：规定了核函数（Kernel）将会在几个核上执行，对于不同的硬件架构和算子类型，numBlocks的设置规则有一些区别，详见[核函数（Kernel）配置](../../language_extension/simd_builtin_keywords.md#核函数配置)。每个执行该核函数（Kernel）的核会被分配一个逻辑ID，即block\_idx，可以在核函数（Kernel）的实现中使用内置变量[block_idx](../../language_extension/simd_builtin_keywords.md#内置变量)获取。需要注意的是，在使用`__mix__`函数执行空间限定符的场景下，处在同一核上的Vector Core的block_idx取值相同，详细说明和示例请参考[内置变量](../../language_extension/simd_builtin_keywords.md#内置变量)；
-   dynUBufSize：Dynamic Unified Buffer（UB） Size，是配置UB动态内存分配的空间的大小（仅限UB，不包括L1 Buffer等），单位为bytes，默认设置为0；
-   stream：类型为aclrtStream，stream用于维护一些异步操作的执行顺序，确保按照应用程序中的代码调用顺序在device上执行，默认设置为nullptr。stream创建等管理接口请参考[《Runtime运行时API》](https://hiascend.com/document/redirect/CannCommunityRuntimeApi)。

如下名为add\_custom的核函数（Kernel），实现两个矢量的相加，调用示例如下：

```
// numBlocks is set to 8: Indicates that the add_kernel function is invoked on 8 cores. Each core will execute the kernel independently and in parallel.
// dynUBufSize is set to 0: Indicates that no dynamic memory space is allocated for the UB. The argument list for the kernel is x, y, z.
add_kernel<<<8, 0, stream>>>(x, y, z);

// The invocation format for a template kernel function is as follows:
// 1. Process Float32 vector addition.
add_kernel<float><<<8, 0, stream>>>(x_float, y_float, z_float);
// 2. Simply change the template parameter to int32_t to perform integer computation.
add_kernel<int32_t><<<8, 0, stream>>>(x_int, y_int, z_int);
```

核函数（Kernel）的调用是异步的，核函数（Kernel）的调用结束后，控制权立刻返回给主机端，可以调用以下**aclrtSynchronizeStream**函数来强制主机端程序等待所有核函数（Kernel）执行完毕。

```
aclError aclrtSynchronizeStream(aclrtStream stream);
```

aclrtSynchronizeStream的具体用法参考[《Runtime运行时API》](https://hiascend.com/document/redirect/CannCommunityRuntimeApi)。

## Blocks索引内置变量

在通过 <<<numBlocks, ...>>> 启动多核并行计算后，多个AI Core会同时执行同一份核函数（Kernel）代码。为了避免多个核重复处理同一块数据，我们需要在代码中对全局数据进行切分。

Ascend C提供了内置变量block_idx和block_num，帮助每个核精准定位自己需要处理的数据切片：
-   block_num：获取当前核函数（Kernel）被分配的逻辑核总数（其值等于内核调用时配置的numBlocks）。
-   block_idx：获取当前执行代码的逻辑核ID，取值范围为[0, block_num - 1]。

在Mix场景（使用`__mix__`函数执行空间限定符）下，Vector核（AIV）还需要通过sub_block_idx来确定在组合内的位置：
-   sub_block_num：获取当前组合内Vector核的数量（即每个组合中AIV的个数）。
-   sub_block_idx：获取当前Vector核在组合内的索引，取值范围为[0, sub_block_num - 1]。

在Mix场景中，AIV核的完整逻辑位置由block_idx和sub_block_idx共同确定：`logic_idx = block_idx * sub_block_num + sub_block_idx`。开发者应使用[GetBlockIdx](../../../../api/SIMD-API/basic_api/tool_interface/system_resources_and_variables/GetBlockIdx.md)和[GetSubBlockIdx](../../../../api/SIMD-API/basic_api/tool_interface/system_resources_and_variables/GetSubBlockIdx_ISASI.md) API组合获取逻辑位置，而非直接使用内置变量（更多说明参见[内置变量](../../language_extension/simd_builtin_keywords.md#内置变量)中的注意事项）。

以下示例展示了如何利用block_idx和block_num，将总长度为TOTAL_LENGTH的一维数据均匀分配给多个核进行处理：

```
// Assume the total amount of data to be processed is predefined in a macro.
constexpr uint32_t TOTAL_LENGTH = 8192;

__global__ __vector__ void add_kernel(__gm__ float* x, __gm__ float* y, __gm__ float* z)
{
    // 1. Get the logical ID of the current core and the total number of logical cores participating in the computation.
    uint32_t core_idx = block_idx;
    uint32_t core_num = block_num;

    // 2. Calculate the amount of data to be processed by the current core.
    // Assume TOTAL_LENGTH is perfectly divisible by core_num.
    uint32_t data_per_core = TOTAL_LENGTH / core_num;

    // 3. Calculate the data offset of the current core in Global Memory.
    uint32_t offset = core_idx * data_per_core;

    // 4. Offset the global pointers to obtain the addresses of the data slices belonging to the current core.
    __gm__ float* x_local = x + offset;
    __gm__ float* y_local = y + offset;
    __gm__ float* z_local = z + offset;

    // 5. Subsequent logic: Each core independently processes its own data slice (e.g., move to UB, vector addition, write back, etc.).
    // op.Init(x_local, y_local, z_local, data_per_core);
    // op.Process();
}
```
假设我们通过add_kernel<<<8, 0, stream>>>(x, y, z);启动了上述核函数（Kernel）（即block_num = 8）。
-   当block_idx = 0的核执行时，它的offset = 0，将负责处理全局数组中索引从0到1023的数据。
-   当block_idx = 1的核执行时，它的offset = 1024，将负责处理全局数组中索引从1024到2047的数据。
-   以此类推，8个核完全并行且互不干扰地完成了8192个数据的计算。通过这种分片机制，block_idx实现了软硬件之间的一一映射。
