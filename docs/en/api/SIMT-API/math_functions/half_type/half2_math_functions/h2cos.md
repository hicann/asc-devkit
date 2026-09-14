# h2cos

<!-- md-trans-meta sourceCommit=1e3d48cde951e4cd58f9cded9fce17c1f24fa02d translatedAt=2026-09-03T12:44:49.475Z pushedAt=2026-09-04T08:03:26.190Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Obtains the trigonometric cosine value of each input element.

![](../../../../figures/zh-cn_formulaimage_0000002513329620.png)

## Prototype

```
inline half2 h2cos(half2 x)
```

## Description

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns the trigonometric cosine value of each element of the input data.

-   When the input element is inf, the return value is nan.
-   When the input element is -inf, the return value is nan.
-   When the input element is nan, the return value is nan.

## Constraints

None

## Required Header File

To use the **half2** type interfaces, include the "simt\_api/asc\_fp16.h" header file.

```
#include "simt_api/asc_fp16.h"
```

## Example

-   SIMT programming scenario:

    ```
    __global__ __launch_bounds__(1024) void KernelCos(half2* dst, half2* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = h2cos(x[idx]); // Obtain the trigonometric cosine value of the idx-th element at the source address src.
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelCos(__gm__ half2* dst, __gm__ half2* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = h2cos(x[idx]); // Obtain the trigonometric cosine value of the idx-th element at the source address src.
    }
    ```

