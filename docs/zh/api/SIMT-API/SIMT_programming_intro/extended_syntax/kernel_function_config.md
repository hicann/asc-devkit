# 核函数（Kernel）配置

## 核函数（Kernel）定义

核函数（Kernel）是SIMT编程的Device侧入口函数，负责协调整个算子的执行流程。函数定义语法为：

```cpp
__global__ void kernel_name(uint32_t* param1, float* param2, ...);
```

关键修饰符说明如下：

-   `__global__`：必需修饰符，作用为标识核函数（Kernel），表明可在Host侧通过<<<...\>\>\>调用。

核函数（Kernel）定义有以下几个约束：

-   返回值类型必须是void；
-   入参支持基础数据类型和基础数据类型的指针类型，如int32\_t、float、int32\_t\*、float\*等。
-   指针参数必须是指向Global Memory上的内存地址。

## \_\_launch\_bounds\_\_\(N\)

在多线程并发执行时，每个线程使用较少的寄存器可以让单个线程块内启动更多的线程，从而提升性能。因此，编译器会采用启发式算法，将寄存器溢出（register spilling）和指令数量控制在最低水平，同时尽量减少寄存器的使用量。应用程序可以通过在`__global__`函数定义中使用`__launch_bounds__()`限定符来限制启动边界（launch bounds），提供附加信息辅助编译器优化这一过程，这属于可选配置。

`__launch_bounds__()`：函数标记宏，在核函数（Kernel）上可选配置，用于在编译期指定核函数（Kernel）启动的最大线程数。若未配置`__launch_bounds__`，最大线程数默认为1024。参数N需要满足：

-   N \>= blockDim.x \* blockDim.y \*blockDim.z
-   N的取值范围为1到2048。

    最大线程数决定了每个线程可分配到的寄存器数量，具体对应关系请见下表，寄存器用于存储线程中的局部变量，若局部变量的个数超出寄存器个数，容易出现寄存器溢出等问题。建议最大线程数与核函数（Kernel）启动时配置的线程数保持一致。

    **表1**  \_\_launch\_bounds\_\_(N)与每个线程可用的寄存器个数的关系

    | N | 每个线程可用寄存器个数 |
    | --- | --- |
    | 1025~2048 | 16 |
    | 513~1024 | 32 |
    | 257~512 | 64 |
    | 1~256 | 127 |

## <<<\>\>\>调用

在SIMT编程下使用<<<\>\>\>调用`__global__`限定符修饰的函数时必须指定执行配置，形如：

```cpp
<<<blocks_per_grid, threads_per_block, dyn_ubuf_size, stream>>>
```

执行配置由4个参数决定，详细用法说明参考[核函数（Kernel）配置](../../../../guide/programming_guide/language_extension/simt_builtin_keywords.md#核函数配置)：

-   blocks\_per\_grid：[dim3](./builtin_data_types.md#dim3)类型，用于指定网格（grid）的维度与规模，blocks\_per\_grid.x \* blocks\_per\_grid.y \* blocks\_per\_grid.z等于启动的线程块总数，且不能超过65535。
-   threads\_per\_block：[dim3](./builtin_data_types.md#dim3)类型，用于指定每个线程块（block）的维度与规模，threads\_per\_block.x \* threads\_per\_block.y \* threads\_per\_block.z等于每个线程块包含的线程数，需要小于等于\_\_launch\_bounds\_\_配置。
-   dyn\_ubuf\_size：size\_t类型，用于指定每个线程块动态分配的共享内存大小，单位为字节。这部分内存供数组使用，具体用法请参考[共享内存](../../../../guide/programming_guide/programming_model/ai_core_simt_programming/memory_hierarchy.md#共享内存)中的“动态申请”方式。
-   stream：aclrtStream类型指针，指定关联的流，用于维护异步操作的执行顺序。

以下示例展示了核函数（Kernel）的声明与调用方式。

```cpp
// 声明
__global__ void add_custom(float* x, float* y, float* z, uint64_t total_length);
// 调用
add_custom<<<blocks_per_grid, threads_per_block, dyn_ubuf_size, stream>>>(x, y, z, 1024);
```
