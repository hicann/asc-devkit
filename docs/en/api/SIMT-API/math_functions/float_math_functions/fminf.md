# fminf

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-01T09:54:16.749Z pushedAt=2026-09-03T07:28:07.123Z -->

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

![](../../../figures/zh-cn_formulaimage_0000002518941923.png)

## Function Prototype

```
inline float fminf(float x, float y)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |
| y | Input | Source operand. |

## Return Value

Returns the minimum value of the two input data.

-   When x is -0 and y is +0, x is returned.
-   When x is +0 and y is -0, y is returned.
-   When x is nan, the return value is y.
-   When y is nan, the return value is x.
-   When both x and y are nan, the return value is nan.
-   When either x or y is -inf, the return value is -inf.
-   When x is inf, the return value is y.
-   When y is inf, the return value is x.

## Constraints

None

## Header Files to Include

To use this API, include the **simt\_api/math\_functions.h** header file.

```
#include "simt_api/math_functions.h"
```

## Example

- SIMT programming scenario:

    ```
    __global__ __launch_bounds__(256) void compute_fminf(float *result, const float *x, const float *y, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = fminf(x[idx], y[idx]);
    }
    ```

- SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(256) inline void compute_fminf_vf(__gm__ float *result, __gm__ const float *x, __gm__ const float *y, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = fminf(x[idx], y[idx]);
    }

    __global__ __vector__ void run_fminf(__gm__ float *result, __gm__ const float *x, __gm__ const float *y, uint32_t count)
    {
        asc_vf_call<compute_fminf_vf>(dim3(256), result, x, y, count);
    }
    ```

The input and output examples are as follows:

```
x: 0.25, 0.75, 1.25, 1.75
y: 1.5, 2.5, 3.5, 4.5
result: 0.25 0.75 1.25 1.75
```
