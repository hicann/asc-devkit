# \_\_hmax\_nan

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-03T13:12:58.856Z pushedAt=2026-09-04T08:03:26.265Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Obtains the maximum value of the two input data. If either input is nan, the result is nan.

![](../../../../figures/zh-cn_formulaimage_0000002545918676.png)

## Prototype

```
half __hmax_nan(const half x, const half y)
```

## Description

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |
| y | Input | Source operand. |

## Return Value

Returns the maximum of the two input values.

-   When **x** is **+0** and **y** is **-0**, **x** is returned.
-   When **x** is **-0** and **y** is **+0**, **y** is returned.
-   When either **x** or **y** is **nan**, the return value is **nan**.
-   When either **x** or **y** is **inf**, the return value is **inf**.
-   When **x** is **-inf**, the return value is **y**.
-   When **y** is **-inf**, the return value is **x**.

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
    __global__ __launch_bounds__(1024) void KernelHmax_nan(half* dst, half* x, half* y)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __hmax_nan(x[idx], y[idx]);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelHmax_nan(__gm__ half* dst, __gm__ half* x, __gm__ half* y)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __hmax_nan(x[idx], y[idx]);
    }
    ```

