# expf

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-01T09:48:12.981Z pushedAt=2026-09-03T07:28:07.096Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Computes e raised to the power of x based on the input x.

![](../../../figures/zh-cn_formulaimage_0000002516816333.png)

## Function Prototype

```
inline float expf(float x)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand, of the float data type. |

## Return Value

Returns e raised to the power of x.

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
    __global__ __launch_bounds__(256) void compute_expf(float *result, const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = expf(x[idx]);
    }
    ```

- SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(256) inline void compute_expf_vf(__gm__ float *result, __gm__ const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = expf(x[idx]);
    }

    __global__ __vector__ void run_expf(__gm__ float *result, __gm__ const float *x, uint32_t count)
    {
        asc_vf_call<compute_expf_vf>(dim3(256), result, x, count);
    }
    ```

The following shows an example of the input and output:

```
x: 0.25, 0.75, 1.25, 1.75
result: 1.284025 2.117 3.490343 5.754602
```
