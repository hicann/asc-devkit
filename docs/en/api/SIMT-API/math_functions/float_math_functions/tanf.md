# tanf

<!-- md-trans-meta sourceCommit=1e3d48cde951e4cd58f9cded9fce17c1f24fa02d translatedAt=2026-09-01T10:32:47.007Z pushedAt=2026-09-03T07:28:07.220Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Obtains the trigonometric tangent value of the input data.

![](../../../figures/zh-cn_formulaimage_0000002516816325.png)

## Function Prototype

```
inline float tanf(float x)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns the tangent value of the input data.

-   When x is inf, the return value is nan.
-   When x is -inf, the return value is nan.
-   When x is nan, the return value is nan.

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
    __global__ __launch_bounds__(256) void compute_tanf(float *result, const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = tanf(x[idx]);
    }
    ```

- SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(256) inline void compute_tanf_vf(__gm__ float *result, __gm__ const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = tanf(x[idx]);
    }

    __global__ __vector__ void run_tanf(__gm__ float *result, __gm__ const float *x, uint32_t count)
    {
        asc_vf_call<compute_tanf_vf>(dim3(256), result, x, count);
    }
    ```

The following is an example of the input and output:

```
x: 0.25, 0.75, 1.25, 1.75
result: 0.2553419 0.9315965 3.00957 -5.52038
```
