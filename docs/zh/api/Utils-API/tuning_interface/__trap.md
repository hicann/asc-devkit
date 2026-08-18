# \_\_trap

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
<!-- npu="x90" id7 -->
- Kirin X90：不支持
<!-- end id7 -->
<!-- npu="9030" id8 -->
- Kirin 9030：不支持
<!-- end id8 -->

## 功能说明

在SIMT代码中调用此接口会中断算子的运行，适用于核函数（Kernel）侧异常场景的调试。

## 函数原型

```cpp
inline void __trap()
```

## 参数说明

无

## 返回值说明

无

## 约束说明

无

## 需要包含的头文件

使用该接口需要包含"utils/debug/asc\_assert.h"头文件。

```cpp
#include "utils/debug/asc_assert.h"
```

## 调用示例

-   SIMT编程场景：

    ```cpp
    __global__ __launch_bounds__(1024) inline void SimtKernel(float* dst, float* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (isnan(x[idx])) {
            __trap();
        }
        dst[idx] = x[idx];
    }
    ```

-   SIMD与SIMT混合编程场景：

    ```cpp
    __simt_vf__ __launch_bounds__(1024) inline void SimtKernel(__gm__ float* dst, __gm__ float* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (isnan(x[idx])) {
            __trap();
        }
        dst[idx] = x[idx];
    }
    ```
