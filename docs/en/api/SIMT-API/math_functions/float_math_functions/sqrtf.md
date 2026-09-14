# sqrtf

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-01T10:32:35.099Z pushedAt=2026-09-03T07:28:07.218Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Obtains the square root of the input data **x**.

![](../../../figures/zh-cn_formulaimage_0000002516816361.png)

## Function Prototype

```
inline float sqrtf(float x)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **x** | Input | Source operand, that is, the input data. |

## Return Value

Returns the square root of the input data.

-   When **x** is 0, the return value is 0.
-   When x is inf, the return value is inf.
-   When **x** is -inf, the return value is nan.
-   When x is nan, the return value is nan.

## Constraints

The input data range supported by this API is x greater than or equal to 0; otherwise, the return value is nan.

<!-- npu="950" id7 -->
For Ascend 950PR/Ascend 950DT, this API does not support the Subnormal scenario: input and output values within the Subnormal range are flushed to 0 with the sign preserved.
<!-- end id7 -->

## Header Files to Include

To use this API, include the **simt\_api/math\_functions.h** header file.

```
#include "simt_api/math_functions.h"
```

## Example

- SIMT programming scenario:

    ```
    __global__ __launch_bounds__(256) void compute_sqrtf(float *result, const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = sqrtf(x[idx]);
    }
    ```

- SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(256) inline void compute_sqrtf_vf(__gm__ float *result, __gm__ const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = sqrtf(x[idx]);
    }

    __global__ __vector__ void run_sqrtf(__gm__ float *result, __gm__ const float *x, uint32_t count)
    {
        asc_vf_call<compute_sqrtf_vf>(dim3(256), result, x, count);
    }
    ```

The following is an example of the input and output:

```
x: 0.25, 1.25, 2.25, 3.25
result: 0.5 1.118034 1.5 1.802776
```
