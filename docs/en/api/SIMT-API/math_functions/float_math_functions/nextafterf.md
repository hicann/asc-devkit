# nextafterf

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-01T10:12:08.963Z pushedAt=2026-09-03T07:28:07.160Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

For two data values x and y:

If y is greater than x, the next representable floating-point value greater than x is returned.

If y is less than x, the next representable floating-point value less than x is returned.

If y is equal to x, x is returned.

## Function Prototype

```
inline float nextafterf(float x, float y)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |
| y | Input | Source operand. |

## Return Value

-   When x is not equal to y, the next representable floating-point value after x in the direction of y is returned.
-   When x is equal to y, x is returned.
-   If either x or y is nan, nan is returned.
-   If x = +inf and y is not nan, 3.4028235e+38 is returned.
-   If x = -inf and y is not nan, returns -3.4028235e+38.

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
    __global__ __launch_bounds__(256) void compute_nextafterf(float *result, const float *x, const float *y, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = nextafterf(x[idx], y[idx]);
    }
    ```

- SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(256) inline void compute_nextafterf_vf(__gm__ float *result, __gm__ const float *x, __gm__ const float *y, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = nextafterf(x[idx], y[idx]);
    }

    __global__ __vector__ void run_nextafterf(__gm__ float *result, __gm__ const float *x, __gm__ const float *y, uint32_t count)
    {
        asc_vf_call<compute_nextafterf_vf>(dim3(256), result, x, y, count);
    }
    ```

The input and output examples are as follows:

```
x: 0.25, 0.75, 1.25, 1.75
y: 1.5, 2.5, 3.5, 4.5
result: 0.25 0.7500001 1.25 1.75
```
