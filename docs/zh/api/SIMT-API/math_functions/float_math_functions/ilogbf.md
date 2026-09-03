# ilogbf

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/float_math_functions/ilogbf_res.md#id1 -->

## 功能说明

头文件路径为：`"simt_api/math_functions.h"`。

计算以2为底，输入数据绝对值的对数，并对结果向下取整，返回整数。

![](../../../figures/zh-cn_formulaimage_0000002484776384.png)

## 函数原型

```cpp
inline int ilogbf(float x)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| x | 输入 | 源操作数。 |

## 返回值说明

对于有限非零值，返回以2为底的x的绝对值的对数，并向下取整后的整数值。

特殊取值场景下的返回值如下表所示。

| 输入值x | 返回值 |
| --- | --- |
| +0 | ASCRT_MIN_VAL_S |
| -0 | ASCRT_MIN_VAL_S |
| inf | ASCRT_MAX_VAL_S |
| -inf | ASCRT_MAX_VAL_S |
| nan | ASCRT_MIN_VAL_S |

## 约束说明

无

## 调用示例

- SIMT编程场景：

    ```cpp
    #include "simt_api/math_functions.h"

    __global__ __launch_bounds__(256) void compute_ilogbf(int *result, const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = ilogbf(x[idx]);
    }
    ```

- SIMD与SIMT混合编程场景：

    ```cpp
    #include "simt_api/math_functions.h"

    __simt_vf__ __launch_bounds__(256) inline void compute_ilogbf_vf(__gm__ int *result, __gm__ const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = ilogbf(x[idx]);
    }

    __global__ __vector__ void run_ilogbf(__gm__ int *result, __gm__ const float *x, uint32_t count)
    {
        asc_vf_call<compute_ilogbf_vf>(dim3(256), result, x, count);
    }
    ```

输入输出示例如下：

```
x：0.25, 0.75, 1.25, 1.75
result: -2 -1 0 0
```
