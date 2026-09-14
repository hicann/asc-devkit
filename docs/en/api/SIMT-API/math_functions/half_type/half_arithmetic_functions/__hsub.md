# \_\_hsub

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-04T02:15:48.394Z pushedAt=2026-09-04T11:39:12.112Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Computes the result of subtracting two half data values and rounds the result following the CAST\_RINT mode.

## Function Prototype

```
half __hsub(const half x, const half y)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |
| y | Input | Source operand. |

## Return Value

Returns the result of subtracting the input data. This API is affected by the global saturation mode. The special values are as follows:

| x value | y value | non-saturation mode return value | saturation mode return value |
| --- | --- | --- | --- |
| ±inf | finite value | ±inf | ±ASCRT_MAX_NORMAL_FP16 |
| finite value | ±inf | ∓inf | ∓ASCRT_MAX_NORMAL_FP16 |
| ±inf | ±inf | nan | 0 |
| ±inf | ∓inf | ±inf | ±ASCRT_MAX_NORMAL_FP16 |
| finite value (including ±0) | -x | +0 | +0 |
| either x or y is nan | either x or y is nan | nan | 0 |  |

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
    __global__ __launch_bounds__(1024) void KernelHsub(half* dst, half* x, half* y)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __hsub(x[idx], y[idx]);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelHsub(__gm__ half* dst, __gm__ half* x, __gm__ half* y)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __hsub(x[idx], y[idx]);
    }
    ```
