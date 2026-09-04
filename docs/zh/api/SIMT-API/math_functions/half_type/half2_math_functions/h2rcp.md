# h2rcp

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/half_type/half2_math_functions/h2rcp_res.md#id1 -->

## 功能说明

头文件路径为：`"simt_api/asc_fp16.h"`。

获取输入数据x各元素的倒数。

![](../../../../figures/zh-cn_formulaimage_0000002513334410.png)

## 函数原型

```cpp
inline half2 h2rcp(half2 x)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| x | 输入 | 源操作数，输入数据。 |

## 返回值说明

输入数据各元素的倒数。本接口受全局饱和寄存器影响，特殊值如下：

| x值 | 非饱和模式返回值 | 饱和模式返回值 |
| --- | --- | --- |
| 0 | inf | ASCRT\_MAX\_NORMAL\_FP16 |
| ASCRT\_MAX\_NORMAL\_FP16 | inf | ASCRT\_MAX\_NORMAL\_FP16 |
| -0 | -inf | -ASCRT\_MAX\_NORMAL\_FP16 |
| nan | nan | 0 |
| inf | 0 | 0 |
| -inf | -0 | -0 |

## 约束说明

无

## 调用示例

-   SIMT编程场景：

    ```cpp
    #include "simt_api/asc_fp16.h"

    __global__ __launch_bounds__(1024) void KernelRcp(half2* dst, half2* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = h2rcp(x[idx]);
    }
    ```

-   SIMD与SIMT混合编程场景：

    ```cpp
    #include "simt_api/asc_fp16.h"

    __simt_vf__ __launch_bounds__(1024) inline void KernelRcp(__gm__ half2* dst, __gm__ half2* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = h2rcp(x[idx]);
    }
    ```
