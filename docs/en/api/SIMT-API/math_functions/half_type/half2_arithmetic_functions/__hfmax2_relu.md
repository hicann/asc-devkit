# \_\_hfmax2\_relu

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-03T12:58:10.631Z pushedAt=2026-09-04T08:03:26.232Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Computes the multiply-add result of each component of the input **half2** data (the first two inputs are multiplied and then added to the third input), and rounds the result following the CAST\_RINT mode. Negative results are set to 0.

## Prototype

```
half2 __hfmax2_relu(const half2 x, const half2 y, const half2 z)
```

## Description

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **x** | Input | Source operand. |
| **y** | Input | Source operand. |
| **z** | Input | Source operand. |

## Return Value

Returns the multiply-add result of each component of the input data. The computed components a, b, and c satisfy the following conditions:

-   If a is ±inf and b is ±0, nan is returned.
-   If a is ±0 and b is ±inf, nan is returned.
-   If a\*b is +inf and c is -inf, nan is returned.
-   If a\*b is -inf and c is +inf, nan is returned.
-   If a\*b+c exceeds the maximum value of the corresponding type range, +inf is returned.
-   If a\*b+c is less than the minimum value of the corresponding type range, 0 is returned.
-   If any of a, b, or c is nan, nan is returned.

## Constraints

None

## Required Header File

To use this API, include the "simt\_api/asc\_fp16.h" header file.

```
#include "simt_api/asc_fp16.h"
```

## Example

-   SIMT programming scenario:

    ```
    // Use short vectors to improve data transfer efficiency.
    __global__ __launch_bounds__(1024) void simt_hfmax2_relu(half* x, half* y, half* z, half* dst, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Each thread processes one half2 data item, that is, two half data items. Therefore, threads with idx equal to or greater than half the input_total_length do not process data.
        if (idx >= input_total_length / 2) {
            return;
        }
        half2* input1 = (half2*)x;
        half2* input2 = (half2*)y;
        half2* input3 = (half2*)z;
        half2* out = (half2*)dst;
        out[idx] = __hfmax2_relu(input1[idx], input2[idx], input3[idx]);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    // Use short vectors to improve data transfer efficiency.
    __simt_vf__ __launch_bounds__(1024) inline void simt_hfmax2_relu(__gm__ half2* x, __gm__ half2* y, __gm__ half2* z, __gm__ half2* dst, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Each thread processes one half2 data item, that is, two half data items. Therefore, threads with idx equal to or greater than half the input_total_length do not process data.
        if (idx >= input_total_length / 2) {
            return;
        }
        dst[idx] = __hfmax2_relu(x[idx], y[idx], z[idx]);
    }

    __global__ __vector__ void compute_kernel(__gm__ half* x, __gm__ half* y, __gm__ half* z, __gm__ half* dst, uint32_t input_total_length)
    {
        asc_vf_call<simt_hfmax2_relu>(dim3(1024), (__gm__ half2*)x, (__gm__ half2*)y, (__gm__ half2*)z, (__gm__ half2*)dst, input_total_length);
    }
    ```

