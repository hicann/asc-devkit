# \_\_ushort\_as\_bfloat16

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

## 功能说明

将unsigned short int的数据按位重新解释为bfloat16，即将unsigned short int的数据存储的位按照bfloat16的格式进行读取。

## 函数原型

```cpp
inline bfloat16_t __ushort_as_bfloat16(const unsigned short int x)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| x | 输入 | 源操作数。 |

## 返回值说明

unsigned short int的数据按位重新解释为bfloat16的值。特殊值如下：

| 输入（uint16 位模式） | 返回值 |
|---|---|
| 0 | 0 |
| -0 | -0 |
| inf | inf |
| -inf | -inf |
| nan | nan |
| ASCRT_MAX_NORMAL_BF16 | ASCRT_MAX_NORMAL_BF16 |
| -ASCRT_MAX_NORMAL_BF16 | -ASCRT_MAX_NORMAL_BF16 |
| ASCRT_MIN_DENORM_BF16 | ASCRT_MIN_DENORM_BF16 |
| 1.0 | 1.0 |

## 约束说明

无

## 需要包含的头文件

使用该接口需要包含"simt\_api/asc\_bf16.h"头文件。

```cpp
#include "simt_api/asc_bf16.h"
```

## 调用示例

-   SIMT编程场景：

    ```cpp
    __global__ __launch_bounds__(1024) void kernel__ushort_as_bfloat16(bfloat16_t* dst, unsigned short int* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __ushort_as_bfloat16(x[idx]);
    }
    ```

-   SIMD与SIMT混合编程场景：

    ```cpp
    __simt_vf__ __launch_bounds__(1024) inline void kernel__ushort_as_bfloat16(__gm__ bfloat16_t* dst, __gm__ unsigned short int* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __ushort_as_bfloat16(x[idx]);
    }
    ```
