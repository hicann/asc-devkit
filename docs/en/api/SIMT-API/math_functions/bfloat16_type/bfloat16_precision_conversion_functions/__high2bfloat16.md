# \_\_high2bfloat16

<!-- md-trans-meta sourceCommit=6e11244b08ff98f28bdfed28700f33faca049b68 translatedAt=2026-10-27T02:35:49.271Z pushedAt=2026-10-27T03:16:59.225Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__high2bfloat16_res.md#id1 -->

## Description

The header file path is: `"simt_api/asc_bf16.h"`.

Extracts the high 16 bits of the input bfloat16x2 and returns them.

## Prototype

```cpp
inline bfloat16_t __high2bfloat16(const bfloat16x2_t x)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns the high 16 bits of the input bfloat16x2. The special values are as follows:

| High 16 Bits of x | Return Value |
|---|---|
| 0 | 0 |
| -0 | -0 |
| nan | nan |
| inf | inf |
| -inf | -inf |
| ASCRT_MAX_NORMAL_BF16 | ASCRT_MAX_NORMAL_BF16 |
| -ASCRT_MAX_NORMAL_BF16 | -ASCRT_MAX_NORMAL_BF16 |

## Constraints

None

## Examples

-   SIMT programming scenario:

    ```cpp
    #include "simt_api/asc_bf16.h"

    // Use short vectors to improve data transfer efficiency.
    __aicore__ void simt_high2bfloat16(bfloat16x2_t* input, bfloat16_t* output, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Each thread processes one bfloat16x2_t value, that is, two bfloat16_t values. Therefore, threads with idx >= input_total_length / 2 do not process data.
        if (idx >= input_total_length / 2) {
            return;
        }
        output[idx] = __high2bfloat16(input[idx]);
    }
    __global__ __launch_bounds__(1024) void cast_kernel(bfloat16_t* input, bfloat16_t* output, uint32_t input_total_length)
    {
        simt_high2bfloat16(( bfloat16x2_t*)input, output, input_total_length);
    }
    ```

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/asc_bf16.h"

    // Use short vectors to improve data transfer efficiency.
    __simt_vf__ __launch_bounds__(1024) inline void simt_high2bfloat16(__gm__ bfloat16x2_t* input, __gm__ bfloat16_t* output, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Each thread processes one bfloat16x2_t value, that is, two bfloat16_t values. Therefore, threads with idx >= input_total_length / 2 do not process data.
        if (idx >= input_total_length / 2) {
            return;
        }
        output[idx] = __high2bfloat16(input[idx]);
    }
    __global__ __vector__ void cast_kernel(__gm__ bfloat16_t* input, __gm__ bfloat16_t* output, uint32_t input_total_length)
    {
        asc_vf_call<simt_high2bfloat16>(dim3(1024), (__gm__ bfloat16x2_t*)input, output, input_total_length);
    }
    ```

The input and output examples are as follows:

```text
x: (0.5, -1.0), (1.0, 2.0), (3.0, 4.0), (0.0, 3.0), (nan, 1.0), (1.0, 2.0), (inf, -inf), (nan, nan)
result: -1.0, 2.0, 4.0, 3.0, 1.0, 2.0, -inf, nan
```
