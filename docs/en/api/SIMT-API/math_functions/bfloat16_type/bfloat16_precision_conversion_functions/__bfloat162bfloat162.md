# \_\_bfloat162bfloat162

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-08-29T07:25:26.113Z pushedAt=2026-08-29T09:42:03.766Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Fills the input data into the two components of **bfloat16x2** and returns the converted **bfloat16x2** data.

## Function Prototype

```
inline bfloat16x2_t __bfloat162bfloat162(const bfloat16_t x)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns the input data filled as **bfloat16x2** data.

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
    __aicore__ void simt_bfloat162bfloat162(bfloat16_t* input, bfloat16x2_t* output, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx > input_total_length) {
            return;
        }
        output[idx] = __bfloat162bfloat162(input[idx]);
    }
    __global__ __launch_bounds__(1024) void cast_kernel(bfloat16_t* input, bfloat16_t* output, uint32_t input_total_length)
    {
        simt_bfloat162bfloat162(input, (bfloat16x2_t*)output, input_total_length);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    // Use short vectors to improve data transfer efficiency.
    __simt_vf__ __launch_bounds__(1024) inline void simt_bfloat162bfloat162(__gm__ bfloat16_t* input, __gm__ bfloat16x2_t* output, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx > input_total_length) {
            return;
        }
        output[idx] = __bfloat162bfloat162(input[idx]);
    }
    __global__ __vector__ void cast_kernel(__gm__ bfloat16_t* input, __gm__ bfloat16_t* output, uint32_t input_total_length)
    {
        asc_vf_call<simt_bfloat162bfloat162>(dim3(1024), input, (__gm__ bfloat16x2_t*)output, input_total_length);
    }
    ```
