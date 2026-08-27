# h2cos

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/bfloat16_type/bfloat16x2_math_functions/h2cos_res.md#id1 -->

## 功能说明

获取输入数据各元素的三角函数余弦值。

![](../../../../figures/zh-cn_formulaimage_0000002545900882.png)

## 函数原型

```cpp
inline bfloat16x2_t h2cos(bfloat16x2_t x)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| x | 输入 | 源操作数。 |

## 返回值说明

输入数据各元素的三角函数余弦值。本接口受全局饱和模式影响，特殊值如下：

| 输入 | 非饱和模式返回值 | 饱和模式返回值 |
|---|---|---|
| nan | nan | 0 |
| inf | nan | 0 |
| -inf | nan | 0 |

## 约束说明

无

## 需要包含的头文件

使用bfloat16x2_t类型接口需要包含`simt_api/asc_bf16.h`头文件。

```cpp
#include "simt_api/asc_bf16.h"
```

## 调用示例

-   SIMT编程场景：

    ```cpp
    __global__ __launch_bounds__(1024) void KernelCos(bfloat16x2_t* dst, bfloat16x2_t* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = h2cos(x[idx]); // 对src源地址的第idx个元素取三角函数余弦值
    }
    ```

-   SIMD与SIMT混合编程场景：

    ```cpp
    __simt_vf__ __launch_bounds__(1024) inline void KernelCos(__gm__ bfloat16x2_t* dst, __gm__ bfloat16x2_t* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = h2cos(x[idx]); // 对src源地址的第idx个元素取三角函数余弦值
    }
    ```
