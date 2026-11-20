# \_\_hif822half2

<!-- md-trans-meta sourceCommit=59c3a526dd351fa43a267dc7a2d150dfa30a63e6 translatedAt=2026-10-26T08:53:26.509Z pushedAt=2026-10-26T11:03:37.465Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/fp8_type/fp8_conversion_functions/__hif822half2_res.md#id1 -->

## Description

The header file path is: `"simt_api/asc_fp8.h"`.

Converts the two components of hifloat8x2_t type data to half-precision floating-point numbers and returns the converted half2 type data.

## Prototype

```cpp
inline half2 __hif822half2(const hifloat8x2_t x)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

The half2 type data converted from the hifloat8x2\_t type input. Special values are as follows:

| x Component Value | Return Value in Non-saturation Mode | Return Value in Saturation Mode |
|---|---|---|
| ±0 | 0 | 0 |
| nan | nan | 0 |
| inf | inf | ASCRT_MAX_NORMAL_FP16 |
| -inf | -inf | -ASCRT_MAX_NORMAL_FP16 |

## Constraints

This API is not supported in SIMT programming scenarios.

## Examples

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/asc_fp8.h"

    // Use short vectors to improve data transfer efficiency.
    __simt_vf__ __launch_bounds__(1024) inline void simt_hif822half2(__gm__ hifloat8x2_t* input, __gm__ half2* output, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Each thread processes one hifloat8x2_t type data item, that is, two hifloat8_t type data items. Therefore, threads with idx >= input_total_length / 2 do not process data.
        if (idx >= input_total_length / 2) {
            return;
        }
        output[idx] = __hif822half2(input[idx]);
    }
    __global__ __vector__ void cast_kernel(__gm__ uint8_t* input,  __gm__ half* output, uint32_t input_total_length)
    {
        asc_vf_call<simt_hif822half2>(dim3(1024), (__gm__ hifloat8x2_t*)input, (__gm__ half2*)output, input_total_length);
    }
    ```

The input and output examples are as follows:

```text
x(hifloat8x2_t): (0, 1), (0.25, -0.75), (inf, -inf), (nan, 8)
result(half2): (0.0, 1.0), (0.25, -0.75), (inf, -inf), (nan, 8.0)
```
