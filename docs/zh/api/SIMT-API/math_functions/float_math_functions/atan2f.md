# atan2f

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/float_math_functions/atan2f_res.md#id1 -->

## 功能说明

头文件路径为：`"simt_api/math_functions.h"`。

获取输入数据y/x的反正切值。

![](../../../figures/zh-cn_formulaimage_0000002516816331.png)

## 函数原型

```cpp
inline float atan2f(float y, float x)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| y | 输入 | 源操作数。 |
| x | 输入 | 源操作数。 |

## 返回值说明

y/x的反正切值。

特殊取值场景下的返回值如下表所示。

| 输入值x | 输入值y | 返回值 |
| --- | --- | --- |
| 任意值 | nan | nan |
| nan | 任意值 | nan |
| 正值（含+0） | +0 | +0 |
| 正值（含+0） | -0 | -0 |
| 负值（含-0） | +0 | π |
| 负值（含-0） | -0 | -π |
| inf | inf | π/4 |
| inf | -inf | -π/4 |
| inf | 1 | 0.0 |
| -inf | inf | 3π/4 |
| -inf | -inf | -3π/4 |
| -inf | 1 | π |
| 1 | inf | π/2 |
| 1 | -inf | -π/2 |

## 约束说明

<!-- npu="950" id7 -->
针对Ascend 950PR/Ascend 950DT，本接口不支持Subnormal场景：本接口内部实现使用到了除法运算符，由于除法运算符不支持Subnormal场景，当x和y均为Subnormal数据时，本接口最终返回nan；当仅y为Subnormal数据且x为正数且非Subnormal数据时，本接口最终返回0；当仅y为Subnormal数据且x为负数且非Subnormal数据时，本接口最终返回与y同号的π。
<!-- end id7 -->

## 调用示例

- SIMT编程场景：

    ```cpp
    #include "simt_api/math_functions.h"

    __global__ __launch_bounds__(256) void compute_atan2f(float *result, const float *x, const float *y, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = atan2f(y[idx], x[idx]);
    }
    ```

- SIMD与SIMT混合编程场景：

    ```cpp
    #include "simt_api/math_functions.h"

    __simt_vf__ __launch_bounds__(256) inline void compute_atan2f_vf(__gm__ float *result, __gm__ const float *x, __gm__ const float *y, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = atan2f(y[idx], x[idx]);
    }

    __global__ __vector__ void run_atan2f(__gm__ float *result, __gm__ const float *x, __gm__ const float *y, uint32_t count)
    {
        asc_vf_call<compute_atan2f_vf>(dim3(256), result, x, y, count);
    }
    ```

输入输出示例如下：

```
y：1.5, 2.5, 3.5, 4.5
x：0.25, 0.75, 1.25, 1.75
result: 1.405648 1.27934 1.227772 1.199905
```
