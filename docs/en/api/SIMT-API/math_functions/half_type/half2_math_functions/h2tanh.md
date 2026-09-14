# h2tanh

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-03T12:53:26.469Z pushedAt=2026-09-04T08:03:26.209Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not Supported
- Atlas A2 training products/Atlas A2 inference products: Not Supported
- Atlas 200I/500 A2 inference product: Not Supported
- Atlas inference products AI Core: Not Supported
- Atlas inference products Vector Core: Not Supported
- Atlas training products: Not Supported

## Description

Obtains the hyperbolic tangent value of the trigonometric function for each input element.

![](../../../../figures/zh-cn_formulaimage_0000002513334392.png)

## Prototype

```
inline half2 h2tanh(half2 x)
```

## Description

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns the hyperbolic tangent of each element of the input data.

-   When the input element is inf, the return value is 1.0.
-   When the input element is -inf, the return value is -1.0.
-   When the input element is nan, the return value is nan.

## Constraints

None

## Required Header File

To use the half2 type interfaces, include the "simt\_api/asc\_fp16.h" header file.

```
#include "simt_api/asc_fp16.h"
```

## Example

-   SIMT programming scenario:

    ```
    __global__ __launch_bounds__(1024) void KernelTanh(half2* dst, half2* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = h2tanh(x[idx]);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelTanh(__gm__ half2* dst, __gm__ half2* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = h2tanh(x[idx]);
    }
    ```

