# expm1f

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-01T09:49:22.546Z pushedAt=2026-09-03T07:28:07.098Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Given the input **x**, computes e raised to the power of **x** minus 1.

![](../../../figures/zh-cn_formulaimage_0000002484776378.png)

## Function Prototype

```
inline float expm1f(float x)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns e raised to the power of **x** minus 1.

-   When x is inf, the return value is inf.
-   When x is -inf, the return value is -1.0.
-   When x is nan, the return value is nan.

## Constraints

<!-- npu="950" id7 -->
For Ascend 950PR/Ascend 950DT, this API does not support the Subnormal input scenario: the internal implementation of this API uses **expf**. Since **expf** does not support the Subnormal scenario, when the input **x** is within the Subnormal range, the return value is 0; when **x** is a large negative number and the result of **expf** is within the Subnormal range, the final result of this API is -1.0.
<!-- end id7 -->

## Header Files to Include

To use this API, include the **simt\_api/math\_functions.h** header file.

```
#include "simt_api/math_functions.h"
```

## Example

- SIMT programming scenario:

    ```
    __global__ __launch_bounds__(256) void compute_expm1f(float *result, const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = expm1f(x[idx]);
    }
    ```

- SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(256) inline void compute_expm1f_vf(__gm__ float *result, __gm__ const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = expm1f(x[idx]);
    }

    __global__ __vector__ void run_expm1f(__gm__ float *result, __gm__ const float *x, uint32_t count)
    {
        asc_vf_call<compute_expm1f_vf>(dim3(256), result, x, count);
    }
    ```

The input and output examples are as follows:

```
x: 0.25, 0.75, 1.25, 1.75
result: 0.2840254 1.117 2.490343 4.754602
```
