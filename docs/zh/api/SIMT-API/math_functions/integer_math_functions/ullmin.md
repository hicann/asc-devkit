# ullmin

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/integer_math_functions/ullmin_res.md#id1 -->

## 功能说明

获取两个输入数据中的最小值。

![](../../../figures/zh-cn_formulaimage_0000002521590974.png)

## 函数原型

```cpp
inline unsigned long long int ullmin(const unsigned long long int x, const unsigned long long int y)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| x | 输入 | 源操作数。 |
| y | 输入 | 源操作数。 |

## 返回值说明

两个输入数据中的最小值。

## 约束说明

无

## 需要包含的头文件

使用该接口需要包含`simt_api/math_functions.h`头文件。

```cpp
#include "simt_api/math_functions.h"
```

## 调用示例

-   SIMT编程场景：

    ```cpp
    __global__ __launch_bounds__(1024) void KernelUllmin(unsigned long long int* dst, unsigned long long int* x, unsigned long long int* y)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = ullmin(x[idx], y[idx]);
    }
    ```

-   SIMD与SIMT混合编程场景：

    ```cpp
    __simt_vf__ __launch_bounds__(1024) inline void KernelUllmin(__gm__ unsigned long long int* dst, __gm__ unsigned long long int* x, __gm__ unsigned long long int* y)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = ullmin(x[idx], y[idx]);
    }
    ```
