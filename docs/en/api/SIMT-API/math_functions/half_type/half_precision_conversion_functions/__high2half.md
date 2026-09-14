# \_\_high2half

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-04T01:23:15.069Z pushedAt=2026-09-04T11:39:11.986Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Extracts the high 16 bits of the input **half2** and returns them.

## Function Prototype

```
inline half __high2half(const half2 x)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns the high 16 bits of the input **half2**.

## Constraints

None

## Header Files to Include

To use this API, include the "simt\_api/asc\_fp16.h" header file.

```
#include "simt_api/asc_fp16.h"
```

## Example

-   SIMT programming scenario:

    ```
    // Use short vectors to improve data transfer efficiency.
    __aicore__ void simt_high2half(half2* input, half* output, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Each thread processes one half2 value, that is, two half values. Therefore, threads with idx equal to or greater than half the input_total_length do not process data.
        if (idx > input_total_length / 2) {
            return;
        }
        output[idx] = __high2half(input[idx]);
    }
    __global__ __launch_bounds__(1024) void cast_kernel(half* input, half* output, uint32_t input_total_length)
    {
        simt_high2half((half2*)input, output, input_total_length);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    // Use short vectors to improve data transfer efficiency.
    __simt_vf__ __launch_bounds__(1024) inline void simt_high2half(__gm__ half2* input, __gm__ half* output, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Each thread processes one half2 value, that is, two half values. Therefore, threads with idx equal to or greater than half the input_total_length do not process data.
        if (idx > input_total_length / 2) {
            return;
        }
        output[idx] = __high2half(input[idx]);
    }
    __global__ __vector__ void cast_kernel(__gm__ half* input, __gm__ half* output, uint32_t input_total_length)
    {
        asc_vf_call<simt_high2half>(dim3(1024), (__gm__ half2*)input, output, input_total_length);
    }
    ```

