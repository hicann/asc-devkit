# asc\_stcg

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/memory_access_functions/asc_stcg_res.md#id1 -->

## 功能说明

头文件路径为：`"simt_api/device_functions.h"`（除half、half2、bfloat16_t、bfloat16x2_t类型之外的接口）、`"simt_api/asc_fp16.h"`（half和half2类型接口）、`"simt_api/asc_bf16.h"`（bfloat16_t和bfloat16x2_t类型接口）。

将指定数据存储到Global Memory的地址address中，并缓存到L2 Cache，但不缓存至Data Cache。

## 函数原型

```cpp
inline void asc_stcg(long int* address, long int val)
```

```cpp
inline void asc_stcg(unsigned long int* address, unsigned long int val)
```

```cpp
inline void asc_stcg(long long int* address, long long int val)
```

```cpp
inline void asc_stcg(unsigned long long int* address, unsigned long long int val)
```

```cpp
inline void asc_stcg(long2* address, long2 val)
```

```cpp
inline void asc_stcg(ulong2* address, ulong2 val)
```

```cpp
inline void asc_stcg(long4* address, long4 val)
```

```cpp
inline void asc_stcg(ulong4* address, ulong4 val)
```

```cpp
inline void asc_stcg(longlong2* address, longlong2 val)
```

```cpp
inline void asc_stcg(ulonglong2* address, ulonglong2 val)
```

```cpp
inline void asc_stcg(longlong4* address, longlong4 val)
```

```cpp
inline void asc_stcg(ulonglong4* address, ulonglong4 val)
```

```cpp
inline void asc_stcg(signed char* address, signed char val)
```

```cpp
inline void asc_stcg(unsigned char* address, unsigned char val)
```

```cpp
inline void asc_stcg(char* address, char val)
```

```cpp
inline void asc_stcg(char2* address, char2 val)
```

```cpp
inline void asc_stcg(uchar2* address, uchar2 val)
```

```cpp
inline void asc_stcg(char4* address, char4 val)
```

```cpp
inline void asc_stcg(uchar4* address, uchar4 val)
```

```cpp
inline void asc_stcg(short* address, short val)
```

```cpp
inline void asc_stcg(unsigned short* address, unsigned short val)
```

```cpp
inline void asc_stcg(short2* address, short2 val)
```

```cpp
inline void asc_stcg(ushort2* address, ushort2 val)
```

```cpp
inline void asc_stcg(short4* address, short4 val)
```

```cpp
inline void asc_stcg(ushort4* address, ushort4 val)
```

```cpp
inline void asc_stcg(int* address, int val)
```

```cpp
inline void asc_stcg(unsigned int* address, unsigned int val)
```

```cpp
inline void asc_stcg(int2* address, int2 val)
```

```cpp
inline void asc_stcg(uint2* address, uint2 val)
```

```cpp
inline void asc_stcg(int4* address, int4 val)
```

```cpp
inline void asc_stcg(uint4* address, uint4 val)
```

```cpp
inline void asc_stcg(float* address, float val)
```

```cpp
inline void asc_stcg(float2* address, float2 val)
```

```cpp
inline void asc_stcg(float4* address, float4 val)
```

```cpp
inline void asc_stcg(bfloat16_t* address, bfloat16_t val)
```

```cpp
inline void asc_stcg(bfloat16x2_t* address, bfloat16x2_t val)
```

```cpp
inline void asc_stcg(half* address, half val)
```

```cpp
inline void asc_stcg(half2* address, half2 val)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| address | 输入 | Global Memory的地址。 |
| val | 输入 | 源操作数。 |

## 返回值说明

无

## 约束说明

无

## 调用示例

-   SIMT编程场景：

    ```cpp
    #include "simt_api/device_functions.h"

    __global__ __launch_bounds__(1024) void kernel_asc_stcg(float* dst, float* val, uint32_t input_total_length)
    {
        uint32_t idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= input_total_length) {
            return;
        }
        asc_stcg(dst + idx, val[idx]);
    }
    ```

-   SIMD与SIMT混合编程场景：

    SIMD与SIMT混合编程场景，需要显式使用地址空间限定符表示地址空间：`__gm__`表示Global Memory内存空间。

    ```cpp
    #include "simt_api/device_functions.h"

    __simt_vf__ __launch_bounds__(1024) inline void kernel_asc_stcg(__gm__ float* dst, __gm__ float* val, uint32_t input_total_length)
    {
        uint32_t idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= input_total_length) {
            return;
        }
        asc_stcg(dst + idx, val[idx]);
    }
    ```
