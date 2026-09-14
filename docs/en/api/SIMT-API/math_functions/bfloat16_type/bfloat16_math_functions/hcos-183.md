# hcos

<!-- md-trans-meta sourceCommit=1e3d48cde951e4cd58f9cded9fce17c1f24fa02d translatedAt=2026-08-29T09:47:24.945Z pushedAt=2026-09-01T09:20:52.841Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Obtains the trigonometric cosine value of the input data.

![](../../../../figures/zh-cn_formulaimage_0000002545900866.png)

## Function Prototype

```
inline bfloat16_t hcos(bfloat16_t x)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns the trigonometric cosine value of the input data. This API is affected by the global saturation mode. The special values are as follows:

| x Value | Non-saturation Mode Return Value | Saturation Mode Return Value |
| --- | --- | --- |
| inf, -inf, nan | nan | 0 |

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
    __global__ __launch_bounds__(1024) void KernelCos(bfloat16_t* dst, bfloat16_t* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = hcos(x[idx]); // Obtain the trigonometric cosine value of the idx-th element at the source address src.
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelCos(__gm__ bfloat16_t* dst, __gm__ bfloat16_t* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = hcos(x[idx]); // Obtain the trigonometric cosine value of the idx-th element at the source address src.
    }
    ```

