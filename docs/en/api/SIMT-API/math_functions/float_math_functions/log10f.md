# log10f

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-01T10:05:33.826Z pushedAt=2026-09-03T07:28:07.144Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Obtains the base-10 logarithm of the input data.

![](../../../figures/zh-cn_formulaimage_0000002516816341.png)

## Function Prototype

```
inline float log10f(float x)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

The base-10 logarithm of x.

-   When x is inf, the return value is inf.
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
    __global__ __launch_bounds__(256) void compute_log10f(float *result, const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = log10f(x[idx]);
    }
    ```

- SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(256) inline void compute_log10f_vf(__gm__ float *result, __gm__ const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = log10f(x[idx]);
    }

    __global__ __vector__ void run_log10f(__gm__ float *result, __gm__ const float *x, uint32_t count)
    {
        asc_vf_call<compute_log10f_vf>(dim3(256), result, x, count);
    }
    ```

The input and output example is as follows:

```
x: 1, 2, 3, 4
result: 0 0.30103 0.4771213 0.60206
```
