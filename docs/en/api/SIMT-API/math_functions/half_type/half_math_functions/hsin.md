# hsin

<!-- md-trans-meta sourceCommit=1e3d48cde951e4cd58f9cded9fce17c1f24fa02d translatedAt=2026-09-04T02:06:38.096Z pushedAt=2026-09-04T11:39:12.088Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Obtains the trigonometric sine value of the input data.

![](../../../../figures/zh-cn_formulaimage_0000002533360287.png)

## Function Prototype

```
inline half hsin(half x)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Obtains the trigonometric sine value of the input data.

-   When x is inf, the return value is nan.
-   When x is -inf, the return value is nan.
-   When x is nan, the return value is nan.

## Constraints

None

## Header Files to Include

To use the half type API, include the "simt\_api/asc\_fp16.h" header file.

```
#include "simt_api/asc_fp16.h"
```

## Example

-   SIMT programming scenario:

    ```
    __global__ __launch_bounds__(1024) void KernelSin(half* dst, half* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = hsin(x[idx]); // Obtain the sine value of the idx-th element at the source address src.
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelSin(__gm__ half* dst, __gm__ half* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = hsin(x[idx]); // Obtain the sine value of the idx-th element at the source address src.
    }
    ```

