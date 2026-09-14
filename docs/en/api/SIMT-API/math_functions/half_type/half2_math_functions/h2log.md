# h2log

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-03T12:47:35.001Z pushedAt=2026-09-04T08:03:26.197Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Obtains the natural logarithm of each element of the input data.

![](../../../../figures/zh-cn_formulaimage_0000002513334400.png)

## Prototype

```
inline half2 h2log(half2 x)
```

## Description

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns the natural logarithm of each input element.

-   When the input element is less than 0 or is NaN, the return value is NaN.
-   When the input element is 0, the return value is -infinity.
-   When the input element is infinity, the return value is infinity.

## Constraints

None

## Required Header Files

To use the half2 type APIs, include the "simt\_api/asc\_fp16.h" header file.

```
#include "simt_api/asc_fp16.h"
```

## Example

-   SIMT programming scenario:

    ```
    __global__ __launch_bounds__(1024) void KernelLog(half2* dst, half2* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = h2log(x[idx]);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelLog(__gm__ half2* dst, __gm__ half2* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = h2log(x[idx]);
    }
    ```

