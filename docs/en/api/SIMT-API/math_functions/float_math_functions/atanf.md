# atanf

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-01T09:36:24.585Z pushedAt=2026-09-03T07:28:07.064Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Obtains the arc tangent value of the input data.

![](../../../figures/zh-cn_formulaimage_0000002484776372.png)

## Function Prototype

```
inline float atanf(float x)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns the arc tangent value of the input data.

-   When x is inf, the return value is π/2.
-   When x is -inf, the return value is -π/2.
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
    __global__ __launch_bounds__(256) void compute_atanf(float *result, const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = atanf(x[idx]);
    }
    ```

- SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(256) inline void compute_atanf_vf(__gm__ float *result, __gm__ const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = atanf(x[idx]);
    }

    __global__ __vector__ void run_atanf(__gm__ float *result, __gm__ const float *x, uint32_t count)
    {
        asc_vf_call<compute_atanf_vf>(dim3(256), result, x, count);
    }
    ```

The following is an example of input and output:

```
x: 0.25, 0.75, 1.25, 1.75
result: 0.2449787 0.6435012 0.8960554 1.05165
```
