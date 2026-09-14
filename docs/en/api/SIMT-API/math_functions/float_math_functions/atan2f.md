# atan2f

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-01T09:35:08.575Z pushedAt=2026-09-03T07:28:07.063Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Obtains the arctangent of y/x.

![](../../../figures/zh-cn_formulaimage_0000002516816331.png)

## Function Prototype

```
inline float atan2f(float y, float x)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| y | Input | Source operand. |
| x | Input | Source operand. |

## Return Value

Returns the arctangent of y/x.

The return values in special value scenarios are shown in the following table.

| Input value x | Input value y | Return value |
| --- | --- | --- |
| Any value | nan | nan |
| nan | Any value | nan |
| Positive value (including +0) | +0 | +0 |
| Positive value (including +0) | -0 | -0 |
| Negative value (including -0) | +0 | π |
| Negative value (including -0) | -0 | -π |
| inf | inf | π/4 |
| inf | -inf | -π/4 |
| inf | 1 | 0.0 |
| -inf | inf | 3π/4 |
| -inf | -inf | -3π/4 |
| -inf | 1 | π |
| 1 | inf | π/2 |
| 1 | -inf | -π/2 |

## Constraints

<!-- npu="950" id7 -->
For Ascend 950PR/Ascend 950DT, this interface does not support the Subnormal scenario: the internal implementation of this interface uses the division operator, which does not support the Subnormal scenario. When both x and y are Subnormal data, this interface ultimately returns nan; when only y is Subnormal data and x is a positive non-Subnormal value, this interface ultimately returns 0; when only y is Subnormal data and x is a negative non-Subnormal value, this interface ultimately returns π with the same sign as y.
<!-- end id7 -->

## Header Files to Include

To use this API, include the **simt\_api/math\_functions.h** header file.

```
#include "simt_api/math_functions.h"
```

## Example

- SIMT programming scenario:

    ```
    __global__ __launch_bounds__(256) void compute_atan2f(float *result, const float *x, const float *y, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = atan2f(y[idx], x[idx]);
    }
    ```

- SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(256) inline void compute_atan2f_vf(__gm__ float *result, __gm__ const float *x, __gm__ const float *y, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = atan2f(y[idx], x[idx]);
    }

    __global__ __vector__ void run_atan2f(__gm__ float *result, __gm__ const float *x, __gm__ const float *y, uint32_t count)
    {
        asc_vf_call<compute_atan2f_vf>(dim3(256), result, x, y, count);
    }
    ```

The input and output examples are as follows:

```
y: 1.5, 2.5, 3.5, 4.5
x: 0.25, 0.75, 1.25, 1.75
result: 1.405648 1.27934 1.227772 1.199905
```
