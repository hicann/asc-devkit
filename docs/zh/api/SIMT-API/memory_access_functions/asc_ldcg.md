# asc\_ldcg

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/memory_access_functions/asc_ldcg_res.md#id1 -->

## 功能说明

头文件路径为：`"simt_api/device_functions.h"`（除half、half2、bfloat16_t、bfloat16x2_t类型之外的接口）、`"simt_api/asc_fp16.h"`（half和half2类型接口）、`"simt_api/asc_bf16.h"`（bfloat16_t和bfloat16x2_t类型接口）。

从L2 Cache加载缓存的数据，如果缓存命中，则直接返回数据。L2 Cache与Global Memory之间的数据一致性由硬件保证。若未命中，则从Global Memory地址预加载数据缓存至L2 Cache，并返回数据。

## 函数原型

```cpp
inline long int asc_ldcg(long int* address)
```

```cpp
inline unsigned long int asc_ldcg(unsigned long int* address)
```

```cpp
inline long long int asc_ldcg(long long int* address)
```

```cpp
inline unsigned long long int asc_ldcg(unsigned long long int* address)
```

```cpp
inline long2 asc_ldcg(long2* address)
```

```cpp
inline ulong2 asc_ldcg(ulong2* address)
```

```cpp
inline long4 asc_ldcg(long4* address)
```

```cpp
inline ulong4 asc_ldcg(ulong4* address)
```

```cpp
inline longlong2 asc_ldcg(longlong2* address)
```

```cpp
inline ulonglong2 asc_ldcg(ulonglong2* address)
```

```cpp
inline longlong4 asc_ldcg(longlong4* address)
```

```cpp
inline ulonglong4 asc_ldcg(ulonglong4* address)
```

```cpp
inline signed char asc_ldcg(signed char* address)
```

```cpp
inline unsigned char asc_ldcg(unsigned char* address)
```

```cpp
inline char2 asc_ldcg(char2* address)
```

```cpp
inline uchar2 asc_ldcg(uchar2* address)
```

```cpp
inline char4 asc_ldcg(char4* address)
```

```cpp
inline uchar4 asc_ldcg(uchar4* address)
```

```cpp
inline short asc_ldcg(short* address)
```

```cpp
inline unsigned short asc_ldcg(unsigned short* address)
```

```cpp
inline short2 asc_ldcg(short2* address)
```

```cpp
inline ushort2 asc_ldcg(ushort2* address)
```

```cpp
inline short4 asc_ldcg(short4* address)
```

```cpp
inline ushort4 asc_ldcg(ushort4* address)
```

```cpp
inline int asc_ldcg(int* address)
```

```cpp
inline unsigned int asc_ldcg(unsigned int* address)
```

```cpp
inline int2 asc_ldcg(int2* address)
```

```cpp
inline uint2 asc_ldcg(uint2* address)
```

```cpp
inline int4 asc_ldcg(int4* address)
```

```cpp
inline uint4 asc_ldcg(uint4* address)
```

```cpp
inline float asc_ldcg(float* address)
```

```cpp
inline float2 asc_ldcg(float2* address)
```

```cpp
inline float4 asc_ldcg(float4* address)
```

```cpp
inline bfloat16_t asc_ldcg(bfloat16_t* address)
```

```cpp
inline bfloat16x2_t asc_ldcg(bfloat16x2_t* address)
```

```cpp
inline half asc_ldcg(half* address)
```

```cpp
inline half2 asc_ldcg(half2* address)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| address | 输入 | Global Memory的地址。 |

## 返回值说明

L2 Cache中的缓存数据或输入的Global Memory地址上的数据。

## 约束说明

无

## 调用示例

-   SIMT编程场景：

    ```cpp
    #include "simt_api/device_functions.h"

    __global__ __launch_bounds__(1024) void kernel_asc_ldcg(float* dst, float* src)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = asc_ldcg(src + idx);
    }
    ```

-   SIMD与SIMT混合编程场景：

    SIMD与SIMT混合编程场景，需要显式使用地址空间限定符表示地址空间：`__gm__`表示Global Memory内存空间。

    ```cpp
    #include "simt_api/device_functions.h"

    __simt_vf__ __launch_bounds__(1024) inline void kernel_asc_ldcg(__gm__ float* dst, __gm__ float* src)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = asc_ldcg(src + idx);
    }
    ```
