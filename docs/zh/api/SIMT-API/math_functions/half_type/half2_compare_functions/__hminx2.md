# \_\_hminx2

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/half_type/half2_compare_functions/__hminx2_res.md#id1 -->

## 功能说明

获取两个half2类型数据各分量的最小值。

## 函数原型

```cpp
half2 __hminx2(const half2 x, const half2 y)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| x | 输入 | 源操作数。 |
| y | 输入 | 源操作数。 |

## 返回值说明

输入数据各分量的最小值。该接口受全局饱和寄存器影响，特殊值如下：

| x分量 | y分量 | 非饱和模式返回值 | 饱和模式返回值 |
| --- | --- | --- | --- |
| 0 | -0 | -0 | -0 |
| -0 | 0 | -0 | -0 |
| -0 | -0 | -0 | -0 |
| 0 | 0 | 0 | 0 |
| nan | 任意值 | nan | 0 |
| 任意值 | nan | nan | 0 |
| nan | nan | nan | 0 |
| 任意非nan值 | -inf | -inf | -ASCRT\_MAX\_NORMAL\_FP16 |
| -inf | 任意非nan值 | -inf | -ASCRT\_MAX\_NORMAL\_FP16 |
| inf | inf | inf | ASCRT\_MAX\_NORMAL\_FP16 |

## 约束说明

无

## 需要包含的头文件

使用该接口需要包含`simt_api/asc_fp16.h`头文件。

```cpp
#include "simt_api/asc_fp16.h"
```

## 调用示例

-   SIMT编程场景：

    ```cpp
    // 使用短向量可提升数据搬运效率
    __global__ __launch_bounds__(1024) void simt_hminx2(half* x, half* y, half* dst, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // 每个线程处理1个half2类型的数据，即2个half类型的数据，因此idx >= input_total_length / 2的线程不处理数据
        if (idx >= input_total_length / 2) {
            return;
        }
        half2* input1 = (half2*)x;
        half2* input2 = (half2*)y;
        half2* out = (half2*)dst;
        out[idx] = __hminx2(input1[idx], input2[idx]);
    }
    ```

-   SIMD与SIMT混合编程场景：

    ```cpp
    // 使用短向量可提升数据搬运效率
    __simt_vf__ __launch_bounds__(1024) inline void simt_hminx2(__gm__ half2* x, __gm__ half2* y, __gm__ half2* dst, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // 每个线程处理1个half2类型的数据，即2个half类型的数据，因此idx >= input_total_length / 2的线程不处理数据
        if (idx >= input_total_length / 2) {
            return;
        }
        dst[idx] = __hminx2(x[idx], y[idx]);
    }

    __global__ __vector__ void compare_kernel(__gm__ half* x, __gm__ half* y, __gm__ half* dst, uint32_t input_total_length)
    {
        asc_vf_call<simt_hminx2>(dim3(1024), (__gm__ half2*)x, (__gm__ half2*)y, (__gm__ half2*)dst, input_total_length);
    }
    ```
