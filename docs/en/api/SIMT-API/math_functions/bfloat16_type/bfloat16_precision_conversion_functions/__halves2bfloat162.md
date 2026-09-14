# \_\_halves2bfloat162

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-08-29T08:04:59.374Z pushedAt=2026-08-29T09:42:03.849Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Fills the two components of **bfloat16x2** with the input data respectively, and returns the filled data.

## Function Prototype

```
inline bfloat16x2_t __halves2bfloat162(const bfloat16_t x, const bfloat16_t y)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |
| y | Input | Source operand. |

## Return Value

Fills the two components of **bfloat16x2** with the input data respectively, and returns the filled data.

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
    __aicore__ void simt_halves2bfloat162(bfloat16_t* input1, bfloat16_t* input2, bfloat16x2_t* output, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx > input_total_length) {
            return;
        }
        output[idx] = __halves2bfloat162(input1[idx], input2[idx]);
    }
    __global__ __launch_bounds__(1024) void cast_kernel(bfloat16_t* input1,  bfloat16_t* input2, bfloat16_t* output, uint32_t input_total_length)
    {
        simt_halves2bfloat162(input1, input2, (bfloat16x2_t*)output, input_total_length);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    // Use short vectors to improve data transfer efficiency.
    __simt_vf__ __launch_bounds__(1024) inline void simt_halves2bfloat162(__gm__ bfloat16_t* input1, __gm__ bfloat16_t* input2, __gm__ bfloat16x2_t* output, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx > input_total_length) {
            return;
        }
        output[idx] = __halves2bfloat162(input1[idx], input2[idx]);
    }
    __global__ __vector__ void cast_kernel(__gm__ bfloat16_t* input1,  __gm__ bfloat16_t* input2, __gm__ bfloat16_t* output, uint32_t input_total_length)
    {
        asc_vf_call<simt_halves2bfloat162>(dim3(1024), input1, input2, (__gm__ bfloat16x2_t*)output, input_total_length);
    }
    ```

