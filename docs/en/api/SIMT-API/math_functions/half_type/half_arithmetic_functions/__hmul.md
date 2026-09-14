# \_\_hmul

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-04T02:14:21.791Z pushedAt=2026-09-04T11:39:12.109Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Computes the product of two half values and rounds the result following the CAST\_RINT mode.

## Function Prototype

```
half __hmul(const half x, const half y)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |
| y | Input | Source operand. |

## Return Value

Returns the product of the input values.

-   When neither the inputs nor the result is nan, the sign of x\*y is the XOR of the signs of x and y.
-   \_\_hmul\(x, y\) is equivalent to \_\_hmul\(y, x\).
-   This API is affected by the global saturation mode. The special values are as follows:

    | x value | y value | non-saturation mode return value | saturation mode return value |
    | --- | --- | --- | --- |
    | Non-zero value | ±inf | The sign is determined by the XOR of the signs of x and y, and the value is inf. | The sign is determined by the XOR of the signs of x and y, and the value is ASCRT_MAX_NORMAL_FP16. |
    | ±0 | ±inf | nan | 0 |
    | ±0 | Finite value | The sign of the return value is determined by the XOR of the signs of x and y, and the value is 0. | The sign of the return value is determined by the XOR of the signs of x and y, and the value is 0. |
    | ±inf | ∓inf | ±inf | ±ASCRT_MAX_NORMAL_FP16 |
    | Either x or y is nan | Either x or y is nan | nan | 0 |

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
    __global__ __launch_bounds__(1024) void KernelHmul(half* dst, half* x, half* y)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __hmul(x[idx], y[idx]);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelHmul(__gm__ half* dst, __gm__ half* x, __gm__ half* y)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __hmul(x[idx], y[idx]);
    }
    ```
