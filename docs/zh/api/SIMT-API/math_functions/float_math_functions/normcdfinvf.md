# normcdfinvf

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/float_math_functions/normcdfinvf_res.md#id1 -->

## 功能说明

获取输入数据x的标准正态累积分布的逆函数。

![](../../../figures/zh-cn_formulaimage_0000002533422599.png)

## 函数原型

```cpp
inline float normcdfinvf(float x)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| x | 输入 | 源操作数。 |

## 返回值说明

获取输入数据x的标准正态累积分布的逆函数。

-   当`x`为0时，返回值为-inf。
-   当`x`为nan时，返回值为nan。
-   当`x`为1时，返回值为inf。
-   当`x`∉[0,1]时，返回nan。

## 约束说明

无

## 需要包含的头文件

使用该接口需要包含`simt_api/math_functions.h`头文件。

```cpp
#include "simt_api/math_functions.h"
```

## 调用示例

- SIMT编程场景：

    ```cpp
    __global__ __launch_bounds__(256) void compute_normcdfinvf(float *result, const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = normcdfinvf(x[idx]);
    }
    ```

- SIMD与SIMT混合编程场景：

    ```cpp
    __simt_vf__ __launch_bounds__(256) inline void compute_normcdfinvf_vf(__gm__ float *result, __gm__ const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = normcdfinvf(x[idx]);
    }

    __global__ __vector__ void run_normcdfinvf(__gm__ float *result, __gm__ const float *x, uint32_t count)
    {
        asc_vf_call<compute_normcdfinvf_vf>(dim3(256), result, x, count);
    }
    ```

输入输出示例如下：

```
x：-0.75, -0.25, 0.25, 0.75
result: nan nan -0.6744897 0.6744897
```
