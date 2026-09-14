# isfinite

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-01T09:58:34.043Z pushedAt=2026-09-03T07:28:07.130Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Determines whether a floating-point number is finite (not inf or nan).

## Function Prototype

```
inline bool isfinite(float x)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

-   false: The input is nan, inf, or -inf.
-   true: The input is a finite number.

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
    __global__ __launch_bounds__(256) void compute_isfinite(bool *result, const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = isfinite(x[idx]);
    }
    ```

- SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(256) inline void compute_isfinite_vf(__gm__ bool *result, __gm__ const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = isfinite(x[idx]);
    }

    __global__ __vector__ void run_isfinite(__gm__ bool *result, __gm__ const float *x, uint32_t count)
    {
        asc_vf_call<compute_isfinite_vf>(dim3(256), result, x, count);
    }
    ```

The following shows an example of the input and output:

```
x: 0.25, 0.75, 1.25, 1.75
result: 1 1 1 1
```
