# powf

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-01T10:16:35.583Z pushedAt=2026-09-03T07:28:07.170Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Obtains the y-th power of the input data x.

![](../../../figures/zh-cn_formulaimage_0000002516816365.png)

## Function Prototype

```
inline float powf(float x, float y)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand, the base of the power operation. |
| y | Input | Source operand, the exponent of the power operation. |

## Return Value

The result of x raised to the power of y.

-   If x^y exceeds the maximum range of float, the return value is inf.
-   If x is ±0 and y is less than 0 and is odd, the return value is ±inf.
-   If x is ±0 and y is less than 0 and is not odd, the return value is inf.
-   If x is ±0 and y is greater than 0 and is odd, the return value is ±0.
-   If x is ±0 and y is greater than 0 and is not odd, the return value is 0.
-   If x is -1 and y is ±inf, the return value is 1.
-   If x is 1 and y is any value (including nan), the return value is 1.
-   If y is ±0 and x is any value (including nan), the return value is 1.
-   If x is less than 0 and y is not an integer, the return value is nan.
-   If |x| < 1 and y is -inf, the return value is inf.
-   If |x| > 1 and y is -inf, the return value is 0.
-   If |x| < 1 and y is inf, the return value is 0.
-   If |x| > 1 and y is inf, the return value is inf.
-   If x is -inf and y is less than 0 and is odd, the return value is -0.
-   If x is -inf and y is less than 0 and is not odd, the return value is 0.
-   If x is -inf, y is greater than 0 and is odd, the return value is -inf.
-   If x is -inf, y is greater than 0 and is not odd, the return value is inf.
-   If x is inf and y is less than 0, the return value is 0.
-   If x is inf and y is greater than 0, the return value is inf.
-   In the following boundary scenarios, the return value is nan.
    -   x is nan and y is not 0.
    -   y is nan and x is not 1.
    -   Both x and y are nan.

## Constraints

<!-- npu="950" id7 -->
For Ascend 950PR/Ascend 950DT, this API does not support the Subnormal scenario: input and output values within the Subnormal range are flushed to 0 with the sign preserved.
<!-- end id7 -->

## Header Files to Include

To use this API, include the **simt\_api/math\_functions.h** header file.

```
#include "simt_api/math_functions.h"
```

## Example

- SIMT programming scenario:

    ```
    __global__ __launch_bounds__(256) void compute_powf(float *result, const float *x, const float *y, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = powf(x[idx], y[idx]);
    }
    ```

- SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(256) inline void compute_powf_vf(__gm__ float *result, __gm__ const float *x, __gm__ const float *y, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = powf(x[idx], y[idx]);
    }

    __global__ __vector__ void run_powf(__gm__ float *result, __gm__ const float *x, __gm__ const float *y, uint32_t count)
    {
        asc_vf_call<compute_powf_vf>(dim3(256), result, x, y, count);
    }
    ```

The input and output examples are as follows:

```
x: 0.25, 0.75, 1.25, 1.75
y: 1.25, 2.25, 3.25, 4.25
result: 0.176777 0.523465 2.065178 10.787274
```
