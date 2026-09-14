# \_\_hadd

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-08-29T09:57:26.835Z pushedAt=2026-09-01T09:20:52.896Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Computes the sum of two bfloat16 values and rounds the result following the **CAST_RINT** mode.

## Function Prototype

```
bfloat16_t __hadd(const bfloat16_t x, const bfloat16_t y)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |
| y | Input | Source operand. |

## Return Value

Returns the sum of the input data. This API is not affected by the global saturation mode. Special values are as follows:

-   \_\_hadd\(x, y\) is equivalent to \_\_hadd\(y, x\).
-   When x is a finite value and y is ±inf, the return value is ±inf.
-   When x is ±inf and y is ±inf, the return value is ±inf.
-   When x is ±inf and y is ∓inf, the return value is nan.
-   When x is ±0 and y is ±0, the return value is ±0.
-   For a finite value x (including ±0), when x = -y, the return value is +0.
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
    __global__ __launch_bounds__(1024) void KernelHadd(bfloat16_t* dst, bfloat16_t* x, bfloat16_t* y)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __hadd(x[idx], y[idx]);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelHadd(__gm__ bfloat16_t* dst, __gm__ bfloat16_t* x, __gm__ bfloat16_t* y)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __hadd(x[idx], y[idx]);
    }
    ```

