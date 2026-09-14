# h2sqrt

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-03T12:53:06.593Z pushedAt=2026-09-04T08:03:26.207Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Obtains the square root of each input element of **x**.

![](../../../../figures/zh-cn_formulaimage_0000002513334406.png)

## Prototype

```
inline half2 h2sqrt(half2 x)
```

## Description

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand, input data. |

## Return Value

Returns the square root of each element of the input data.

-   When the input element is 0, the return value is 0.
-   When the input element is inf, the return value is inf.
-   When the input element is -inf, the return value is nan.
-   When the input element is nan, the return value is nan.

## Constraints

The range of each input element supported by this API is x greater than or equal to 0; otherwise, the return value is nan.

## Required Header Files

To use the **half2** type interface, include the "simt\_api/asc\_fp16.h" header file.

```
#include "simt_api/asc_fp16.h"
```

## Example

-   SIMT programming scenario:

    ```
    __global__ __launch_bounds__(1024) void KernelSqrt(half2* dst, half2* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = h2sqrt(x[idx]);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelSqrt(__gm__ half2* dst, __gm__ half2* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = h2sqrt(x[idx]);
    }
    ```

