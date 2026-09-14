# hexp10

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-04T01:57:38.156Z pushedAt=2026-09-04T11:39:12.070Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Given the input **x**, obtains 10 raised to the power of **x**.

![](../../../../figures/zh-cn_formulaimage_0000002533480319.png)

## Function Prototype

```
inline half hexp10(half x)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

10 raised to the power of x.

-   When x is inf, the return value is inf.
-   When x is -inf, the return value is 0.
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
    __global__ __launch_bounds__(1024) void KernelExp10(half* dst, half* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = hexp10(x[idx]);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelExp10(__gm__ half* dst, __gm__ half* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = hexp10(x[idx]);
    }
    ```

