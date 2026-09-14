# remquof

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-01T10:19:35.451Z pushedAt=2026-09-03T07:28:07.179Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Obtains the remainder r of the input data x divided by y. r = x - ny, where n is the quotient of x divided by y, and n is the integer nearest to the floating-point result of x divided by y. When the floating-point result of x divided by y is equidistant from the two nearest integers, the quotient is the even integer. The quotient is also assigned to the pointer variable quo.

## Function Prototype

```
inline float remquof(float x, float y, int *quo)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |
| y | Input | Source operand. |
| quo | Output | Address in Unified Buffer, Global Memory, or stack space, used to store the quotient of the division operation. |

## Return Value

Returns the remainder of the input data x divided by y.

-   When either x or y is nan, the return value is nan.
-   When y = 0, the return value is nan.
-   When x = inf or -inf, the return value is nan.
-   When x is a finite value and y = inf or -inf, x is returned.

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
    __global__ __launch_bounds__(256) void compute_remquof(float *result, int *quo, const float *x, const float *y, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = remquof(x[idx], y[idx], quo + idx);
    }
    ```

- SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(256) inline void compute_remquof_vf(__gm__ float *result, __gm__ int *quo, __gm__ const float *x, __gm__ const float *y, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = remquof(x[idx], y[idx], quo + idx);
    }

    __global__ __vector__ void run_remquof(__gm__ float *result, __gm__ int *quo, __gm__ const float *x, __gm__ const float *y, uint32_t count)
    {
        asc_vf_call<compute_remquof_vf>(dim3(256), result, quo, x, y, count);
    }
    ```

The input and output examples are as follows:

```
x: 0.25, 0.75, 1.25, 1.75
y: 1.25, 2.25, 3.25, 4.25
result: 0.25 0.75 1.25 1.75
quo: 0 0 0 0
```
