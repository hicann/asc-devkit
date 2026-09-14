# fmodf

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-01T09:54:53.797Z pushedAt=2026-09-03T07:28:07.124Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Obtains the remainder of input data **x** divided by **y**. When computing the remainder, the quotient takes the integer part of the floating-point result of **x** divided by **y**.

## Function Prototype

```
inline float fmodf(float x, float y)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |
| y | Input | Source operand. |

## Return Value

Returns the remainder of input data **x** divided by **y**.

-   When **x** is 0 and **y** is not 0, the return value is 0.
-   When **x** is a finite number and **y** is inf or -inf, the return value is **x**.
-   When **x** is inf or -inf, or **y** is 0, the return value is nan.
-   When either x or y is nan, the return value is nan.

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
    __global__ __launch_bounds__(256) void compute_fmodf(float *result, const float *x, const float *y, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = fmodf(x[idx], y[idx]);
    }
    ```

- SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(256) inline void compute_fmodf_vf(__gm__ float *result, __gm__ const float *x, __gm__ const float *y, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = fmodf(x[idx], y[idx]);
    }

    __global__ __vector__ void run_fmodf(__gm__ float *result, __gm__ const float *x, __gm__ const float *y, uint32_t count)
    {
        asc_vf_call<compute_fmodf_vf>(dim3(256), result, x, y, count);
    }
    ```

The input and output examples are as follows:

```
x: 0.25, 0.75, 1.25, 1.75
y: 1.25, 2.25, 3.25, 4.25
result: 0.25 0.75 1.25 1.75
```
