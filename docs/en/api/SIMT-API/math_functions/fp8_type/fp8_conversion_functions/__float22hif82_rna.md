# \_\_float22hif82\_rna

<!-- md-trans-meta sourceCommit=59c3a526dd351fa43a267dc7a2d150dfa30a63e6 translatedAt=2026-10-26T08:48:12.726Z pushedAt=2026-10-26T11:03:37.449Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/fp8_type/fp8_conversion_functions/__float22hif82_rna_res.md#id1 -->

## Description

The header file path is: `"simt_api/asc_fp8.h"`.

Converts the two components of float2 type data to hifloat8_t precision following the CAST_ROUND mode, and returns the converted hifloat8x2_t type data.

## Prototype

```cpp
inline hifloat8x2_t __float22hif82_rna(const float2 x)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns hifloat8x2\_t type data converted from the two input components following the CAST\_ROUND mode. This API is affected by the global saturation mode. The special values are as follows:

| x Component Value | Return Value in Non-saturation Mode | Return Value in Saturation Mode |
|---|---|---|
| ±0 | 0 | 0 |
| nan | nan | 0 |
| inf | inf | 32768 |
| -inf | -inf | -32768 |

## Constraints

This API is not supported in SIMT programming scenarios.

## Examples

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/asc_fp8.h"

    // Use short vectors to improve data transfer efficiency.
    __simt_vf__ __launch_bounds__(1024) inline void simt_float22hif82_rna(__gm__ float2* input, __gm__ hifloat8x2_t* output, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Each thread processes one float2 type data item, that is, two float type data items. Therefore, threads with idx >= input_total_length / 2 do not process data.
        if (idx >= input_total_length / 2) {
            return;
        }
        output[idx] = __float22hif82_rna(input[idx]);
    }
    __global__ __vector__ void cast_kernel(__gm__ float* input, __gm__ uint8_t* output, uint32_t input_total_length)
    {
        asc_vf_call<simt_float22hif82_rna>(dim3(1024), (__gm__ float2*)input, (__gm__ hifloat8x2_t*)output, input_total_length);
    }
    ```

The input and output examples are as follows:

```text
x(float2): (0.0, 1.75), (0.25, 16.25), (128.25, 8.0), (nan, inf)
result(hifloat8x2_t): (0, 1.75), (0.25, 16), (128, 8), (nan, inf)
```
