# h2sin

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/half_type/half2_math_functions/h2sin_res.md#id1 -->

## 功能说明

头文件路径为：`"simt_api/asc_fp16.h"`。

获取输入数据各元素的三角函数正弦值。

![](../../../../figures/formulaimage_0000002513174474.png)

## 函数原型

```cpp
inline half2 h2sin(half2 x)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| x | 输入 | 源操作数。 |

## 返回值说明

输入数据各元素的三角函数正弦值。本接口受全局饱和寄存器影响，特殊值如下：

| x值 | 非饱和模式返回值 | 饱和模式返回值 |
| --- | --- | --- |
| 0 | 0 | 0 |
| -0 | -0 | -0 |
| nan | nan | 0 |
| ±inf | nan | 0 |

## 约束说明

本接口的饱和行为由全局饱和模式决定，SIMT编程场景暂不支持设置CTRL寄存器，本接口按非饱和模式执行；如需设置饱和模式，请在SIMD与SIMT混合编程场景下设置，设置方式参见[饱和模式设置](../../common_constraints.md#饱和模式设置)。

## 调用示例

-   SIMT编程场景：

    ```cpp
    #include "simt_api/asc_fp16.h"

    __global__ __launch_bounds__(1024) void KernelSin(half2* dst, half2* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = h2sin(x[idx]); // 对src源地址的第idx个元素取三角函数正弦值
    }
    ```

-   SIMD与SIMT混合编程场景：

    ```cpp
    #include "simt_api/asc_fp16.h"

    __simt_vf__ __launch_bounds__(1024) inline void KernelSin(__gm__ half2* dst, __gm__ half2* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = h2sin(x[idx]); // 对src源地址的第idx个元素取三角函数正弦值
    }
    ```

输入输出示例如下：

```text
x: (-1.25, 0.5), (2.5, -3.75), (-2, 1.25), (0.75, -0.25)
result: (-0.949219, 0.479492), (0.598633, 0.571777), (-0.90918, 0.949219), (0.681641, -0.247437)
```
