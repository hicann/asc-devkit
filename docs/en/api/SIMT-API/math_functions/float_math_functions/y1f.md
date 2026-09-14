# y1f

<!-- md-trans-meta sourceCommit=1e3d48cde951e4cd58f9cded9fce17c1f24fa02d translatedAt=2026-09-01T10:37:21.411Z pushedAt=2026-09-03T07:28:07.230Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Obtains the value of the Bessel function of the second kind of order 1, y1, for the input data x.

![](../../../figures/zh-cn_formulaimage_0000002484776434.png)

## Function Prototype

```
inline float y1f(float x)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

The value of the Bessel function of the second kind of order 1, y1, for the input data.

-   When x < 0, the return value is nan.
-   When x = 0, the return value is -inf.
-   When x = inf, the return value is 0.
-   When x = nan, the return value is nan.

## Constraints

<!-- npu="950" id7 -->
-   For Ascend 950PR/Ascend 950DT, this API does not support the Subnormal scenario: the internal implementation of this API uses the division operator. Since the division operator does not support the Subnormal scenario, in extremely rare cases the divisor of an internal intermediate computed value is Subnormal data, which causes the final result of this API to be -inf.
<!-- end id7 -->

## Header Files to Include

To use this API, include the **simt\_api/math\_functions.h** header file.

```
#include "simt_api/math_functions.h"
```

## Example

- SIMT programming scenario:

    ```
    __global__ __launch_bounds__(256) void compute_y1f(float *result, const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = y1f(x[idx]);
    }
    ```

- SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(256) inline void compute_y1f_vf(__gm__ float *result, __gm__ const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = y1f(x[idx]);
    }

    __global__ __vector__ void run_y1f(__gm__ float *result, __gm__ const float *x, uint32_t count)
    {
        asc_vf_call<compute_y1f_vf>(dim3(256), result, x, count);
    }
    ```


The input and output examples are as follows:

```
x: 1, 2, 3, 4
result: -0.7812128 -0.1070324 0.3246745 0.3979257
```
