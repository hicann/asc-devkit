# modff

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-01T10:10:38.203Z pushedAt=2026-09-03T07:28:07.156Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Decomposes the input parameter into a fractional part and an integer part.

## Function Prototype

```
inline float modff(float x, float *n)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |
| n | Output | Address in Unified Buffer, Global Memory, or stack space, used to store the integer part of the input data. |

## Return Value

Returns the fractional part of the input parameter.

-   When x is nan, the return value is nan.
-   When x is inf, the return value is 0.
-   When x is -inf, the return value is 0.

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
    __global__ __launch_bounds__(256) void compute_modff(float *result, float *integer_part, const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = modff(x[idx], integer_part + idx);
    }
    ```

- SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(256) inline void compute_modff_vf(__gm__ float *result, __gm__ float *integer_part, __gm__ const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = modff(x[idx], integer_part + idx);
    }

    __global__ __vector__ void run_modff(__gm__ float *result, __gm__ float *integer_part, __gm__ const float *x, uint32_t count)
    {
        asc_vf_call<compute_modff_vf>(dim3(256), result, integer_part, x, count);
    }
    ```

The input and output examples are as follows:

```
x: 0.25, 0.75, 1.25, 1.75
result: 0.25 0.75 0.25 0.75
integer_part: 0 0 1 1
```
