# \_\_float22half2\_rn

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-04T00:43:39.488Z pushedAt=2026-09-04T11:39:11.858Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Converts **float2** type data to **half2** type following the CAST\_RINT mode, and returns the converted **half2** type data.

## Function Prototype

```
inline half2 __float22half2_rn(const float2 x)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns the **half2** type data converted from the two input components following the CAST\_RINT mode.

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
    __aicore__ void simt_float22half2_rn(float2* input, half2* output, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Each thread processes one float2 type data item, that is, two float type data items. Therefore, threads with idx equal to or greater than half the input_total_length do not process data.
        if (idx >= input_total_length / 2) {
            return;
        }
        output[idx] = __float22half2_rn(input[idx]);
    }

    __global__ __launch_bounds__(1024) void cast_kernel(float* input, half* output, uint32_t input_total_length)
    {
        simt_float22half2_rn((float2*)input, (half2*)output, input_total_length);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    // Use short vectors to improve data transfer efficiency.
    __simt_vf__ __launch_bounds__(1024) inline void simt_float22half2_rn(__gm__ float2* input, __gm__ half2* output, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Each thread processes one float2 type data item, that is, two float type data items. Therefore, threads with idx equal to or greater than half the input_total_length do not process data.
        if (idx >= input_total_length / 2) {
            return;
        }
        output[idx] = __float22half2_rn(input[idx]);
    }

    __global__ __vector__ void cast_kernel(__gm__ float* input, __gm__ half* output, uint32_t input_total_length)
    {
        asc_vf_call<simt_float22half2_rn>(dim3(1024), (__gm__ float2*)input, (__gm__ half2*)output, input_total_length);
    }
    ```

