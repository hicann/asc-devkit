# 内置数据类型

SIMT编程提供了一系列适用于Device侧的数据类型，包括标量、短向量以及dim3结构体。

## 标量类型

标量数据类型覆盖布尔型（bool）、整型（uint8/int8到uint64/int64）、浮点型（float8\_e4m3、float8\_e5m2、hifloat8、half、bfloat16、float）。

- 布尔型：  
  bool：全0代表false，否则代表true；占用8bit内存；取值为true或false。
- 整型：
  - uint8_t：unsigned char；占用8bit内存；取值范围为[0, 255]。
  - int8_t：signed char；占用8bit内存；取值范围为[-128, 127]。
  - uint16_t：unsigned short；占用16bit内存；取值范围为[0, 65535]。
  - int16_t：signed short；占用16bit内存；取值范围为[-32768, 32767]。
  - uint32_t：unsigned int；占用32bit内存；取值范围为[0, 4294967295]。
  - int32_t：signed int；占用32bit内存；取值范围为[-2147483648, 2147483647]。
  - uint64_t：unsigned long；占用64bit内存；取值范围为[0, 18446744073709551615]。
  - int64_t：signed long；占用64bit内存；取值范围为[-9223372036854775808, 9223372036854775807]。
- 浮点型：
  - float8_e4m3_t：符号位宽1，指数位宽4，尾数位宽3；占用8bit内存；取值范围为[2^6 - 2^9, 2^9 - 2^6]。
  - float8_e5m2_t：符号位宽1，指数位宽5，尾数位宽2；占用8bit内存；取值范围为[2^13 - 2^16, 2^16 - 2^13]。
  - hifloat8_t：符号位宽1，点域位宽2，指数与尾数位宽由点域编码决定；占用8bit内存；点域编码决定数据精度与取值范围。
  - half：符号位宽1，指数位宽5，尾数位宽10；占用16bit内存；取值范围为[2^5 - 2^16, 2^16 - 2^5]。
  - bfloat16_t：符号位宽1，指数位宽8，尾数位宽7；占用16bit内存；取值范围为[2^120 - 2^128, 2^128 - 2^120]。
  - float：符号位宽1，指数位宽8，尾数位宽23；占用32bit内存；取值范围为[2^104 - 2^128, 2^128 - 2^104]。

## 短向量类型

短向量类型是一种在SIMT编程模型中提供的固定长度向量类型，用于简化向量数据的表示和操作。该类型适用于处理包含多个分量的数据，如坐标、颜色、向量运算等。

-   内存特点：

    紧凑存储：短向量类型在内存中连续存储，无填充。

    对齐要求：遵循自然对齐原则，提升访问效率。

    跨线程共享：可存储在Unified Buffer（UB）中供线程块内共享。

    直接内存访问：支持直接从Global Memory加载和存储。

-   应用场景：

    颜色处理：RGB/RGBA颜色值的操作。

    向量运算：物理模拟、图形渲染中的向量计算。

    数据打包：将多个相关值打包处理。

    内存访问优化：通过向量化提升内存带宽利用率。

-   短向量变量访问：

    变量通过.x、.y、.z、.w的方式进行访问。

当前已支持的短向量数据类型如下：

**表1**  短向量数据类型

| 元素数据类型 | Vector X2 | Vector X4 |
| --- | --- | --- |
| unsigned char | uchar2 | uchar4 |
| signed char | char2 | char4 |
| unsigned short (16bit) | ushort2 | ushort4 |
| signed short (16bit) | short2 | short4 |
| unsigned int | uint2 | uint4 |
| signed int | int2 | int4 |
| unsigned long long (64bit) | ulonglong2 | ulonglong4 |
| signed long long (64bit) | longlong2 | longlong4 |
| unsigned long (64bit) | ulong2 | ulong4 |
| signed long (64bit) | long2 | long4 |
| 浮点型，1符号位，2指数位，1尾数位 | float4_e2m1x2_t | - |
| 浮点型，1符号位，1指数位，2尾数位 | float4_e1m2x2_t | - |
| 浮点型，1符号位，4指数位，3尾数位 | float8_e4m3x2_t | - |
| 浮点型，1符号位，5指数位，2尾数位 | float8_e5m2x2_t | - |
| 浮点型hifloat8 | hifloat8x2_t | - |
| 浮点型，1符号位，5指数位，10尾数位 | half2 | - |
| 浮点型，1符号位，8指数位，7尾数位 | bfloat16x2_t | - |
| 浮点型，1符号位，8指数位，23尾数位 | float2 | float4 |

每种短向量的内存大小与地址对齐大小如下：

**表2**  短向量数据类型内存大小

| 数据类型 | 内存大小（字节） | 地址对齐（字节） |
| --- | --- | --- |
| char2, uchar2 | 2 | 2 |
| char4, uchar4 | 4 | 4 |
| short2, ushort2 | 4 | 4 |
| short4, ushort4 | 8 | 8 |
| int2, uint2 | 8 | 8 |
| int4, uint4 | 16 | 16 |
| long2, ulong2 | 16 | 16 |
| long4, ulong4 | 32 | 32 |
| longlong2, ulonglong2 | 16 | 16 |
| longlong4, ulonglong4 | 32 | 32 |
| float2 | 8 | 8 |
| float4 | 16 | 16 |
| float4_e2m1x2_t, float4_e1m2x2_t | 1 | 1 |
| float8_e4m3x2_t, float8_e5m2x2_t, hifloat8x2_t | 2 | 2 |
| half2, bfloat16x2_t | 4 | 4 |

SIMT编程提供了用于构造短向量的函数。这些构造函数可以将固定个数的同类型标量值组合成一个短向量类型，如make\_int2函数功能为将两个int类型的标量作为输入，组合成一个int2类型的短向量类型作为输出。函数列表如下：

-   make_int2

    ```cpp
    inline int2 make_int2(int x, int y)
    ```

-   make_int4

    ```cpp
    inline int4 make_int4(int x, int y, int z, int w)
    ```

-   make_uint2

    ```cpp
    inline uint2 make_uint2(unsigned int x, unsigned int y)
    ```

-   make_uint4

    ```cpp
    inline uint4 make_uint4(unsigned int x, unsigned int y, unsigned int z, unsigned int w)
    ```

-   make_ulonglong2

    ```cpp
    inline ulonglong2 make_ulonglong2(unsigned long long int x, unsigned long long int y)
    ```

-   make_ulonglong4

    ```cpp
    inline ulonglong4 make_ulonglong4(unsigned long long int x, unsigned long long int y, unsigned long long int z, unsigned long long int w)
    ```

-   make_longlong2

    ```cpp
    inline longlong2 make_longlong2(long long int x, long long int y)
    ```

-   make_longlong4

    ```cpp
    inline longlong4 make_longlong4(long long int x, long long int y, long long int z, long long int w)
    ```

-   make_ulong2

    ```cpp
    inline ulong2 make_ulong2(unsigned long int x, unsigned long int y)
    ```

-   make_ulong4

    ```cpp
    inline ulong4 make_ulong4(unsigned long int x, unsigned long int y, unsigned long int z,unsigned long int w)
    ```

-   make_long2

    ```cpp
    inline long2 make_long2(long int x, long int y)
    ```

-   make_long4

    ```cpp
    inline long4 make_long4(long int x, long int y, long int z, long int w)
    ```

-   make_float2

    ```cpp
    inline float2 make_float2(float x, float y)
    ```

-   make_float4

    ```cpp
    inline float4 make_float4(float x, float y, float z, float w)
    ```

-   make_short2

    ```cpp
    inline short2 make_short2(short x, short y)
    ```

-   make_short4

    ```cpp
    inline short4 make_short4(short x, short y, short z, short w)
    ```

-   make_ushort2

    ```cpp
    inline ushort2 make_ushort2(unsigned short x, unsigned short y)
    ```

-   make_ushort4

    ```cpp
    inline ushort4 make_ushort4(unsigned short x, unsigned short y, unsigned short z,unsigned short w)
    ```

-   make_uchar2

    ```cpp
    inline uchar2 make_uchar2(unsigned char x, unsigned char y)
    ```

-   make_uchar4

    ```cpp
    inline uchar4 make_uchar4(unsigned char x, unsigned char y, unsigned char z, unsigned char w)
    ```

-   make_char2

    ```cpp
    inline char2 make_char2(signed char x, signed char y)
    ```

-   make_char4

    ```cpp
    inline char4 make_char4(signed char x, signed char y, signed char z, signed char w)
    ```

-   make_half2

    ```cpp
    inline half2 make_half2(half x, half y)
    ```

-   make_bfloat162

    ```cpp
    inline bfloat16x2_t make_bfloat162(bfloat16_t x, bfloat16_t y)
    ```

使用短向量构造函数需要包含`simt_api/vector_functions.h`，调用示例如下：

```cpp
#include "simt_api/vector_functions.h"
__global__ __launch_bounds__(1024) void kernel_make_int2(int2* dst, int* x, int* y)
{
    int idx = threadIdx.x + blockIdx.x * blockDim.x;
    dst[idx] = make_int2(x[idx], y[idx]);
}
```

## dim3<a name="section02911713103312"></a>

用于指定和获取线程网格（Grid）、线程块（Thread Block）在x、y、z维度上的内置结构体。

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
