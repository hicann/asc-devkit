# \_\_half22hif82\_rh

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/fp8_type/fp8_conversion_functions/__half22hif82_rh_res.md#id1 -->

## 功能说明

将half2类型数据的两个分量遵循CAST\_HYBRID模式转换为hifloat8\_t精度，返回转换后的hifloat8x2\_t类型数据。

## 函数原型

```cpp
inline hifloat8x2_t __half22hif82_rh(const half2 x)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| x | 输入 | 源操作数。 |

## 返回值说明

将输入的两个分量遵循CAST\_HYBRID模式转换成的hifloat8x2\_t类型数据。本接口受全局饱和模式影响，特殊值如下：

| x分量值 | 非饱和模式返回值 | 饱和模式返回值 |
|---|---|---|
| ±0 | 0 | 0 |
| nan | nan | 0 |
| inf | inf | hif8 最大有限值 |
| -inf | -inf | -hif8 最大有限值 |
| 超出 hif8 范围的 half 有限值 | ±inf | ±hif8 最大有限值 |

## 约束说明

SIMT编程场景当前不支持使用该接口。

## 需要包含的头文件

使用该接口需要包含`simt_api/asc_fp8.h`头文件。

```cpp
#include "simt_api/asc_fp8.h"
```

## 调用示例

-   SIMD与SIMT混合编程场景：

    ```cpp
    // 使用短向量可提升数据搬运效率
    __simt_vf__ __launch_bounds__(1024) inline void simt_half22hif82_rh(__gm__ half2* input, __gm__ hifloat8x2_t* output, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // 每个线程处理1个half2类型的数据，即2个half类型的数据，因此idx >= input_total_length / 2的线程不处理数据
        if (idx > input_total_length /2) {
            return;
        }
        output[idx] = __half22hif82_rh(input[idx]);
    }
    __global__ __vector__ void cast_kernel(__gm__ half* input,  __gm__ uint8_t* output, uint32_t input_total_length)
    {
        asc_vf_call<simt_half22hif82_rh>(dim3(1024), (__gm__ half2*)input, (__gm__ hifloat8x2_t*)output, input_total_length);
    }
    ```
