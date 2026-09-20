# cospif

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR&950DT系列产品：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2系列产品：不支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas推理系列产品AI Core：不支持
- Atlas推理系列产品Vector Core：不支持
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas训练系列产品：不支持
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/float_math_functions/cospif_res.md#id1 -->

## 功能说明

头文件路径为：`"simt_api/math_functions.h"`。

获取输入数据与π相乘的余弦值。

![](../../../figures/zh-cn_formulaimage_0000002516816349.png)

## 函数原型

```cpp
inline float cospif(float x)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| x | 输入 | 源操作数。 |

## 返回值说明

输入数据与π相乘的余弦值。
- 当输入x为有限值且Abs\(x\)\>=2^24时，直接返回1.0。
- 当输入x为整数且|Abs\(x\)\<2^24时，x为偶数返回1.0，x为奇数返回-1.0。
- 当输入x=n+0.5，返回值为0.0。
- 当x为inf时，返回值为nan。
- 当x为-inf时，返回值为nan。
- 当x为nan时，返回值为nan。

## 约束说明

无

## 调用示例

- SIMT编程场景：

    ```cpp
    #include "simt_api/math_functions.h"

    __global__ __launch_bounds__(256) void compute_cospif(float *result, const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = cospif(x[idx]);
    }
    ```

- SIMD与SIMT混合编程场景：

    ```cpp
    #include "simt_api/math_functions.h"

    __simt_vf__ __launch_bounds__(256) inline void compute_cospif_vf(__gm__ float *result, __gm__ const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = cospif(x[idx]);
    }

    __global__ __vector__ void run_cospif(__gm__ float *result, __gm__ const float *x, uint32_t count)
    {
        asc_vf_call<compute_cospif_vf>(dim3(256), result, x, count);
    }
    ```

输入输出示例如下：

```
x：0.25, 0.75, 1.25, 1.75
result: 0.7071068 -0.7071068 -0.7071066 0.707107
```
