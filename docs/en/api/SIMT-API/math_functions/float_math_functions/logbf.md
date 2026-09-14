# logbf

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-01T10:07:29.748Z pushedAt=2026-09-03T07:28:07.148Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Computes the base-2 logarithm of the input data and rounds the result down to the nearest integer, returning a floating-point number.

![](../../../figures/zh-cn_formulaimage_0000002516816343.png)

## Function Prototype

```
inline float logbf(float x)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns the base-2 logarithm of x, rounded down to the nearest integer.

-   When x is inf, the return value is inf.
-   When x is -inf, the return value is inf.
-   When x is nan, the return value is nan.
-   When x is 0, the return value is -inf.

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
    __global__ __launch_bounds__(256) void compute_logbf(float *result, const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = logbf(x[idx]);
    }
    ```

- SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(256) inline void compute_logbf_vf(__gm__ float *result, __gm__ const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = logbf(x[idx]);
    }

    __global__ __vector__ void run_logbf(__gm__ float *result, __gm__ const float *x, uint32_t count)
    {
        asc_vf_call<compute_logbf_vf>(dim3(256), result, x, count);
    }
    ```

The following shows an example of the input and output:

```
x: 1, 2, 3, 4
result: 0 1 1 2
```
