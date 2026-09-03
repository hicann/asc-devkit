# powf

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/float_math_functions/powf_res.md#id1 -->

## 功能说明

头文件路径为：`"simt_api/math_functions.h"`。

获取输入数据x的y次幂。

![](../../../figures/zh-cn_formulaimage_0000002516816365.png)

## 函数原型

```cpp
inline float powf(float x, float y)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| x | 输入 | 源操作数，幂计算的底数。 |
| y | 输入 | 源操作数，幂计算的指数。 |

## 返回值说明

x的y次幂的结果。

-   若x^y超出float最大范围，返回值为inf。
-   若x为±0，y小于0并且为奇数，返回值为±inf。
-   若x为±0，y小于0并且不为奇数，返回值为inf。
-   若x为±0，y大于0并且为奇数，返回值为±0。
-   若x为±0，y大于0并且不为奇数，返回值为0。
-   若x为-1，y为±inf，返回值为1。
-   若x为1，y为任意值（包括nan），返回值为1。
-   若y为±0，x为任意值（包括nan），返回值为1。
-   若x小于0，y不为整数，返回值为nan。
-   若|x|<1，y为-inf，返回值为inf。
-   若|x|\>1，y为-inf，返回值为0。
-   若|x|<1，y为inf，返回值为0。
-   若|x|\>1，y为inf，返回值为inf。
-   若x为-inf，y小于0并且为奇数，返回值为-0。
-   若x为-inf，y小于0并且不为奇数，返回值为0。
-   若x为-inf，y大于0并且为奇数，返回值为-inf。
-   若x为-inf，y大于0并且不为奇数，返回值为inf。
-   若x为inf，y小于0，返回值为0。
-   若x为inf，y大于0，返回值为inf。
-   在如下边界场景，返回值为nan。
    -   x为nan，y不为0。
    -   y为nan，x不为1。
    -   x、y均为nan

## 约束说明

<!-- npu="950" id7 -->
针对Ascend 950PR/Ascend 950DT，本接口不支持Subnormal场景：处于Subnormal范围内的输入和输出值，都会被刷新为保留符号的0。
<!-- end id7 -->

## 调用示例

- SIMT编程场景：

    ```cpp
    #include "simt_api/math_functions.h"

    __global__ __launch_bounds__(256) void compute_powf(float *result, const float *x, const float *y, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = powf(x[idx], y[idx]);
    }
    ```

- SIMD与SIMT混合编程场景：

    ```cpp
    #include "simt_api/math_functions.h"

    __simt_vf__ __launch_bounds__(256) inline void compute_powf_vf(__gm__ float *result, __gm__ const float *x, __gm__ const float *y, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = powf(x[idx], y[idx]);
    }

    __global__ __vector__ void run_powf(__gm__ float *result, __gm__ const float *x, __gm__ const float *y, uint32_t count)
    {
        asc_vf_call<compute_powf_vf>(dim3(256), result, x, y, count);
    }
    ```

输入输出示例如下：

```
x：0.25, 0.75, 1.25, 1.75
y：1.25, 2.25, 3.25, 4.25
result: 0.176777 0.523465 2.065178 10.787274
```
