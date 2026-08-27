# sqrtf

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/float_math_functions/sqrtf_res.md#id1 -->

## 功能说明

获取输入数据`x`的平方根。

![](../../../figures/zh-cn_formulaimage_0000002516816361.png)

## 函数原型

```cpp
inline float sqrtf(float x)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| x | 输入 | 源操作数，输入数据。 |

## 返回值说明

输入数据的平方根。

-   当x为0时，返回值为0。
-   当x为inf时，返回值为inf。
-   当x为-inf时，返回值为nan。
-   当x为nan时，返回值为nan。

## 约束说明

-   本接口支持的输入数据范围为x大于等于0，否则返回值为nan。
-   本接口受编译选项[--cce-use-fast-math](../../../../guide/programming_guide/compilation_and_execution/operator_compilation/ai_core_operator_compilation.md#常用的编译选项)的影响。
    -   --cce-use-fast-math=true：开启快速计算模式，不支持Subnormal场景的低精度平方根运算，处于Subnormal范围内的输入和输出值，都会被刷新为保留符号的0。
    -   --cce-use-fast-math=false：支持Subnormal场景的高精度平方根运算。

## 需要包含的头文件

使用该接口需要包含`simt_api/math_functions.h`头文件。

```cpp
#include "simt_api/math_functions.h"
```

## 调用示例

- SIMT编程场景：

    ```cpp
    __global__ __launch_bounds__(256) void compute_sqrtf(float *result, const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = sqrtf(x[idx]);
    }
    ```

- SIMD与SIMT混合编程场景：

    ```cpp
    __simt_vf__ __launch_bounds__(256) inline void compute_sqrtf_vf(__gm__ float *result, __gm__ const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = sqrtf(x[idx]);
    }

    __global__ __vector__ void run_sqrtf(__gm__ float *result, __gm__ const float *x, uint32_t count)
    {
        asc_vf_call<compute_sqrtf_vf>(dim3(256), result, x, count);
    }
    ```

输入输出示例如下：

```
x：0.25, 1.25, 2.25, 3.25
result: 0.5 1.118034 1.5 1.802776
```
