# SIMD&SIMT BuiltIn关键字<a name="ZH-CN_TOPIC_0000002593949295"></a>

## 函数执行空间限定符<a name="section1074418132518"></a>

SIMD与SIMT混合编程使用的函数执行空间限定符与SIMD编程一致，详细说明请参见[SIMD BuiltIn关键字的函数执行空间限定符](simd_builtin_keywords.md#section1074418132518)。

## VF函数执行限定符

### \_\_simt\_vf\_\_<a name="section1009183746521"></a>

\_\_simt\_vf\_\_用于标记SIMT VF入口函数，函数无返回值。函数定义示例如下：

```cpp
__simt_vf__ inline void function_name(
    __gm__ uint32_t* gm_param,
    __ubuf__ float* ubuf_param,
    uint64_t scalar_param, ...);
```

SIMT VF函数定义中的关键修饰符说明如下：

| 修饰符 | 作用 |
| --- | --- |
| \_\_simt_vf\_\_ | 函数标识符，标识SIMT VF函数。 |
| inline | 建议内联，实际是否内联由编译器决定。 |
| \_\_gm\_\_ | 内存空间修饰符，标识内存空间为GM。 |
| \_\_ubuf\_\_ | 内存空间修饰符，标识内存空间为UB。 |

只支持在核函数或\_\_aicore\_\_函数中调用SIMT VF函数，调用接口为[asc\_vf\_call](../../../api/SIMT-API/SIMD_SIMT_hybrid_programming_intro/extended_syntax/kernel_function_config.md#asc_vf_call调用)，示例如下：

```cpp
asc_vf_call<function_name>(dim3(blockDim), arg1, arg2, ...);
```

SIMT VF函数有以下约束：

-   入参仅支持Ascend C的[内置数据类型](simt_builtin_keywords.md#section1835494915576)（int32\_t、uint32\_t、float、half等）及其组成的指针、数组、结构体类型，且指针类型必须指向GM或者UB内存。
-   函数返回类型必须是void。
-   SIMT VF内只能调用\_\_simt\_callee\_\_函数或\_\_callee\_\_函数。

### \_\_simt\_callee\_\_

\_\_simt\_callee\_\_子函数是SIMT VF函数内部调用的辅助函数，函数内部可以使用SIMT内置变量。定义示例如下：

```cpp
__simt_callee__ uint32_t simt_helper(__gm__ uint32_t* gm_param, __ubuf__ float* ubuf_param, uint64_t scalar_param, ...);
```

调用示例如下：

```cpp
uint32_t result = simt_helper(arg1, arg2, ...);
```

该函数有以下约束：

-   入参仅支持Ascend C的[内置数据类型](simt_builtin_keywords.md#section1835494915576)（int32\_t、uint32\_t、float、half等）及对应的指针类型。
-   函数返回值只能是Ascend C的[内置数据类型](simt_builtin_keywords.md#section1835494915576)（int32\_t、uint32\_t、float、half等）及对应的指针类型。
-   函数内只能调用\_\_simt\_callee\_\_函数或\_\_callee\_\_函数。

### \_\_simd\_vf\_\_与\_\_simd\_callee\_\_

\_\_simd\_vf\_\_用于标记SIMD VF入口函数，\_\_simd\_callee\_\_是供SIMD VF内部调用子函数，详细说明请参见[SIMD BuiltIn关键字的SIMD VF函数标识符](simd_builtin_keywords.md#section192521344610)。

## 地址空间限定符<a name="section1624210295308"></a>

SIMD与SIMT混合编程使用的地址空间限定符与SIMD编程一致，详细说明请参见[SIMD BuiltIn关键字的地址空间限定符](simd_builtin_keywords.md#section1624210295308)。

## 核函数配置

### 核函数定义

核函数是SIMD与SIMT混合编程的Device侧入口函数，负责协调整个算子的执行流程，包括VF的调度和调用。vector计算单元的混合编程场景下，函数定义语法为：

```cpp
__global__ __vector__ void kernel_name(__gm__ type* param1, __gm__ type* param2, ...);
```

关键修饰符说明如下：

-   \_\_global\_\_：必需修饰符，作用为标识核函数，表明可在Host侧通过<<<...\>\>\>调用。
-   \_\_vector\_\_：必需修饰符，作用为标识函数是在Device侧AIV核上执行。

核函数定义有以下几个约束：

-   返回值类型必须是void；
-   入参支持指针类型（需使用\_\_gm\_\_修饰）和Ascend C内置数据类型；
-   指针参数必须是指向Global Memory上的内存地址，使用\_\_gm\_\_修饰。

### \_\_launch\_bounds\_\_\(N\)

在多线程并发执行时，每个线程使用较少的寄存器可以让单个线程块内启动更多的线程，从而提升性能。因此，编译器会采用启发式算法，将寄存器溢出（register spilling）和指令数量控制在最低水平，同时尽量减少寄存器的使用量。应用程序可以通过在\_\_global\_\_函数定义中使用\_\_launch\_bounds\_\_\(\)限定符来限制启动边界（launch bounds），提供附加信息辅助编译器优化这一过程，这属于可选配置。

\_\_launch\_bounds\_\_\(N\)：函数标记宏，在SIMT VF入口函数上可选配置，用于在编译期指定SIMT VF启动的最大线程数，参数N的取值范围为1到2048。若未配置\_\_launch\_bounds\_\_，最大线程数默认为1024。

最大线程数决定了每个线程可分配的寄存器数量，具体对应关系请见下表，寄存器用于存储线程中的局部变量，若局部变量的个数超出寄存器个数，容易出现栈溢出等问题。建议最大线程数与启动VF任务的dim3线程数保持一致。

**表1**  \_\_launch\_bounds\_\_(N)与每个线程可用寄存器个数的关系

| N | 每个线程可用寄存器个数 |
| --- | --- |
| 1025~2048 | 16 |
| 513~1024 | 32 |
| 257~512 | 64 |
| 1~256 | 127 |

### <<<\>\>\>调用

SIMD与SIMT混合编程是在SIMD编程模型的核函数执行流程基础上引入SIMT VF（Vector Function）子任务。核函数仍按SIMD编程方式在Host侧通过<<<...\>\>\>启动，语法如下：

```cpp
kernel_name<<<block_num, dyn_ub_size, stream>>>(args...);
```

内核调用符内的配置参数说明如下：

| 参数 | 类型 | 说明 | 约束 |
| --- | --- | --- | --- |
| block_num | uint32_t | 设置核函数启用的核数 | 取值范围[1, 65535] |
| dyn_ub_size | uint32_t | 指定动态内存大小，单位为字节 | 不超过最大可配置值：256KB - 8KB - 32KB - 静态内存 |
| stream | aclrtStream | 用于维护异步操作执行顺序 | 无 |

## VF函数的asc\_vf\_call调用

### SIMT VF的asc\_vf\_call调用
SIMD与SIMT混合编程以SIMD核函数作为Device侧入口，在核函数或`__aicore__`函数中通过`asc_vf_call`启动SIMT VF子任务，通过参数配置，启动指定数目的线程，执行指定的SIMT VF函数。其函数原型如下：

```cpp
template <auto funcPtr, typename... Args>
__aicore__ inline void asc_vf_call(dim3 threadNums, Args &&...args)
```

其中模板参数为指定的SIMT VF函数名以及SIMT VF函数参数，具体描述如下：

**表2**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| funcPtr | 用于指定SIMT入口核函数。 |
| Args | 定义可变参数，用于传递实参到SIMT入口核函数。 |

**表3**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| threadNums | 输入 | dim3结构，定义为{dimx，dimy，dimz}，用于指定SIMT线程块内线程数量。线程总数为dimx * dimy * dimz，该值的大小必须小于等于2048，建议为32的倍数。 |
| args | 输入 | 可变参数，用于传递实参到SIMT入口核函数。 |

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
    // asc_vf_call启动SIMT VF子任务，函数名为：add_simt，配置blockDim为dim3{1024, 1, 1}
    asc_vf_call<add_simt>(dim3{1024, 1, 1}, z, x, y);
}
```

### SIMD VF的asc\_vf\_call调用

使用`asc_vf_call`接口调用SIMD VF入口函数启动VF子任务，详细接口说明参见
[asc\_vf\_call接口说明](../../../api/SIMD-API/basic_api/reg_vector_compute/vf_call/asc_vf_call.md)。

### SIMT VF与SIMD VF的调用差异

SIMT VF与SIMD VF均通过`asc_vf_call`接口在核函数或`__aicore__`函数中启动VF子任务，但二者面向的并行执行模型不同：SIMT VF以线程为基本执行单元，实现线程级并行；SIMD VF基于矢量寄存器对连续数据实现数据级并行。执行模型的差异进一步决定了二者在调用形式上存在差异，具体如下表所示：

| 对比项 | SIMT VF调用 | SIMD VF调用 |
| --- | --- | --- |
| VF入口函数标识 | 使用__simt_vf__修饰。 | 使用__simd_vf__修饰。 |
| 调用形式 | asc_vf_call<simt_func>(dim3(threadNums), arg1, arg2, ...) | asc_vf_call<simd_func>(arg1, arg2, ...) |
| 第一个运行时参数 | 必须传入dim3类型的线程配置，用于指定SIMT线程块内线程数量。 | 没有线程配置参数，调用参数从SIMD VF函数的第一个实参开始传入。 |
| 指针入参 | 支持指向GM或UB的指针，需按实际内存空间使用__gm__或__ubuf__修饰。 | 指针入参需使用__ubuf__地址空间限定符修饰，不能直接访问GM内存。 |
| 典型用途 | 处理复杂控制流、离散访存、线程级索引映射等不规则片段。 | 处理连续规整的向量计算片段。 |

## 内置变量<a name="zh-cn_topic_0000002571575581_section13165113520576"></a>

当前提供了以下仅在SIMT VF内可用的内置变量：  
-   gridDim<a name="zh-cn_topic_0000002571575581_li20760123812911"></a>

    内置全局变量，只能在SIMT VF函数中使用，表示整个计算任务在各个维度上分别由多少个线程块构成。gridDim.x <= 65535；gridDim.y、gridDim.z必须为1。

-   blockDim<a name="zh-cn_topic_0000002571575581_li076017381191"></a>

    内置全局变量，在SIMT VF函数中可以直接使用，用于获取线程块中配置的线程的三维层次结构。blockDim.x，blockDim.y，blockDim.z分别表示线程块中三个维度的线程数，各个维度上线程数需满足blockDim.x \* blockDim.y \*blockDim.z  <= 2048。

-   blockIdx

    内置全局变量，只能在SIMT VF函数中使用，用于获取块索引。表示当前线程所在的线程块在整个网格中的位置坐标。blockIdx.x的范围是0到gridDim.x - 1，blockIdx.y和blockIdx.z目前只能返回0。

-   threadIdx

    内置全局变量，只能在SIMT VF函数中使用，用于获取当前线程在线程块内部的索引。threadIdx.x，threadIdx.y，threadIdx.z分别表示当前线程在3个维度的索引，threadIdx.x的范围为\[0, blockDim.x\)，threadIdx.y的范围为\[0, blockDim.y\)，threadIdx.z的范围为\[0, blockDim.z\)。线程块内线程的索引计算方式如下：

    -   对于一维线程块，线程块内线程的索引为threadIdx.x。
    -   对于二维线程块，线程块内线程的索引为\(threadIdx.x + threadIdx.y \* blockDim.x\)。
    -   对于三维线程块，线程块内线程的索引为\(threadIdx.x + threadIdx.y \* blockDim.x + threadIdx.z \* blockDim.x \* blockDim.y\)。

-   warpSize

    运行时变量，表示一个线程束（Warp）中的线程数量，当前为固定值32。

SIMD核函数层级的内置变量参见[SIMD核函数层内置变量](./simd_builtin_keywords.md#内置变量)。

### 不同层级的内置变量使用限制

Host侧<<<...\>\>\>调用配置的是外层核函数的逻辑核数；SIMT VF内部的线程架构类内置变量描述的是SIMT VF子任务的线程层次。两类概念所属层级不同，不能混用，下表列出了SIMD核函数层级和SIMT VF线程层级中常见概念的差异。

| 内置变量或方法 | 所属层级 | 使用位置 | 含义 |
| --- | --- | --- | --- |
| `AscendC::GetBlockNum()` | SIMD核函数层级 | 核函数或`__aicore__`函数，SIMT VF内部不可见 | 获取Host侧<<<...\>\>\>调用配置的核函数逻辑核数，即`block_num`。 |
| `AscendC::GetBlockIdx()` | SIMD核函数层级 | 核函数或`__aicore__`函数，SIMT VF内部不可见 | 获取当前执行核函数的逻辑核索引。 |
| `gridDim` | SIMT VF线程层级 | `__simt_vf__`或`__simt_callee__`函数，SIMT VF外部不可见 | 获取SIMT VF线程层次中的网格维度，表示本次SIMT VF子任务在各维度上的线程块数量。其值由Host侧<<<...\>\>\>调用配置的核函数逻辑核数决定。 |
| `blockIdx` | SIMT VF线程层级 | `__simt_vf__`或`__simt_callee__`函数，SIMT VF外部不可见 | 获取SIMT VF线程层次中的线程块索引，表示当前线程所在的SIMT线程块在VF执行网格中的位置。 |
| `blockDim` | SIMT VF线程层级 | `__simt_vf__`或`__simt_callee__`函数，SIMT VF外部不可见 | 获取SIMT VF线程块内的线程三维结构，其值由调用SIMT VF时`asc_vf_call`的第一个`dim3`参数指定。 |
| `threadIdx` | SIMT VF线程层级 | `__simt_vf__`或`__simt_callee__`函数，SIMT VF外部不可见 | 获取当前SIMT线程在线程块内部的索引。 |

使用时需注意：

-   SIMD侧核函数不能使用SIMT VF线程层级相关内置变量，SIMT VF也无法感知SIMD核函数层级的逻辑核编号或逻辑核数量。
-   如果SIMT VF需要使用外层SIMD流程中的逻辑核编号或逻辑核数量，建议在核函数或`__aicore__`函数中通过`AscendC::GetBlockIdx()`、`AscendC::GetBlockNum()`获取对应值后，以普通参数传入SIMT VF。

## 内置数据类型<a name="zh-cn_topic_0000002571575581_section1880403364916"></a><a name="cn_topic_0000002571575581_section1880403364916"></a>

SIMD与SIMT混合编程使用的内置数据类型与SIMT编程一致，详细说明请参见[SIMT BuiltIn关键字的内置数据类型](simt_builtin_keywords.md#section1835494915576)。

## 运算符<a name="zh-cn_topic_0000002571575581_section186787252339"></a>

SIMD与SIMT混合编程支持的运算符与SIMT编程一致，详细说明请参见[SIMT BuiltIn关键字的运算符](simt_builtin_keywords.md#section186787252339)。
