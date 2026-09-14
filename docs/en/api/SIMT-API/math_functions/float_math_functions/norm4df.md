# norm4df

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-01T10:13:32.932Z pushedAt=2026-09-03T07:28:07.165Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Obtains the square root of the sum of squares of the input data a, b, c, and d, that is, a^2 + b^2 + c^2 + d^2.

![](../../../figures/zh-cn_formulaimage_0000002484776408.png)

## Function Prototype

```
inline float norm4df(float a, float b, float c, float d)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| a | Input | Source operand. |
| b | Input | Source operand. |
| c | Input | Source operand. |
| d | Input | Source operand. |

## Return Value

Returns the square root of a^2 + b^2 + c^2 + d^2.

-   If the square root of a^2 + b^2 + c^2 + d^2 exceeds the maximum range of float, the return value is inf.
-   If any one or more of a, b, c, and d are ±inf, the return value is inf.
-   If any one or more of a, b, c, and d are nan and none of them are ±inf, the return value is nan.

## Constraints

<!-- npu="950" id7 -->
For Ascend 950PR/Ascend 950DT, this API does not support the subnormal scenario: the internal implementation of this API uses the division operator. Since the division operator does not support the subnormal scenario, when all inputs are subnormal data, this API ultimately returns nan.
<!-- end id7 -->

## Header Files to Include

To use this API, include the **simt\_api/math\_functions.h** header file.

```
#include "simt_api/math_functions.h"
```

## Example

- SIMT programming scenario:

    ```
    __global__ __launch_bounds__(256) void compute_norm4df(float *result, const float *x, const float *y, const float *z, const float *w, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = norm4df(x[idx], y[idx], z[idx], w[idx]);
    }
    ```

- SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(256) inline void compute_norm4df_vf(__gm__ float *result, __gm__ const float *x, __gm__ const float *y, __gm__ const float *z, __gm__ const float *w, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = norm4df(x[idx], y[idx], z[idx], w[idx]);
    }

    __global__ __vector__ void run_norm4df(__gm__ float *result, __gm__ const float *x, __gm__ const float *y, __gm__ const float *z, __gm__ const float *w, uint32_t count)
    {
        asc_vf_call<compute_norm4df_vf>(dim3(256), result, x, y, z, w, count);
    }
    ```

Input and output examples are as follows:

```
x: 0.25, 0.75, 1.25, 1.75
y: 1.5, 2.5, 3.5, 4.5
z: -0.5, 0.5, 1.5, 2.5
w: 2, 3, 4, 5
result: 2.561738 4.007805 5.662376 7.386643
```
