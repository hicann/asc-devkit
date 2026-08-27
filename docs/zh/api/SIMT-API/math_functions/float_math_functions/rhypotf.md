# rhypotf

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/float_math_functions/rhypotf_res.md#id1 -->

## 功能说明

获取输入数据x、y的平方和x^2 + y^2的平方根的倒数。

![](../../../figures/zh-cn_formulaimage_0000002484776404.png)

## 函数原型

```cpp
inline float rhypotf(float x, float y)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| x | 输入 | 源操作数。 |
| y | 输入 | 源操作数。 |

## 返回值说明

x^2 + y^2的平方根的倒数。

-   若x^2 + y^2的平方根超出float最大范围，返回值为0。
-   若x^2 + y^2的平方根的倒数超出float最大范围，返回值为inf。
-   若x和y都为0，返回值为inf。
-   若x，y任意一个为±inf，返回值为0。
-   若x，y任意一个为nan，另一个不为±inf，返回值为nan。

## 约束说明

<!-- npu="950" id7 -->
针对Ascend 950PR/Ascend 950DT，本接口不支持Subnormal场景：本接口内部实现使用到了除法运算符，由于除法运算符不支持Subnormal场景，当所有输入均为Subnormal数据时，本接口最终返回nan。
<!-- end id7 -->

## 需要包含的头文件

使用该接口需要包含`simt_api/math_functions.h`头文件。

```cpp
#include "simt_api/math_functions.h"
```

## 调用示例

- SIMT编程场景：

    ```cpp
    __global__ __launch_bounds__(256) void compute_rhypotf(float *result, const float *x, const float *y, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = rhypotf(x[idx], y[idx]);
    }
    ```

- SIMD与SIMT混合编程场景：

    ```cpp
    __simt_vf__ __launch_bounds__(256) inline void compute_rhypotf_vf(__gm__ float *result, __gm__ const float *x, __gm__ const float *y, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = rhypotf(x[idx], y[idx]);
    }

    __global__ __vector__ void run_rhypotf(__gm__ float *result, __gm__ const float *x, __gm__ const float *y, uint32_t count)
    {
        asc_vf_call<compute_rhypotf_vf>(dim3(256), result, x, y, count);
    }
    ```

输入输出示例如下：

```
x：0.25, 0.75, 1.25, 1.75
y：1.5, 2.5, 3.5, 4.5
result: 0.6575959 0.3831305 0.2690691 0.2071121
```
