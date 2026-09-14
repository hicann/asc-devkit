# \_\_low2bfloat16

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-08-29T08:16:59.440Z pushedAt=2026-08-29T09:42:03.881Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Returns the lower 16 bits of the input data.

## Function Prototype

```
inline bfloat16_t __low2bfloat16(const bfloat16x2_t x)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

The lower 16 bits of the input data.

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
    // Using short vectors improves data transfer efficiency.
    __aicore__ void simt_low2bfloat16(bfloat16x2_t* input, bfloat16_t* output, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Each thread processes one bfloat16x2_t value, that is, two bfloat16_t values. Therefore, threads with idx equal to or greater than half the input_total_length do not process data.
        if (idx > input_total_length / 2) {
            return;
        }
        output[idx] = __low2bfloat16(input[idx]);
    }
    __global__ __launch_bounds__(1024) void cast_kernel(bfloat16_t* input, bfloat16_t* output, uint32_t input_total_length)
    {
        simt_low2bfloat16((bfloat16x2_t*)input, output, input_total_length);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    // Using short vectors improves data transfer efficiency.
    __simt_vf__ __launch_bounds__(1024) inline void simt_low2bfloat16(__gm__ bfloat16x2_t* input, __gm__ bfloat16_t* output, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Each thread processes one bfloat16x2_t value, that is, two bfloat16_t values. Therefore, threads with idx equal to or greater than half the input_total_length do not process data.
        if (idx > input_total_length / 2) {
            return;
        }
        output[idx] = __low2bfloat16(input[idx]);
    }
    __global__ __vector__ void cast_kernel(__gm__ bfloat16_t* input, __gm__ bfloat16_t* output, uint32_t input_total_length)
    {
        asc_vf_call<simt_low2bfloat16>(dim3(1024), (__gm__ bfloat16x2_t*)input, output, input_total_length);
    }
    ```

