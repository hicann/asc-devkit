# \_\_int\_as\_float

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__int_as_float_res.md#id1 -->

## 功能说明

将整数中的位重新解释为浮点数，即将整数存储的位按照float的格式进行读取。

## 函数原型

```cpp
inline float __int_as_float(const int x)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| x | 输入 | 源操作数。 |

## 返回值说明

输入整数中的位重新解释成的浮点数。特殊值如下：

| x值 | 返回值 |
|---|---|
| 0x00000000 | 0 |
| 0x80000000 | -0 |
| 0x7F800000 | inf |
| 0xFF800000 | -inf |
| 0x7FFFFFFF | nan |
| 0x7F7FFFFF | ASCRT_MAX_NORMAL_F |
| 0xFF7FFFFF | -ASCRT_MAX_NORMAL_F |
| 0x00000001 | ASCRT_MIN_DENORM_F |

## 约束说明

无

## 需要包含的头文件

使用该接口需要包含`simt_api/device_functions.h`头文件。

```cpp
#include "simt_api/device_functions.h"
```

## 调用示例

-   SIMT编程场景：

    ```cpp
    __global__ __launch_bounds__(1024) void kernel__int_as_float(float* dst, int32_t* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __int_as_float(x[idx]);
    }
    ```

-   SIMD与SIMT混合编程场景：

    ```cpp
    __simt_vf__ __launch_bounds__(1024) inline void kernel__int_as_float(__gm__ float* dst, __gm__ int32_t* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __int_as_float(x[idx]);
    }
    ```
