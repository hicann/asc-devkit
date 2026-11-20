# \_\_half22hif82\_rna

<!-- md-trans-meta sourceCommit=59c3a526dd351fa43a267dc7a2d150dfa30a63e6 translatedAt=2026-10-26T08:51:49.722Z pushedAt=2026-10-26T11:03:37.462Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/fp8_type/fp8_conversion_functions/__half22hif82_rna_res.md#id1 -->

## Description

The header file path is: `"simt_api/asc_fp8.h"`.

Converts the two components of half2 type data to hifloat8\_t precision following the CAST\_ROUND mode, and returns the converted hifloat8x2\_t type data.

## Prototype

```cpp
inline hifloat8x2_t __half22hif82_rna(const half2 x)
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
| nan | nan | +0 |
| inf | inf | 32768 |
| -inf | -inf | -32768 |
| ASCRT_MAX_NORMAL_FP16 | inf | 32768 |
| -ASCRT_MAX_NORMAL_FP16 | -inf | -32768 |
| ASCRT_MIN_DENORM_FP16 | 0 | 0 |

## Constraints

This API is not supported in SIMT programming scenarios.

## Examples

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/asc_fp8.h"

    // Use short vectors to improve data transfer efficiency.
    __simt_vf__ __launch_bounds__(1024) inline void simt_half22hif82_rna(__gm__ half2* input, __gm__ hifloat8x2_t* output, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Each thread processes one half2 type data item, that is, two half type data items. Therefore, threads with idx >= input_total_length / 2 do not process data.
        if (idx >= input_total_length / 2) {
            return;
        }
        output[idx] = __half22hif82_rna(input[idx]);
    }
    __global__ __vector__ void cast_kernel(__gm__ half* input,  __gm__ uint8_t* output, uint32_t input_total_length)
    {
        asc_vf_call<simt_half22hif82_rna>(dim3(1024), (__gm__ half2*)input, (__gm__ hifloat8x2_t*)output, input_total_length);
    }
    ```

The input and output examples are as follows:

```text
x(half2): (0.0, 17.0), (41.0, 9.0), (-41.0, 8.0), (nan, inf)
result(hifloat8x2_t): (0, 16), (40, 9), (-40, 8), (nan, inf)
```
