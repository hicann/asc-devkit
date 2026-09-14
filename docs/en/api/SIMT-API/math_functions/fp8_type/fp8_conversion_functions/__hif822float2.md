# \_\_hif822float2

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-01T10:47:46.955Z pushedAt=2026-09-03T07:28:07.248Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Converts the two components of `hifloat8x2\_t` type data into floating-point numbers and returns the converted `float2` type data.

## Function Prototype

```
inline float2 __hif822float2(const hifloat8x2_t x)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **x** | Input | Source operand. |

## Return Value

Returns the `float2` type data converted from the `hifloat8x2_t` type input.

## Constraints

This API is not supported in the SIMT programming scenario.

## Header Files to Include

To use this API, include the "simt\_api/asc\_fp8.h" header file.

```
#include "simt_api/asc_fp8.h"
```

## Example

-   SIMD and SIMT hybrid programming scenario:

    ```
    // Using short vectors improves data transfer efficiency.
    __simt_vf__ __launch_bounds__(1024) inline void simt_hif822float2(__gm__ hifloat8x2_t* input, __gm__ float2* output, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Each thread processes one `hifloat8x2_t` type data item, that is, two `hifloat8_t` type data items. Therefore, threads with with idx equal to or greater than half the input_total_length do not process data.
        if (idx > input_total_length /2) {
            return;
        }
        output[idx] = __hif822float2(input[idx]);
    }
    __global__ __vector__ void cast_kernel(__gm__ uint8_t* input,  __gm__ float* output, uint32_t input_total_length)
    {
        asc_vf_call<simt_hif822float2>(dim3(1024), (__gm__ hifloat8x2_t*)input, (__gm__ float2*)output, input_total_length);
    }
    ```

