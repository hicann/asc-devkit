# \_\_fmaf\_ieee\_rn

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/float_math_functions/__fmaf_ieee_rn_res.md#id1 -->

## 功能说明

头文件路径为：`"simt_api/math_functions.h"`。

遵循CAST_RINT模式，对输入数据x、y、z，计算x与y相乘加上z的结果。

![](../../../figures/formulaimage_0000002531284496.png)

## 函数原型

```cpp
inline float __fmaf_ieee_rn(float x, float y, float z)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| x | 输入 | 源操作数 |
| y | 输入 | 源操作数 |
| z | 输入 | 源操作数 |

## 返回值说明

x * y + z的单精度乘加结果。

-   x为±inf，y为±0，返回nan。
-   x为±0，y为±inf，返回nan。
-   x*y为inf，z为-inf，返回nan。
-   x*y为-inf，z为inf，返回nan。
-   x*y+z超出float范围的最大值，返回inf。
-   x*y+z小于float范围的最小值，返回-inf。
-   x、y、z任意一个为nan，返回nan。

## 约束说明

无

## 调用示例

-   SIMT编程场景：

    ```cpp
    #include "simt_api/math_functions.h"

    __global__ __launch_bounds__(256) void kernel_fmaf_ieee_rn(
        float* result, const float* x, const float* y, const float* z)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        result[idx] = __fmaf_ieee_rn(x[idx], y[idx], z[idx]);
    }
    ```

-   SIMD与SIMT混合编程场景：

    ```cpp
    #include "simt_api/math_functions.h"

    __simt_vf__ __launch_bounds__(256) inline void kernel_fmaf_ieee_rn(
        __gm__ float* result, __gm__ const float* x, __gm__ const float* y,
        __gm__ const float* z)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        result[idx] = __fmaf_ieee_rn(x[idx], y[idx], z[idx]);
    }
    ```
输入输出示例如下：

```text
x: 0.25, 0.75, -3.0
y: 1.5, 2.5, 4.0
z: -0.5, 0.5, 7.0
result: -0.125, 2.375, -5.0
```

