# \_\_hmax

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-03T13:13:29.167Z pushedAt=2026-09-04T08:03:26.266Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Obtains the maximum value of the two input data.

![](../../../../figures/zh-cn_formulaimage_0000002533455019.png)

## Prototype

```
inline half __hmax(half x, half y)
```

## Description

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **x** | Input | Source operand. |
| **y** | Input | Source operand. |

## Return Value

Returns the maximum of the two input values.

-   When x is +0 and y is -0, returns x.
-   When x is -0 and y is +0, returns y.
-   When x is nan, returns y.
-   When y is nan, returns x.
-   When both x and y are nan, returns nan.
-   When either x or y is inf, returns inf.
-   When x is -inf, returns y.
-   When y is -inf, returns x.

## Constraints

None

## Required Header File

To use the **half** type interface, include the "simt\_api/asc\_fp16.h" header file.

```
#include "simt_api/asc_fp16.h"
```

## Example

-   **SIMT** programming scenario:

    ```
    __global__ __launch_bounds__(1024) void KernelMax(half* dst, half* x, half* y)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __hmax(x[idx], y[idx]);
    }
    ```

-   **SIMD** and **SIMT** hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelMax(__gm__ half* dst, __gm__ half* x, __gm__ half* y)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __hmax(x[idx], y[idx]);
    }
    ```

