# y1f

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/float_math_functions/y1f_res.md#id1 -->

## 功能说明

获取输入数据x的1阶第二类贝塞尔函数y1的值。

![](../../../figures/zh-cn_formulaimage_0000002484776434.png)

## 函数原型

```cpp
inline float y1f(float x)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| x | 输入 | 源操作数。 |

## 返回值说明

输入数据的1阶第二类贝塞尔函数y1的值。

-   当x<0时，返回值为nan。
-   当x=0时，返回值为-inf。
-   当x=inf时，返回值为0。
-   当x=nan时，返回值为nan。

## 约束说明

<!-- npu="950" id7 -->
-   针对Ascend 950PR/Ascend 950DT，本接口不支持Subnormal场景：本接口内部实现使用到了除法运算符。由于除法运算符不支持Subnormal场景，在极少数场景下内部中间计算值的除数为Subnormal数据，会导致本接口最终结果为-inf。
<!-- end id7 -->

## 需要包含的头文件

使用该接口需要包含`simt_api/math_functions.h`头文件。

```cpp
#include "simt_api/math_functions.h"
```

## 调用示例

- SIMT编程场景：

    ```cpp
    __global__ __launch_bounds__(256) void compute_y1f(float *result, const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = y1f(x[idx]);
    }
    ```

- SIMD与SIMT混合编程场景：

    ```cpp
    __simt_vf__ __launch_bounds__(256) inline void compute_y1f_vf(__gm__ float *result, __gm__ const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = y1f(x[idx]);
    }

    __global__ __vector__ void run_y1f(__gm__ float *result, __gm__ const float *x, uint32_t count)
    {
        asc_vf_call<compute_y1f_vf>(dim3(256), result, x, count);
    }
    ```


输入输出示例如下：

```
x：1, 2, 3, 4
result: -0.7812128 -0.1070324 0.3246745 0.3979257
```
