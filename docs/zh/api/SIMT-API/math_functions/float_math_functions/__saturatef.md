# \_\_saturatef

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/float_math_functions/__saturatef_res.md#id1 -->

## 功能说明

头文件路径为：`"simt_api/device_functions.h"`。

将输入数据转换至\[0.0, 1.0\]区间。具体来说，若输入数据大于或等于1，则结果为1，若输入数据小于或等于0，则结果为0，若输入数据在\[0.0, 1.0\]区间，则结果为原输入数据。

## 函数原型

```cpp
float __saturatef(float x)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| x | 输入 | 源操作数。 |

## 返回值说明

输入数据转换至\[0.0, 1.0\]区间的结果。

-   当`x`≤0时，返回值为0。
-   当`x`≥1时，返回值为1。
-   当0<`x`<1时，返回值为`x`。
-   当`x`为nan时，返回值为0。

## 约束说明

无

## 调用示例

- SIMT编程场景：

    ```cpp
    #include "simt_api/device_functions.h"

    __global__ __launch_bounds__(256) void compute_saturate_intrinsic(float *result, const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = __saturatef(x[idx]);
    }
    ```

- SIMD与SIMT混合编程场景：

    ```cpp
    #include "simt_api/device_functions.h"

    __simt_vf__ __launch_bounds__(256) inline void compute_saturate_intrinsic_vf(__gm__ float *result, __gm__ const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = __saturatef(x[idx]);
    }

    __global__ __vector__ void run_saturate_intrinsic(__gm__ float *result, __gm__ const float *x, uint32_t count)
    {
        asc_vf_call<compute_saturate_intrinsic_vf>(dim3(256), result, x, count);
    }
    ```

输入输出示例如下：

```
x：0.25, 0.75, 1.25, 1.75
result: 0.25 0.75 1 1
```
