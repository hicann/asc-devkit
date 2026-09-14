# jnf

<!-- md-trans-meta sourceCommit=1e3d48cde951e4cd58f9cded9fce17c1f24fa02d translatedAt=2026-09-01T10:01:39.521Z pushedAt=2026-09-03T07:28:07.136Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Obtains the value of the n-th order Bessel function of the first kind, jn, for the input data x.

![](../../../figures/zh-cn_formulaimage_0000002484776432.png)

## Function Prototype

```
inline float jnf(int n, float x)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| n | Input | Source operand. |
| x | Input | Source operand. |

## Return Value

Returns the value of the n-th order Bessel function of the first kind, jn, for the input data.

-   When n < 0, the return value is nan.
-   When x is inf or -inf, the return value is 0.
-   When x is nan, the return value is nan.

## Constraints

-   The maximum value of n is 128.
<!-- npu="950" id7 -->
-   For Ascend 950PR/Ascend 950DT, this API does not support the Subnormal scenario: the internal implementation of this API uses expf. Since expf does not support the Subnormal scenario, in rare cases the expf result computed internally is Subnormal data, causing the final result of this API to be 0.
<!-- end id7 -->

## Header Files to Include

To use this API, include the **simt\_api/math\_functions.h** header file.

```
#include "simt_api/math_functions.h"
```

## Example

- SIMT programming scenario:

    ```
    __global__ __launch_bounds__(256) void compute_jnf(float *result, const int *n, const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = jnf(n[idx], x[idx]);
    }
    ```

- SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(256) inline void compute_jnf_vf(__gm__ float *result, __gm__ const int *n, __gm__ const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = jnf(n[idx], x[idx]);
    }

    __global__ __vector__ void run_jnf(__gm__ float *result, __gm__ const int *n, __gm__ const float *x, uint32_t count)
    {
        asc_vf_call<compute_jnf_vf>(dim3(256), result, n, x, count);
    }
    ```

The input/output example is as follows:

```
n: 1, 2, 3, 1
x: 0.25, 0.75, 1.25, 1.75
result: 0.124026 0.06707399 0.03686836 0.5801562
```
