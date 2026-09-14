# fdimf

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-01T09:50:45.557Z pushedAt=2026-09-03T07:28:07.101Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Obtains the difference between two input values. If the difference is less than 0, 0 is returned.

![](../../../figures/zh-cn_formulaimage_0000002486622056.png)

## Function Prototype

```
inline float fdimf(float x, float y)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |
| y | Input | Source operand. |

## Return Value

Returns the difference between the input values. If the difference is less than 0, 0 is returned.

-   When either x or y is nan, the return value is nan.
-   When x is inf and y is -inf, the return value is inf.
-   When x is -inf and y is inf, the return value is 0.
-   When x is inf and y is inf, the return value is 0.

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
    __global__ __launch_bounds__(256) void compute_fdimf(float *result, const float *x, const float *y, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = fdimf(x[idx], y[idx]);
    }
    ```

- SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(256) inline void compute_fdimf_vf(__gm__ float *result, __gm__ const float *x, __gm__ const float *y, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = fdimf(x[idx], y[idx]);
    }

    __global__ __vector__ void run_fdimf(__gm__ float *result, __gm__ const float *x, __gm__ const float *y, uint32_t count)
    {
        asc_vf_call<compute_fdimf_vf>(dim3(256), result, x, y, count);
    }
    ```

The input and output examples are as follows:

```
x: 0.25, 0.75, 1.25, 1.75
y: 1.5, 2.5, 3.5, 4.5
result: 0 0 0 0
```
