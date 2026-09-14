# \_\_floats2half2\_rn

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-04T00:59:40.492Z pushedAt=2026-09-04T11:39:11.909Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Converts the input data x and y to the half type following the CAST\_RINT mode, fills them into the first and second halves of half2 respectively, and returns the converted half2 data.

## Function Prototype

```
inline half2 __floats2half2_rn(const float x, const float y)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |
| y | Input | Source operand. |

## Return Value

The result of converting the input float data to the half type following the CAST\_RINT mode and filling them into the first and second halves of half2 respectively.

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
    __aicore__ void simt_floats2half2_rn(float* input1, float* input2, half2* output, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx > input_total_length) {
            return;
        }
        output[idx] = __floats2half2_rn(input1[idx], input2[idx]);
    }
    __global__ __launch_bounds__(1024) void cast_kernel(float* input1, float* input2, half* output, uint32_t input_total_length)
    {
        simt_floats2half2_rn(input1, input2, (half2*)output, input_total_length);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    // Use short vectors to improve data transfer efficiency.
    __simt_vf__ __launch_bounds__(1024) inline void simt_floats2half2_rn(__gm__ float* input1, __gm__ float* input2, __gm__ half2* output, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx > input_total_length) {
            return;
        }
        output[idx] = __floats2half2_rn(input1[idx], input2[idx]);
    }
    __global__ __vector__ void cast_kernel(__gm__ float* input1,  __gm__ float* input2, __gm__ half* output, uint32_t input_total_length)
    {
        asc_vf_call<simt_floats2half2_rn>(dim3(1024), input1, input2, (__gm__ half2*)output, input_total_length);
    }
    ```

