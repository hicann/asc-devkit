# \_\_float22bfloat162\_ru\_sat

<!-- md-trans-meta sourceCommit=7b786840f05dedef3280f2d5a265e24854520432 translatedAt=2026-10-23T14:16:49.892Z pushedAt=2026-10-26T06:23:22.866Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float22bfloat162_ru_sat_res.md#id1 -->

## Description

The header file path is: `"simt_api/asc_bf16.h"`.

In saturation mode, converts the two components of float2 type data to bfloat16 precision following the CAST\_CEIL mode, and returns the converted bfloat16x2\_t type data.

## Prototype

```cpp
inline bfloat16x2_t __float22bfloat162_ru_sat(const float2 x)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns bfloat16x2_t data converted from the two input components following the CAST_CEIL mode in saturation mode. The special values are as follows:

| x Component Value | Return Value in Non-saturation Mode  | Return Value in Saturation Mode |
|---|---|---|
| 0 | 0 | 0 |
| -0 | -0 | -0 |
| nan | nan | 0 |
| inf | inf | ASCRT_MAX_NORMAL_BF16 |
| -inf | -inf | -ASCRT_MAX_NORMAL_BF16 |
| >ASCRT_MAX_NORMAL_BF16 | inf | ASCRT_MAX_NORMAL_BF16 |
| <-ASCRT_MAX_NORMAL_BF16 | -ASCRT_MAX_NORMAL_BF16 | -ASCRT_MAX_NORMAL_BF16 |

## Constraints

Before using this API, set the CTRL\[60\] register to 0; otherwise, the saturation mode does not take effect. For the setting method, see [Controlling Saturation Behavior](../../data_type_conversion/overview.md#section1194916101549).

In SIMT programming scenarios, the saturation mode of this API does not take effect because the CTRL register cannot be set.

## Examples

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/asc_bf16.h"

    // Use short vectors to improve data transfer efficiency.
    __simt_vf__ __launch_bounds__(1024) inline void simt_float22bfloat162_ru_sat(__gm__ float2* input, __gm__ bfloat16x2_t* output, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Each thread processes one float2 data item, that is, two float data items. Therefore, threads with idx >= input_total_length / 2 do not process data.
        if (idx >= input_total_length / 2) {
            return;
        }
        output[idx] = __float22bfloat162_ru_sat(input[idx]);
    }

    __global__ __vector__ void cast_kernel(__gm__ float* input, __gm__ bfloat16_t* output, uint32_t input_total_length)
    {
        asc_vf_call<simt_float22bfloat162_ru_sat>(dim3(1024), (__gm__ float2*)input, (__gm__ bfloat16x2_t*)output, input_total_length);
    }
    ```

The input and output examples are as follows:

```text
x: (128.25, nan), (128.5, 0.0), (128.75, -inf), (-128.5, inf), (inf, -128.5), (-inf, 128.75), (0.0, 128.5), (nan, 128.25)
result: (129.0, 0.0), (129.0, 0.0), (129.0, -3.38953e+38), (-128.0, 3.38953e+38), (3.38953e+38, -128.0), (-3.38953e+38, 129.0), (0.0, 129.0), (0.0, 129.0)
```
