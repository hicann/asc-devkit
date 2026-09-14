# \_\_hmul

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-08-29T10:00:10.233Z pushedAt=2026-09-01T09:20:52.902Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Computes the product of two bfloat16 values and rounds the result following the CAST\_RINT mode.

## Function Prototype

```
bfloat16_t __hmul(const bfloat16_t x, const bfloat16_t y)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |
| y | Input | Source operand. |

## Return Value

Returns the product of the input values. This API is not affected by the global saturation mode. The special values are as follows:

-   When neither the inputs nor the result is nan, the sign of x\*y is the XOR of the signs of x and y.
-   \_\_hmul\(x, y\) is equivalent to \_\_hmul\(y, x\).
-   When x is a non-zero value and y is ±inf, the sign of the return value is determined by the XOR of the signs of x and y, and the value is inf.
-   When x is ±0 and y is ±inf, the return value is nan.
-   When x is ±0 and y is a finite value, the sign of the return value is determined by the XOR of the signs of x and y, and the value is 0.
-   When either x or y is nan, the return value is nan.

## Constraints

None

## Header Files to Include

To use this API, include the "simt\_api/asc\_bf16.h" header file.

```
#include "simt_api/asc_bf16.h"
```

## Example

-   SIMT programming scenario:

    ```
    __global__ __launch_bounds__(1024) void KernelHmul(bfloat16_t* dst, bfloat16_t* x, bfloat16_t* y)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __hmul(x[idx], y[idx]);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelHmul(__gm__ bfloat16_t* dst, __gm__ bfloat16_t* x, __gm__ bfloat16_t* y)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __hmul(x[idx], y[idx]);
    }
    ```

