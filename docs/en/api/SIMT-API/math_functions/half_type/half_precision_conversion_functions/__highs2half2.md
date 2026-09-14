# \_\_highs2half2

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-04T01:24:44.227Z pushedAt=2026-09-04T11:39:11.988Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Extracts the high 16 bits of each of the two half2 inputs, fills them into a half2 value, and returns the filled data.

## Function Prototype

```
inline half2 __highs2half2(const half2 x, const half2 y)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |
| y | Input | Source operand. |

## Return Value

Extracts the high 16 bits of each of the two half2 inputs and fills them into a half2 result.

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
    // Using short vectors improves data transfer efficiency.
    __aicore__ void simt_highs2half2(half2* input1, half2* input2, half2* output, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Each thread processes one half2 value, that is, two half values. Therefore, threads with idx equal to or greater than half the input_total_length do not process data.
        if (idx > input_total_length / 2) {
            return;
        }
        output[idx] = __highs2half2(input1[idx], input2[idx]);
    }
    __global__ __launch_bounds__(1024) void cast_kernel(half* input1, half* input2, half* output, uint32_t input_total_length)
    {
        simt_highs2half2((half2*)input1, (half2*)input2, (half2*)output, input_total_length);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    // Using short vectors improves data transfer efficiency.
    __simt_vf__ __launch_bounds__(1024) inline void simt_highs2half2(__gm__ half2* input1, __gm__ half2* input2, __gm__ half2* output, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Each thread processes one half2 value, that is, two half values. Therefore, threads with idx equal to or greater than half the input_total_length do not process data.
        if (idx > input_total_length / 2) {
            return;
        }
        output[idx] = __highs2half2(input1[idx], input2[idx]);
    }
    __global__ __vector__ void cast_kernel(__gm__ half* input1,  __gm__ half* input2, __gm__ half* output, uint32_t input_total_length)
    {
        asc_vf_call<simt_highs2half2>(dim3(1024), (__gm__ half2*)input1, (__gm__ half2*)input2, (__gm__ half2*)output, input_total_length);
    }
    ```

