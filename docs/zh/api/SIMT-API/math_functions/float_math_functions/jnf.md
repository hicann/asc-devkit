# jnf

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/float_math_functions/jnf_res.md#id1 -->

## 功能说明

获取输入数据x的n阶第一类贝塞尔函数jn的值。

![](../../../figures/zh-cn_formulaimage_0000002484776432.png)

## 函数原型

```cpp
inline float jnf(int n, float x)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| n | 输入 | 源操作数。 |
| x | 输入 | 源操作数。 |

## 返回值说明

输入数据的n阶第一类贝塞尔函数jn的值。

-   当n<0时，返回值为nan。
-   当x=inf或-inf时，返回值为0。
-   当x=nan时，返回值为nan。

## 约束说明

-   n的最大取值为128。
<!-- npu="950" id7 -->
-   针对Ascend 950PR/Ascend 950DT，本接口不支持Subnormal场景：本接口内部实现使用到了expf，由于expf不支持Subnormal场景，在极少数场景下内部计算的expf结果为Subnormal数据，导致本接口最终结果为0。
<!-- end id7 -->

## 需要包含的头文件

使用该接口需要包含`simt_api/math_functions.h`头文件。

```cpp
#include "simt_api/math_functions.h"
```

## 调用示例

- SIMT编程场景：

    ```cpp
    __global__ __launch_bounds__(256) void compute_jnf(float *result, const int *n, const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = jnf(n[idx], x[idx]);
    }
    ```

- SIMD与SIMT混合编程场景：

    ```cpp
    __simt_vf__ __launch_bounds__(256) inline void compute_jnf_vf(__gm__ float *result, __gm__ const int *n, __gm__ const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = jnf(n[idx], x[idx]);
    }

    __global__ __vector__ void run_jnf(__gm__ float *result, __gm__ const int *n, __gm__ const float *x, uint32_t count)
    {
        asc_vf_call<compute_jnf_vf>(dim3(256), result, n, x, count);
    }
    ```

输入输出示例如下：

```
n：1, 2, 3, 1
x：0.25, 0.75, 1.25, 1.75
result: 0.124026 0.06707399 0.03686836 0.5801562
```
