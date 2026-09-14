# ullmin

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-08-29T03:14:01.760Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Obtains the minimum value of the two input data.

![](../../../figures/zh-cn_formulaimage_0000002521590974.png)

## Function Prototype

```
inline unsigned long long int ullmin(const unsigned long long int x, const unsigned long long int y)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |
| y | Input | Source operand. |

## Return Value

Returns the minimum value of the two input data.

## Constraints

None

## Header Files to Include

To use this API, include the **simt\_api/math\_functions.h** header file.

```
#include "simt_api/math_functions.h"
```

## Example

-   SIMT programming scenario:

    ```
    __global__ __launch_bounds__(1024) void KernelUllmin(unsigned long long int* dst, unsigned long long int* x, unsigned long long int* y)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = ullmin(x[idx], y[idx]);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelUllmin(__gm__ unsigned long long int* dst, __gm__ unsigned long long int* x, __gm__ unsigned long long int* y)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = ullmin(x[idx], y[idx]);
    }
    ```

