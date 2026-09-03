# \_\_float2bfloat162\_rn

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float2bfloat162_rn_res.md#id1 -->

## 功能说明

头文件路径为：`"simt_api/asc_bf16.h"`。

将float类型数据遵循CAST\_RINT模式转换为bfloat16类型并填充到bfloat16x2的前后两部分，返回填充后的bfloat16x2类型数据。

## 函数原型

```cpp
inline bfloat16x2_t __float2bfloat162_rn(const float x)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| x | 输入 | 源操作数。 |

## 返回值说明

将输入数据遵循CAST\_RINT模式转换为bfloat16类型并填充到bfloat16x2的前后两部分，返回填充后的数据。本接口受全局饱和模式影响，特殊值如下：

| x值 | 非饱和模式返回值 | 饱和模式返回值 |
|---|---|---|
| +0 | (0, 0) | (0, 0) |
| -0 | (-0, -0) | (-0, -0) |
| nan | (nan, nan) | (0, 0) |
| inf | (inf, inf) | (ASCRT_MAX_NORMAL_BF16, ASCRT_MAX_NORMAL_BF16) |
| -inf | (-inf, -inf) | (-ASCRT_MAX_NORMAL_BF16, -ASCRT_MAX_NORMAL_BF16) |
| ＞ASCRT_MAX_NORMAL_BF16 | (inf, inf) | (ASCRT_MAX_NORMAL_BF16, ASCRT_MAX_NORMAL_BF16) |
| ＜-ASCRT_MAX_NORMAL_BF16 | (-inf, -inf) | (-ASCRT_MAX_NORMAL_BF16, -ASCRT_MAX_NORMAL_BF16) |

## 约束说明

无

## 调用示例

-   SIMT编程场景：

    ```cpp
    #include "simt_api/asc_bf16.h"

    // 使用短向量可提升数据搬运效率
    __aicore__ void simt_float2bfloat162_rn(float* input, bfloat16x2_t* output, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx > input_total_length) {
            return;
        }
        output[idx] = __float2bfloat162_rn(input[idx]);
    }
    __global__ __launch_bounds__(1024) void cast_kernel(float* input, bfloat16_t* output, uint32_t input_total_length)
    {
        simt_float2bfloat162_rn(input, (bfloat16x2_t*)output, input_total_length);
    }
    ```

-   SIMD与SIMT混合编程场景：

    ```cpp
    #include "simt_api/asc_bf16.h"

    // 使用短向量可提升数据搬运效率
    __simt_vf__ __launch_bounds__(1024) inline void simt_float2bfloat162_rn(__gm__ float* input, __gm__ bfloat16x2_t* output, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx > input_total_length) {
            return;
        }
        output[idx] = __float2bfloat162_rn(input[idx]);
    }
    __global__ __vector__ void cast_kernel(__gm__ float* input,  __gm__ bfloat16_t* output, uint32_t input_total_length)
    {
        asc_vf_call<simt_float2bfloat162_rn>(dim3(1024), input, (__gm__ bfloat16x2_t*)output, input_total_length);
    }
    ```
