# asc\_ldca

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：不支持
- Atlas 推理系列产品Vector Core：不支持
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas 训练系列产品：不支持
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/memory_access_functions/asc_ldca_res.md#id1 -->

## 功能说明

首先从Data Cache加载缓存数据，若未命中，则尝试从L2 Cache加载。L2 Cache与Global Memory之间的数据一致性由硬件保证，但Data Cache和Global Memory之间的数据一致性并不能保证。如果Data Cache和L2 Cache中均未找到所需数据，则从Global Memory中读取数据，然后将其缓存到L2 Cache和Data Cache中。  
默认访存的底层实现与本接口一致，例如：
```cpp
dst[idx] = src[idx];
``` 
其底层实现等价于 
```cpp
dst[idx] = asc_ldca(src + idx);
```

## 函数原型

```cpp
inline long int asc_ldca(long int* address)
```

```cpp
inline unsigned long int asc_ldca(unsigned long int* address)
```

```cpp
inline long long int asc_ldca(long long int* address)
```

```cpp
inline unsigned long long int asc_ldca(unsigned long long int* address)
```

```cpp
inline long2 asc_ldca(long2* address)
```

```cpp
inline ulong2 asc_ldca(ulong2* address)
```

```cpp
inline long4 asc_ldca(long4* address)
```

```cpp
inline ulong4 asc_ldca(ulong4* address)
```

```cpp
inline longlong2 asc_ldca(longlong2* address)
```

```cpp
inline ulonglong2 asc_ldca(ulonglong2* address)
```

```cpp
inline longlong4 asc_ldca(longlong4* address)
```

```cpp
inline ulonglong4 asc_ldca(ulonglong4* address)
```

```cpp
inline signed char asc_ldca(signed char* address)
```

```cpp
inline unsigned char asc_ldca(unsigned char* address)
```

```cpp
inline char2 asc_ldca(char2* address)
```

```cpp
inline uchar2 asc_ldca(uchar2* address)
```

```cpp
inline char4 asc_ldca(char4* address)
```

```cpp
inline uchar4 asc_ldca(uchar4* address)
```

```cpp
inline short asc_ldca(short* address)
```

```cpp
inline unsigned short asc_ldca(unsigned short* address)
```

```cpp
inline short2 asc_ldca(short2* address)
```

```cpp
inline ushort2 asc_ldca(ushort2* address)
```

```cpp
inline short4 asc_ldca(short4* address)
```

```cpp
inline ushort4 asc_ldca(ushort4* address)
```

```cpp
inline int asc_ldca(int* address)
```

```cpp
inline unsigned int asc_ldca(unsigned int* address)
```

```cpp
inline int2 asc_ldca(int2* address)
```

```cpp
inline uint2 asc_ldca(uint2* address)
```

```cpp
inline int4 asc_ldca(int4* address)
```

```cpp
inline uint4 asc_ldca(uint4* address)
```

```cpp
inline float asc_ldca(float* address)
```

```cpp
inline float2 asc_ldca(float2* address)
```

```cpp
inline float4 asc_ldca(float4* address)
```

```cpp
inline bfloat16_t asc_ldca(bfloat16_t* address)
```

```cpp
inline bfloat16x2_t asc_ldca(bfloat16x2_t* address)
```

```cpp
inline half asc_ldca(half* address)
```

```cpp
inline half2 asc_ldca(half2* address)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| address | 输入 | Global Memory的地址。 |

## 返回值说明

返回输入指向的Global Memory的数据。读取数据的执行流程如下：

- 若Data Cache命中，直接从Data Cache返回该地址对应的数据；
- 若Data Cache未命中但L2 Cache命中，从L2 Cache返回该地址对应的数据；
- 若Data Cache和L2 Cache均未命中，从Global Memory读取该地址对应的数据并返回。

## 约束说明

无

## 需要包含的头文件

使用除half、half2、bfloat16_t、bfloat16x2_t类型之外的接口需要包含`simt_api/device_functions.h`头文件，使用half和half2类型接口需要包含`simt_api/asc_fp16.h`头文件，使用bfloat16_t和bfloat16x2_t类型接口需要包含`simt_api/asc_bf16.h`头文件。

```cpp
#include "simt_api/device_functions.h"
```

```cpp
#include "simt_api/asc_fp16.h"
```

```cpp
#include "simt_api/asc_bf16.h"
```

## 调用示例

-   SIMT编程场景：

    ```cpp
    __global__ __launch_bounds__(1024) void kernel_asc_ldca(float* dst, float* src)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = asc_ldca(src + idx);
    }
    ```

-   SIMD与SIMT混合编程场景：

    SIMD与SIMT混合编程场景，需要显式使用地址空间限定符表示地址空间：`__gm__`表示Global Memory内存空间。

    ```cpp
    __simt_vf__ __launch_bounds__(1024) inline void kernel_asc_ldca(__gm__ float* dst, __gm__ float* src)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = asc_ldca(src + idx);
    }
    ```
