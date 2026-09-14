# hsqrt

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-04T02:08:07.701Z pushedAt=2026-09-04T11:39:12.090Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Obtains the square root of the input data **x**.

![](../../../../figures/zh-cn_formulaimage_0000002533480327.png)

## Function Prototype

```
inline half hsqrt(half x)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **x** | Input | Source operand, input data. |

## Return Value

Returns the square root of the input data.

-   When **x** is 0, the return value is 0.
-   When x is inf, the return value is inf.
-   When x is -inf, the return value is nan.
-   When x is nan, the return value is nan.

## Constraints

The input data range supported by this API is x greater than or equal to 0; otherwise, the return value is nan.

## Header Files to Include

To use the half type API, include the "simt\_api/asc\_fp16.h" header file.

```
#include "simt_api/asc_fp16.h"
```

## Example

-   SIMT programming scenario:

    ```
    __global__ __launch_bounds__(1024) void KernelSqrt(half* dst, half* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = hsqrt(x[idx]);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelSqrt(__gm__ half* dst, __gm__ half* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = hsqrt(x[idx]);
    }
    ```

