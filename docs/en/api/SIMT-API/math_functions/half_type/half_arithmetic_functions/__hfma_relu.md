# \_\_hfma\_relu

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-04T02:12:56.118Z pushedAt=2026-09-04T11:39:12.107Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

For the input half data **x**, **y**, and **z**, computes the result of **x** multiplied by **y** plus **z**, and rounds the result following the CAST\_RINT mode. Negative results are set to 0.

## Function Prototype

```
half __hfma_relu(const half x, const half y, const half z)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |
| y | Input | Source operand. |
| z | Input | Source operand. |

## Return Value

The value of **x** \* **y** + **z**, set to 0 if the result is negative. This API is affected by the global saturation mode. The special values are as follows:

| x value | y value | z value | Non-saturation mode return value | Saturation mode return value |
| --- | --- | --- | --- | --- |
| ±inf | ±0 | — | nan | 0 |
| ±0 | ±inf | — | nan | 0 |
| x*y = inf | x*y = inf |-inf | nan | 0 |
| x*y = -inf | x*y = -inf |inf | nan | 0 |
| x*y+z exceeds ASCRT_MAX_NORMAL_FP16 | - | - | inf | ASCRT_MAX_NORMAL_FP16 |
| x*y+z is less than ASCRT_MIN_NORMAL_FP16 | - | - | 0 | 0 |
| Any of x, y, or z is nan |  Any of x, y, or z is nan | Any of x, y, or z is nan |nan | 0 |  |  |

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
    __global__ __launch_bounds__(1024) void KernelHfma_relu(half* dst, half* x, half* y, half* z)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __hfma_relu(x[idx], y[idx], z[idx]);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelHfma_relu(__gm__ half* dst, __gm__ half* x, __gm__ half* y, __gm__ half* z)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __hfma_relu(x[idx], y[idx], z[idx]);
    }
    ```
