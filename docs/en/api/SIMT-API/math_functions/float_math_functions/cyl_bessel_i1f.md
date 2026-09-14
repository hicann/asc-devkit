# cyl\_bessel\_i1f

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-01T09:42:20.393Z pushedAt=2026-09-03T07:28:07.084Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Obtains the value of the first-order regular modified cylindrical Bessel function of the input data **x**.

![](../../../figures/zh-cn_formulaimage_0000002484776426.png)

## Function Prototype

```
inline float cyl_bessel_i1f(float x)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

The value of the first-order regular modified cylindrical Bessel function of the input data.

-   When **x** is 0, the return value is 0.
-   When x is inf, the return value is inf.
-   When x is -inf, the return value is -inf.
-   When x is nan, the return value is nan.
-   If the return value exceeds the maximum range of float, the return value is inf.
-   If the return value exceeds the minimum range of float, the return value is -inf.

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
    __global__ __launch_bounds__(256) void compute_cyl_bessel_i1f(float *result, const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = cyl_bessel_i1f(x[idx]);
    }
    ```

- SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(256) inline void compute_cyl_bessel_i1f_vf(__gm__ float *result, __gm__ const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = cyl_bessel_i1f(x[idx]);
    }

    __global__ __vector__ void run_cyl_bessel_i1f(__gm__ float *result, __gm__ const float *x, uint32_t count)
    {
        asc_vf_call<compute_cyl_bessel_i1f_vf>(dim3(256), result, x, count);
    }
    ```

The input and output examples are as follows:

```
x: 0.25, 0.75, 1.25, 1.75
result: 0.1259791 0.4019924 0.7552814 1.255537
```
