# \_\_hmin

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-08-29T06:41:43.131Z pushedAt=2026-08-29T07:04:53.918Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Obtains the minimum value of the two input data.

![](../../../../figures/zh-cn_formulaimage_0000002545900874.png)

## Function Prototype

```
inline bfloat16_t __hmin(bfloat16_t x, bfloat16_t y)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |
| y | Input | Source operand. |

## Return Value

The minimum value of the two input data. This API is not affected by the global saturation mode. The special values are as follows:

| x Value | y Value | Return Value |
| --- | --- | --- |
| +0 | -0 | -0 |
| -0 | +0 | -0 |
| nan | Non-special number (not nan, inf, or -inf) | y value |
| Non-special number (not nan, inf, or -inf) | nan | x value |
| Both x and y are nan | nan |  |
| Either x or y is -inf | -inf |  |
| inf | Non-special number (not nan, inf, or -inf) | y |
| Non-special number (not nan, inf, or -inf) | inf | x |

## Constraints

None

## Header Files to Include

To use the bfloat16\_t interface, include the "simt\_api/asc\_bf16.h" header file.

```
#include "simt_api/asc_bf16.h"
```

## Example

-   SIMT programming scenario:

    ```
    __global__ __launch_bounds__(1024) void KernelMin(bfloat16_t* dst, bfloat16_t* x, bfloat16_t* y)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __hmin(x[idx], y[idx]);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelMin(__gm__ bfloat16_t* dst, __gm__ bfloat16_t* x, __gm__ bfloat16_t* y)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __hmin(x[idx], y[idx]);
    }
    ```
