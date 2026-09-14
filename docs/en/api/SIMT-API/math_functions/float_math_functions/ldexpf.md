# ldexpf

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-01T10:02:39.985Z pushedAt=2026-09-03T07:28:07.138Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Obtains the result of the input **x** multiplied by 2 to the power of **exp**.

![](../../../figures/zh-cn_formulaimage_0000002484776400.png)

## Function Prototype

```
inline float ldexpf(float x, int exp)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand, input data. |
| exp | Input | Source operand, exponent. |

## Return Value

Returns the result of the input data **x** multiplied by 2 to the power of **exp**.

-   When x is nan, the return value is nan.
-   When x is inf, the return value is inf.
-   When x is -inf, the return value is -inf.

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
    __global__ __launch_bounds__(256) void compute_ldexpf(float *result, const int *n, const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = ldexpf(x[idx], n[idx]);
    }
    ```

- SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(256) inline void compute_ldexpf_vf(__gm__ float *result, __gm__ const int *n, __gm__ const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = ldexpf(x[idx], n[idx]);
    }

    __global__ __vector__ void run_ldexpf(__gm__ float *result, __gm__ const int *n, __gm__ const float *x, uint32_t count)
    {
        asc_vf_call<compute_ldexpf_vf>(dim3(256), result, n, x, count);
    }
    ```

The following shows an example of the input and output:

```
n: 1, 2, 3, 1
x: 0.25, 0.75, 1.25, 1.75
result: 0.5 3 10 3.5
```
