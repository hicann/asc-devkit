# \_\_floats2bfloat162\_rn

<!-- md-trans-meta sourceCommit=6e11244b08ff98f28bdfed28700f33faca049b68 translatedAt=2026-10-27T02:34:52.809Z pushedAt=2026-10-27T03:16:59.221Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__floats2bfloat162_rn_res.md#id1 -->

## Description

The header file path is: `"simt_api/asc_bf16.h"`.

Converts the input data x and y to the bfloat16 type following the CAST\_RINT mode, fills them into the first and second halves of bfloat16x2 respectively, and returns the converted bfloat16x2 type data.

## Prototype

```cpp
inline bfloat16x2_t __floats2bfloat162_rn(const float x, const float y)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |
| y | Input | Source operand. |

## Return Value

Converts the input float type data to the bfloat16 type following the CAST\_RINT mode, fills them into the first and second halves of bfloat16x2 respectively, and returns the converted data. This API is affected by the global saturation mode. The special values are as follows:

| Input | Return Value in Non-saturation Mode  | Return Value in Saturation Mode |
|---|---|---|
| 0 | 0 | 0 |
| -0 | -0 | -0 |
| nan | nan | 0 |
| inf | inf | ASCRT_MAX_NORMAL_BF16 |
| -inf | -inf | -ASCRT_MAX_NORMAL_BF16 |
| >ASCRT_MAX_NORMAL_BF16 | inf | ASCRT_MAX_NORMAL_BF16 |
| <-ASCRT_MAX_NORMAL_BF16 | -inf | -ASCRT_MAX_NORMAL_BF16 |

## Constraints

None

## Examples

-   SIMT programming scenario:

    ```cpp
    #include "simt_api/asc_bf16.h"

    // Use short vectors to improve data transfer efficiency.
    __aicore__ void simt_floats2bfloat162_rn(float* input1, float* input2, bfloat16x2_t* output, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= input_total_length) {
            return;
        }
        output[idx] = __floats2bfloat162_rn(input1[idx], input2[idx]);
    }
    __global__ __launch_bounds__(1024) void cast_kernel(float* input1, float* input2, bfloat16_t* output, uint32_t input_total_length)
    {
        simt_floats2bfloat162_rn(input1, input2, (bfloat16x2_t*)output, input_total_length);
    }
    ```

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/asc_bf16.h"

    // Use short vectors to improve data transfer efficiency.
    __simt_vf__ __launch_bounds__(1024) inline void simt_floats2bfloat162_rn(__gm__ float* input1, __gm__ float* input2, __gm__ bfloat16x2_t* output, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= input_total_length) {
            return;
        }
        output[idx] = __floats2bfloat162_rn(input1[idx], input2[idx]);
    }
    __global__ __vector__ void cast_kernel(__gm__ float* input1,  __gm__ float* input2, __gm__ bfloat16_t* output, uint32_t input_total_length)
    {
        asc_vf_call<simt_floats2bfloat162_rn>(dim3(1024), input1, input2, (__gm__ bfloat16x2_t*)output, input_total_length);
    }
    ```

The input and output examples are as follows:

```text
x: 128.25, 128.5, 128.75, -128.5, 0.0, -0.0, inf, nan
y: nan, inf, -0.0, 0.0, -128.5, 128.75, 128.5, 128.25
result: (128.0, nan), (128.0, inf), (129.0, -0.0), (-128.0, 0.0), (0.0, -128.0), (-0.0, 129.0), (inf, 128.0), (nan, 128.0)
```
