# \_\_saturatef

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-01T09:33:20.851Z pushedAt=2026-09-03T07:28:07.057Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Converts the input data to the [0.0, 1.0] range. Specifically, if the input number is greater than or equal to 1, then the result is 1; if the input number is less than or equal to 0, then the result is 0; if the input number is within the [0.0, 1.0] range, then the result is the original input data.

## Function Prototype

```
float __saturatef(float x)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **x** | Input | Source operand. |

## Return Value

Returns the result of converting the input data to the [0.0, 1.0] range.

-   When x≤0, the return value is 0.
-   When x≥1, the return value is 1.
-   When 0<x<1, the return value is x.
-   When x is nan, the return value is 0.

## Constraints

None

## Header Files to Include

To use this API, include the **simt\_api/device\_functions.h** header file.

```
#include "simt_api/device_functions.h"
```

## Example

- SIMT programming scenario:

    ```
    __global__ __launch_bounds__(256) void compute_saturate_intrinsic(float *result, const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = __saturatef(x[idx]);
    }
    ```

- SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(256) inline void compute_saturate_intrinsic_vf(__gm__ float *result, __gm__ const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = __saturatef(x[idx]);
    }

    __global__ __vector__ void run_saturate_intrinsic(__gm__ float *result, __gm__ const float *x, uint32_t count)
    {
        asc_vf_call<compute_saturate_intrinsic_vf>(dim3(256), result, x, count);
    }
    ```

The input and output examples are as follows:

```
x：0.25, 0.75, 1.25, 1.75
result: 0.25 0.75 1 1
```
