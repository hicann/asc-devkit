# 核函数（Kernel）配置

## 核函数（Kernel）定义

核函数（Kernel）是SIMD与SIMT混合编程的Device侧入口函数，负责协调整个算子的执行流程，包括VF的调度和调用。vector计算单元的混合编程场景下，函数定义语法为：

```cpp
__global__ __vector__ void kernel_name(__gm__ type* param1, __gm__ type* param2, ...);
```

关键修饰符说明如下：

-   `__global__`：必需修饰符，作用为标识核函数（Kernel），表明可在Host侧通过<<<...\>\>\>调用。
-   `__vector__`：必需修饰符，作用为标识函数是在Device侧AIV核上执行。

核函数（Kernel）定义有以下几个约束：

-   返回值类型必须是void；
-   入参支持指针类型（需使用`__gm__`修饰）和Ascend C内置数据类型；
-   指针参数必须是指向Global Memory上的内存地址，使用`__gm__`修饰。

## \_\_launch\_bounds\_\_\(N\)

在多线程并发执行时，每个线程使用较少的寄存器可以让单个线程块内启动更多的线程，从而提升性能。因此，编译器会采用启发式算法，将寄存器溢出（register spilling）和指令数量控制在最低水平，同时尽量减少寄存器的使用量。应用程序可以通过在`__global__`函数定义中使用`__launch_bounds__()`限定符来限制启动边界（launch bounds），提供附加信息辅助编译器优化这一过程，这属于可选配置。

`__launch_bounds__(N)`：函数标记宏，在SIMT VF入口函数上可选配置，用于在编译期指定SIMT VF启动的最大线程数。若未配置`__launch_bounds__`，最大线程数默认为1024。参数N需要满足：

-   N \>= dimx \* dimy \* dimz；dimx, dimy, dimz为表示线程的dim3结构体。
-   N的取值范围为1到2048。

    最大线程数决定了每个线程可分配的寄存器数量，具体对应关系请见下表，寄存器用于存储线程中的局部变量，若局部变量的个数超出寄存器个数，容易出现寄存器溢出等问题。建议最大线程数与启动VF任务的dim3线程数保持一致。

    **表1**  \_\_launch\_bounds\_\_(N)与每个线程可用寄存器个数的关系

    | N | 每个线程可用寄存器个数 |
    | --- | --- |
    | 1025~2048 | 16 |
    | 513~1024 | 32 |
    | 257~512 | 64 |
    | 1~256 | 127 |

## <<<\>\>\>调用

核函数（Kernel）的调用是通过<<<...\>\>\>内核调用符在Host侧调用，语法如下：

```cpp
kernel_name<<<block_num, dyn_ub_size, stream>>>(args...);
```

内核调用符内的配置参数说明如下：

| 参数 | 类型 | 说明 | 约束 |
| --- | --- | --- | --- |
| block_num | uint32_t | 设置核函数（Kernel）启用的核数 | 取值范围[1, 65535] |
| dyn_ub_size | uint32_t | 指定动态内存大小，单位为字节 | 不超过最大可配置值：256KB - 8KB - 32KB - 静态内存 |
| stream | aclrtStream | 用于维护异步操作执行顺序 | 无 |

> [!NOTE]说明
> 
> 由于混合编程场景<<<>>>调用时，参数block_num的数据类型为uint32_t，因此SIMT架构中的gridDim是一维的，即：gridDim.x=block_num，gridDim.y和gridDim.z均为1。

## asc\_vf\_call调用<a name="asc-vf-call"></a>

在SIMD与SIMT混合编程场景，需使用asc\_vf\_call启动SIMT VF（Vector Function）子任务，通过参数配置，启动指定数目的线程，执行指定的SIMT VF函数。其函数原型如下：

```cpp
template <auto funcPtr, typename... Args>
__aicore__ inline void asc_vf_call(dim3 threadNums, Args &&...args)
```

其中模板参数为指定的SIMT VF函数名以及SIMT VF函数参数，具体描述如下：

**表2**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| funcPtr | 用于指定SIMT入口核函数（Kernel）。 |
| Args | 定义可变参数，用于传递实参到SIMT入口核函数（Kernel）。 |

**表3**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| threadNums | 输入 | [dim3类型](./builtin_data_types.md#dim3)，用于指定SIMT线程块内线程数量。线程总数为x * y * z，该值的大小必须小于等于2048，建议为32的倍数。 |
| args | 输入 | 可变参数，用于传递实参到SIMT入口核函数（Kernel）。 |

以下示例展示了SIMD与SIMT混合编程场景下如何使用asc\_vf\_call调用\_\_simt\_vf\_\_函数。

```cpp
#include "simt_api/common_functions.h"
__simt_vf__ inline void add_simt(
    __gm__ float* dst, __gm__ float* src0, __gm__ float* src1)
{
    // simt代码
    int idx = threadIdx.x + blockIdx.x * blockDim.x;
    dst[idx] = src0[idx] + src1[idx];
}

__global__ __vector__ void add_custom(__gm__ float* x, __gm__ float* y, __gm__ float* z)
{
    // asc_vf_call启动SIMT VF子任务，函数名为：add_simt，配置threadNums为dim3{1024, 1, 1}
    asc_vf_call<add_simt>(dim3{1024, 1, 1}, z, x, y);
}
```
