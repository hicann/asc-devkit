# \_\_isnanx2

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-03T12:42:41.942Z pushedAt=2026-09-04T08:03:26.187Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Determines whether the two components of a **half2** value are **nan**.

## Prototype

```
half2 __isnanx2(half2 x)
```

## Description

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **x** | Input | Source operand. |

## Return Value

Determines whether each component of the input data is nan. If a component is nan, the corresponding result is 1.0; otherwise, it is 0.0.

## Constraints

None

## Required Header Files

To use this API, include the "simt\_api/asc\_fp16.h" header file.

```
#include "simt_api/asc_fp16.h"
```

## Example

-   SIMT programming scenario:

    ```
    // Using short vectors improves data transfer efficiency.
    __global__ __launch_bounds__(1024) void simt_isnanx2(half* x, half* dst, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Each thread processes one half2 value, that is, two half values. Therefore, threads with idx equal to or greater than half the input_total_length do not process data.
        if (idx >= input_total_length / 2) {
            return;
        }
        half2* input = (half2*)x;
        half2* out = (half2*)dst;
        out[idx] = __isnanx2(input[idx]);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    // Using short vectors improves data transfer efficiency.
    __simt_vf__ __launch_bounds__(1024) inline void simt_isnanx2(__gm__ half2* x, __gm__ half2* dst, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Each thread processes one half2 value, that is, two half values. Therefore, threads with idx equal to or greater than half the input_total_length do not process data.
        if (idx >= input_total_length / 2) {
            return;
        }
        dst[idx] = __isnanx2(x[idx]);
    }

    __global__ __vector__ void compute_kernel(__gm__ half* x, __gm__ half* dst, uint32_t input_total_length)
    {
        asc_vf_call<simt_isnanx2>(dim3(1024), (__gm__ half2*)x, (__gm__ half2*)dst, input_total_length);
    }
    ```

