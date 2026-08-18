# SIMT BuiltIn关键字<a name="ZH-CN_TOPIC_0000002477663934"></a>

## 函数执行空间限定符<a name="section204112391232"></a>

函数执行空间限定符（Function Execution Space Qualifier）指示函数是在Host侧执行还是在Device侧执行，以及能被调用的空间范围。

**表1**  函数执行空间限定符概览

<a name="table121121062614"></a>
| 函数执行空间限定符 | 执行空间（Host） | 执行空间（Device） | 允许调用函数空间（Host） | 允许调用函数空间（Device） |
| --- | --- | --- | --- | --- |
| \_\_host\_\_，无限定符 | √ | x | √ | x |
| \_\_aicore\_\_ | x | √ | x | √ |
| \_\_global\_\_ | x | √ | √ | x |

\_\_global\_\_修饰的函数是核函数（Kernel）入口，有以下使用约束：

-   函数返回类型必须为void，不能是class、struct或者union的成员函数。
-   不支持递归调用。
-   对\_\_global\_\_函数的调用是异步的，调用后即返回Host侧的主机线程。
-   只能被Host侧函数调用，在Device上执行。

\_\_aicore\_\_修饰的函数只能在Device侧执行，只能被\_\_global\_\_函数，或者其他\_\_aicore\_\_函数调用。

\_\_host\_\_修饰的函数只能在Host侧被调用和执行。

## 内存空间限定符<a name="section20787845172517"></a>

使用内存空间限定符\_\_ubuf\_\_来表示动、静态内存，静态内存的大小在编译期是确定的，动态内存的大小在核函数（Kernel）执行时确定。

-   静态内存通过数组分配：

    ```cpp
    __ubuf__ half staticBuf[1024];
    ```

-   动态内存通过以下方式申请使用：

    ```cpp
    extern __ubuf__ half dynamicBuf[]; 
    ```

    动态内存的实际内存大小需要在核函数（Kernel）启动时配置，具体内容请参考[核函数（Kernel）配置](#section97005415463)。

## 内置常量<a name="section784531219338"></a>

<a name="table878553753515"></a>
| 常量名 | 取值 | 功能 |
| --- | --- | --- |
| constexpr uint64_t ASC_UB_SIZE | 取值由当前AI处理器决定，若该AI处理器不存在这块空间，则默认配置为0。 | 表示当前AI处理器架构下UB的容量，可用于编译期获取UB资源大小。 |

## 内置结构体<a name="section13165113520576"></a>

-   dim3<a name="li1136665405"></a>

    dim3是内置结构体类型，由3个无符号整数组成，结构体定义为

    ```cpp
    struct dim3{
        unsigned int x, y, z;
        dim3(unsigned int x, unsigned int y = 1, unsigned int z = 1)
            :x(x), y(y), z(z) {}
    };
    ```

    用于指定3个不同维度的大小，三维总数为x \* y \* z。开发者可以通过如下方式创建dim3结构。

    ```cpp
    dim3(x); // 创建一维结构，dimy和dimz为默认值1
    dim3(x, y); // 创建二维结构，dimz为默认值1
    dim3(x, y, z); // 创建三维结构
    ```

## 内置变量<a name="section13165113520577"></a>


当前提供了以下仅在Device上可用的dim3结构的内置变量：

-   gridDim<a name="li20760123812911"></a>

    内置全局变量，只能在核函数（Kernel）中使用，表示整个计算任务在各个维度上分别由多少个线程块构成。
-   blockDim<a name="li076017381191"></a>

    内置全局变量，在核函数（Kernel）中可以直接使用，用于获取线程块中配置的线程的三维层次结构，即启动核函数（Kernel）时配置的dim3结构体实例值。blockDim.x，blockDim.y，blockDim.z分别表示线程块中三个维度的线程数。

-   blockIdx<a name="li1676053814914"></a>

    内置全局变量，只能在核函数（Kernel）中使用，用于获取块索引。表示当前线程所在的线程块在整个网格中的位置坐标。

    -   blockIdx.x的范围是[0, gridDim.x - 1]。
    -   blockIdx.y的范围是[0, gridDim.y - 1]。
    -   blockIdx.z的范围是[0, gridDim.z - 1]。

-   threadIdx<a name="li7760123814919"></a>

    内置全局变量，在核函数（Kernel）中可以直接使用，用于获取当前线程在线程块内部的索引。threadIdx.x，threadIdx.y，threadIdx.z分别表示当前线程在3个维度的索引，threadIdx.x的范围为\[0, blockDim.x\)，threadIdx.y的范围为\[0, blockDim.y\)，threadIdx.z的范围为\[0, blockDim.z\)。线程块内线程的索引与线程ID对应关系如下：

    -   对于一维线程块，其线程ID为blockIdx.x \* blockDim.x + threadIdx.x。

        ![](../../figures/thread_id_1.png)

    -   对于二维线程块，其线程ID为二维结构，其计算公式为：

        ```cpp
        thread_id_x = blockIdx.x * blockDim.x + threadIdx.x;
        thread_id_y = blockIdx.y * blockDim.y + threadIdx.y;
        ```

    -   对于三维线程块，其线程ID为三维结构，其计算公式为：

        ```cpp
        thread_id_x = blockIdx.x * blockDim.x + threadIdx.x;
        thread_id_y = blockIdx.y * blockDim.y + threadIdx.y;
        thread_id_z = blockIdx.z * blockDim.z + threadIdx.z;
        ```

当前提供了以下仅在Device上可用的int类型的内置变量：

-   warpSize

    const int32_t类型变量，表示一个线程束（Warp）中的线程数量，当前为固定值32。

## 内置数据类型<a name="section1835494915576"></a>

目前提供了一系列适用于Device侧的数据类型，包括标量和短向量。短向量是由多个元素组成的简单向量。

**表2**  标量数据类型

<a name="table114921244133211"></a>
| 类型 | 数据类型 | 描述 | Size（bit） | 取值范围 |
| --- | --- | --- | --- | --- |
| 布尔型 | bool | 全0代表false，否则代表true。 | 8 | true, false |
| 整型 | uint8_t | unsigned char | 8 | [0, 255] |
| 整型 | int8_t | signed char | 8 | [-128, 127] |
| 整型 | uint16_t | unsigned short | 16 | [0, 65535] |
| 整型 | int16_t | signed short | 16 | [-32768, 32767] |
| 整型 | uint32_t | unsigned int | 32 | [0, 4294967295] |
| 整型 | int32_t | signed int | 32 | [-2147483648, 2147483647] |
| 整型 | uint64_t | unsigned long | 64 | [0,18446744073709551615] |
| 整型 | int64_t | signed long | 64 | [-9223372036854775808, 9223372036854775807] |
| 浮点型 | float8_e4m3_t | 符号位宽1，指数位宽4，尾数位宽3 | 8 | [2<sup>6</sup> - 2<sup>9</sup>, 2<sup>9</sup> - 2<sup>6</sup>] |
| 浮点型 | float8_e5m2_t | 符号位宽1，指数位宽5，尾数位宽2 | 8 | [2<sup>13</sup> - 2<sup>16</sup>, 2<sup>16</sup> - 2<sup>13</sup>] |
| 浮点型 | hifloat8_t | 符号位宽1，点域位宽2，指数与尾数位宽由点域编码决定 | 8 | 点域编码决定数据精度与取值范围 |
| 浮点型 | half | 符号位宽1，指数位宽5，尾数位宽10 | 16 | [2<sup>5</sup> - 2<sup>16</sup>, 2<sup>16</sup> - 2<sup>5</sup>] |
| 浮点型 | bfloat16_t | 符号位宽1，指数位宽8，尾数位宽7 | 16 | [2<sup>120</sup> - 2<sup>128</sup>, 2<sup>128</sup> - 2<sup>120</sup>] |
| 浮点型 | float | 符号位宽1，指数位宽8，尾数位宽23 | 32 | [2<sup>104</sup> - 2<sup>128</sup>, 2<sup>128</sup> - 2<sup>104</sup>] |

短向量数据类型分为Vector X2、Vector X3、Vector X4，表示一个短向量变量有2、3、4个元素，当前支持的类型分布如下：

<a name="table49986503175"></a>
| 元素数据类型 | Vector X2 | Vector X3 | Vector X4 |
| --- | --- | --- | --- |
| unsigned char | uchar2 | uchar3 | uchar4 |
| signed char | char2 | char3 | char4 |
| unsigned short (16bit) | ushort2 | ushort3 | ushort4 |
| signed short (16bit) | short2 | short3 | short4 |
| unsigned int | uint2 | uint3 | uint4 |
| signed int | int2 | int3 | int4 |
| 无符号的长整型(64bit) | ulonglong2 | ulonglong3 | ulonglong4 |
| 有符号的长整型(64bit) | longlong2 | longlong3 | longlong4 |
| 无符号的长整型(64bit) | ulong2 | ulong3 | ulong4 |
| 有符号的长整型(64bit) | long2 | long3 | long4 |
| 浮点型，1符号位，2指数位，1尾数位 | float4_e2m1x2_t | - | - |
| 浮点型，1符号位，1指数位，2尾数位 | float4_e1m2x2_t | - | - |
| 浮点型，1符号位，4指数位，3尾数位 | float8_e4m3x2_t | - | - |
| 浮点型，1符号位，5指数位，2尾数位 | float8_e5m2x2_t | - | - |
| 浮点型hif8 | hifloat8x2_t | - | - |
| 浮点型，1符号位，5指数位，10尾数位 | half2 | - | - |
| 浮点型，1符号位，8指数位，7尾数位 | bfloat16x2_t | - | - |
| 浮点型，1符号位，8指数位，23尾数位 | float2 | float3 | float4 |

**表3**  短向量数据类型

<a name="table13856144241"></a>
| 数据类型 | 内存大小（字节） | 地址对齐（字节） |
| --- | --- | --- |
| char2、 uchar2 | 2 | 2 |
| char3、 uchar3、 char4、 uchar4 | 4 | 4 |
| short2、 ushort2 | 4 | 4 |
| short3、 ushort3、 short4、 ushort4 | 8 | 8 |
| int2、 uint2 | 8 | 8 |
| int3、 uint3、 int4、 uint4 | 16 | 16 |
| long2、 ulong2 | 16 | 16 |
| long3、 ulong3、 long4、 ulong4 | 32 | 32 |
| longlong2、 ulonglong2 | 16 | 16 |
| longlong3、 ulonglong3、 longlong4、 ulonglong4 | 32 | 32 |
| float2 | 8 | 8 |
| float3、 float4 | 16 | 16 |
| float4_e2m1x2_t、 float4_e1m2x2_t | 1 | 1 |
| float8_e4m3x2_t、 float8_e5m2x2_t、 hifloat8x2_t | 2 | 2 |
| half2、bfloat16x2_t | 4 | 4 |

## 运算符<a name="section186787252339"></a>

SIMT编程提供了一系列运算符，用于执行数学运算。以下是支持的运算符列表。

**表4**  SIMT编程支持的运算符列表

<a name="table11745172843710"></a>
| 类别 | 运算符 | bool | int8_t/uint8_t/int16_t/uint16_t/int32_t/uint32_t/int64_t/uint64_t | half/bfloat16_t/float | half2/bfloat16x2_t | hifloat8_t |
| --- | --- | --- | --- | --- | --- | --- |
| 算术运算符 | + | x | √ | √ | √ | x |
| 算术运算符 | - | x | √ | √ | √ | x |
| 算术运算符 | * | x | √ | √ | √ | x |
| 算术运算符 | / | x | √ | √ | √ | x |
| 算术运算符 | % | x | √ | x | x | x |
| 算术运算符 | ++ | x | √ | √ | √ | x |
| 算术运算符 | -- | x | √ | √ | √ | x |
| 算术运算符 | - (取反) | x | √ | √ | √ | x |
| 比较运算符 | &lt; | x | √ | √ | x | x |
| 比较运算符 | &lt;= | x | √ | √ | x | x |
| 比较运算符 | &gt; | x | √ | √ | x | x |
| 比较运算符 | &gt;= | x | √ | √ | x | x |
| 比较运算符 | == | x | √ | √ | x | x |
| 比较运算符 | != | x | √ | √ | x | x |
| 位运算符 | & | x | √ | x | x | x |
| 位运算符 | \| | x | √ | x | x | x |
| 位运算符 | ^ | x | √ | x | x | x |
| 位运算符 | ~ | x | √ | x | x | x |
| 位运算符 | &lt;&lt; | x | √ | x | x | x |
| 位运算符 | &gt;&gt; | x | √ | x | x | x |
| 逻辑运算符 | && | √ | √ | √ | x | x |
| 逻辑运算符 | \|\| | √ | √ | √ | x | x |
| 逻辑运算符 | ! | √ | √ | √ | x | x |
| 条件运算符 | a ? b : c | √ | √ | √ | √ | x |

> **约束说明**：除法运算符（/）当前不支持Subnormal（指数位全为0，且尾数位不全为0，表示接近零的极小值）输入，当输入为Subnormal值时会被刷为0。

运算符使用示例如下所示：

```cpp
// 加法运算
res[idx] = x[idx] + y[idx]; 

// 取反运算
x[idx] = (-x[idx]);

// 比较运算
if (x[idx] > y[idx]) {
    res[idx] = x[idx];
} else {
    res[idx] = y[idx];
}

// 按位与运算
res[idx] = x[idx] & y[idx];

// 逻辑或运算
if (x[idx] || y[idx]) {
    res[idx] = 1;
}

// 条件运算
res[idx] = x[idx] > y[idx] ? x[idx] : y[idx];
```

## 核函数（Kernel）配置<a name="核函数配置"></a><a name="section97005415463"></a>

在调用\_\_global\_\_限定符修饰的函数时必须指定执行配置。执行配置通过在函数名和带括号的参数列表之间插入如下形式的表达式来指定：

```cpp
<<<blocks_per_grid, threads_per_block, dyn_ubuf_size, stream>>>
```

其中：

-   blocks\_per\_grid：dim3类型，用于指定网格（Grid）的维度与规模。blocks\_per\_grid.x \* blocks\_per\_grid.y \* blocks\_per\_grid.z等于启动的线程块总数，不能超过65535。
-   threads\_per\_block：dim3类型，用于指定每个线程块（Thread Block）的维度与规模。threads\_per\_block.x \* threads\_per\_block.y \* threads\_per\_block.z等于每个线程块包含的线程数，需要小于等于\_\_launch\_bounds\_\_配置。
-   dyn\_ubuf\_size：size\_t类型，用于指定每个线程块动态分配的共享内存大小，单位为字节。这部分内存供数组使用，具体用法请参考[共享内存](../programming_model/ai_core_simt_programming/memory_hierarchy.md#共享内存)中的“动态申请”方式。
-   stream：aclrtStream类型，指定关联的流，用于维护异步操作的执行顺序。

以下示例展示了核函数（Kernel）的声明与调用方式。

```cpp
// 声明
__global__ void add_custom(float* x, float* y, float* z, uint64_t total_length);
// 调用
uint32_t blocks_per_grid = 48; // Number of thread blocks (Grid size)
uint32_t threads_per_block = 256; // Number of threads per block (Block size)
size_t dyn_ubuf_size = 1024; // need 1024 Byte dynamic memory
add_custom<<<blocks_per_grid, threads_per_block, dyn_ubuf_size, stream>>>(x, y, z, 1024); //blocks_per_grid和threads_per_block会被隐式转换为dim3类型
```

在执行函数之前，会先对上述配置参数进行校验。如果blocks\_per\_grid或threads\_per\_block超出设备的最大允许规模，或dyn\_ubuf\_size超过分配静态内存后剩余的可用共享内存，该函数将会执行失败。

一个核函数（Kernel）所使用的寄存器数量决定了单个线程块内可启动的线程数上限。核函数（Kernel）使用的寄存器数量通过 \_\_launch\_bounds\_\_\(\)限定符或 \_\_maxnreg\_\_\(\)限定符指定。

使用上述两个可选配置的限定符时，请注意如下约束：
-   \_\_launch\_bounds\_\_或\_\_maxnreg\_\_只能在\_\_global\_\_函数中使用。
-   同一函数不能同时配置\_\_launch\_bounds\_\_和\_\_maxnreg\_\_。

在多线程并发执行时，每个线程使用较少的寄存器可以让单个线程块内启动更多的线程。因此，编译器会采用启发式算法，将寄存器溢出（register spilling）和指令数量控制在最低水平，同时尽量减少寄存器的使用量。应用程序可以通过在\_\_global\_\_函数定义中使用\_\_launch\_bounds\_\_\(\)限定符来限制启动边界（launch bounds），提供附加信息辅助编译器优化这一过程，这属于可选配置。

-   \_\_launch\_bounds\_\_\(N\) <a name="li23861114618"></a>
  
    函数标记宏，在核函数（Kernel）上可选配置，用于指定核函数（Kernel）启动的最大线程数。最大线程数决定了每个线程可分配的寄存器数量，具体对应关系请见下表，寄存器用于存储线程中的局部变量，若局部变量的个数超出寄存器个数，容易出现寄存器溢出等问题。建议最大线程数与启动核函数（Kernel）时的dim3线程数保持一致。

    **表5**  \_\_launch\_bounds\_\_(N)与每个线程可用的寄存器个数的关系
    
    <a name="table1715318510594"></a>

    | N | 每个线程可用寄存器个数 |
    | --- | --- |
    | 1025~2048 | 16 |
    | 513~1024 | 32 |
    | 257~512 | 64 |
    | 1~256 | 127 |

    配置SIMT函数最大线程数为512，每个Thread可用寄存器数为64，示例如下：

    ```cpp
    __global__ __launch_bounds__(512) inline void add(__gm__ uint8_t* x, __gm__ uint8_t* y, __gm__ uint8_t* z)
    ```
    \_\_launch\_bounds\_\_\(N\)的参数N需要满足：
    -   N \>= dimx \* dimy \* dimz；dimx, dimy, dimz为表示线程的dim3结构体。
    -   N的取值范围为1到2048。
    -   若未配置\_\_launch\_bounds\_\_，最大线程数默认为1024。

为了支持底层性能调优，应用程序可以通过在\_\_global\_\_函数定义中使用 \_\_maxnreg\_\_\(\)限定符，用于向编译器传递性能调优意图。该限定符直接限制单个线程在一个线程块内最多可分配的寄存器数量。

-   \_\_maxnreg\_\_\(N\) <a name="section_maxnreg"></a>

    函数标记宏，在核函数（Kernel）上可选配置，用于在编译期指定单个线程在一个线程块内最多可分配的寄存器数量。

    \_\_maxnreg\_\_\(N\)的参数N需要满足：
    -   N的取值范围为(0, 128]区间的整数;
    -   若输入值N的范围为(0, 16]，则每个线程最多可以使用16个寄存器；若输入值N的范围为(16, 32]，则每个线程最多可以使用32个寄存器；若输入值N的范围为(32, 64]，则每个线程最多可以使用64个寄存器；若输入值N的范围为(64, 128]，则每个线程最多可以使用127个寄存器；
    -   若未配置\_\_maxnreg\_\_，单个线程最多可分配的寄存器数量默认为32。

    每个线程可用的最大寄存器数量对每个block实际启动的线程数有限制，具体对应关系请见下表。

    **表6**  \_\_maxnreg\_\_的每个Thread最多可分配的寄存器数与每个block实际可启动的线程数

    | 单个线程最多可分配的寄存器数量(个) | 每个block实际可启动的线程数(个) |
    | --- | --- |
    | 16 | 1~2048 |
    | 32 | 1~1024 |
    | 64 | 1~512 |
    | 127 | 1~256 |

    配置SIMT函数单个线程最多可分配的寄存器数量为64，示例如下：

    ```cpp
    __global__ __maxnreg__(64) void add(__gm__ uint8_t* x, __gm__ uint8_t* y, __gm__ uint8_t* z)
    ```
