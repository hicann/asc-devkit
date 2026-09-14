# \_\_float2bfloat162\_rn

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-08-29T07:59:48.807Z pushedAt=2026-08-29T09:42:03.838Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Converts **float** data to **bfloat16** following the **CAST\_RINT** mode and fills both the first and second halves of **bfloat16x2**, returning the resulting **bfloat16x2** data.

## Function Prototype

```
inline bfloat16x2_t __float2bfloat162_rn(const float x)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Converts the input data to **bfloat16** following the **CAST_RINT** mode and fills both the first and second halves of **bfloat16x2**, returning the resulting data.

## Constraints

None

## Header Files to Include

To use this API, include the "simt\_api/asc\_bf16.h" header file.

```
#include "simt_api/asc_bf16.h"
```

## Example

-   SIMT programming scenario:

    ```
    // Use short vectors to improve data transfer efficiency.
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

-   SIMD and SIMT hybrid programming scenario:

    ```
    // Use short vectors to improve data transfer efficiency.
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

