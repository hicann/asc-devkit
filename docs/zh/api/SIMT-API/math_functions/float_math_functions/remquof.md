# remquof

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/float_math_functions/remquof_res.md#id1 -->

## 功能说明

头文件路径为：`"simt_api/math_functions.h"`。

获取输入数据x除以y的余数r。r=x-ny，其中n是x除以y的商，n取最接近x除以y浮点数结果的整数，当x除以y的浮点数结果与左右最接近的整数距离相等时，商取偶数，同时将商赋值给指针变量quo。

## 函数原型

```cpp
inline float remquof(float x, float y, int *quo)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| x | 输入 | 源操作数。 |
| y | 输入 | 源操作数。 |
| quo | 输出 | UB、Global Memory或栈空间的地址，用于存储除法运算的商。 |

## 返回值说明

输入数据x除以y的余数。

-   x，y任意一个为nan时，返回值为nan。
-   y=0时，返回值为nan。
-   x=inf或-inf时，返回值为nan。
-   x为有限值，y=inf或-inf时，返回x。

## 约束说明

无

## 调用示例

- SIMT编程场景：

    ```cpp
    #include "simt_api/math_functions.h"

    __global__ __launch_bounds__(256) void compute_remquof(float *result, int *quo, const float *x, const float *y, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = remquof(x[idx], y[idx], quo + idx);
    }
    ```

- SIMD与SIMT混合编程场景：

    ```cpp
    #include "simt_api/math_functions.h"

    __simt_vf__ __launch_bounds__(256) inline void compute_remquof_vf(__gm__ float *result, __gm__ int *quo, __gm__ const float *x, __gm__ const float *y, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = remquof(x[idx], y[idx], quo + idx);
    }

    __global__ __vector__ void run_remquof(__gm__ float *result, __gm__ int *quo, __gm__ const float *x, __gm__ const float *y, uint32_t count)
    {
        asc_vf_call<compute_remquof_vf>(dim3(256), result, quo, x, y, count);
    }
    ```

输入输出示例如下：

```
x：0.25, 0.75, 1.25, 1.75
y：1.25, 2.25, 3.25, 4.25
result: 0.25 0.75 1.25 1.75
quo: 0 0 0 0
```
