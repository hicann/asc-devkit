# asc\_stwt

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/memory_access_functions/asc_stwt_res.md#id1 -->

## 功能说明

将指定数据存储到Global Memory的地址`address`中，并缓存至Data Cache和L2 Cache。

## 函数原型

```cpp
inline void asc_stwt(long int* address, long int val)
```

```cpp
inline void asc_stwt(unsigned long int* address, unsigned long int val)
```

```cpp
inline void asc_stwt(long long int* address, long long int val)
```

```cpp
inline void asc_stwt(unsigned long long int* address, unsigned long long int val)
```

```cpp
inline void asc_stwt(long2* address, long2 val)
```

```cpp
inline void asc_stwt(ulong2* address, ulong2 val)
```

```cpp
inline void asc_stwt(long4* address, long4 val)
```

```cpp
inline void asc_stwt(ulong4* address, ulong4 val)
```

```cpp
inline void asc_stwt(longlong2* address, longlong2 val)
```

```cpp
inline void asc_stwt(ulonglong2* address, ulonglong2 val)
```

```cpp
inline void asc_stwt(longlong4* address, longlong4 val)
```

```cpp
inline void asc_stwt(ulonglong4* address, ulonglong4 val)
```

```cpp
inline void asc_stwt(signed char* address, signed char val)
```

```cpp
inline void asc_stwt(unsigned char* address, unsigned char val)
```

```cpp
inline void asc_stwt(char* address, char val)
```

```cpp
inline void asc_stwt(char2* address, char2 val)
```

```cpp
inline void asc_stwt(uchar2* address, uchar2 val)
```

```cpp
inline void asc_stwt(char4* address, char4 val)
```

```cpp
inline void asc_stwt(uchar4* address, uchar4 val)
```

```cpp
inline void asc_stwt(short* address, short val)
```

```cpp
inline void asc_stwt(unsigned short* address, unsigned short val)
```

```cpp
inline void asc_stwt(short2* address, short2 val)
```

```cpp
inline void asc_stwt(ushort2* address, ushort2 val)
```

```cpp
inline void asc_stwt(short4* address, short4 val)
```

```cpp
inline void asc_stwt(ushort4* address, ushort4 val)
```

```cpp
inline void asc_stwt(int* address, int val)
```

```cpp
inline void asc_stwt(unsigned int* address, unsigned int val)
```

```cpp
inline void asc_stwt(int2* address, int2 val)
```

```cpp
inline void asc_stwt(uint2* address, uint2 val)
```

```cpp
inline void asc_stwt(int4* address, int4 val)
```

```cpp
inline void asc_stwt(uint4* address, uint4 val)
```

```cpp
inline void asc_stwt(float* address, float val)
```

```cpp
inline void asc_stwt(float2* address, float2 val)
```

```cpp
inline void asc_stwt(float4* address, float4 val)
```

```cpp
inline void asc_stwt(bfloat16_t* address, bfloat16_t val)
```

```cpp
inline void asc_stwt(bfloat16x2_t* address, bfloat16x2_t val)
```

```cpp
inline void asc_stwt(half* address, half val)
```

```cpp
inline void asc_stwt(half2* address, half2 val)
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

## 需要包含的头文件

使用除half、half2、bfloat16\_t、bfloat16x2\_t类型之外的接口需要包含`simt_api/device_functions.h`头文件，使用half和half2类型接口需要包含`simt_api/asc_fp16.h`头文件，使用bfloat16\_t和bfloat16x2\_t类型接口需要包含`simt_api/asc_bf16.h`头文件。

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
    __global__ __launch_bounds__(1024) void kernel_asc_stwt(float* dst, float* val, uint32_t input_total_length)
    {
        uint32_t idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= input_total_length) {
            return;
        }
        asc_stwt(dst + idx, val[idx]);
    }
    ```

-   SIMD与SIMT混合编程场景：

    SIMD与SIMT混合编程场景，需要显式使用地址空间限定符表示地址空间：`__gm__`表示Global Memory内存空间。

    ```cpp
    __simt_vf__ __launch_bounds__(1024) inline void kernel_asc_stwt(__gm__ float* dst, __gm__ float* val, uint32_t input_total_length)
    {
        uint32_t idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= input_total_length) {
            return;
        }
        asc_stwt(dst + idx, val[idx]);
    }
    ```
