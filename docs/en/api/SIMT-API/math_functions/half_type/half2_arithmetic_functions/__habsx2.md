# \_\_habsx2

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-03T12:56:02.110Z pushedAt=2026-09-04T08:03:26.227Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Computes the absolute value of each component of the input **half2** data.

## Prototype

```
half2 __habsx2(const half2 x)
```

## Description

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **x** | Input | Source operand. |

## Return Value

Returns the absolute value of each component of the input data.

-   When the input element is ±0, the return value is +0.
-   When the input element is nan, the return value is nan.
-   When the input element is inf, the return value is inf.
-   When the input element is -inf, the return value is inf.

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
    __global__ __launch_bounds__(1024) void simt_habsx2(half* x, half* dst, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Each thread processes one half2 data item, that is, two half data items. Therefore, threads with idx equal to or greater than half the input_total_length do not process data.
        if (idx >= input_total_length / 2) {
            return;
        }
        half2* input = (half2*)x;
        half2* out = (half2*)dst;
        out[idx] = __habsx2(input[idx]);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    // Using short vectors improves data transfer efficiency.
    __simt_vf__ __launch_bounds__(1024) inline void simt_habsx2(__gm__ half2* x, __gm__ half2* dst, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Each thread processes one half2 data item, that is, two half data items. Therefore, threads with idx equal to or greater than half the input_total_length do not process data.
        if (idx >= input_total_length / 2) {
            return;
        }
        dst[idx] = __habsx2(x[idx]);
    }

    __global__ __vector__ void compute_kernel(__gm__ half* x, __gm__ half* dst, uint32_t input_total_length)
    {
        asc_vf_call<simt_habsx2>(dim3(1024), (__gm__ half2*)x, (__gm__ half2*)dst, input_total_length);
    }
    ```

