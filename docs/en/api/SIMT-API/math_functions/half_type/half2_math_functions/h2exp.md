# h2exp

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-03T12:46:02.501Z pushedAt=2026-09-04T08:03:26.193Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Given an input x, for each element of x, obtains e raised to the power of that element.

![](../../../../figures/zh-cn_formulaimage_0000002513174468.png)

## Prototype

```
inline half2 h2exp(half2 x)
```

## Description

**Table 1**  Parameters

| Parameter Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns e raised to the power of each input element.

-   When the input element is inf, the return value is inf.
-   When the input element is -inf, the return value is 0.
-   When the input element is nan, the return value is nan.

## Constraints

None

## Required Header File

To use the **half2** type interface, include the "simt\_api/asc\_fp16.h" header file.

```
#include "simt_api/asc_fp16.h"
```

## Example

-   **SIMT** programming scenario:

    ```
    __global__ __launch_bounds__(1024) void KernelExp(half2* dst, half2* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = h2exp(x[idx]);
    }
    ```

-   **SIMD** and **SIMT** hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelExp(__gm__ half2* dst, __gm__ half2* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = h2exp(x[idx]);
    }
    ```

