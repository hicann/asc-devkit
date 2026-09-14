# \_\_umul64hi

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-08-29T03:05:38.671Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Obtains the high 64 bits of the product of the input uint64 data **x** and **y**.

![](../../../figures/zh-cn_formulaimage_0000002531050522.png)

## Function Prototype

```
unsigned long long __umul64hi(unsigned long long x, unsigned long long y)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand, multiplier. |
| y | Input | Source operand, multiplier. |

## Return Value

Returns the high 64 bits of the product of the input uint64 data **x** and **y**.

## Constraints

None

## Header Files to Include

To use this API, include the **simt\_api/device\_functions.h** header file.

```
#include "simt_api/device_functions.h"
```

## Example

-   SIMT programming scenario:

    ```
    __global__ __launch_bounds__(1024) void KernelUmul64hi(unsigned long long* dst, unsigned long long* x, unsigned long long* y)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __umul64hi(x[idx], y[idx]);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelUmul64hi(__gm__ unsigned long long* dst, __gm__ unsigned long long* x, __gm__ unsigned long long* y)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __umul64hi(x[idx], y[idx]);
    }
    ```

