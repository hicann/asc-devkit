# rnormf

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/float_math_functions/rnormf_res.md#id1 -->

## 功能说明

获取输入数据a中前n个元素的平方和a\[0\]^2 + a\[1\]^2 + ...+ a\[n-1\]^2的平方根的倒数。

![](../../../figures/zh-cn_formulaimage_0000002516816371.png)

## 函数原型

```cpp
inline float rnormf(int n, float* a)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| n | 输入 | 源操作数。输入数据a中连续计算的元素个数。 |
| a | 输入 | 源操作数。UB、Global Memory或栈空间的地址。 |

## 返回值说明

a\[0\]^2 + a\[1\]^2 + ...+ a\[n-1\]^2的平方根的倒数。

-   若a\[0\]^2 + a\[1\]^2 + ...+ a\[n-1\]^2的平方根超出float最大范围，返回值为0。
-   若a\[0\]^2 + a\[1\]^2 + ...+ a\[n-1\]^2平方根的倒数超出float最大范围，返回值为inf。
-   若a\[0\]、a\[1\] 、... 、a\[n-1\]都为0，返回值为inf。
-   若a\[0\]、a\[1\] 、... 、a\[n-1\]任意一个或多个为±inf，返回值为0。
-   若a\[0\]、a\[1\] 、... 、a\[n-1\]任意一个或多个为nan同时不是±inf，返回值为nan。
-   若n小于1，返回a\[0\]绝对值的倒数，若a\[0\]为0，返回值为inf。

## 约束说明

-   输入数据a的长度必须大于等于参数n。
-   若n过大，接口性能无法保证。
<!-- npu="950" id7 -->
-   针对Ascend 950PR/Ascend 950DT，本接口不支持Subnormal场景：本接口内部实现使用到了除法运算符，由于除法运算符不支持Subnormal场景，当输入a的所有元素均为Subnormal数据时，本接口最终结果为nan。
<!-- end id7 -->

## 需要包含的头文件

使用该接口需要包含`simt_api/math_functions.h`头文件。

```cpp
#include "simt_api/math_functions.h"
```

## 调用示例

- SIMT编程场景：

    ```cpp
    __global__ __launch_bounds__(256) void compute_rnormf(float *result, const int *n, float *vector_data, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = rnormf(n[idx], vector_data + idx * 4);
    }
    ```

- SIMD与SIMT混合编程场景：

    ```cpp
    __simt_vf__ __launch_bounds__(256) inline void compute_rnormf_vf(__gm__ float *result, __gm__ const int *n, __gm__ float *vector_data, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = rnormf(n[idx], vector_data + idx * 4);
    }

    __global__ __vector__ void run_rnormf(__gm__ float *result, __gm__ const int *n, __gm__ float *vector_data, uint32_t count)
    {
        asc_vf_call<compute_rnormf_vf>(dim3(256), result, n, vector_data, count);
    }
    ```

输入输出示例如下：

```
n：1, 2, 3, 1
vector_data：[[1, 2, 3, 4], [2, 3, 4, 5], [3, 4, 5, 6], [4, 5, 6, 7]]
result: 1 0.2773501 0.1414214 0.25
```
