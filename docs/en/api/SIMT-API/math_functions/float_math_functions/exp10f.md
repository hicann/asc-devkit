# exp10f

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-01T09:46:44.323Z pushedAt=2026-09-03T07:28:07.094Z -->

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

![](../../../figures/zh-cn_formulaimage_0000002516816335.png)

## Function Prototype

```
inline float exp10f(float x)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. The supported data type is float. |

## Return Value

10 raised to the power of x.

-   When x is inf, the return value is inf.
-   When x is -inf, the return value is 0.
-   When x is nan, the return value is nan.

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
    __global__ __launch_bounds__(256) void compute_exp10f(float *result, const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = exp10f(x[idx]);
    }
    ```

- SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(256) inline void compute_exp10f_vf(__gm__ float *result, __gm__ const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = exp10f(x[idx]);
    }

    __global__ __vector__ void run_exp10f(__gm__ float *result, __gm__ const float *x, uint32_t count)
    {
        asc_vf_call<compute_exp10f_vf>(dim3(256), result, x, count);
    }
    ```

The following is an example of input and output:

```
x: 0.25, 0.75, 1.25, 1.75
result: 1.778279 5.623414 17.7828 56.23413
```
