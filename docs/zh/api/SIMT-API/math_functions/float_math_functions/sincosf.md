# sincosf

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/float_math_functions/sincosf_res.md#id1 -->

## 功能说明

头文件路径为：`"simt_api/math_functions.h"`。

获取输入数据的三角函数正弦值和余弦值。

![](../../../figures/zh-cn_formulaimage_0000002516816357.png)

## 函数原型

```cpp
inline void sincosf(float x, float *s, float *c)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| x | 输入 | 源操作数。 |
| s | 输出 | UB、Global Memory或栈空间的地址，用于存储输入数据的三角函数正弦值。 |
| c | 输出 | UB、Global Memory或栈空间的地址，用于存储输入数据的三角函数余弦值。 |

## 返回值说明

-   当输入x为inf、-inf、nan时，输出值为nan。

## 约束说明

无

## 调用示例

- SIMT编程场景：

    ```cpp
    #include "simt_api/math_functions.h"

    __global__ __launch_bounds__(256) void compute_sincosf(float *sin_result, float *cos_result, const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        sincosf(x[idx], sin_result + idx, cos_result + idx);
    }
    ```

- SIMD与SIMT混合编程场景：

    ```cpp
    #include "simt_api/math_functions.h"

    __simt_vf__ __launch_bounds__(256) inline void compute_sincosf_vf(__gm__ float *sin_result, __gm__ float *cos_result, __gm__ const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        sincosf(x[idx], sin_result + idx, cos_result + idx);
    }

    __global__ __vector__ void run_sincosf(__gm__ float *sin_result, __gm__ float *cos_result, __gm__ const float *x, uint32_t count)
    {
        asc_vf_call<compute_sincosf_vf>(dim3(256), sin_result, cos_result, x, count);
    }
    ```

输入输出示例如下：

```
x：0.25, 0.75, 1.25, 1.75
sin_result: 0.247404 0.6816388 0.9489846 0.983986
cos_result: 0.9689124 0.7316889 0.3153224 -0.1782461
```
