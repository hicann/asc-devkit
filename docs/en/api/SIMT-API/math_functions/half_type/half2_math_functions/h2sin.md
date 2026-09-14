# h2sin

<!-- md-trans-meta sourceCommit=1e3d48cde951e4cd58f9cded9fce17c1f24fa02d translatedAt=2026-09-03T12:52:07.169Z pushedAt=2026-09-04T08:03:26.206Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Obtains the sine value of each element of the input data.

![](../../../../figures/zh-cn_formulaimage_0000002513174474.png)

## Prototype

```
inline half2 h2sin(half2 x)
```

## Description

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **x** | Input | Source operand. |

## Return Value

Returns the sine of each input element.

-   When the input element is inf, the return value is nan.
-   When the input element is -inf, the return value is nan.
-   When the input element is nan, the return value is nan.

## Constraints

None

## Required Header File

To use the **half2** type interface, include the "simt\_api/asc\_fp16.h" header file.

```
#include "simt_api/asc_fp16.h"
```

## Example

-   SIMT programming scenario:

    ```
    __global__ __launch_bounds__(1024) void KernelSin(half2* dst, half2* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = h2sin(x[idx]); // Compute the sine of the idx-th element at the src source address.
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelSin(__gm__ half2* dst, __gm__ half2* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = h2sin(x[idx]); // Compute the sine of the idx-th element at the src source address.
    }
    ```

