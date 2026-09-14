# sinhf

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-01T10:31:12.251Z pushedAt=2026-09-03T07:28:07.216Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Obtains the hyperbolic sine of the input data.

![](../../../figures/zh-cn_formulaimage_0000002516816353.png)

## Function Prototype

```
inline float sinhf(float x)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns the hyperbolic sine of the input data.

-   When x is inf, the return value is inf.
-   When x is -inf, the return value is -inf.
-   When x is nan, the return value is nan.
- When **x** is 0, the return value is 0.

## Constraints

This API supports input data within the range [-89.4, 89.4].

## Header Files to Include

To use this API, include the **simt\_api/math\_functions.h** header file.

```
#include "simt_api/math_functions.h"
```

## Example

- SIMT programming scenario:

    ```
    __global__ __launch_bounds__(256) void compute_sinhf(float *result, const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = sinhf(x[idx]);
    }
    ```

- SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(256) inline void compute_sinhf_vf(__gm__ float *result, __gm__ const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = sinhf(x[idx]);
    }

    __global__ __vector__ void run_sinhf(__gm__ float *result, __gm__ const float *x, uint32_t count)
    {
        asc_vf_call<compute_sinhf_vf>(dim3(256), result, x, count);
    }
    ```

The input and output example is as follows:

```
x: 0.25, 0.75, 1.25, 1.75
result: 0.2526123 0.8223166 1.601919 2.790414
```
