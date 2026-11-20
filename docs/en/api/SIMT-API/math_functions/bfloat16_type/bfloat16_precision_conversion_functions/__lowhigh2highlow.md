# \_\_lowhigh2highlow

<!-- md-trans-meta sourceCommit=6e11244b08ff98f28bdfed28700f33faca049b68 translatedAt=2026-10-27T02:43:01.206Z pushedAt=2026-10-27T03:16:59.237Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__lowhigh2highlow_res.md#id1 -->

## Description

The header file path is: `"simt_api/asc_bf16.h"`.

Swaps the high and low 16 bits of the input data and returns the result.

## Prototype

```cpp
inline bfloat16x2_t __lowhigh2highlow(const bfloat16x2_t x)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns the result of swapping the high and low 16 bits of the input data. The special values are as follows:

| x Value | Return Value |
|---|---|
| (0, -0) | (-0, 0) |
| (nan, normal value) | (normal value, nan) |
| (inf, -inf) | (-inf, inf) |
| (ASCRT_MAX_NORMAL_BF16, -ASCRT_MAX_NORMAL_BF16) | (-ASCRT_MAX_NORMAL_BF16, ASCRT_MAX_NORMAL_BF16) |
| (nan, nan) | (nan, nan) |

## Constraints

None

## Examples

-   SIMT programming scenario:

    ```cpp
    #include "simt_api/asc_bf16.h"

    // Using short vectors improves data transfer efficiency.
    __aicore__ void simt_lowhigh2highlow(bfloat16x2_t* input, bfloat16x2_t* output, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Each thread processes one bfloat16x2_t value, that is, two bfloat16_t values. Therefore, threads with idx >= input_total_length / 2 do not process data.
        if (idx >= input_total_length / 2) {
            return;
        }
        output[idx] = __lowhigh2highlow(input[idx]);
    }
    __global__ __launch_bounds__(1024) void cast_kernel(bfloat16_t* input, bfloat16_t* output, uint32_t input_total_length)
    {
        simt_lowhigh2highlow((bfloat16x2_t*)input, (bfloat16x2_t*)output, input_total_length);
    }
    ```

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/asc_bf16.h"

    // Using short vectors improves data transfer efficiency.
    __simt_vf__ __launch_bounds__(1024) inline void simt_lowhigh2highlow(__gm__ bfloat16x2_t* input, __gm__ bfloat16x2_t* output, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Each thread processes one bfloat16x2_t value, that is, two bfloat16_t values. Therefore, threads with idx >= input_total_length / 2 do not process data.
        if (idx >= input_total_length / 2) {
            return;
        }
        output[idx] = __lowhigh2highlow(input[idx]);
    }
    __global__ __vector__ void cast_kernel(__gm__ bfloat16_t* input, __gm__ bfloat16_t* output, uint32_t input_total_length)
    {
        asc_vf_call<simt_lowhigh2highlow>(dim3(1024), (__gm__ bfloat16x2_t*)input, (__gm__ bfloat16x2_t*)output, input_total_length);
    }
    ```

The input and output examples are as follows:

```text
x: (0.5, -1.0), (1.0, 2.0), (3.0, 4.0), (0.0, 3.0), (nan, 1.0), (1.0, 2.0), (inf, -inf), (nan, nan)
result: (-1.0, 0.5), (2.0, 1.0), (4.0, 3.0), (3.0, 0.0), (1.0, nan), (2.0, 1.0), (-inf, inf), (nan, nan)
```
