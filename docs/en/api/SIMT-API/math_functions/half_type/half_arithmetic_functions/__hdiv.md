# \_\_hdiv

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-04T02:11:30.399Z pushedAt=2026-09-04T11:39:12.105Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Computes the quotient of two half values and rounds the result following the CAST\_RINT mode.

## Function Prototype

```
half __hdiv(const half x, const half y)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |
| y | Input | Source operand. |

## Return Value

Returns the result of dividing the input data values.

-   When neither the input nor the result is nan, the sign of x/y is the XOR of the signs of x and y.
-   This API is affected by the global saturation mode. The special values are as follows:

    | x value | y value | non-saturation mode return value | saturation mode return value |
    | --- | --- | --- | --- |
    | ±0 | ±0 | nan | 0 |
    | ±inf | ±inf | nan | 0 |
    | finite value | ±inf | The sign of the return value is determined by the XOR of the signs of x and y, and the value is 0. | The sign of the return value is determined by the XOR of the signs of x and y, and the value is 0. |
    | ±inf | finite value | The sign of the return value is determined by the XOR of the signs of x and y, and the value is inf. | The sign of the return value is determined by the XOR of the signs of x and y, and the value is ASCRT_MAX_NORMAL_FP16. |
    | non-zero | ±0 | The sign of the return value is determined by the XOR of the signs of x and y, and the value is inf. | The sign of the return value is determined by the XOR of the signs of x and y, and the value is ASCRT_MAX_NORMAL_FP16. |
    | ±0 | non-zero | The sign of the return value is determined by the XOR of the signs of x and y, and the value is 0. | The sign of the return value is determined by the XOR of the signs of x and y, and the value is 0. |
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
    __global__ __launch_bounds__(1024) void KernelHdiv(half* dst, half* x, half* y)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __hdiv(x[idx], y[idx]);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelHdiv(__gm__ half* dst, __gm__ half* x, __gm__ half* y)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __hdiv(x[idx], y[idx]);
    }
    ```
